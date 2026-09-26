// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "WebUi.h"
#include "Config.h"
#include "Page.h"
#include "Branding.h"
#include "FirelabsCore.h"
#include "Display.h"
#include "Ui.h"
#include "Screen.h"
#include "CheckIn.h"
#include <WebServer.h>
#include <esp_ota_ops.h>
#include <esp_heap_caps.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <vector>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>

static WebServer server(80);
static Config* cfg_ = nullptr;
static FirelabsCore* core_ = nullptr;
static bool begun_ = false;
static uint32_t rebootAt = 0;
static int extraPlants_ = 0;
static String debugBundle_;  // pending showcase bundle
static bool frozen_ = false;   // a showcase bundle is on screen

extern "C" int fl_lv_internal(void);
extern "C" void fl_lv_set_internal(int on);

// OTA in two steps: download the whole image into PSRAM, then install it from RAM.
// Writing flash while the upload streams in keeps switching the cache off, and wifi's
// PSRAM buffers drop packets each time; TCP backs off past WebServer's 5 s data
// timeout and the upload is aborted. Downloading first keeps flash quiet until the
// network is done.
static uint8_t* otaBuf = nullptr;
static size_t otaCap = 0, otaLen = 0;
static bool otaOk = false;
static const char* otaErr = "";
static const char* const UPDATE_SUB = "Keep the display plugged in. It restarts when done.";

// Bar plus "70% · 1.2 of 1.7 MB"; the status strip is only redrawn when the
// percent moves, since each redraw goes through LVGL.
static void otaProgress(size_t done, size_t total, const char* verb) {
  static int lastPct = -1;
  Display::setUpdateProgress(done, total);
  int pct = total ? (int)((uint64_t)done * 100 / total) : 0;
  if (done == 0) lastPct = -1;
  if (pct == lastPct) return;
  lastPct = pct;
  char line[64];
  snprintf(line, sizeof(line), "%d%%  \xC2\xB7  %.1f of %.1f MB%s", pct, done / 1048576.0,
           total / 1048576.0, verb);
  Display::setUpdateStatus(line);
}

static void otaFree() {
  heap_caps_free(otaBuf);
  otaBuf = nullptr;
  otaCap = otaLen = 0;
}

// Writes the downloaded image to the spare slot and makes it the boot slot.
static bool otaInstall() {
  const esp_partition_t* part = esp_ota_get_next_update_partition(nullptr);
  if (!part) { otaErr = "no OTA slot"; return false; }
  if (otaLen < 1024 || otaBuf[0] != 0xE9) { otaErr = "not a firmware image"; return false; }
  if (otaLen > part->size) { otaErr = "image too big"; return false; }
  Display::setUpdateText("Installing update", UPDATE_SUB);
  otaProgress(0, otaLen, " written");
  esp_ota_handle_t h = 0;
  if (esp_ota_begin(part, OTA_WITH_SEQUENTIAL_WRITES, &h) != ESP_OK) { otaErr = "begin"; return false; }
  const size_t CHUNK = 16 * 1024;
  uint32_t slowest = 0;
  for (size_t off = 0; off < otaLen; off += CHUNK) {
    size_t n = min(CHUNK, otaLen - off);
    uint32_t t0 = millis();
    if (esp_ota_write(h, otaBuf + off, n) != ESP_OK) {
      esp_ota_abort(h);
      otaErr = "write";
      return false;
    }
    slowest = max(slowest, millis() - t0);
    otaProgress(off + n, otaLen, " written");
  }
  if (esp_ota_end(h) != ESP_OK) { otaErr = "image check failed"; return false; }
  if (esp_ota_set_boot_partition(part) != ESP_OK) { otaErr = "set boot"; return false; }
  Serial.printf("[FL-PD] OTA installed %u bytes into %s, slowest 16 KB write %lu ms\n",
                (unsigned)otaLen, part->label, slowest);
  return true;
}

// Pull OTA: fetch the image from a URL into PSRAM, then install it. This is the
// rollout path (HA hands out the URL); HTTPClient streams in big reads with our own
// stall timeout, where the push route goes through WebServer's byte-by-byte multipart
// parser and its hard 5 s limit.
static String pullUrl;

static bool otaDownload(const String& url) {
  NetworkClientSecure tls;
  NetworkClient plain;
  bool https = url.startsWith("https://");
  if (https) tls.setInsecure();  // LAN/HA-served; the image is checked by esp_ota_end
  HTTPClient http;
  http.setTimeout(20000);
  if (!http.begin(https ? (NetworkClient&)tls : plain, url)) { otaErr = "bad URL"; return false; }
  int code = http.GET();
  if (code != 200) {
    Serial.printf("[FL-PD] OTA GET %s -> %d\n", url.c_str(), code);
    otaErr = "download failed";
    http.end();
    return false;
  }
  int len = http.getSize();
  if (len <= 0) { otaErr = "no length"; http.end(); return false; }
  otaFree();
  otaCap = len;
  otaBuf = (uint8_t*)heap_caps_malloc(otaCap, MALLOC_CAP_SPIRAM);
  if (!otaBuf) { otaErr = "no room to download"; http.end(); return false; }
  NetworkClient* stream = http.getStreamPtr();
  uint32_t t0 = millis(), lastData = millis();
  while (otaLen < otaCap) {
    size_t avail = stream->available();
    if (avail) {
      int n = stream->readBytes(otaBuf + otaLen, min(avail, otaCap - otaLen));
      if (n > 0) {
        otaLen += n;
        lastData = millis();
        otaProgress(otaLen, otaCap, "");
      }
    } else if (!stream->connected() || millis() - lastData > 20000) {
      break;
    } else {
      delay(2);
    }
  }
  http.end();
  Serial.printf("[FL-PD] OTA downloaded %u of %u bytes in %lu ms\n", (unsigned)otaLen, (unsigned)otaCap, millis() - t0);
  if (otaLen != otaCap) { otaErr = "download stalled"; return false; }
  return true;
}

// scheme://host[:port] of the webhook URL: an address this device already reaches.
static String webhookOrigin() {
  const String& w = cfg_->webhookUrl;
  int start = w.indexOf("://");
  if (start < 0) return "";
  int end = w.indexOf('/', start + 3);
  return end < 0 ? w : w.substring(0, end);
}

// Tries each candidate URL in turn, three attempts in all.
static void otaPull(const String& urls) {
  CheckIn::pause(true);
  Display::beginUpdateScreen("Downloading update", UPDATE_SUB);
  bool ok = false;
  std::vector<String> list;
  for (int from = 0; from < (int)urls.length();) {
    int nl = urls.indexOf('\n', from);
    String u = urls.substring(from, nl < 0 ? urls.length() : nl);
    if (u.length()) list.push_back(u);
    from = nl < 0 ? urls.length() : nl + 1;
  }
  for (int attempt = 1; attempt <= 3 && !ok && !list.empty(); attempt++) {
    const String& url = list[(attempt - 1) % list.size()];
    otaErr = "";
    Display::setUpdateText("Downloading update", UPDATE_SUB);
    otaProgress(0, 1, "");
    Serial.printf("[FL-PD] OTA from %s\n", url.c_str());
    ok = otaDownload(url) && otaInstall();
    if (!ok) Serial.printf("[FL-PD] OTA attempt %d failed: %s\n", attempt, otaErr);
  }
  otaFree();
  if (ok) {
    Serial.println("[FL-PD] OTA done, restarting");
    rebootAt = millis() + 800;
    return;
  }
  Display::endUpdateScreen();
  CheckIn::pause(false);
}

void WebUi::startOta(const String& url) { pullUrl = url; }

int WebUi::extraPlants() { return extraPlants_; }

bool WebUi::takeDebugBundle(String& json) {
  if (!debugBundle_.length()) return false;
  json = debugBundle_;
  debugBundle_ = "";
  frozen_ = true;
  return true;
}

bool WebUi::bundleFrozen() { return frozen_; }

static void servePage() { server.send_P(200, "text/html", CONFIG_HTML); }

void WebUi::begin(Config& cfg, FirelabsCore& core) {
  cfg_ = &cfg;
  core_ = &core;

  server.on("/", HTTP_GET, servePage);
  server.onNotFound(servePage);

  // Identity for the FireLabs HA integration (config_flow probe + device info).
  server.on("/api/status", HTTP_GET, []() {
    JsonDocument d;
    d["mac"] = WiFi.macAddress();
    d["model"] = FL_MODEL;
    d["name"] = core_->deviceName;
    d["fw"] = FL_FW_VERSION;
    String out;
    serializeJson(d, out);
    server.send(200, "application/json", out);
  });

  // HA pushes the display entities here so a change applies at once, not on the next
  // check-in. Any subset of backlight, brightness, auto_dim, dim_after, dim_level.
  server.on("/api/display", HTTP_POST, []() {
    DisplaySettings s = Screen::settings();
    if (!parseDisplaySettings(server.arg("plain"), s)) {
      server.send(400, "application/json", "{\"err\":1}");
      return;
    }
    Screen::apply(s, false);
    server.send(200, "application/json", "{\"ok\":1}");
  });

#ifdef FL_DEBUG_PORTAL
  server.on("/api/debug", HTTP_GET, []() {
    server.send(200, "application/json", Display::debugJson());
  });

  // Pad each bundle with n made-up plants to exercise paging; cleared by a reboot.
  server.on("/api/debug/extra", HTTP_POST, []() {
    extraPlants_ = constrain(server.arg("n").toInt(), 0, 9);
    server.send(200, "application/json", String("{\"extra\":") + extraPlants_ + "}");
  });

  // The panel as raw RGB565 (800x480, little-endian): README screenshots.
  server.on("/api/debug/screenshot", HTTP_GET, []() {
    bool owned = false;
    uint8_t* fb = Display::captureFrame(owned);
    if (!fb) {
      server.send(500, "text/plain", "no frame");
      return;
    }
    const size_t len = Display::WIDTH * Display::HEIGHT * 2;
    server.setContentLength(len);
    server.send(200, "application/octet-stream", "");
    for (size_t off = 0; off < len; off += 16384)
      server.sendContent((const char*)fb + off, min((size_t)16384, len - off));
    if (owned) heap_caps_free(fb);
  });

  // Showcase data: a bundle in the body replaces what check-ins bring until reboot.
  server.on("/api/debug/bundle", HTTP_POST, []() {
    debugBundle_ = server.arg("plain");
    server.send(200, "application/json", "{\"ok\":1}");
  });

  // Holds the water-now pulse at ?opa= (0-255) for GIF frames; -1 lets it run.
  server.on("/api/debug/pulse", HTTP_POST, []() {
    Ui::debugPulse(server.arg("opa").toInt());
    server.send(200, "application/json", "{\"ok\":1}");
  });

  // Switches screens: ?show=overview, wifi, or a plant name.
  server.on("/api/debug/show", HTTP_POST, []() {
    bool ok = Ui::debugShow(server.arg("show"));
    server.send(ok ? 200 : 404, "application/json", ok ? "{\"ok\":1}" : "{\"error\":\"unknown\"}");
  });

  // Shows the update screen at pct percent without updating; pct=-1 hides it.
  server.on("/api/debug/updatescreen", HTTP_POST, []() {
    int pct = server.arg("pct").toInt();
    if (pct < 0) Display::endUpdateScreen();
    else {
      const size_t image = 1780768;  // a real release's size, so the status line reads true
      Display::beginUpdateScreen("Downloading update", UPDATE_SUB);
      otaProgress((image * min(pct, 100) + 99) / 100, image, "");
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });

  // Erases and writes the spare OTA slot one 4 KB sector at a time, timing each, with
  // no network traffic in between: separates a flash problem from an upload one.
  // LVGL allocator A/B: internal=1 runs LVGL on plain malloc after a soft restart.
  server.on("/api/debug/lvmem", HTTP_POST, []() {
    fl_lv_set_internal(server.arg("internal") == "1");
    server.send(200, "application/json", String("{\"internal\":") + fl_lv_internal() + "}");
    rebootAt = millis() + 500;
  });

  // Erases and writes the spare OTA slot one 4 KB sector at a time, timing each, with
  // no network traffic in between. Replies first; results go to serial. lcd=off stops
  // the panel and wifi=off drops wifi before the test (both restart afterwards).
  server.on("/api/debug/flashtest", HTTP_POST, []() {
    const esp_partition_t* part = esp_ota_get_next_update_partition(nullptr);
    uint32_t from = strtoul(server.arg("from").c_str(), nullptr, 0);
    uint32_t to = strtoul(server.arg("to").c_str(), nullptr, 0);
    bool screen = server.arg("screen") == "1";
    bool lcdOff = server.arg("lcd") == "off";
    bool wifiOff = server.arg("wifi") == "off";
    if (!part || to <= from || to > part->size) {
      server.send(400, "application/json", "{\"error\":\"range\"}");
      return;
    }
    server.send(200, "application/json", "{\"started\":1}");
    delay(200);
    CheckIn::pause(true);
    if (screen) Display::beginUpdateScreen("Flash test", "Writing the spare slot");
    if (lcdOff) Display::stopPanel();
    if (wifiOff) WiFi.mode(WIFI_OFF);
    static uint8_t buf[4096];
    for (size_t i = 0; i < sizeof(buf); i++) buf[i] = i * 7;
    uint32_t worstErase = 0, worstWrite = 0;
    Serial.printf("[FL-PD] flashtest %s 0x%lx..0x%lx lcd=%s wifi=%s lvgl=%s\n", part->label, from, to,
                  lcdOff ? "off" : "on", wifiOff ? "off" : "on", fl_lv_internal() ? "internal" : "psram");
    Serial.flush();
    for (uint32_t off = from & ~0xFFFu; off < to; off += 4096) {
      Display::trace(10, off);
      uint32_t t0 = micros();
      esp_err_t e1 = esp_partition_erase_range(part, off, 4096);
      uint32_t t1 = micros();
      Display::trace(11, off);
      esp_err_t e2 = esp_partition_write(part, off, buf, sizeof(buf));
      uint32_t t2 = micros();
      worstErase = max(worstErase, t1 - t0);
      worstWrite = max(worstWrite, t2 - t1);
      Serial.printf("[flash] 0x%06lx erase %lu us (%d) write %lu us (%d)\n", part->address + off,
                    t1 - t0, e1, t2 - t1, e2);
      Serial.flush();
    }
    Serial.printf("[FL-PD] flashtest done, worst erase %lu us, worst write %lu us\n", worstErase, worstWrite);
    Serial.flush();
    if (screen) Display::endUpdateScreen();
    CheckIn::pause(false);
    if (lcdOff || wifiOff) ESP.restart();
  });
#endif

  server.on("/api/config", HTTP_GET, []() {
    JsonDocument d;
    d["host"] = core_->hostname();
    d["mac"] = WiFi.macAddress();
    d["name"] = core_->deviceName;
    d["webhook"] = cfg_->webhookUrl;
    String out;
    serializeJson(d, out);
    server.send(200, "application/json", out);
  });

  server.on("/api/save", HTTP_POST, []() {
    JsonDocument d;
    if (deserializeJson(d, server.arg("plain"))) {
      server.send(400, "application/json", "{\"err\":1}");
      return;
    }
    if (d["webhook"].is<const char*>()) cfg_->webhookUrl = (const char*)d["webhook"];
    cfg_->save();
    server.send(200, "application/json", "{\"ok\":1}");
    rebootAt = millis() + 1500;
  });

  server.on("/api/reset", HTTP_POST, []() {
    cfg_->clear();
    core_->clear();  // also wipe wifi/name so it reboots into setup
    server.send(200, "application/json", "{\"ok\":1}");
    rebootAt = millis() + 1000;
  });

  // {"path": "/api/...", "url": "http://..."}: the device fetches and installs it (see
  // otaPull). "path" is resolved against the webhook's origin and tried first; HA's
  // own idea of its URL (internal or external) isn't always one this device reaches.
  server.on("/api/ota", HTTP_POST, []() {
    JsonDocument d;
    String urls;
    if (!deserializeJson(d, server.arg("plain"))) {
      String origin = webhookOrigin();
      if (d["path"].is<const char*>() && origin.length()) urls += origin + (const char*)d["path"] + "\n";
      if (d["url"].is<const char*>()) urls += String((const char*)d["url"]) + "\n";
    }
    if (!urls.length()) {
      server.send(400, "application/json", "{\"error\":\"url or path\"}");
      return;
    }
    pullUrl = urls;
    server.send(202, "application/json", "{\"started\":1}");
  });

  server.on("/update", HTTP_POST,
    []() {
      bool ok = otaOk && otaInstall();
      otaFree();
      if (!ok) Serial.printf("[FL-PD] OTA failed: %s\n", otaErr);
      server.send(ok ? 200 : 500, "text/plain", ok ? "OK, rebooting" : String("FAIL: ") + otaErr);
      if (ok) rebootAt = millis() + 800;
      else {
        Display::endUpdateScreen();
        CheckIn::pause(false);
      }
    },
    []() {
      HTTPUpload& up = server.upload();
      if (up.status == UPLOAD_FILE_START) {
        CheckIn::pause(true);
        Display::beginUpdateScreen("Downloading update", UPDATE_SUB);
        otaFree();
        otaErr = "";
        // The multipart body is a little longer than the image, so this always fits.
        otaCap = server.clientContentLength();
        otaBuf = otaCap ? (uint8_t*)heap_caps_malloc(otaCap, MALLOC_CAP_SPIRAM) : nullptr;
        otaOk = otaBuf != nullptr;
        if (!otaOk) otaErr = "no room to download";
        Serial.printf("[FL-PD] OTA download of %u bytes starts at %lu ms\n", (unsigned)otaCap, millis());
      } else if (up.status == UPLOAD_FILE_WRITE) {
        if (otaOk && otaLen + up.currentSize <= otaCap) {
          memcpy(otaBuf + otaLen, up.buf, up.currentSize);
          if ((otaLen + up.currentSize) / 131072 != otaLen / 131072)
            Serial.printf("[FL-PD] OTA download %u KB at %lu ms\n", (unsigned)((otaLen + up.currentSize) / 1024), millis());
          otaLen += up.currentSize;
        } else if (otaOk) {
          otaOk = false;
          otaErr = "upload bigger than announced";
        }
        otaProgress(otaLen, otaCap, "");
      } else if (up.status == UPLOAD_FILE_ABORTED) {
        Serial.printf("[FL-PD] OTA upload aborted at %u of %u bytes, %lu ms\n", (unsigned)otaLen, (unsigned)otaCap, millis());
        otaOk = false;
        otaErr = "upload aborted";
        otaFree();
        Display::endUpdateScreen();
        CheckIn::pause(false);
      }
    });

  server.begin();
  begun_ = true;
}

void WebUi::loop() {
  if (!begun_) return;
  server.handleClient();
  if (pullUrl.length() && !rebootAt) {
    String url = pullUrl;
    pullUrl = "";
    otaPull(url);
  }
  if (rebootAt && millis() > rebootAt) ESP.restart();
}

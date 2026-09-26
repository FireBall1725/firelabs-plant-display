// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
// FireLabs Plant Display.
// Onboarding (wifi, captive portal, config storage, factory reset) is handled by the
// shared FirelabsCore library. This file wires the panel, the HA check-in, and the
// screens together. Unlike the weather display this one is mains powered and always
// on, so it polls instead of sleeping.
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <sys/time.h>
#include <errno.h>
#include "Branding.h"
#include "CheckIn.h"
#include "Config.h"
#include "Display.h"
#include "Screen.h"
#include "Ui.h"
#include "WebUi.h"
#include "FirelabsCore.h"

// LVGL renders on the loop task. Drawing the detail card with the water-now pulse (a
// clipped, rounded card under animated edges) overflowed Arduino's default 8 KB stack
// ("Stack canary watchpoint triggered (loopTask)") the moment a thirsty plant was opened.
SET_LOOP_TASK_STACK_SIZE(16 * 1024);
#ifdef FL_DEMO
#include "DemoBundle.h"
#endif

static const uint32_t SETUP_RETRY_MS = 5UL * 60 * 1000;  // provisioned but wifi down

static Config cfg;
static FirelabsCore core;
static Bundle bundle;
static bool online = false;

static void startServices() {
  MDNS.begin(core.hostname().c_str());
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("firelabs", "tcp", 80);  // HA zeroconf discovery
  MDNS.addServiceTxt("firelabs", "tcp", "model", FL_MODEL);
  MDNS.addServiceTxt("firelabs", "tcp", "mac", WiFi.macAddress());
  WebUi::begin(cfg, core);
}

// HA sends its local time; the clock stores that wall-clock as if it were UTC.
// Debug builds: made-up plants after the real ones, so paging can be seen with
// fewer than four real plants. Moisture varies so the sort order shuffles.
static void padBundle(Bundle& b) {
  int n = WebUi::extraPlants();
  if (!n || b.plants.empty()) return;
  static const char* const NAMES[] = {"Pothos", "Snake plant", "Fern", "Aloe", "Peace lily",
                                      "Rosemary", "Cactus", "Mint", "Orchid"};
  static const char* const ICONS[] = {"tropical", "succulent", "fern", "succulent", "flowering",
                                      "herb", "cactus", "herb", "flowering"};
  Plant base = b.plants.back();
  for (int i = 0; i < n; i++) {
    Plant p = base;
    p.name = NAMES[i];
    p.icon = ICONS[i];
    p.species = "test plant";
    p.moisture.has = true;
    p.moisture.v = 6 + i * 9;
    b.plants.push_back(p);
  }
}

static void syncClock(time_t wallClock) {
  if (!wallClock) return;
  struct timeval tv = {wallClock, 0};
  settimeofday(&tv, nullptr);
}

static void say(const char* title, const String& body) {
  Ui::showMessage(title, body);
  for (int i = 0; i < 40; i++) {  // let the message paint before the reboot
    Display::tick();
    delay(10);
  }
}

static void factoryReset() {
  Serial.println("[FL-PD] factory reset");
  say("Resetting", "Erasing settings and restarting into setup.");
  cfg.clear();
  core.factoryReset();
}

static void onWifiAction(Ui::WifiAction a) {
  switch (a) {
    case Ui::WifiAction::Restart:
      Serial.println("[FL-PD] restart from the wifi sheet");
      say("Restarting", "");
      ESP.restart();
      break;
    case Ui::WifiAction::ChangeWifi:
      // Forget the network only: the name and the Home Assistant webhook stay.
      Serial.println("[FL-PD] forgetting wifi");
      say("Changing Wi-Fi", "Restarting into setup. The Home Assistant link is kept.");
      core.wifiSsid = "";
      core.wifiPass = "";
      core.save();
      ESP.restart();
      break;
    case Ui::WifiAction::FactoryReset:
      factoryReset();
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.printf("\n[FL-PD] FireLabs Plant Display %s\n", FL_FW_VERSION);

  // Mount (and on first boot format) LittleFS before the panel starts: a long
  // erase with the RGB panel streaming from PSRAM trips the interrupt watchdog.
  core.apPrefix = FL_AP_PREFIX;
  core.hostPrefix = FL_HOST_PREFIX;
  core.deviceNoun = "display";
  core.begin();
  cfg.load();

  if (!Display::begin()) {
    Serial.println("[FL-PD] display failed");
  }
  Ui::begin();
  Screen::begin();
  Ui::onFactoryReset(factoryReset);
  Ui::onWifiAction(onWifiAction);
  Ui::onLampTap([]() { CheckIn::sendAction("toggle_light"); });
  Ui::showMessage("Starting", "");
  Display::tick();

#ifdef FL_DEMO
  // Demo build: no wifi, a fixed bundle, so the screens can be checked on the panel.
  if (parseBundle(DEMO_BUNDLE, bundle)) {
    syncClock(bundle.updated);
    Ui::setBundle(bundle);
  }
  return;
#endif

  // No core.enableButton(0): GPIO0 is a panel data line on this board. Reset is a
  // tap on the signal bars, a 10 s hold on the clock, or the web UI.

  if (!core.hasWifi()) {
    Serial.println("[FL-PD] no wifi, setup portal");
    Ui::showMessage("Set up Wi-Fi",
                    "On your phone, join the Wi-Fi network\n" + core.apSsid() +
                        "\nthen open http://192.168.4.1 if the setup page doesn't appear.");
    Display::tick();
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      switch (event) {
        case ARDUINO_EVENT_WIFI_AP_START: Serial.println("[FL-PD] AP started"); break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED: Serial.println("[FL-PD] phone joined the AP"); break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
          Serial.printf("[FL-PD] gave out " IPSTR "\n", IP2STR(&info.wifi_ap_staipassigned.ip));
          break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("[FL-PD] phone left the AP"); break;
        default: break;
      }
    });
    core.startSetupPortal(0);
    Serial.printf("[FL-PD] portal up: %s at %s\n", core.apSsid().c_str(),
                  WiFi.softAPIP().toString().c_str());
    return;
  }

  Ui::showMessage("Connecting", "Joining " + core.wifiSsid + "...");
  Display::tick();
  if (!core.connect(20000)) {
    Serial.println("[FL-PD] wifi failed, setup portal with retry");
    Ui::showMessage("Can't join Wi-Fi",
                    "Couldn't reach " + core.wifiSsid + ". To change networks, join\n" +
                        core.apSsid() + "\nand open http://192.168.4.1. Retrying in 5 minutes.");
    Display::tick();
    core.startSetupPortal(SETUP_RETRY_MS);
    return;
  }
  online = true;
  // Mains powered: no modem sleep. With it on (Arduino's default) every packet waits
  // for a beacon, pings ran 30-250 ms and OTA downloads crawled or stalled.
  WiFi.setSleep(false);
  Serial.printf("[FL-PD] online %s\n", WiFi.localIP().toString().c_str());
  startServices();

  if (!cfg.isConfigured()) {
    Ui::showMessage("Add me to Home Assistant",
                    "Add this display with the FireLabs integration, then paste the check-in "
                    "URL from its options into\nhttp://" + core.hostname() + ".local  (" +
                        WiFi.localIP().toString() + ")");
    return;
  }

  Ui::showMessage("Waiting for Home Assistant", "Checking in with " + cfg.webhookUrl);
  CheckIn::begin(cfg.webhookUrl);
}

void loop() {
#ifdef FL_DEBUG_PORTAL
  static uint32_t beatMs = 0, loops = 0;
  loops++;
  if (millis() - beatMs > 5000) {
    uint32_t frames, skips;
    Display::stats(frames, skips);
    Serial.printf("[FL-PD] heartbeat: %lu loops/5s, %lu frames, %lu skipped fills, wifi %s, heap %u, stack free %u\n",
                  (unsigned long)loops, (unsigned long)frames, (unsigned long)skips,
                  WiFi.isConnected() ? "sta" : (WiFi.getMode() & WIFI_AP ? "ap" : "off"), ESP.getFreeHeap(),
                  (unsigned)uxTaskGetStackHighWaterMark(nullptr));
    beatMs = millis();
    loops = 0;
  }
  // One-shot self test: can anything reach the portal's web server over TCP?
  static bool selfTested = false;
  static WiFiClient probe;
  if (!selfTested && !online && millis() > 8000 && WiFi.getMode() & WIFI_AP) {
    selfTested = true;
    errno = 0;
    bool ok = probe.connect(WiFi.softAPIP(), 80, 2000);
    Serial.printf("[FL-PD] self-test connect to %s:80 -> %s (errno %d %s)\n",
                  WiFi.softAPIP().toString().c_str(), ok ? "connected" : "FAILED", errno,
                  strerror(errno));
    WiFiClient probe53;
    Serial.printf("[FL-PD] self-test tcp 127.0.0.1:80 -> %s\n",
                  probe53.connect(IPAddress(127, 0, 0, 1), 80, 1000) ? "connected" : "FAILED");
    if (ok) probe.print("GET /selftest HTTP/1.0\r\nHost: 192.168.4.1\r\n\r\n");
  }
  if (probe.connected() && probe.available()) {
    String line = probe.readStringUntil('\n');
    Serial.printf("[FL-PD] self-test reply: %s\n", line.c_str());
    probe.stop();
  }
#endif
  core.loop();
  WebUi::loop();

  String showcase;
  if (WebUi::takeDebugBundle(showcase) && parseBundle(showcase, bundle)) {
    syncClock(bundle.updated);
    Screen::setLightSleep(false);
    Ui::setBundle(bundle);
  }
  Bundle fresh;
  if (online && CheckIn::take(fresh) && !WebUi::bundleFrozen()) {
    bundle = std::move(fresh);
    syncClock(bundle.updated);
    Screen::setLightSleep(bundle.screenFollowsLight && bundle.hasLight && !bundle.lightOn);
    if (bundle.hasDisplay) Screen::apply(bundle.display, true);
    padBundle(bundle);
    Ui::setBundle(bundle);
  }
  if (online && CheckIn::everOk()) {
    uint32_t window = max<uint32_t>(3UL * bundle.pollSec * 1000, 180000);
    Ui::setOffline(millis() - CheckIn::lastOkMs() > window);
  }

  Ui::tick();
  Screen::tick();
  if (Screen::takeChanged() && online) CheckIn::requestNow();  // a touch turned the backlight on
  Display::tick();
  delay(5);
}

// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "CheckIn.h"
#include "Branding.h"
#include "Screen.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

namespace {
  String url_;
  TaskHandle_t task_ = nullptr;
  SemaphoreHandle_t lock_ = nullptr;
  Bundle* pending_ = nullptr;
  volatile uint16_t pollSec_ = 60;
  volatile bool everOk_ = false;
  volatile uint32_t lastOkMs_ = 0;
  volatile int lastCode_ = 0;
  String action_;  // guarded by lock_

  bool post(const char* wake, const String& action, Bundle& out, int& code) {
    String body;
    {
      JsonDocument d;
      if (action.length()) d["action"] = action;
      d["version"] = FL_FW_VERSION;
      d["wake"] = wake;
      d["rssi"] = WiFi.RSSI();
      d["uptime"] = (uint32_t)(millis() / 1000);
      d["backlight"] = Screen::backlightWanted();
      serializeJson(d, body);
    }

    // https works (setInsecure: encrypted, no CA pin), same as the weather display.
    bool isHttps = url_.startsWith("https://") || url_.startsWith("HTTPS://");
    WiFiClient plain;
    WiFiClientSecure secure;
    WiFiClient* client = &plain;
    if (isHttps) {
      secure.setInsecure();
      client = &secure;
    }

    HTTPClient http;
    if (!http.begin(*client, url_)) {
      code = -1;
      return false;
    }
    http.setTimeout(isHttps ? 12000 : 8000);
    http.addHeader("Content-Type", "application/json");
    code = http.POST(body);
    bool ok = code == 200 && parseBundle(http.getString(), out);
    http.end();
    return ok;
  }

  volatile bool paused_ = false, busy_ = false;

  void run(void*) {
    const char* wake = "boot";
    for (;;) {
      busy_ = !paused_ && WiFi.isConnected();
      if (busy_) {
        xSemaphoreTake(lock_, portMAX_DELAY);
        String action = action_;
        action_ = "";
        xSemaphoreGive(lock_);
        Bundle* b = new Bundle();
        int code = 0;
        bool ok = post(wake, action, *b, code);
        lastCode_ = code;
        if (ok) {
          pollSec_ = b->pollSec;
          xSemaphoreTake(lock_, portMAX_DELAY);
          delete pending_;
          pending_ = b;
          xSemaphoreGive(lock_);
          everOk_ = true;
          lastOkMs_ = millis();
          wake = "poll";
        } else {
          delete b;
          Serial.printf("[FL-PD] check-in failed http=%d\n", code);
        }
      }
      busy_ = false;
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS((uint32_t)pollSec_ * 1000));
    }
  }
}

void CheckIn::begin(const String& url) {
  url_ = url;
  lock_ = xSemaphoreCreateMutex();
  // TLS needs a deep stack; core 0 keeps the network off the LVGL core.
  xTaskCreatePinnedToCore(run, "checkin", 16384, nullptr, 1, &task_, 0);
}

void CheckIn::requestNow() {
  if (task_) xTaskNotifyGive(task_);
}

void CheckIn::sendAction(const char* action) {
  if (!lock_) return;
  xSemaphoreTake(lock_, portMAX_DELAY);
  action_ = action;
  xSemaphoreGive(lock_);
  requestNow();
}

bool CheckIn::take(Bundle& out) {
  if (!lock_) return false;
  Bundle* b = nullptr;
  xSemaphoreTake(lock_, portMAX_DELAY);
  b = pending_;
  pending_ = nullptr;
  xSemaphoreGive(lock_);
  if (!b) return false;
  out = std::move(*b);
  delete b;
  return true;
}

bool CheckIn::everOk() { return everOk_; }

void CheckIn::pause(bool on) {
  paused_ = on;
  if (!on) return;
  for (uint32_t t0 = millis(); busy_ && millis() - t0 < 15000;) delay(20);
}
uint32_t CheckIn::lastOkMs() { return lastOkMs_; }
int CheckIn::lastCode() { return lastCode_; }

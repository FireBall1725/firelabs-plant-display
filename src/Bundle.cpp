// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "Bundle.h"
#include <ArduinoJson.h>
#include <math.h>
#include <mbedtls/base64.h>

// Days since 1970-01-01 for a proleptic Gregorian date (Howard Hinnant's algorithm).
static int64_t daysFromCivil(int y, unsigned m, unsigned d) {
  y -= m <= 2;
  const int64_t era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = (unsigned)(y - era * 400);
  const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + (int64_t)doe - 719468;
}

time_t parseLocalTime(const char* iso) {
  if (!iso) return 0;
  int y, mo, d, h = 0, mi = 0, s = 0;
  int n = sscanf(iso, "%d-%d-%dT%d:%d:%d", &y, &mo, &d, &h, &mi, &s);
  if (n < 3) return 0;
  return (time_t)(daysFromCivil(y, mo, d) * 86400 + h * 3600 + mi * 60 + s);
}

static void reading(JsonObjectConst p, const char* key, Reading& out) {
  JsonVariantConst v = p[key];
  if (v.isNull()) return;
  out.has = true;
  out.v = v.as<float>();
  String ageKey = String(key) + "_age";
  out.age = p[ageKey] | 0;
}

bool parseBundle(const String& json, Bundle& out) {
  JsonDocument doc;
  if (deserializeJson(doc, json)) return false;
  out = Bundle{};
  out.receivedMs = millis();
  out.updated = parseLocalTime(doc["updated"] | "");

  JsonObjectConst light = doc["light"];
  if (!light.isNull()) {
    out.hasLight = !light["on"].isNull();
    out.lightOn = light["on"] | false;
    out.hasPower = !light["power"].isNull();
    out.power = light["power"] | 0.0f;
  }

  for (JsonObjectConst p : doc["plants"].as<JsonArrayConst>()) {
    Plant pl;
    pl.name = (const char*)(p["name"] | "");
    pl.species = (const char*)(p["species"] | "");
    pl.sensor = (const char*)(p["sensor"] | "");
    pl.icon = (const char*)(p["icon"] | "");
    const char* mask = p["icon_mask"] | "";
    if (*mask) {
      size_t n = 0;
      pl.mask.resize(MARK_PX * MARK_PX);
      int err = mbedtls_base64_decode(pl.mask.data(), pl.mask.size(), &n,
                                      (const unsigned char*)mask, strlen(mask));
      if (err || n != pl.mask.size()) pl.mask.clear();
    }
    reading(p, "moisture", pl.moisture);
    reading(p, "temp", pl.temp);
    reading(p, "lux", pl.lux);
    reading(p, "ec", pl.ec);
    reading(p, "battery", pl.battery);
    pl.hasFloor = !p["floor"].isNull();
    pl.floor = p["floor"] | 0.0f;
    pl.hasCeiling = !p["ceiling"].isNull();
    pl.ceiling = p["ceiling"] | 0.0f;
    pl.watered = parseLocalTime(p["watered"] | "");
    pl.historyStart = parseLocalTime(p["history_start"] | "");
    JsonArrayConst h = p["history"];
    pl.history.reserve(h.size());
    for (JsonVariantConst v : h) pl.history.push_back(v.isNull() ? NAN : v.as<float>());
    out.plants.push_back(std::move(pl));
  }

  JsonObjectConst s = doc["settings"];
  out.pollSec = s["poll_sec"] | 60;
  if (out.pollSec < 15) out.pollSec = 15;
  out.screenFollowsLight = s["screen_follows_light"] | false;
  JsonObjectConst ds = s["display"];
  if (!ds.isNull()) {
    out.hasDisplay = true;
    out.display.backlight = ds["backlight"] | true;
    out.display.brightness = ds["brightness"] | 100;
    out.display.autoDim = ds["auto_dim"] | false;
    out.display.dimAfterSec = ds["dim_after"] | 60;
    out.display.dimLevel = ds["dim_level"] | 30;
  }

  out.otaVersion = (const char*)(doc["ota"]["version"] | "");
  out.otaUrl = (const char*)(doc["ota"]["url"] | "");
  return true;
}

bool parseDisplaySettings(const String& json, DisplaySettings& out) {
  JsonDocument d;
  if (deserializeJson(d, json)) return false;
  bool any = false;
  if (d["backlight"].is<bool>()) { out.backlight = d["backlight"]; any = true; }
  if (d["brightness"].is<int>()) { out.brightness = d["brightness"]; any = true; }
  if (d["auto_dim"].is<bool>()) { out.autoDim = d["auto_dim"]; any = true; }
  if (d["dim_after"].is<int>()) { out.dimAfterSec = d["dim_after"]; any = true; }
  if (d["dim_level"].is<int>()) { out.dimLevel = d["dim_level"]; any = true; }
  return any;
}

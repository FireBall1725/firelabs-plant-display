// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "Config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* Config::kPath = "/config.json";

bool Config::load() {
  LittleFS.begin(true);
  File f = LittleFS.open(kPath, "r");
  if (!f) return false;
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return false;
  webhookUrl = doc["webhook"] | "";
  return true;
}

bool Config::save() const {
  LittleFS.begin(true);
  JsonDocument doc;
  doc["webhook"] = webhookUrl;
  File f = LittleFS.open(kPath, "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();
  return true;
}

void Config::clear() {
  LittleFS.begin(true);
  LittleFS.remove(kPath);
}

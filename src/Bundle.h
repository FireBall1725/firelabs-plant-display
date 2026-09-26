// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
#include <vector>

// The plant bundle the device gets back from the HA webhook on each check-in.
// The field names here are the contract; the firelabs-hass "PD" model fills them.
// Times are local wall-clock seconds (HA's local time read as if it were UTC),
// the same trick the weather display uses, so no timezone table on the device.

constexpr int MARK_PX = 34;

struct Reading {
  bool has = false;
  float v = 0;
  uint32_t age = 0;  // seconds since the sensor last reported, as of Bundle::updated
};

struct Plant {
  String name;
  String species;
  String sensor;  // label for the sensor device, e.g. "Plant Sensor 6C31"
  String icon;    // plant mark: succulent, herb, seedling, cactus, fern, flowering, tropical
  std::vector<uint8_t> mask;  // an uploaded mark: MARK_PX x MARK_PX A8, empty for the built-ins
  Reading moisture, temp, lux, ec, battery;
  bool hasFloor = false, hasCeiling = false;
  float floor = 0, ceiling = 0;
  time_t watered = 0;          // 0 = unknown
  time_t historyStart = 0;     // start of the first hourly slot
  std::vector<float> history;  // hourly mean moisture, NAN where HA has none
};

// Screen controls from the HA entities (backlight switch, brightness, auto dim).
struct DisplaySettings {
  bool backlight = true;
  uint8_t brightness = 100;   // percent; software dimming, the backlight is on/off only
  bool autoDim = false;
  uint16_t dimAfterSec = 60;
  uint8_t dimLevel = 30;      // percent brightness while dimmed
};

struct Bundle {
  time_t updated = 0;
  uint32_t receivedMs = 0;  // millis() when parsed, to age readings between check-ins
  bool hasLight = false, lightOn = false;
  bool hasPower = false;
  float power = 0;
  std::vector<Plant> plants;
  uint16_t pollSec = 60;
  bool screenFollowsLight = false;
  bool hasDisplay = false;
  DisplaySettings display;
  String otaVersion, otaUrl;
};

bool parseBundle(const String& json, Bundle& out);

// Merges whichever DisplaySettings fields the JSON carries into `out`; false if none.
bool parseDisplaySettings(const String& json, DisplaySettings& out);

// "2026-09-25T16:31:00" -> wall-clock seconds; 0 if unparseable.
time_t parseLocalTime(const char* iso);

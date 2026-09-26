// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
#include <vector>
#include "Bundle.h"

// Everything the screens say about a plant that isn't a raw reading: status,
// trend, forecast, and staleness. Pure logic, no LVGL.
namespace Analysis {

enum class Status { Ok, Warn, Bad };
enum class Trend { Unknown, Falling, Steady, Rising, FlatWeek };

constexpr uint32_t STALE_S = 2 * 3600;          // a reading older than this dims
constexpr uint32_t BATTERY_STALE_S = 7 * 86400; // battery is only sent every few days
constexpr float SOON_DAYS = 3;                  // forecast inside this -> Warn

struct Derived {
  Status status = Status::Ok;
  Trend trend = Trend::Unknown;
  float ratePerDay = 0;       // moisture points per day over the last 3 days
  float daysToFloor = NAN;    // only when falling toward a known floor
  float lo48 = NAN, hi48 = NAN;
  float weekMin = NAN, weekMax = NAN;
  float delta3d = NAN;        // change over the last 72 hours
  bool moistureStale = false, ecStale = false, batteryStale = false;
  uint32_t seenAge = UINT32_MAX;  // newest report across the plant's sensors
};

// `extraAge` is how many seconds have passed since the bundle was built.
Derived derive(const Plant& p, uint32_t extraAge);

// Indices into `plants`, most in need first.
std::vector<int> byNeed(const std::vector<Plant>& plants, const std::vector<Derived>& d);

}

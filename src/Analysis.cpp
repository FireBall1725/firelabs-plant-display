// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "Analysis.h"
#include <algorithm>
#include <math.h>

namespace Analysis {

// Mean of the non-missing slots in [from, to).
static float meanOf(const std::vector<float>& h, int from, int to) {
  float sum = 0;
  int n = 0;
  for (int i = std::max(from, 0); i < std::min(to, (int)h.size()); i++) {
    if (!isnan(h[i])) {
      sum += h[i];
      n++;
    }
  }
  return n ? sum / n : NAN;
}

static void rangeOf(const std::vector<float>& h, int from, float& lo, float& hi) {
  lo = NAN;
  hi = NAN;
  for (int i = std::max(from, 0); i < (int)h.size(); i++) {
    if (isnan(h[i])) continue;
    if (isnan(lo) || h[i] < lo) lo = h[i];
    if (isnan(hi) || h[i] > hi) hi = h[i];
  }
}

Derived derive(const Plant& p, uint32_t extraAge) {
  Derived d;
  const std::vector<float>& h = p.history;
  const int n = h.size();

  auto age = [&](const Reading& r) { return r.age + extraAge; };
  for (const Reading* r : {&p.moisture, &p.temp, &p.lux, &p.ec}) {
    if (r->has) d.seenAge = std::min(d.seenAge, age(*r));
  }
  d.moistureStale = p.moisture.has && age(p.moisture) > STALE_S;
  d.ecStale = p.ec.has && age(p.ec) > STALE_S;
  d.batteryStale = p.battery.has && age(p.battery) > BATTERY_STALE_S;

  rangeOf(h, 0, d.weekMin, d.weekMax);
  rangeOf(h, n - 48, d.lo48, d.hi48);

  // Slope from the mean of the last 6 hours against the 6 hours around 72 h ago:
  // wide enough windows that the daily dew wobble averages out.
  float now = meanOf(h, n - 6, n);
  float then = meanOf(h, n - 75, n - 69);
  if (!isnan(now) && !isnan(then)) {
    d.delta3d = now - then;
    d.ratePerDay = d.delta3d / 3.0f;
  }

  float current = p.moisture.has ? p.moisture.v : now;
  if (!isnan(d.weekMin) && d.weekMax - d.weekMin <= 3) {
    d.trend = Trend::FlatWeek;
  } else if (isnan(d.delta3d)) {
    d.trend = Trend::Unknown;
  } else if (d.ratePerDay < -1.0f) {
    d.trend = Trend::Falling;
    if (p.hasFloor && current > p.floor) d.daysToFloor = (current - p.floor) / -d.ratePerDay;
  } else if (d.ratePerDay > 1.0f) {
    d.trend = Trend::Rising;
  } else {
    d.trend = Trend::Steady;
  }

  if (p.hasFloor && p.moisture.has && p.moisture.v < p.floor) {
    d.status = Status::Bad;
  } else if (!isnan(d.daysToFloor) && d.daysToFloor < SOON_DAYS) {
    d.status = Status::Warn;
  }
  return d;
}

std::vector<int> byNeed(const std::vector<Plant>& plants, const std::vector<Derived>& d) {
  std::vector<int> order(plants.size());
  for (size_t i = 0; i < order.size(); i++) order[i] = i;
  auto rank = [&](int i) {
    switch (d[i].status) {
      case Status::Bad: return 0;
      case Status::Warn: return 1;
      default: return 2;
    }
  };
  std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
    if (rank(a) != rank(b)) return rank(a) < rank(b);
    if (d[a].status == Status::Warn) return d[a].daysToFloor < d[b].daysToFloor;
    float ma = plants[a].moisture.has ? plants[a].moisture.v : 1e9f;
    float mb = plants[b].moisture.has ? plants[b].moisture.v : 1e9f;
    return ma < mb;
  });
  return order;
}

}

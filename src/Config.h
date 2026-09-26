// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>

// Device settings only. WiFi credentials and the friendly name live in
// FirelabsCore; everything else (plants, thresholds, poll interval) comes from HA.
class Config {
public:
  String webhookUrl;

  bool load();
  bool save() const;
  void clear();
  bool isConfigured() const { return webhookUrl.length() > 0; }

private:
  static const char* kPath;
};

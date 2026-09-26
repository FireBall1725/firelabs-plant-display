// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
class Config;
class FirelabsCore;

// Device config web UI (online): check-in URL + OTA + reset. WiFi onboarding is
// the FireLabs core's job, not this.
namespace WebUi {
void begin(Config& cfg, FirelabsCore& core);
void loop();
int extraPlants();
// Fetch firmware from url and install it (runs from loop()).
void startOta(const String& url);  // debug builds: test plants to append to each bundle
}

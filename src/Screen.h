// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
#include "Bundle.h"

// Screen power and brightness: the HA backlight switch, software brightness, auto dim,
// and "screen follows the grow light". The backlight on this board is on/off only
// (a CH422G pin), so brightness and dimming are a black overlay on LVGL's top layer.
// A touch on a dark or dimmed screen only wakes it; if HA had switched the backlight
// off, the touch turns it back on and HA hears about it on the next check-in.
namespace Screen {
void begin();
void apply(const DisplaySettings& s, bool fromBundle);
const DisplaySettings& settings();
void setLightSleep(bool enabled);  // screen follows the light, and the light is off
void tick();

// True once after a touch changed a setting HA should hear about.
bool takeChanged();
bool backlightWanted();  // for check-in telemetry (safe to read from the check-in task)
}

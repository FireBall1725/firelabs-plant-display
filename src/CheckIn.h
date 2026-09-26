// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
#include "Bundle.h"

// Background check-in (SPEC.md "Data transport"): a task on core 0 POSTs telemetry
// to the HA webhook every poll interval and parses the plant bundle out of the
// response, so a slow network never stalls LVGL on core 1.
namespace CheckIn {
void begin(const String& url);
void requestNow();

// Sends `action` (e.g. "toggle_light") with an immediate check-in.
void sendAction(const char* action);

// Hands over the newest bundle if one arrived since the last call.
bool take(Bundle& out);

bool everOk();

// Stops check-ins (e.g. during OTA); pausing waits for one already in flight.
void pause(bool on);
uint32_t lastOkMs();
int lastCode();
}

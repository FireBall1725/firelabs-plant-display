// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
#include <functional>
#include "Bundle.h"

// The two screens from the approved mockup (overview and detail) plus a plain
// message screen for setup and errors. LVGL calls only; run from loop().
namespace Ui {
void begin();
void showMessage(const char* title, const String& body);
void setBundle(const Bundle& b);
void setOffline(bool offline);
void tick();

// Holding the clock for 10 s calls this (the board's BOOT button is a panel data line).
void onFactoryReset(std::function<void()> cb);

// The signal bars open a sheet with Restart, Change Wi-Fi (forget the network, keep
// the HA link) and Factory reset; the last two take a confirming second tap.
enum class WifiAction { Restart, ChangeWifi, FactoryReset };
void onWifiAction(std::function<void(WifiAction)> cb);

// Tapping the grow light pill flips it on screen at once, then calls this.
void onLampTap(std::function<void()> cb);
}

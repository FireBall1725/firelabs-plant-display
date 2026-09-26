// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>
#include <lvgl.h>

// Panel, touch, and backlight bring-up, plus the LVGL display and input device.
namespace Display {
  constexpr int WIDTH = 800;
  constexpr int HEIGHT = 480;

  // Brings up the IO expander, panel, touch, and LVGL. Returns false if the panel failed.
  bool begin();

  // Runs LVGL timers and the screen-sleep check; call from loop().
  void tick();

  void setBacklight(bool on);
  bool touchFound();

  // True while the backlight is off (animations skip redraws then).
  bool isAsleep();

  // While set, the next touch only calls the wake callback; it isn't passed on to
  // the UI, so tapping a dark or dimmed screen never opens a card by accident.
  void setWakeGuard(bool on);
  void onWake(void (*cb)());

  // Firmware update screen, drawn by the refresh ISR from internal RAM so it holds
  // still through flash writes. LVGL pauses until endUpdateScreen().
  void beginUpdateScreen(const char* title, const char* sub);
  void setUpdateText(const char* title, const char* sub);  // swap the text while it's up
  void setUpdateStatus(const char* text);  // the line under the bar, e.g. "70% · 1.2 of 1.7 MB"
  void setUpdateProgress(uint32_t done, uint32_t total);
  bool updating();
  // RGB565 copy source of what's on screen; free it with heap_caps_free if owned.
  uint8_t* captureFrame(bool& owned);
  void endUpdateScreen();
  // Debug: while the update screen is up, the refresh ISR prints stage/arg ~4x a second.
  void trace(uint32_t stage, uint32_t arg);
  // Debug: stops the RGB panel for good (restart to get it back).
  void stopPanel();

  // Frames scanned out and bounce fills skipped (cache off) since the last call.
  void stats(uint32_t &frames, uint32_t &skips);
  String debugJson();
}

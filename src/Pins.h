// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <Arduino.h>

// Waveshare ESP32-S3-Touch-LCD-4.3 (non-B). GPIO0 is a panel data line, not a free button.

namespace Pins {
  constexpr int I2C_SDA = 8;
  constexpr int I2C_SCL = 9;
  constexpr int TOUCH_INT = 4;

  constexpr int LCD_HSYNC = 46;
  constexpr int LCD_VSYNC = 3;
  constexpr int LCD_DE = 5;
  constexpr int LCD_PCLK = 7;

  // esp_lcd order: B3..B7, G2..G7, R3..R7
  constexpr int LCD_DATA[16] = {
    14, 38, 18, 17, 10,
    39, 0, 45, 48, 47, 21,
    1, 2, 42, 41, 40,
  };
}

// CH422G output bits (EXIO0..EXIO7)
namespace Exio {
  constexpr uint8_t TOUCH_RST = 1 << 1;
  constexpr uint8_t BACKLIGHT = 1 << 2;
  constexpr uint8_t LCD_RST = 1 << 3;
  constexpr uint8_t SD_CS = 1 << 4;
  constexpr uint8_t USB_SEL = 1 << 5;
}

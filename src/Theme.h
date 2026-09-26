// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#pragma once
#include <lvgl.h>

// Colours from The Watering Clock / the approved plant display mockup.
namespace Theme {
  inline lv_color_t ground() { return lv_color_hex(0x0e1012); }
  inline lv_color_t card()   { return lv_color_hex(0x191c1f); }
  inline lv_color_t inset()  { return lv_color_hex(0x23272b); }
  inline lv_color_t hair()   { return lv_color_hex(0x2b3036); }
  inline lv_color_t ink()    { return lv_color_hex(0xe7e9ec); }
  inline lv_color_t dim()    { return lv_color_hex(0x8a9099); }
  inline lv_color_t dimmer() { return lv_color_hex(0x5f666e); }

  // One hue per measurement.
  inline lv_color_t moist()  { return lv_color_hex(0x4f8ad6); }
  inline lv_color_t fert()   { return lv_color_hex(0x58a05c); }
  inline lv_color_t light()  { return lv_color_hex(0xdfae3c); }

  inline lv_color_t bad()    { return lv_color_hex(0xc8503f); }
  inline lv_color_t warn()   { return lv_color_hex(0xd98b39); }
  inline lv_color_t ok()     { return lv_color_hex(0x58a05c); }

  // Pill text on the tinted pill backgrounds.
  inline lv_color_t badText()  { return lv_color_hex(0xeb9184); }
  inline lv_color_t warnText() { return lv_color_hex(0xe9b276); }
  inline lv_color_t goodText() { return lv_color_hex(0x8cc08f); }
  inline lv_color_t lampText() { return lv_color_hex(0xebc673); }

  // CSS alphas as LVGL opacities.
  constexpr lv_opa_t NOTCH_OPA = 56;  // 22%
  constexpr lv_opa_t PILL_OPA = 46;   // 18%
  constexpr lv_opa_t LAMP_OPA = 41;   // 16%
}

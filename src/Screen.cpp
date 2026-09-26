// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "Screen.h"
#include "Display.h"
#include <lvgl.h>

extern uint32_t applyWakes;

namespace {
  constexpr uint32_t LIGHT_SLEEP_IDLE_MS = 60000;
  constexpr uint32_t LOCAL_WINS_MS = 15000;  // a bundle already in flight mustn't undo a touch

  DisplaySettings cur;
  volatile bool wanted = true;
  bool lightSleep = false;
  bool changed = false;
  uint32_t localChangeMs = 0;
  lv_obj_t* veil = nullptr;
  int veilLevel = -1;

  void setLevel(int percent) {
    percent = constrain(percent, 5, 100);
    if (percent == veilLevel) return;
    veilLevel = percent;
    lv_obj_set_style_bg_opa(veil, (lv_opa_t)((100 - percent) * 255 / 100), 0);
    lv_obj_set_hidden(veil, percent >= 100);
  }

  void onWake() {
    if (!cur.backlight) {
      cur.backlight = true;
      wanted = true;
      changed = true;
      localChangeMs = millis();
    }
  }
}

namespace Screen {

void begin() {
  veil = lv_obj_create(lv_layer_top());
  lv_obj_remove_style_all(veil);
  lv_obj_set_size(veil, Display::WIDTH, Display::HEIGHT);
  lv_obj_set_style_bg_color(veil, lv_color_black(), 0);
  lv_obj_set_scrollable(veil, false);
  lv_obj_set_clickable(veil, false);
  setLevel(100);
  Display::onWake(onWake);
}

void apply(const DisplaySettings& s, bool fromBundle) {
  if (fromBundle && localChangeMs && millis() - localChangeMs < LOCAL_WINS_MS) return;
  DisplaySettings next = s;
  next.brightness = constrain(next.brightness, 10, 100);
  next.dimLevel = constrain(next.dimLevel, 5, 100);
  if (next.dimAfterSec < 5) next.dimAfterSec = 5;

  // Every check-in carries the settings; unchanged ones must not count as a touch,
  // or the screen wakes once a minute after going dark with the grow light.
  bool wake = (next.backlight && !cur.backlight) || next.brightness != cur.brightness;
  cur = next;
  wanted = cur.backlight;
  if (wake) {
    applyWakes++;
    lv_display_trigger_activity(nullptr);  // show the change you just made
  }
}

const DisplaySettings& settings() { return cur; }

void setLightSleep(bool enabled) { lightSleep = enabled; }

void tick() {
  if (Display::updating()) {  // no sleeping or dimming mid-update
    Display::setBacklight(true);
    return;
  }
  uint32_t idle = lv_display_get_inactive_time(nullptr);
  bool off = !cur.backlight || (lightSleep && idle > LIGHT_SLEEP_IDLE_MS);
  bool dim = !off && cur.autoDim && idle > (uint32_t)cur.dimAfterSec * 1000;
  Display::setBacklight(!off);
  setLevel(dim ? min(cur.dimLevel, cur.brightness) : cur.brightness);
  Display::setWakeGuard(off || dim);
}

bool takeChanged() {
  bool c = changed;
  changed = false;
  return c;
}

bool backlightWanted() { return wanted; }

}

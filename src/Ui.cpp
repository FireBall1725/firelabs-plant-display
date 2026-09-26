// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "Ui.h"
#include "Analysis.h"
#include "Display.h"
#include "Fonts.h"
#include "Icons.h"
#include "Theme.h"
#include <WiFi.h>
#include <src/misc/cache/instance/lv_image_cache.h>
#include <math.h>

using namespace Analysis;

namespace {

// Geometry from the mockup, in panel pixels.
constexpr int STRIP_H = 44;
constexpr int PAD = 12;
constexpr int CARD_GAP = 10;
constexpr int CARD_H = Display::HEIGHT - STRIP_H - PAD;
constexpr int CHART_H = 170;
constexpr int DETAIL_CHART_H = 210;
constexpr int DETAIL_LEFT_W = 238;
constexpr int READING_H = 57;
constexpr int BIG_BASELINE = 52;
constexpr uint32_t DETAIL_TIMEOUT_MS = 60000;
constexpr uint32_t RESET_HOLD_MS = 10000;
constexpr uint32_t CONFIRM_MS = 4000;
constexpr uint32_t PULSE_MS = 1400;
constexpr int PER_PAGE = 3;
constexpr uint32_t PAGE_TIMEOUT_MS = 60000;

const char* const WEEKDAY[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* const MONTH[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// ---------- small builders ----------

lv_obj_t* box(lv_obj_t* parent) {
  lv_obj_t* o = lv_obj_create(parent);
  lv_obj_remove_style_all(o);
  lv_obj_set_scrollable(o, false);
  lv_obj_set_clickable(o, false);
  lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  return o;
}

void flex(lv_obj_t* o, lv_flex_flow_t flow, int gap,
          lv_flex_align_t main = LV_FLEX_ALIGN_START,
          lv_flex_align_t cross = LV_FLEX_ALIGN_START) {
  lv_obj_set_layout(o, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(o, flow);
  lv_obj_set_flex_align(o, main, cross, cross);
  lv_obj_set_style_pad_row(o, gap, 0);
  lv_obj_set_style_pad_column(o, gap, 0);
}

// Setters that skip no-op writes. LVGL redraws on every style or text set, even an
// unchanged one, and every redraw competes with the panel refill for PSRAM bandwidth;
// the 30 s refresh used to redraw the whole screen that way and made it blink.
void fill(lv_obj_t* o, lv_color_t c, lv_opa_t opa = LV_OPA_COVER) {
  if (!lv_color_eq(lv_obj_get_style_bg_color(o, LV_PART_MAIN), c)) lv_obj_set_style_bg_color(o, c, 0);
  if (lv_obj_get_style_bg_opa(o, LV_PART_MAIN) != opa) lv_obj_set_style_bg_opa(o, opa, 0);
}

void setText(lv_obj_t* l, const char* t) {
  const char* cur = lv_label_get_text(l);
  if (!cur || strcmp(cur, t) != 0) lv_label_set_text(l, t);
}

void setTextColor(lv_obj_t* o, lv_color_t c) {
  if (!lv_color_eq(lv_obj_get_style_text_color(o, LV_PART_MAIN), c)) lv_obj_set_style_text_color(o, c, 0);
}

void setRecolor(lv_obj_t* o, lv_color_t c) {
  if (!lv_color_eq(lv_obj_get_style_image_recolor(o, LV_PART_MAIN), c)) lv_obj_set_style_image_recolor(o, c, 0);
}

void setHidden(lv_obj_t* o, bool hidden) {
  if (lv_obj_has_flag(o, LV_OBJ_FLAG_HIDDEN) != hidden) lv_obj_set_hidden(o, hidden);
}

void setPos(lv_obj_t* o, int32_t x, int32_t y) {
  if (lv_obj_get_style_x(o, LV_PART_MAIN) != x || lv_obj_get_style_y(o, LV_PART_MAIN) != y) lv_obj_set_pos(o, x, y);
}

lv_obj_t* label(lv_obj_t* parent, const lv_font_t* f, lv_color_t c, const char* t = "") {
  lv_obj_t* l = lv_label_create(parent);
  lv_obj_set_style_text_font(l, f, 0);
  lv_obj_set_style_text_color(l, c, 0);
  lv_label_set_text(l, t);
  return l;
}

lv_obj_t* spans(lv_obj_t* parent) {
  lv_obj_t* g = lv_spangroup_create(parent);
  lv_obj_set_size(g, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_clickable(g, false);
  return g;
}

lv_span_t* span(lv_obj_t* g, const lv_font_t* f, lv_color_t c) {
  lv_span_t* s = lv_spangroup_add_span(g);
  lv_style_t* st = lv_span_get_style(s);
  lv_style_set_text_font(st, f);
  lv_style_set_text_color(st, c);
  return s;
}

void setSpanColor(lv_obj_t* g, lv_span_t* s, lv_color_t c) {
  lv_style_value_t v;
  if (lv_style_get_prop(lv_span_get_style(s), LV_STYLE_TEXT_COLOR, &v) == LV_STYLE_RES_FOUND &&
      lv_color_eq(v.color, c)) return;
  lv_style_set_text_color(lv_span_get_style(s), c);
  lv_spangroup_refresh(g);
}

void setSpan(lv_obj_t* g, lv_span_t* sp, const char* t) {
  const char* cur = lv_span_get_text(sp);
  if (!cur || strcmp(cur, t) != 0) lv_spangroup_set_span_text(g, sp, t);
}

lv_obj_t* icon(lv_obj_t* parent, const lv_image_dsc_t* src, lv_color_t c) {
  lv_obj_t* i = lv_image_create(parent);
  lv_image_set_src(i, src);
  lv_obj_set_style_image_recolor(i, c, 0);
  lv_obj_set_style_image_recolor_opa(i, LV_OPA_COVER, 0);
  return i;
}

// Pixels from a label's top edge to the top of a glyph, for aligning mixed sizes.
int glyphTop(const lv_font_t* f, uint32_t ch) {
  lv_font_glyph_dsc_t g;
  if (!lv_font_get_glyph_dsc(f, &g, ch, 0)) return 0;
  return (f->line_height - f->base_line) - (g.box_h + g.ofs_y);
}

// ---------- text ----------

String num(float v) {
  if (isnan(v)) return "--";
  if (fabsf(v - roundf(v)) < 0.05f) return String((int)lroundf(v));
  return String(v, 1);
}

String minus(float v, int decimals) {
  String s = String(fabsf(v), decimals);
  return (v < 0 ? "\xE2\x88\x92" : "+") + s;  // U+2212 for negatives
}

String seenText(uint32_t age) {
  if (age == UINT32_MAX) return "no reports yet";
  if (age < 60) return "seen just now";
  if (age < 3600) return "seen " + String(age / 60) + " min ago";
  if (age < 48 * 3600) return "seen " + String(age / 3600) + " h ago";
  return "seen " + String(age / 86400) + " d ago";
}

String ageShort(uint32_t age) {
  if (age < 3600) return String(age / 60) + " min";
  if (age < 48 * 3600) return String(age / 3600) + " h";
  return String(age / 86400) + " d";
}

struct tm wall(time_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);  // wall-clock seconds, see Bundle.h
  return tm;
}

String dayMonth(time_t t) {
  struct tm tm = wall(t);
  return String(tm.tm_mday) + " " + MONTH[tm.tm_mon];
}

String weekdayDayMonth(time_t t) {
  struct tm tm = wall(t);
  return String(WEEKDAY[tm.tm_wday]) + " " + tm.tm_mday + " " + MONTH[tm.tm_mon];
}

String hhmm(time_t t) {
  struct tm tm = wall(t);
  char b[6];
  snprintf(b, sizeof(b), "%02d:%02d", tm.tm_hour, tm.tm_min);
  return b;
}

String upper(String s) {
  s.toUpperCase();
  return s;
}

// The plant's chosen mark from plants-hass; older bundles without one fall back to a
// guess from the name.
const lv_image_dsc_t* plantIcon(const String& icon) {
  if (icon == "succulent") return &icon_succulent;
  if (icon == "herb") return &icon_herb;
  if (icon == "cactus") return &icon_cactus;
  if (icon == "fern") return &icon_fern;
  if (icon == "flowering") return &icon_flowering;
  if (icon == "tropical") return &icon_tropical;
  return &icon_seedling;
}

lv_color_t statusHue(Status s) {
  switch (s) {
    case Status::Bad: return Theme::bad();
    case Status::Warn: return Theme::warn();
    default: return Theme::fert();
  }
}

// ---------- one plant card (used by both screens) ----------

struct Chip {
  lv_obj_t* value;
  lv_span_t *num, *unit;
};

struct Tile {
  lv_obj_t* value;
  lv_span_t *num, *unit;
  lv_obj_t* cap;
};

struct Aside {
  lv_obj_t* g;
  lv_span_t *a, *b, *c;
};

struct CardView {
  bool detail = false;
  lv_image_dsc_t mark = {};          // the plant's uploaded mark, when it has one
  std::vector<uint8_t> markData;
  lv_obj_t *card = nullptr, *notch, *icon, *name, *sub;
  Chip chips[3];
  int nchips = 0;
  lv_obj_t *bigWrap, *big, *sup;
  Aside aside[3];
  lv_obj_t* pills;
  lv_obj_t* canvas;
  lv_draw_buf_t* buf = nullptr;
  int cw = 0, ch = 0;
  std::vector<lv_obj_t*> chartLabels;
  uint32_t chartSig = 0;
  Tile tiles[4];
  int ntiles = 0;
  lv_obj_t* facts[4];
  lv_obj_t* edges[4] = {nullptr, nullptr, nullptr, nullptr};
  bool alerting = false;
  String pillSig;
};

Chip makeChip(lv_obj_t* parent, const char* cap) {
  lv_obj_t* c = box(parent);
  flex(c, LV_FLEX_FLOW_COLUMN, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
  lv_obj_t* l = label(c, &font_b10, Theme::dimmer(), cap);
  lv_obj_set_style_text_letter_space(l, 1, 0);
  Chip chip;
  chip.value = spans(c);
  chip.num = span(chip.value, &font_rc20, Theme::ink());
  chip.unit = span(chip.value, &font_rc11, Theme::dim());
  return chip;
}

Tile makeTile(lv_obj_t* parent) {
  lv_obj_t* t = box(parent);
  lv_obj_set_flex_grow(t, 1);
  lv_obj_set_width(t, 1);
  fill(t, Theme::inset());
  lv_obj_set_style_radius(t, 8, 0);
  lv_obj_set_style_pad_hor(t, 9, 0);
  lv_obj_set_style_pad_ver(t, 7, 0);
  flex(t, LV_FLEX_FLOW_COLUMN, 3);
  Tile tile;
  tile.value = spans(t);
  tile.num = span(tile.value, &font_rc19, Theme::ink());
  tile.unit = span(tile.value, &font_rc11, Theme::dim());
  tile.cap = label(t, &font_b9, Theme::dimmer());
  lv_obj_set_style_text_letter_space(tile.cap, 1, 0);
  return tile;
}

lv_obj_t* spacer(lv_obj_t* parent) {
  lv_obj_t* s = box(parent);
  lv_obj_set_width(s, lv_pct(100));
  lv_obj_set_flex_grow(s, 1);
  return s;
}

void makeReading(CardView& cv, lv_obj_t* parent) {
  lv_obj_t* r = box(parent);
  lv_obj_set_width(r, lv_pct(100));
  lv_obj_set_style_pad_top(r, cv.detail ? 14 : 12, 0);
  lv_obj_set_style_pad_bottom(r, 8, 0);
  lv_obj_set_style_pad_hor(r, 14, 0);
  flex(r, LV_FLEX_FLOW_ROW, 12, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);

  cv.bigWrap = box(r);
  lv_obj_set_height(cv.bigWrap, READING_H);
  cv.big = label(cv.bigWrap, &font_rc62, Theme::ink(), "--");
  lv_obj_set_y(cv.big, BIG_BASELINE - (font_rc62.line_height - font_rc62.base_line));
  cv.sup = label(cv.bigWrap, &font_rc20, Theme::ink(), "%");

  lv_obj_t* a = box(r);
  lv_obj_set_style_pad_bottom(a, 3, 0);
  flex(a, LV_FLEX_FLOW_COLUMN, 4);
  for (Aside& as : cv.aside) {
    as.g = spans(a);
    as.a = span(as.g, &font_r12, Theme::dim());
    as.b = span(as.g, &font_r500_12, Theme::ink());
    as.c = span(as.g, &font_r12, Theme::dim());
  }
}

void makeChart(CardView& cv, lv_obj_t* parent, int w, int h) {
  cv.cw = w;
  cv.ch = h;
  cv.canvas = lv_canvas_create(parent);
  cv.buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_RGB565, 0);
  lv_canvas_set_draw_buf(cv.canvas, cv.buf);
  lv_canvas_fill_bg(cv.canvas, Theme::card(), LV_OPA_COVER);
  lv_obj_set_clickable(cv.canvas, false);
}

void makeCard(CardView& cv, lv_obj_t* parent, bool detail, int chartW) {
  cv.detail = detail;
  cv.card = lv_obj_create(parent);
  lv_obj_remove_style_all(cv.card);
  lv_obj_set_scrollable(cv.card, false);
  fill(cv.card, Theme::card());
  lv_obj_set_style_radius(cv.card, 12, 0);
  lv_obj_set_style_clip_corner(cv.card, true, 0);
  flex(cv.card, LV_FLEX_FLOW_COLUMN, 0);

  // Notch: an 88 px rounded square pushed 22 px up and left, so the card clips
  // it into a 66 px corner with the 22 px curve bottom-right and the card's own
  // 12 px curve top-left.
  cv.notch = box(cv.card);
  lv_obj_set_ignore_layout(cv.notch, true);
  lv_obj_set_pos(cv.notch, -22, -22);
  lv_obj_set_size(cv.notch, 88, 88);
  lv_obj_set_style_radius(cv.notch, 22, 0);
  fill(cv.notch, Theme::fert(), Theme::NOTCH_OPA);
  cv.icon = icon(cv.notch, &icon_seedling, Theme::fert());
  lv_obj_set_pos(cv.icon, 22 + 33 - 17, 22 + 33 - 17);

  lv_obj_t* band = box(cv.card);
  lv_obj_set_width(band, lv_pct(100));
  lv_obj_set_style_min_height(band, 66, 0);
  lv_obj_set_style_pad_left(band, 78, 0);
  lv_obj_set_style_pad_right(band, 14, 0);
  lv_obj_set_style_pad_ver(band, 10, 0);
  flex(band, LV_FLEX_FLOW_ROW, 10, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

  lv_obj_t* ident = box(band);
  lv_obj_set_width(ident, 1);
  lv_obj_set_flex_grow(ident, 1);
  flex(ident, LV_FLEX_FLOW_COLUMN, 1);
  cv.name = label(ident, &font_r500_17, Theme::ink());
  cv.sub = label(ident, &font_r13, Theme::dim());
  for (lv_obj_t* l : {cv.name, cv.sub}) {
    lv_obj_set_width(l, lv_pct(100));
    lv_label_set_long_mode(l, LV_LABEL_LONG_MODE_DOTS);
  }

  lv_obj_t* chips = box(band);
  flex(chips, LV_FLEX_FLOW_ROW, 14, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
  if (detail) {
    cv.chips[0] = makeChip(chips, "FLOOR");
    cv.chips[1] = makeChip(chips, "TARGET");
    cv.chips[2] = makeChip(chips, "BATTERY");
    cv.nchips = 3;
  } else {
    cv.chips[0] = makeChip(chips, "FLOOR");
    cv.nchips = 1;
  }

  lv_obj_t* body = cv.card;
  lv_obj_t* right = nullptr;
  if (detail) {
    lv_obj_t* dbody = box(cv.card);
    lv_obj_set_width(dbody, lv_pct(100));
    lv_obj_set_flex_grow(dbody, 1);
    flex(dbody, LV_FLEX_FLOW_ROW, 0);
    body = box(dbody);
    lv_obj_set_size(body, DETAIL_LEFT_W, lv_pct(100));
    lv_obj_set_style_border_side(body, LV_BORDER_SIDE_RIGHT, 0);
    lv_obj_set_style_border_width(body, 1, 0);
    lv_obj_set_style_border_color(body, Theme::hair(), 0);
    flex(body, LV_FLEX_FLOW_COLUMN, 0);
    right = box(dbody);
    lv_obj_set_height(right, lv_pct(100));
    lv_obj_set_width(right, 1);
    lv_obj_set_flex_grow(right, 1);
    flex(right, LV_FLEX_FLOW_COLUMN, 0);
  }

  makeReading(cv, body);

  cv.pills = box(body);
  lv_obj_set_width(cv.pills, lv_pct(100));
  lv_obj_set_style_pad_hor(cv.pills, 14, 0);
  lv_obj_set_style_pad_bottom(cv.pills, 10, 0);
  flex(cv.pills, LV_FLEX_FLOW_ROW_WRAP, 6);

  if (detail) {
    spacer(body);
    lv_obj_t* facts = box(body);
    lv_obj_set_width(facts, lv_pct(100));
    fill(facts, Theme::hair());
    lv_obj_set_style_pad_top(facts, 1, 0);
    flex(facts, LV_FLEX_FLOW_COLUMN, 1);
    const char* names[] = {"Week low / high", "Last 3 days", "Last watered", "Sensor"};
    for (int i = 0; i < 4; i++) {
      lv_obj_t* row = box(facts);
      lv_obj_set_width(row, lv_pct(100));
      fill(row, Theme::card());
      lv_obj_set_style_pad_ver(row, 7, 0);
      lv_obj_set_style_pad_hor(row, 14, 0);
      flex(row, LV_FLEX_FLOW_ROW, 10, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_END);
      label(row, &font_r12, Theme::dim(), names[i]);
      cv.facts[i] = label(row, &font_rc14, Theme::ink());
    }
  }

  lv_obj_t* chartParent = detail ? right : cv.card;
  makeChart(cv, chartParent, chartW, detail ? DETAIL_CHART_H : CHART_H);
  spacer(chartParent);

  lv_obj_t* grid = box(chartParent);
  lv_obj_set_width(grid, lv_pct(100));
  lv_obj_set_style_pad_top(grid, 10, 0);
  lv_obj_set_style_pad_hor(grid, 12, 0);
  lv_obj_set_style_pad_bottom(grid, 12, 0);
  flex(grid, LV_FLEX_FLOW_ROW, 7);
  cv.ntiles = detail ? 4 : 3;
  for (int i = 0; i < cv.ntiles; i++) cv.tiles[i] = makeTile(grid);

  // The water-now pulse: four 3 px strips along the edges, created last so they draw
  // over everything (the card's clip_corner rounds their ends). Strips rather than one
  // card-sized border object, because each pulse step redraws only what it covers.
  const lv_align_t where[4] = {LV_ALIGN_TOP_MID, LV_ALIGN_BOTTOM_MID, LV_ALIGN_LEFT_MID, LV_ALIGN_RIGHT_MID};
  for (int i = 0; i < 4; i++) {
    lv_obj_t* e = box(cv.card);
    lv_obj_set_ignore_layout(e, true);
    if (i < 2) lv_obj_set_size(e, lv_pct(100), 3);
    else lv_obj_set_size(e, 3, lv_pct(100));
    lv_obj_align(e, where[i], 0, 0);
    lv_obj_set_style_bg_color(e, Theme::bad(), 0);
    lv_obj_set_style_bg_opa(e, LV_OPA_TRANSP, 0);
    lv_obj_set_hidden(e, true);
    cv.edges[i] = e;
  }
}

// The overlay covers the whole card, so each change redraws the card; eight steps per
// half-pulse keeps that to ~11 redraws a second instead of every frame.
void pulseStep(void* var, int32_t v) {
  if (Display::isAsleep()) return;  // nothing to see, don't redraw
  lv_opa_t stepped = (lv_opa_t)(v / 32 * 32 + 31);
  lv_obj_t** edges = (lv_obj_t**)var;
  if (lv_obj_get_style_bg_opa(edges[0], LV_PART_MAIN) == stepped) return;
  for (int i = 0; i < 4; i++) lv_obj_set_style_bg_opa(edges[i], stepped, 0);
}

void setAlert(CardView& cv, bool on) {
  if (on == cv.alerting) return;
  cv.alerting = on;
  lv_anim_delete(cv.edges, pulseStep);
  for (lv_obj_t* e : cv.edges) lv_obj_set_hidden(e, !on);
  if (!on) return;
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, cv.edges);
  lv_anim_set_exec_cb(&a, pulseStep);
  lv_anim_set_values(&a, 40, LV_OPA_COVER);
  lv_anim_set_duration(&a, PULSE_MS / 2);
  lv_anim_set_reverse_duration(&a, PULSE_MS / 2);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_start(&a);
}

void addPill(lv_obj_t* parent, lv_color_t bg, lv_opa_t opa, lv_color_t fg,
             const String& text, bool clock = false) {
  lv_obj_t* p = box(parent);
  fill(p, bg, opa);
  lv_obj_set_style_radius(p, 5, 0);
  lv_obj_set_style_pad_hor(p, 8, 0);
  lv_obj_set_style_pad_ver(p, 3, 0);
  flex(p, LV_FLEX_FLOW_ROW, 4, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
  if (clock) icon(p, &icon_clock, fg);
  label(p, &font_r11, fg, text.c_str());
}

void setChip(lv_obj_t* g, const Chip& c, const String& value, const char* unit,
             lv_color_t color = Theme::ink()) {
  setSpan(g, c.num, value.c_str());
  setSpan(g, c.unit, unit);
  setSpanColor(g, c.num, color);
}

void setTile(const Tile& t, const String& value, const char* unit, const String& cap,
             bool stale) {
  setSpan(t.value, t.num, value.c_str());
  setSpan(t.value, t.unit, unit);
  setSpanColor(t.value, t.num, stale ? Theme::dimmer() : Theme::ink());
  setText(t.cap, cap.c_str());
  setTextColor(t.cap, stale ? Theme::warn() : Theme::dimmer());
}

void setAside(const Aside& as, const String& a, const String& b, const String& c = "") {
  setSpan(as.g, as.a, a.c_str());
  setSpan(as.g, as.b, b.c_str());
  setSpan(as.g, as.c, c.c_str());
}

// ---------- chart ----------

inline uint16_t blend565(uint16_t d, lv_color_t c, uint8_t a) {
  uint8_t dr = (d >> 11) << 3, dg = ((d >> 5) & 0x3F) << 2, db = (d & 0x1F) << 3;
  uint8_t r = (c.red * a + dr * (255 - a)) / 255;
  uint8_t g = (c.green * a + dg * (255 - a)) / 255;
  uint8_t b = (c.blue * a + db * (255 - a)) / 255;
  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

inline uint16_t to565(lv_color_t c) {
  return ((c.red >> 3) << 11) | ((c.green >> 2) << 5) | (c.blue >> 3);
}

float sampleAt(const std::vector<float>& h, float fi) {
  int i0 = (int)floorf(fi);
  int i1 = std::min(i0 + 1, (int)h.size() - 1);
  float t = fi - i0;
  float a = h[i0], b = h[i1];
  if (isnan(a)) return b;
  if (isnan(b)) return a;
  return a + (b - a) * t;
}

uint32_t chartSignature(const Plant& p, float ymax) {
  uint32_t h = 2166136261u;
  auto mix = [&](const void* data, size_t n) {
    const uint8_t* b = (const uint8_t*)data;
    for (size_t i = 0; i < n; i++) h = (h ^ b[i]) * 16777619u;
  };
  mix(p.name.c_str(), p.name.length());
  mix(p.history.data(), p.history.size() * sizeof(float));
  mix(&p.historyStart, sizeof(p.historyStart));
  mix(&p.floor, sizeof(p.floor));
  mix(&p.ceiling, sizeof(p.ceiling));
  mix(&p.watered, sizeof(p.watered));
  mix(&ymax, sizeof(ymax));
  return h;
}

lv_obj_t* chartLabel(CardView& cv, const lv_font_t* f, lv_color_t c, const String& text,
                     bool bg) {
  lv_obj_t* l = label(cv.canvas, f, c, text.c_str());
  lv_obj_set_style_text_letter_space(l, 1, 0);
  if (bg) {
    fill(l, Theme::card(), 219);
    lv_obj_set_style_radius(l, 3, 0);
    lv_obj_set_style_pad_hor(l, 5, 0);
    lv_obj_set_style_pad_ver(l, 1, 0);
  }
  cv.chartLabels.push_back(l);
  return l;
}

void renderChart(CardView& cv, const Plant& p, float ymax) {
  uint32_t sig = chartSignature(p, ymax);
  if (sig == cv.chartSig) return;
  cv.chartSig = sig;
  for (lv_obj_t* l : cv.chartLabels) lv_obj_delete(l);
  cv.chartLabels.clear();

  const int w = cv.cw, h = cv.ch;
  const std::vector<float>& hist = p.history;
  const int n = hist.size();
  auto yOf = [&](float v) { return h - 6 - (h - 14) * std::min(v, ymax) / ymax; };
  auto xOf = [&](float i) { return n > 1 ? w * i / (n - 1) : 0.0f; };
  uint8_t* data = cv.buf->data;
  const uint32_t stride = cv.buf->header.stride;
  auto px = [&](int x, int y) -> uint16_t& { return ((uint16_t*)(data + y * stride))[x]; };

  // Background, target band, day lines, dashed floor: flat fills, written directly.
  const uint16_t bg = to565(Theme::card());
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) px(x, y) = bg;

  if (p.hasFloor && p.hasCeiling) {
    int y0 = std::max(0, (int)lroundf(yOf(p.ceiling)));
    int y1 = std::min(h, (int)lroundf(yOf(p.floor)));
    for (int y = y0; y < y1; y++)
      for (int x = 0; x < w; x++) px(x, y) = blend565(px(x, y), Theme::fert(), 31);
  }

  std::vector<std::pair<int, time_t>> midnights;
  if (p.historyStart && n > 1) {
    time_t first = p.historyStart - (p.historyStart % 86400) + 86400;
    for (time_t t = first; t < p.historyStart + (time_t)n * 3600; t += 86400) {
      int x = lroundf(xOf((t - p.historyStart) / 3600.0f));
      if (x <= 0 || x >= w) continue;
      midnights.push_back({x, t});
      for (int y = 0; y < h; y++) px(x, y) = to565(Theme::hair());
    }
  }

  if (p.hasFloor) {
    int yf = lroundf(yOf(p.floor));
    if (yf >= 0 && yf < h)
      for (int x = 0; x < w; x++)
        if ((x / 4) % 2 == 0) px(x, yf) = blend565(px(x, yf), Theme::dim(), 179);
  }

  // Area under the curve, fading from 42% at the curve's peak to nothing at the base.
  std::vector<float> col(w, NAN);
  float ytop = h;
  if (n > 1) {
    for (int x = 0; x < w; x++) {
      float v = sampleAt(hist, (float)x * (n - 1) / (w - 1));
      if (isnan(v)) continue;
      col[x] = yOf(v);
      ytop = std::min(ytop, col[x]);
    }
    for (int x = 0; x < w; x++) {
      if (isnan(col[x])) continue;
      for (int y = std::max(0, (int)ceilf(col[x])); y < h; y++) {
        float a = 0.42f * (1.0f - (y - ytop) / (h - ytop));
        if (a > 0) px(x, y) = blend565(px(x, y), Theme::moist(), (uint8_t)(a * 255));
      }
    }
  }

  // The line, the end dot, and the watering marker are anti-aliased, so they go
  // through LVGL's renderer.
  lv_layer_t layer;
  lv_canvas_init_layer(cv.canvas, &layer);

  if (cv.detail && p.watered && n > 1) {
    float i = (p.watered - p.historyStart) / 3600.0f;
    if (i >= 0 && i <= n - 1) {
      lv_draw_line_dsc_t d;
      lv_draw_line_dsc_init(&d);
      d.color = Theme::ink();
      d.width = 2;
      d.opa = 230;
      float x = xOf(i);
      d.p1 = {x, 0};
      d.p2 = {x, (float)h};
      lv_draw_line(&layer, &d);
      lv_obj_t* l = chartLabel(cv, &font_b9, Theme::ink(),
                               "WATERED " + upper(WEEKDAY[wall(p.watered).tm_wday]) + " " +
                                   hhmm(p.watered),
                               true);
      lv_obj_set_pos(l, (int)x + 5, 8);
    }
  }

  lv_draw_line_dsc_t line;
  lv_draw_line_dsc_init(&line);
  line.color = Theme::moist();
  line.width = 2;
  line.round_start = 1;
  line.round_end = 1;
  int last = -1;
  for (int i = 0; i < n; i++) {
    if (isnan(hist[i])) continue;
    if (last >= 0 && i - last == 1) {
      line.p1 = {xOf(last), yOf(hist[last])};
      line.p2 = {xOf(i), yOf(hist[i])};
      lv_draw_line(&layer, &line);
    }
    last = i;
  }
  if (last >= 0) {
    lv_draw_rect_dsc_t dot;
    lv_draw_rect_dsc_init(&dot);
    dot.bg_color = Theme::moist();
    dot.radius = LV_RADIUS_CIRCLE;
    int cx = lroundf(xOf(last)), cy = lroundf(yOf(hist[last]));
    lv_area_t a = {cx - 4, cy - 4, cx + 3, cy + 3};
    lv_draw_rect(&layer, &dot, &a);
  }
  lv_canvas_finish_layer(cv.canvas, &layer);

  if (p.hasFloor) {
    lv_obj_t* l = chartLabel(cv, &font_b9, Theme::dim(), "FLOOR " + num(p.floor) + "%", true);
    lv_obj_update_layout(l);
    lv_obj_set_pos(l, w - 6 - lv_obj_get_width(l),
                   (int)yOf(p.floor) - lv_obj_get_height(l) / 2);
  }
  if (cv.detail) {
    for (auto& m : midnights) {
      lv_obj_t* l = chartLabel(cv, &font_b9, Theme::dimmer(),
                               upper(WEEKDAY[wall(m.second).tm_wday]), false);
      lv_obj_update_layout(l);
      lv_obj_set_pos(l, m.first + 3, h - 4 - lv_obj_get_height(l));
    }
  }
  lv_obj_invalidate(cv.canvas);
}

// ---------- filling a card from data ----------

void fillCard(CardView& cv, const Plant& p, const Derived& d, uint32_t extraAge, time_t now,
              float ymax) {
  lv_color_t hue = statusHue(d.status);
  fill(cv.notch, hue, Theme::NOTCH_OPA);
  const lv_image_dsc_t* mark = plantIcon(p.icon);
  if (!p.mask.empty()) {
    if (cv.markData != p.mask) {
      cv.markData = p.mask;
      cv.mark.header.magic = LV_IMAGE_HEADER_MAGIC;
      cv.mark.header.cf = LV_COLOR_FORMAT_A8;
      cv.mark.header.w = cv.mark.header.h = cv.mark.header.stride = MARK_PX;
      cv.mark.data = cv.markData.data();
      cv.mark.data_size = cv.markData.size();
      lv_image_cache_drop(&cv.mark);
      lv_image_set_src(cv.icon, &icon_seedling);  // same pointer, new pixels: force a reload
    }
    mark = &cv.mark;
  }
  if (lv_image_get_src(cv.icon) != mark) lv_image_set_src(cv.icon, mark);
  setRecolor(cv.icon, hue);

  setText(cv.name, p.name.c_str());
  String seen = seenText(d.seenAge);
  String sub = (cv.detail && p.species.length()) ? p.species + " \xC2\xB7 " + seen : seen;
  setText(cv.sub, sub.c_str());

  String floorTxt = p.hasFloor ? num(p.floor) : String("--");
  setChip(cv.chips[0].value, cv.chips[0], floorTxt, "%");
  if (cv.detail) {
    setChip(cv.chips[1].value, cv.chips[1], p.hasCeiling ? num(p.ceiling) : String("--"), "%");
    bool battOk = p.battery.has && p.battery.v >= 20;
    setChip(cv.chips[2].value, cv.chips[2], p.battery.has ? num(p.battery.v) : String("--"),
            "%", battOk ? Theme::ok() : Theme::warn());
  }

  // Hero number, with the % superscript aligned to the top of the digits.
  lv_color_t heroColor = d.status == Status::Bad ? Theme::bad()
                       : d.moistureStale         ? Theme::dim()
                                                 : Theme::ink();
  setText(cv.big, p.moisture.has ? String((int)lroundf(p.moisture.v)).c_str() : "--");
  setTextColor(cv.big, heroColor);
  setTextColor(cv.sup, heroColor);
  lv_obj_update_layout(cv.big);
  int capTop = lv_obj_get_y(cv.big) + glyphTop(&font_rc62, '0');
  setPos(cv.sup, lv_obj_get_width(cv.big) + 2, capTop - glyphTop(&font_rc20, '0'));

  // Aside: floor, trend, watered.
  if (p.hasFloor) {
    setAside(cv.aside[0], "Floor ", num(p.floor) + "%",
             p.hasCeiling ? ", target to " + num(p.ceiling) + "%" : String(""));
  } else {
    setAside(cv.aside[0], "No floor set", "");
  }
  switch (d.trend) {
    case Trend::Falling:
      setAside(cv.aside[1], "Down ", num(-d.ratePerDay >= 1.95f ? roundf(-d.ratePerDay) : -d.ratePerDay) + " pts/day");
      break;
    case Trend::Rising:
      setAside(cv.aside[1], "Up ", num(d.ratePerDay >= 1.95f ? roundf(d.ratePerDay) : d.ratePerDay) + " pts/day");
      break;
    case Trend::Steady:
      setAside(cv.aside[1], "Steady at ", num(roundf(d.lo48)) + "\xE2\x80\x93" + num(roundf(d.hi48)) + "%");
      break;
    case Trend::FlatWeek:
      setAside(cv.aside[1], "Flat at ",
               num(roundf(d.weekMin)) + "\xE2\x80\x93" + num(roundf(d.weekMax)) + "%", " all week");
      break;
    default:
      setAside(cv.aside[1], "Not enough history yet", "");
  }
  if (p.watered && now > p.watered) {
    uint32_t ago = now - p.watered;
    if (ago < 3600) setAside(cv.aside[2], "Watered ", "just now");
    else if (ago < 86400) setAside(cv.aside[2], "Watered ", String(ago / 3600) + " h", " ago");
    else if (ago < 7 * 86400) setAside(cv.aside[2], "Watered ", weekdayDayMonth(p.watered));
    else setAside(cv.aside[2], "Watered ", dayMonth(p.watered));
  } else {
    setAside(cv.aside[2], "Watering not recorded", "");
  }

  setAlert(cv, d.status == Status::Bad);

  // Pills, rebuilt only when what they'd say changes.
  struct PillSpec { int kind; String text; bool clock; };  // kind: 0 bad, 1 warn, 2 neutral
  std::vector<PillSpec> pills;
  if (d.status == Status::Bad) {
    pills.push_back({0, "Water now", false});
    if (p.watered && now > p.watered && now - p.watered >= 7 * 86400)
      pills.push_back({2, String((now - p.watered) / 86400) + " days since water", false});
  } else if (d.status == Status::Warn) {
    int days = lroundf(d.daysToFloor);
    pills.push_back({1, days < 1 ? String("Floor within a day")
                                 : "Floor in about " + String(days) + (days == 1 ? " day" : " days"),
                     false});
  }
  if (d.moistureStale) {
    pills.push_back({1, "Moisture " + ageShort(p.moisture.age + extraAge) + " old", true});
  }
  String sig;
  for (const PillSpec& ps : pills) sig += String(ps.kind) + ps.text + "|";
  if (sig != cv.pillSig) {
    cv.pillSig = sig;
    lv_obj_clean(cv.pills);
    for (const PillSpec& ps : pills) {
      if (ps.kind == 0) addPill(cv.pills, Theme::bad(), Theme::PILL_OPA, Theme::badText(), ps.text, ps.clock);
      else if (ps.kind == 1) addPill(cv.pills, Theme::warn(), Theme::PILL_OPA, Theme::warnText(), ps.text, ps.clock);
      else addPill(cv.pills, Theme::inset(), LV_OPA_COVER, Theme::dim(), ps.text, ps.clock);
    }
  }

  // Tiles.
  setTile(cv.tiles[0], p.temp.has ? String(p.temp.v, 1) : String("--"), "\xC2\xB0""C", "TEMP", false);
  setTile(cv.tiles[1], p.lux.has ? num(roundf(p.lux.v)) : String("--"), "lx", "LIGHT", false);
  String fertCap = d.ecStale ? "FERT \xC2\xB7 " + ageShort(p.ec.age + extraAge) : String("FERT");
  fertCap.replace(" min", "M");
  fertCap.replace(" h", " H");
  fertCap.replace(" d", " D");
  setTile(cv.tiles[2], p.ec.has ? num(roundf(p.ec.v)) : String("--"),
          cv.detail ? "\xC2\xB5S/cm" : "\xC2\xB5S", fertCap, d.ecStale);
  if (cv.detail) {
    String cap = "BATT";
    if (p.battery.has && now) {
      time_t at = now - (time_t)(p.battery.age + extraAge);
      struct tm tm = wall(at);
      cap += " \xC2\xB7 " + upper(WEEKDAY[tm.tm_wday]) + " " + tm.tm_mday;
    }
    setTile(cv.tiles[3], p.battery.has ? num(p.battery.v) : String("--"), "%", cap,
            d.batteryStale);

    setText(cv.facts[0], (num(d.weekMin) + " / " + num(d.weekMax) + "%").c_str());
    setText(cv.facts[1], isnan(d.delta3d) ? "--" : (minus(d.delta3d, 1) + " pts").c_str());
    setText(cv.facts[2],
                      p.watered ? (weekdayDayMonth(p.watered) + ", " + hhmm(p.watered)).c_str() : "--");
    setText(cv.facts[3], p.sensor.length() ? p.sensor.c_str() : "--");
  }

  renderChart(cv, p, ymax);
}

// ---------- screens ----------

struct Strip {
  lv_obj_t *date = nullptr, *dots = nullptr, *lamp, *lampText, *lampPower, *lampIcon, *offline, *wifi, *clock;
  lv_obj_t* bars[4];
};

// The Wi-Fi sheet: what the device is joined to, and the ways to reset it.
struct Sheet {
  lv_obj_t *veil, *ssid, *signal, *ip, *host;
  lv_obj_t *forget, *forgetText, *factory, *factoryText;
  int armed = 0;  // 0 none, 1 change wifi, 2 factory reset: waiting for the second tap
  uint32_t armedAt = 0;
};

lv_obj_t* scrOverview;
lv_obj_t* scrDetail;
lv_obj_t* scrMessage;
lv_obj_t* msgTitle;
lv_obj_t* msgBody;
Strip stripOverview, stripDetail;
lv_obj_t* cardRow;
std::vector<CardView> cards;
CardView detailCard;

Bundle bundle;
bool haveBundle = false;
std::vector<Derived> derived;
std::vector<int> order;
String detailName;
bool offline = false;
uint32_t lastTickMs = 0, lastRefreshMs = 0;
uint32_t resetPressMs = 0;
bool resetFired = false;
std::function<void()> resetCb;
std::function<void()> lampCb;
std::function<void(Ui::WifiAction)> wifiCb;
Sheet sheet;
int page = 0;
int pageDots = -1;  // dot count currently built

lv_obj_t* makeScreen() {
  lv_obj_t* s = lv_obj_create(nullptr);
  lv_obj_remove_style_all(s);
  fill(s, Theme::ground());
  lv_obj_set_scrollable(s, false);
  flex(s, LV_FLEX_FLOW_COLUMN, 0);
  return s;
}

void onClockEvent(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_PRESSED) {
    resetPressMs = millis();
    resetFired = false;
  } else if (code == LV_EVENT_PRESSING) {
    if (!resetFired && millis() - resetPressMs > RESET_HOLD_MS) {
      resetFired = true;
      if (resetCb) resetCb();
    }
  }
}

void showDetail(const String& name);
void showOverview();

void onBack(lv_event_t*) { showOverview(); }

void refresh();

int pageCount() {
  int n = bundle.plants.size();
  return n <= PER_PAGE ? 1 : (n + PER_PAGE - 1) / PER_PAGE;
}

void setPage(int p) {
  int pages = pageCount();
  p = ((p % pages) + pages) % pages;
  if (p == page) return;
  page = p;
  refresh();
}

void onDots(lv_event_t*) { setPage(page + 1); }

// Swipe left/right on the overview flips pages; the lifted finger mustn't also
// open the card it started on.
void onGesture(lv_event_t*) {
  lv_indev_t* indev = lv_indev_active();
  if (!indev) return;
  lv_dir_t dir = lv_indev_get_gesture_dir(indev);
  if (dir == LV_DIR_LEFT) setPage(page + 1);
  else if (dir == LV_DIR_RIGHT) setPage(page - 1);
  else return;
  lv_indev_wait_release(indev);
}

void onLamp(lv_event_t*) {
  if (!haveBundle || !bundle.hasLight) return;
  bundle.lightOn = !bundle.lightOn;  // show it now; the next bundle confirms
  refresh();
  if (lampCb) lampCb();
}

int signalBars(int rssi) {
  if (WiFi.status() != WL_CONNECTED) return 0;
  return rssi >= -55 ? 4 : rssi >= -65 ? 3 : rssi >= -75 ? 2 : 1;
}

const char* signalWord(int bars) {
  static const char* const WORDS[] = {"Not connected", "Weak", "Fair", "Good", "Excellent"};
  return WORDS[bars];
}

void disarm() {
  sheet.armed = 0;
  fill(sheet.forget, Theme::inset());
  setTextColor(sheet.forgetText, Theme::ink());
  setText(sheet.forgetText, "Change Wi-Fi");
  fill(sheet.factory, Theme::inset());
  setTextColor(sheet.factoryText, Theme::badText());
  setText(sheet.factoryText, "Factory reset");
}

void updateSheet() {
  if (lv_obj_has_flag(sheet.veil, LV_OBJ_FLAG_HIDDEN)) return;
  int rssi = WiFi.RSSI();
  int bars = signalBars(rssi);
  setText(sheet.ssid, WiFi.status() == WL_CONNECTED ? WiFi.SSID().c_str() : "-");
  String sig = bars ? String(signalWord(bars)) + ", " + rssi + " dBm" : String(signalWord(0));
  setText(sheet.signal, sig.c_str());
  setText(sheet.ip, WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString().c_str() : "-");
  setText(sheet.host, WiFi.getHostname() ? WiFi.getHostname() : "-");
  if (sheet.armed && millis() - sheet.armedAt > CONFIRM_MS) disarm();
}

void openSheet() {
  disarm();
  setHidden(sheet.veil, false);
  updateSheet();
}

void closeSheet() { setHidden(sheet.veil, true); }

// Destructive actions take a second tap within CONFIRM_MS.
void onSheetAction(lv_event_t* e) {
  int which = (int)(intptr_t)lv_event_get_user_data(e);
  if (which == 0) {
    if (wifiCb) wifiCb(Ui::WifiAction::Restart);
    return;
  }
  if (sheet.armed == which && millis() - sheet.armedAt <= CONFIRM_MS) {
    closeSheet();
    if (wifiCb) wifiCb(which == 1 ? Ui::WifiAction::ChangeWifi : Ui::WifiAction::FactoryReset);
    return;
  }
  disarm();
  sheet.armed = which;
  sheet.armedAt = millis();
  lv_obj_t* btn = which == 1 ? sheet.forget : sheet.factory;
  lv_obj_t* text = which == 1 ? sheet.forgetText : sheet.factoryText;
  fill(btn, Theme::bad(), Theme::PILL_OPA);
  setTextColor(text, Theme::badText());
  setText(text, "Tap again to confirm");
}

lv_obj_t* sheetRow(lv_obj_t* parent, const char* name) {
  lv_obj_t* row = box(parent);
  lv_obj_set_width(row, LV_PCT(100));
  flex(row, LV_FLEX_FLOW_ROW, 12, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER);
  label(row, &font_r14, Theme::dim(), name);
  lv_obj_t* v = label(row, &font_r500_14, Theme::ink(), "-");
  lv_obj_set_style_max_width(v, 300, 0);
  lv_label_set_long_mode(v, LV_LABEL_LONG_MODE_DOTS);
  return v;
}

lv_obj_t* sheetButton(lv_obj_t* parent, const char* text, lv_color_t ink, int which, lv_obj_t** textOut) {
  lv_obj_t* b = box(parent);
  lv_obj_set_height(b, 44);
  lv_obj_set_flex_grow(b, 1);
  lv_obj_set_style_radius(b, 8, 0);
  fill(b, Theme::inset());
  flex(b, LV_FLEX_FLOW_ROW, 0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_t* t = label(b, &font_r500_14, ink, text);
  lv_obj_set_clickable(b, true);
  lv_obj_set_style_opa(b, LV_OPA_60, LV_STATE_PRESSED);
  lv_obj_add_event_cb(b, onSheetAction, LV_EVENT_CLICKED, (void*)(intptr_t)which);
  if (textOut) *textOut = t;
  return b;
}

// Built on the top layer before Screen adds its brightness veil, so it dims with the rest.
void makeSheet() {
  sheet.veil = box(lv_layer_top());
  lv_obj_set_size(sheet.veil, Display::WIDTH, Display::HEIGHT);
  fill(sheet.veil, lv_color_black(), LV_OPA_60);
  lv_obj_set_clickable(sheet.veil, true);
  lv_obj_add_event_cb(sheet.veil, [](lv_event_t* e) {
    if (lv_event_get_target(e) == lv_event_get_current_target(e)) closeSheet();
  }, LV_EVENT_CLICKED, nullptr);

  lv_obj_t* card = box(sheet.veil);
  lv_obj_set_width(card, 520);
  lv_obj_center(card);
  fill(card, Theme::card());
  lv_obj_set_style_radius(card, 12, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, Theme::hair(), 0);
  lv_obj_set_style_pad_all(card, 22, 0);
  lv_obj_set_clickable(card, true);  // taps inside the card don't close it
  flex(card, LV_FLEX_FLOW_COLUMN, 12);

  lv_obj_t* head = box(card);
  lv_obj_set_width(head, LV_PCT(100));
  flex(head, LV_FLEX_FLOW_ROW, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER);
  label(head, &font_rc22, Theme::ink(), "Wi-Fi");
  lv_obj_t* done = label(head, &font_r500_14, Theme::moist(), "Done");
  lv_obj_set_clickable(done, true);
  lv_obj_set_ext_click_area(done, 16);
  lv_obj_add_event_cb(done, [](lv_event_t*) { closeSheet(); }, LV_EVENT_CLICKED, nullptr);

  sheet.ssid = sheetRow(card, "Network");
  sheet.signal = sheetRow(card, "Signal");
  sheet.ip = sheetRow(card, "Address");
  sheet.host = sheetRow(card, "Device");

  lv_obj_t* rule = box(card);
  lv_obj_set_size(rule, LV_PCT(100), 1);
  fill(rule, Theme::hair());

  lv_obj_t* buttons = box(card);
  lv_obj_set_width(buttons, LV_PCT(100));
  flex(buttons, LV_FLEX_FLOW_ROW, 10);
  sheetButton(buttons, "Restart", Theme::ink(), 0, nullptr);
  sheet.forget = sheetButton(buttons, "Change Wi-Fi", Theme::ink(), 1, &sheet.forgetText);
  sheet.factory = sheetButton(buttons, "Factory reset", Theme::badText(), 2, &sheet.factoryText);

  lv_obj_t* note = label(card, &font_r12, Theme::dimmer(),
                         "Change Wi-Fi keeps the Home Assistant link. Factory reset erases everything.");
  lv_obj_set_width(note, LV_PCT(100));
  lv_label_set_long_mode(note, LV_LABEL_LONG_MODE_WRAP);

  setHidden(sheet.veil, true);
}

void makeStrip(Strip& st, lv_obj_t* scr, bool detail) {
  lv_obj_t* s = box(scr);
  lv_obj_set_size(s, Display::WIDTH, STRIP_H);
  lv_obj_set_style_pad_hor(s, 14, 0);
  flex(s, LV_FLEX_FLOW_ROW, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER);

  lv_obj_t* left = box(s);
  if (detail) {
    fill(left, Theme::card());
    lv_obj_set_height(left, 30);
    lv_obj_set_style_radius(left, 6, 0);
    lv_obj_set_style_pad_left(left, 8, 0);
    lv_obj_set_style_pad_right(left, 12, 0);
    flex(left, LV_FLEX_FLOW_ROW, 6, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_clickable(left, true);
    lv_obj_add_event_cb(left, onBack, LV_EVENT_CLICKED, nullptr);
    icon(left, &icon_back, Theme::ink());
    label(left, &font_r500_14, Theme::ink(), "Plants");
  } else {
    flex(left, LV_FLEX_FLOW_ROW, 12, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
    label(left, &font_rc22, Theme::ink(), "Plants");
    st.date = label(left, &font_r14, Theme::dim());
    lv_obj_set_style_pad_bottom(st.date, 2, 0);
    st.dots = box(left);
    flex(st.dots, LV_FLEX_FLOW_ROW, 6, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_bottom(st.dots, 6, 0);
    lv_obj_set_style_pad_left(st.dots, 4, 0);
    lv_obj_set_clickable(st.dots, true);
    lv_obj_set_ext_click_area(st.dots, 14);
    lv_obj_add_event_cb(st.dots, onDots, LV_EVENT_CLICKED, nullptr);
    setHidden(st.dots, true);
  }

  lv_obj_t* right = box(s);
  flex(right, LV_FLEX_FLOW_ROW, 14, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

  st.offline = box(right);
  fill(st.offline, Theme::bad(), Theme::PILL_OPA);
  lv_obj_set_height(st.offline, 28);
  lv_obj_set_style_radius(st.offline, 6, 0);
  lv_obj_set_style_pad_hor(st.offline, 11, 0);
  flex(st.offline, LV_FLEX_FLOW_ROW, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
  label(st.offline, &font_r500_14, Theme::badText(), "Home Assistant offline");
  setHidden(st.offline, true);

  st.lamp = box(right);
  lv_obj_set_height(st.lamp, 28);
  lv_obj_set_style_radius(st.lamp, 6, 0);
  lv_obj_set_style_pad_left(st.lamp, 9, 0);
  lv_obj_set_style_pad_right(st.lamp, 11, 0);
  flex(st.lamp, LV_FLEX_FLOW_ROW, 7, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
  st.lampIcon = icon(st.lamp, &icon_bulb, Theme::lampText());
  st.lampText = label(st.lamp, &font_r500_14, Theme::lampText(), "Grow light");
  st.lampPower = label(st.lamp, &font_rc14, Theme::light(), "");
  setHidden(st.lamp, true);
  lv_obj_set_clickable(st.lamp, true);
  lv_obj_set_ext_click_area(st.lamp, 8);
  lv_obj_set_style_opa(st.lamp, LV_OPA_60, LV_STATE_PRESSED);
  lv_obj_add_event_cb(st.lamp, onLamp, LV_EVENT_CLICKED, nullptr);

  st.wifi = box(right);
  flex(st.wifi, LV_FLEX_FLOW_ROW, 2, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
  lv_obj_set_height(st.wifi, 17);
  for (int i = 0; i < 4; i++) {
    st.bars[i] = box(st.wifi);
    lv_obj_set_size(st.bars[i], 4, 5 + 4 * i);
    lv_obj_set_style_radius(st.bars[i], 1, 0);
    fill(st.bars[i], Theme::hair());
  }
  lv_obj_set_clickable(st.wifi, true);
  lv_obj_set_ext_click_area(st.wifi, 14);
  lv_obj_set_style_opa(st.wifi, LV_OPA_60, LV_STATE_PRESSED);
  lv_obj_add_event_cb(st.wifi, [](lv_event_t*) { openSheet(); }, LV_EVENT_CLICKED, nullptr);

  st.clock = label(right, &font_rc22, Theme::ink(), "--:--");
  lv_obj_set_clickable(st.clock, true);
  lv_obj_set_ext_click_area(st.clock, 12);
  lv_obj_add_event_cb(st.clock, onClockEvent, LV_EVENT_ALL, nullptr);
}

void updateStrip(Strip& st) {
  time_t now = time(nullptr);
  if (haveBundle && bundle.updated) {
    setText(st.clock, hhmm(now).c_str());
    if (st.date) {
      struct tm tm = wall(now);
      String d = String(WEEKDAY[tm.tm_wday]) + " " + tm.tm_mday + " " + MONTH[tm.tm_mon];
      setText(st.date, d.c_str());
    }
  }
  setHidden(st.offline, !offline);

  int bars = signalBars(WiFi.RSSI());
  lv_color_t on = bars == 1 ? Theme::warn() : Theme::dim();
  for (int i = 0; i < 4; i++) fill(st.bars[i], i < bars ? on : Theme::hair());

  if (st.dots) {
    int pages = haveBundle ? pageCount() : 1;
    if (pages != pageDots) {
      lv_obj_clean(st.dots);
      for (int i = 0; i < pages; i++) {
        lv_obj_t* d = box(st.dots);
        lv_obj_set_size(d, 7, 7);
        lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0);
        fill(d, Theme::dimmer());
      }
    }
    for (int i = 0; i < pages; i++)
      fill(lv_obj_get_child(st.dots, i), i == page ? Theme::ink() : Theme::dimmer());
    setHidden(st.dots, pages < 2);
  }

  bool showLamp = haveBundle && bundle.hasLight;
  setHidden(st.lamp, !showLamp);
  if (!showLamp) return;
  if (bundle.lightOn) {
    fill(st.lamp, Theme::light(), Theme::LAMP_OPA);
    setRecolor(st.lampIcon, Theme::lampText());
    setTextColor(st.lampText, Theme::lampText());
    setText(st.lampText, "Grow light");
    setText(st.lampPower,
                      bundle.hasPower ? (String((int)lroundf(bundle.power)) + " W").c_str() : "");
  } else {
    fill(st.lamp, Theme::inset());
    setRecolor(st.lampIcon, Theme::dim());
    setTextColor(st.lampText, Theme::dim());
    setText(st.lampText, "Grow light off");
    setText(st.lampPower, "");
  }
  setHidden(st.lampPower, lv_label_get_text(st.lampPower)[0] == 0);
}

float overviewYmax() {
  float ymax = 60;
  for (const Plant& p : bundle.plants) {
    float top = std::max(p.hasCeiling ? p.ceiling : 0.0f, 0.0f);
    for (float v : p.history)
      if (!isnan(v)) top = std::max(top, v);
    ymax = std::max(ymax, ceilf(top / 10) * 10);
  }
  return ymax;
}

float detailYmax(const Plant& p, const Derived& d) {
  float top = std::max(p.hasCeiling ? p.ceiling : 0.0f, isnan(d.weekMax) ? 0.0f : d.weekMax);
  return std::max(20.0f, ceilf(top / 10) * 10);
}

void onCardClick(lv_event_t* e) {
  int slot = page * PER_PAGE + (int)(intptr_t)lv_event_get_user_data(e);
  if (slot < (int)order.size()) showDetail(bundle.plants[order[slot]].name);
}

int cardSlots() {
  return std::max(1, std::min((int)bundle.plants.size(), PER_PAGE));
}

void buildCards() {
  for (CardView& c : cards) lv_anim_delete(c.edges, pulseStep);  // anims point into cards
  lv_obj_clean(cardRow);
  cards.clear();
  cards.resize(cardSlots());
  int n = cards.size();
  int w = (Display::WIDTH - 2 * PAD - CARD_GAP * (n - 1)) / n;
  for (int i = 0; i < (int)cards.size(); i++) {
    makeCard(cards[i], cardRow, false, w);
    lv_obj_set_size(cards[i].card, w, CARD_H);
    lv_obj_set_clickable(cards[i].card, true);
    lv_obj_add_event_cb(cards[i].card, onCardClick, LV_EVENT_CLICKED, (void*)(intptr_t)i);
  }
}

int findPlant(const String& name) {
  for (int i = 0; i < (int)bundle.plants.size(); i++)
    if (bundle.plants[i].name == name) return i;
  return -1;
}

void refresh() {
  if (!haveBundle) return;
  uint32_t extra = (millis() - bundle.receivedMs) / 1000;
  time_t now = time(nullptr);
  derived.clear();
  for (const Plant& p : bundle.plants) derived.push_back(derive(p, extra));
  order = byNeed(bundle.plants, derived);

  if ((int)cards.size() != cardSlots()) buildCards();
  if (page >= pageCount()) page = 0;
  float ymax = overviewYmax();
  for (int i = 0; i < (int)cards.size(); i++) {
    int slot = page * PER_PAGE + i;
    bool used = slot < (int)order.size();
    setHidden(cards[i].card, !used);
    if (!used) continue;
    int k = order[slot];
    fillCard(cards[i], bundle.plants[k], derived[k], extra, now, ymax);
  }

  if (lv_screen_active() == scrDetail) {
    int k = findPlant(detailName);
    if (k < 0) {
      showOverview();
    } else {
      fillCard(detailCard, bundle.plants[k], derived[k], extra, now,
               detailYmax(bundle.plants[k], derived[k]));
    }
  }
  updateStrip(stripOverview);
  updateStrip(stripDetail);
}

void showOverview() {
  lv_screen_load(scrOverview);
}

void showDetail(const String& name) {
  detailName = name;
  int k = findPlant(name);
  if (k < 0) return;
  uint32_t extra = (millis() - bundle.receivedMs) / 1000;
  fillCard(detailCard, bundle.plants[k], derived[k], extra, time(nullptr),
           detailYmax(bundle.plants[k], derived[k]));
  updateStrip(stripDetail);
  lv_screen_load(scrDetail);
}

}  // namespace

namespace Ui {

void begin() {
  scrOverview = makeScreen();
  makeStrip(stripOverview, scrOverview, false);
  cardRow = box(scrOverview);
  lv_obj_set_size(cardRow, Display::WIDTH, CARD_H + PAD);
  lv_obj_set_style_pad_hor(cardRow, PAD, 0);
  lv_obj_set_style_pad_bottom(cardRow, PAD, 0);
  flex(cardRow, LV_FLEX_FLOW_ROW, CARD_GAP);
  lv_obj_add_event_cb(scrOverview, onGesture, LV_EVENT_GESTURE, nullptr);

  scrDetail = makeScreen();
  makeStrip(stripDetail, scrDetail, true);
  lv_obj_t* dwrap = box(scrDetail);
  lv_obj_set_size(dwrap, Display::WIDTH, CARD_H + PAD);
  lv_obj_set_style_pad_hor(dwrap, PAD, 0);
  lv_obj_set_style_pad_bottom(dwrap, PAD, 0);
  makeCard(detailCard, dwrap, true, Display::WIDTH - 2 * PAD - DETAIL_LEFT_W);
  lv_obj_set_size(detailCard.card, Display::WIDTH - 2 * PAD, CARD_H);

  scrMessage = makeScreen();
  lv_obj_set_flex_align(scrMessage, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(scrMessage, 14, 0);
  msgTitle = label(scrMessage, &font_rc32, Theme::ink());
  msgBody = label(scrMessage, &font_r500_17, Theme::dim());
  lv_obj_set_width(msgBody, 620);
  lv_label_set_long_mode(msgBody, LV_LABEL_LONG_MODE_WRAP);
  lv_obj_set_style_text_align(msgBody, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_line_space(msgBody, 6, 0);

  makeSheet();
  lv_screen_load(scrMessage);
}

void showMessage(const char* title, const String& body) {
  setText(msgTitle, title);
  setText(msgBody, body.c_str());
  lv_screen_load(scrMessage);
}

void setBundle(const Bundle& b) {
  bundle = b;
  haveBundle = true;
  refresh();
  lastRefreshMs = millis();
  if (lv_screen_active() == scrMessage) showOverview();
}

void setOffline(bool off) {
  if (off == offline) return;
  offline = off;
  updateStrip(stripOverview);
  updateStrip(stripDetail);
}

void tick() {
  uint32_t ms = millis();
  if (ms - lastTickMs < 1000) return;
  lastTickMs = ms;
  if (!haveBundle) return;

  // Clock every second; ages and staleness every 30 s (charts only redraw on new data).
  updateStrip(stripOverview);
  updateStrip(stripDetail);
  updateSheet();
  if (ms - lastRefreshMs > 30000) {
    lastRefreshMs = ms;
    refresh();
  }
  uint32_t idle = lv_display_get_inactive_time(nullptr);
  if (lv_screen_active() == scrDetail && idle > DETAIL_TIMEOUT_MS) showOverview();
  if (page != 0 && idle > PAGE_TIMEOUT_MS) setPage(0);
}

void onFactoryReset(std::function<void()> cb) { resetCb = cb; }

void onLampTap(std::function<void()> cb) { lampCb = cb; }

void onWifiAction(std::function<void(WifiAction)> cb) { wifiCb = cb; }

}  // namespace Ui

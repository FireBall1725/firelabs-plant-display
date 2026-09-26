// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include "Display.h"
#include "Pins.h"
#include "Fonts.h"
#include "Theme.h"

#include <Wire.h>
#include <esp_heap_caps.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_private/cache_utils.h>
#include <esp_cpu.h>
#include <esp_rom_sys.h>

namespace {
  // CH422G has no register map: each "address" is a command.
  constexpr uint8_t CH422G_MODE = 0x24;
  constexpr uint8_t CH422G_OUT = 0x38;

  constexpr uint16_t GT911_STATUS = 0x814E;
  constexpr uint16_t GT911_POINT1 = 0x814F;
  constexpr uint16_t GT911_CONFIG_RES = 0x8048;

  esp_lcd_panel_handle_t panel = nullptr;
  SemaphoreHandle_t frameDone = nullptr;

  // Our own frame buffers (the panel runs with no_fb). The bounce ISR copies from
  // fbs[shown]; a flush asks for a switch, which is latched at the top of the next frame.
  constexpr size_t FB_BYTES = Display::WIDTH * Display::HEIGHT * 2;
  uint8_t *fbs[2] = {nullptr, nullptr};
  volatile int shown = 0;
  volatile int pending = -1;
  volatile uint32_t statFrames = 0, statSkips = 0;
  volatile uint32_t worstCopyUs = 0, slowCopies = 0, lastSlowMs = 0;
  volatile uint32_t lastFillCycles = 0, lateFills = 0, lastLateMs = 0, worstGapUs = 0;
  volatile uint32_t totFrames = 0, totSkips = 0, totFlushes = 0, totFlushTimeouts = 0, maxFlushWaitMs = 0;
  lv_display_t *disp = nullptr;

  // Firmware update screen. The ISR draws it line by line from internal RAM (a 2 bpp
  // text band and a progress bar), so unlike the frame buffers in PSRAM it stays on
  // screen while a flash write has the cache off: no flicker during OTA.
  constexpr int UP_BAND_Y = 150, UP_BAND_H = 112;
  constexpr int UP_BAR_Y = 300, UP_BAR_H = 8, UP_BAR_X = 220, UP_BAR_W = 360;
  constexpr int UP_ROW_BYTES = Display::WIDTH / 4;
  constexpr int UP_STAT_Y = 322, UP_STAT_H = 28;  // "70% · 1.2 of 1.7 MB" under the bar
  constexpr int UP_STAT_X = 200, UP_STAT_W = 400, UP_STAT_ROW_BYTES = UP_STAT_W / 4;
  uint8_t *upStats[2] = {nullptr, nullptr};
  uint8_t *volatile upStat = nullptr;
  // Internal RAM (the ISR reads them with the cache off), so they only exist during an
  // update: kept around, they took ~56 KB that wifi RX buffers and the TLS check-in need.
  uint8_t *volatile upBand = nullptr;  // UP_BAND_H rows of 2 bpp
  uint16_t upPal[4], upTrack, upFill;
  volatile bool updateMode = false;
  volatile uint32_t upFillPx = 0;           // bar width in pixels
  volatile uint32_t traceStage = 0, traceArg = 0, traceFrames = 0;  // OTA diagnostics

  void IRAM_ATTR fillUpdateLines(uint16_t *px, int y0, int lines) {
    const uint32_t bg2 = (uint32_t)upPal[0] | ((uint32_t)upPal[0] << 16);
    for (int l = 0; l < lines; l++, px += Display::WIDTH) {
      int y = y0 + l;
      const uint8_t *row = nullptr;
      int x0 = 0, bytes = UP_ROW_BYTES;
      if (upBand && y >= UP_BAND_Y && y < UP_BAND_Y + UP_BAND_H) {
        row = upBand + (y - UP_BAND_Y) * UP_ROW_BYTES;
      } else if (upStat && y >= UP_STAT_Y && y < UP_STAT_Y + UP_STAT_H) {
        row = upStat + (y - UP_STAT_Y) * UP_STAT_ROW_BYTES;
        x0 = UP_STAT_X;
        bytes = UP_STAT_ROW_BYTES;
      }
      if (row) {
        const uint32_t bg = (uint32_t)upPal[0] | ((uint32_t)upPal[0] << 16);
        if (x0) for (int i = 0; i < Display::WIDTH / 2; i++) ((uint32_t *)px)[i] = bg;
        uint16_t *out = px + x0;
        for (int b = 0; b < bytes; b++) {
          uint8_t v = row[b];
          out[b * 4 + 0] = upPal[v >> 6];
          out[b * 4 + 1] = upPal[(v >> 4) & 3];
          out[b * 4 + 2] = upPal[(v >> 2) & 3];
          out[b * 4 + 3] = upPal[v & 3];
        }
        continue;
      }
      uint32_t *w = (uint32_t *)px;
      for (int i = 0; i < Display::WIDTH / 2; i++) w[i] = bg2;
      if (y >= UP_BAR_Y && y < UP_BAR_Y + UP_BAR_H) {
        uint32_t fill = upFillPx;
        for (int x = 0; x < UP_BAR_W; x++) px[UP_BAR_X + x] = x < (int)fill ? upFill : upTrack;
      }
    }
  }

  uint8_t exioState = 0;
  uint8_t gtAddr = 0;
  uint16_t gtMaxX = Display::WIDTH;
  uint16_t gtMaxY = Display::HEIGHT;
  int16_t lastX = 0;
  int16_t lastY = 0;

  bool backlightOn = true;
  bool wakeGuard = false;     // screen off or dimmed: the next touch only wakes it
  bool swallowTouch = false;  // the touch that woke the screen, until it lifts
  void (*wakeCb)() = nullptr;
  volatile uint32_t touchReports = 0, wakeTouches = 0, lastWakeMs = 0, lastTouchX = 0, lastTouchY = 0;
  volatile uint32_t backlightOffMs = 0;

  void exioWrite(uint8_t state) {
    exioState = state;
    Wire.beginTransmission(CH422G_OUT);
    Wire.write(state);
    Wire.endTransmission();
  }

  bool gtRead(uint16_t reg, uint8_t *buf, size_t len) {
    Wire.beginTransmission(gtAddr);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    if (Wire.endTransmission() != 0) return false;
    if (Wire.requestFrom(gtAddr, (uint8_t)len) != len) return false;
    for (size_t i = 0; i < len; i++) buf[i] = Wire.read();
    return true;
  }

  void gtWrite(uint16_t reg, uint8_t val) {
    Wire.beginTransmission(gtAddr);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    Wire.write(val);
    Wire.endTransmission();
  }

  bool i2cPresent(uint8_t addr) {
    Wire.beginTransmission(addr);
    return Wire.endTransmission() == 0;
  }

  void i2cScan() {
    String found;
    for (uint8_t a = 0x08; a < 0x78; a++) {
      if (i2cPresent(a)) found += String(" 0x") + String(a, HEX);
    }
    Serial.printf("[i2c] devices:%s\n", found.length() ? found.c_str() : " none");
  }

  // Holding INT low while TP_RST rises latches the GT911 at 0x5D.
  void resetExpanderAndTouch() {
    Wire.beginTransmission(CH422G_MODE);
    Wire.write(0x01);  // IO0-7 as outputs
    Wire.endTransmission();

    exioWrite(Exio::BACKLIGHT | Exio::LCD_RST | Exio::USB_SEL);
    delay(100);
    pinMode(Pins::TOUCH_INT, OUTPUT);
    digitalWrite(Pins::TOUCH_INT, LOW);
    delay(100);
    exioWrite(exioState | Exio::TOUCH_RST);
    delay(200);
    pinMode(Pins::TOUCH_INT, INPUT);
  }

  void touchBegin() {
    for (uint8_t a : {0x5D, 0x14}) {
      if (i2cPresent(a)) {
        gtAddr = a;
        break;
      }
    }
    if (!gtAddr) {
      Serial.println("[touch] GT911 not found");
      return;
    }
    uint8_t res[4];
    if (gtRead(GT911_CONFIG_RES, res, 4)) {
      gtMaxX = res[0] | (res[1] << 8);
      gtMaxY = res[2] | (res[3] << 8);
    }
    Serial.printf("[touch] GT911 at 0x%02X, reports %ux%u\n", gtAddr, gtMaxX, gtMaxY);
    if (!gtMaxX || !gtMaxY) {
      gtMaxX = Display::WIDTH;
      gtMaxY = Display::HEIGHT;
    }
  }

  void touchRead(lv_indev_t *, lv_indev_data_t *data) {
    data->state = LV_INDEV_STATE_RELEASED;
    data->point.x = lastX;
    data->point.y = lastY;
    if (!gtAddr) return;

    uint8_t status;
    if (!gtRead(GT911_STATUS, &status, 1)) return;
    if (!(status & 0x80)) return;  // no fresh sample yet

    uint8_t points = status & 0x0F;
    if (points == 0) swallowTouch = false;
    if (points > 0) {
      touchReports++;
      uint8_t q[7];
      if (gtRead(GT911_POINT1, q, 7)) {
        lastTouchX = q[1] | (q[2] << 8);
        lastTouchY = q[3] | (q[4] << 8);
      }
    }
    if (points > 0 && (wakeGuard || swallowTouch)) {
      // Wake only; keep the touch away from whatever is under the finger.
      if (wakeGuard) {
        wakeTouches++;
        lastWakeMs = millis();
        wakeGuard = false;
        lv_display_trigger_activity(nullptr);
        if (wakeCb) wakeCb();
      }
      swallowTouch = true;
    } else if (points > 0) {
      uint8_t p[7];
      if (gtRead(GT911_POINT1, p, 7)) {
        uint16_t x = p[1] | (p[2] << 8);
        uint16_t y = p[3] | (p[4] << 8);
        lastX = (int32_t)x * Display::WIDTH / gtMaxX;
        lastY = (int32_t)y * Display::HEIGHT / gtMaxY;
        data->point.x = lastX;
        data->point.y = lastY;
        data->state = LV_INDEV_STATE_PRESSED;
      }
    }
    gtWrite(GT911_STATUS, 0);
  }

  // Refills a bounce buffer from the shown frame buffer. The stock libs run this ISR
  // even while a flash write has the cache off (LCD_RGB_ISR_IRAM_SAFE), and PSRAM is
  // unreadable then: touching the frame buffer panics ("Cache disabled but cached memory
  // region accessed"). So skip the copy and let the panel repeat stale lines for the few
  // milliseconds a flash write takes.
  bool IRAM_ATTR onBounceEmpty(esp_lcd_panel_handle_t, void *bounce, int pos_px, int len_bytes,
                               void *) {
    BaseType_t woken = pdFALSE;
    // A refill is due every ~1.2 ms (20 lines at 14 MHz); a gap over 3 ms means the
    // DMA probably scanned out a stale bounce buffer.
    uint32_t now = esp_cpu_get_cycle_count();
    if (lastFillCycles) {
      uint32_t gapUs = (now - lastFillCycles) / 240;
      if (gapUs > worstGapUs) worstGapUs = gapUs;
      if (gapUs > 3000) {  // normal max is ~2.3 ms across the 20 blanking lines
        lateFills++;
        lastLateMs = (uint32_t)(esp_timer_get_time() / 1000);
      }
    }
    lastFillCycles = now;
    if (pos_px == 0) {
      statFrames++;
      totFrames++;
    }
    if (pos_px == 0 && pending >= 0) {
      shown = pending;
      pending = -1;
      xSemaphoreGiveFromISR(frameDone, &woken);
    }
#ifdef FL_DEBUG_PORTAL
    if (updateMode && pos_px == 0 && ++traceFrames % 8 == 0) {
      // ~4 times a second, from ROM so it works with the cache off: where the updater is.
      esp_rom_printf(DRAM_STR("[isr] stage=%u arg=%u cache=%d\n"), traceStage, traceArg,
                     (int)spi_flash_cache_enabled());
    }
#endif
    if (updateMode) {
      fillUpdateLines((uint16_t *)bounce, pos_px / Display::WIDTH, len_bytes / (Display::WIDTH * 2));
    } else if (spi_flash_cache_enabled()) {
      uint32_t c0 = esp_cpu_get_cycle_count();
      memcpy(bounce, fbs[shown] + pos_px * 2, len_bytes);
      // The other bounce buffer lasts ~1.17 ms; a copy slower than that means the DMA
      // caught up and scanned out a half-filled buffer.
      uint32_t copyUs = (esp_cpu_get_cycle_count() - c0) / 240;
      if (copyUs > worstCopyUs) worstCopyUs = copyUs;
      if (copyUs > 1100) {
        slowCopies++;
        lastSlowMs = (uint32_t)(esp_timer_get_time() / 1000);
      }
    } else {
      statSkips++;
      totSkips++;
    }
    return woken == pdTRUE;
  }

  // Direct mode on two frame buffers: hand over the finished one, then block until the
  // ISR has switched to it, after which nothing reads the old one and LVGL can draw there.
  void flush(lv_display_t *d, const lv_area_t *, uint8_t *px) {
    if (lv_display_flush_is_last(d)) {
      xSemaphoreTake(frameDone, 0);
      // px points at the last redrawn area, not necessarily the buffer start, so
      // find the buffer that contains it (== fbs[0] only held for areas at 0,0).
      pending = (px >= fbs[0] && px < fbs[0] + FB_BYTES) ? 0 : 1;
      uint32_t t0 = millis();
      if (xSemaphoreTake(frameDone, pdMS_TO_TICKS(100)) != pdTRUE) totFlushTimeouts++;
      uint32_t waited = millis() - t0;
      if (waited > maxFlushWaitMs) maxFlushWaitMs = waited;
      totFlushes++;
    }
    lv_display_flush_ready(d);
  }

  uint32_t tickMs() { return millis(); }

  void logToSerial(lv_log_level_t, const char *msg) { Serial.print(msg); }

  bool panelBegin() {
    esp_lcd_rgb_panel_config_t cfg = {};
    cfg.clk_src = LCD_CLK_SRC_DEFAULT;
    // 14 MHz (~34 Hz) rather than 16: with wifi also using PSRAM, the bounce refills
    // fell behind while LVGL was rendering and the panel flickered.
    cfg.timings.pclk_hz = 14 * 1000 * 1000;
    cfg.timings.h_res = Display::WIDTH;
    cfg.timings.v_res = Display::HEIGHT;
    cfg.timings.hsync_pulse_width = 4;
    cfg.timings.hsync_back_porch = 8;
    cfg.timings.hsync_front_porch = 8;
    cfg.timings.vsync_pulse_width = 4;
    cfg.timings.vsync_back_porch = 8;
    cfg.timings.vsync_front_porch = 8;
    cfg.timings.flags.pclk_active_neg = 1;
    cfg.data_width = 16;
    cfg.bits_per_pixel = 16;
    cfg.num_fbs = 0;
    cfg.bounce_buffer_size_px = Display::WIDTH * 20;  // 2 x 32 KB of internal RAM
    cfg.dma_burst_size = 64;
    cfg.hsync_gpio_num = Pins::LCD_HSYNC;
    cfg.vsync_gpio_num = Pins::LCD_VSYNC;
    cfg.de_gpio_num = Pins::LCD_DE;
    cfg.pclk_gpio_num = Pins::LCD_PCLK;
    cfg.disp_gpio_num = -1;
    for (int i = 0; i < 16; i++) cfg.data_gpio_nums[i] = Pins::LCD_DATA[i];
    cfg.flags.no_fb = 1;

    for (uint8_t *&fb : fbs) {
      fb = (uint8_t *)heap_caps_aligned_calloc(64, 1, FB_BYTES, MALLOC_CAP_SPIRAM);
      if (!fb) {
        Serial.println("[lcd] no PSRAM for frame buffers");
        return false;
      }
    }

    esp_err_t err = esp_lcd_new_rgb_panel(&cfg, &panel);
    if (err != ESP_OK) {
      Serial.printf("[lcd] esp_lcd_new_rgb_panel failed: %s\n", esp_err_to_name(err));
      return false;
    }

    frameDone = xSemaphoreCreateBinary();
    esp_lcd_rgb_panel_event_callbacks_t cbs = {};
    cbs.on_bounce_empty = onBounceEmpty;
    esp_lcd_rgb_panel_register_event_callbacks(panel, &cbs, nullptr);

    esp_lcd_panel_reset(panel);
    esp_lcd_panel_init(panel);
    return true;
  }
}

uint32_t applyWakes = 0;  // Screen bumps this; read in debugJson

namespace Display {
  bool begin() {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL, 400000);
    resetExpanderAndTouch();
    i2cScan();

    if (!panelBegin()) return false;
    touchBegin();

    void *fb0 = fbs[0];
    void *fb1 = fbs[1];

    lv_init();
    lv_tick_set_cb(tickMs);
    lv_log_register_print_cb(logToSerial);

    disp = lv_display_create(WIDTH, HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(disp, fb0, fb1, WIDTH * HEIGHT * 2, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, flush);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchRead);

    Serial.printf("[lcd] up, fb0=%p fb1=%p, PSRAM free %u KB, internal free %u KB\n",
                  fb0, fb1,
                  heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024,
                  heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024);
    return true;
  }

  void tick() {
    if (!updateMode) lv_timer_handler();
  }

  struct TextLine {
    const lv_font_t *font;
    lv_color_t color;
    const char *text;
    int y0, y1;
  };

  // Draws lines with LVGL into a scratch canvas and packs it to 2 bpp (4 grey steps)
  // in dst, a w x h strip in internal RAM that the ISR can read with the cache off.
  void renderStrip(uint8_t *dst, int w, int h, const TextLine *lines, int n) {
    lv_draw_buf_t *buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_RGB565, 0);
    if (!buf) return;
    lv_obj_t *canvas = lv_canvas_create(lv_layer_sys());
    lv_canvas_set_draw_buf(canvas, buf);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.align = LV_TEXT_ALIGN_CENTER;
    for (int i = 0; i < n; i++) {
      dsc.font = lines[i].font;
      dsc.color = lines[i].color;
      dsc.text = lines[i].text;
      lv_area_t a = {0, lines[i].y0, w - 1, lines[i].y1};
      lv_draw_label(&layer, &dsc, &a);
    }
    lv_canvas_finish_layer(canvas, &layer);
    for (int y = 0; y < h; y++) {
      const uint16_t *src = (const uint16_t *)(buf->data + y * buf->header.stride);
      uint8_t *row = dst + y * (w / 4);
      for (int b = 0; b < w / 4; b++) {
        uint8_t v = 0;
        for (int k = 0; k < 4; k++) {
          int g = (src[b * 4 + k] >> 5) & 0x3F;  // green channel, 0..63
          v = (v << 2) | (g > 52 ? 3 : g > 30 ? 2 : g > 10 ? 1 : 0);
        }
        row[b] = v;
      }
    }
    lv_obj_delete(canvas);
    lv_draw_buf_destroy(buf);
  }

  // The title band is drawn in place (it only changes once, Downloading -> Installing,
  // and a one-frame tear there is fine); the status line swaps between two small strips.
  void setUpdateText(const char *title, const char *sub) {
    uint8_t *band = upBand;
    if (!band) band = (uint8_t *)heap_caps_malloc(UP_ROW_BYTES * UP_BAND_H, MALLOC_CAP_INTERNAL);
    if (!band) return;
    const TextLine lines[] = {
      {&font_rc32, lv_color_white(), title, 18, 60},
      {&font_r500_17, lv_color_make(150, 150, 150), sub, 70, 100},  // the dim palette step
    };
    renderStrip(band, Display::WIDTH, UP_BAND_H, lines, 2);
    upBand = band;
  }

  void setUpdateStatus(const char *text) {
    for (int i = 0; i < 2; i++)
      if (!upStats[i]) upStats[i] = (uint8_t *)heap_caps_malloc(UP_STAT_ROW_BYTES * UP_STAT_H, MALLOC_CAP_INTERNAL);
    if (!upStats[0] || !upStats[1]) return;
    uint8_t *strip = upStat == upStats[0] ? upStats[1] : upStats[0];
    const TextLine line = {&font_r500_17, lv_color_white(), text, 2, UP_STAT_H - 1};
    renderStrip(strip, UP_STAT_W, UP_STAT_H, &line, 1);
    upStat = strip;
  }

  void beginUpdateScreen(const char *title, const char *sub) {
    setUpdateText(title, sub);
    setUpdateStatus("");
    if (updateMode || !upBand) return;
    upPal[0] = lv_color_to_u16(Theme::ground());
    upPal[1] = lv_color_to_u16(lv_color_mix(Theme::dim(), Theme::ground(), 110));
    upPal[2] = lv_color_to_u16(Theme::dim());
    upPal[3] = lv_color_to_u16(Theme::ink());
    upTrack = lv_color_to_u16(Theme::inset());
    upFill = lv_color_to_u16(Theme::moist());
    upFillPx = 0;
    setBacklight(true);
    updateMode = true;
  }

  bool updating() { return updateMode; }

  void trace(uint32_t stage, uint32_t arg) {
    traceStage = stage;
    traceArg = arg;
  }

  void stopPanel() {
    if (!panel) return;
    esp_lcd_panel_del(panel);
    panel = nullptr;
  }

  void setUpdateProgress(uint32_t done, uint32_t total) {
    if (total) upFillPx = (uint64_t)UP_BAR_W * min(done, total) / total;
  }

  // Back to the LVGL frame buffers (a failed update); a good one just restarts.
  void endUpdateScreen() {
    updateMode = false;
    // Wait out a refill that may still be reading the strips, then give the RAM back.
    delay(5);
    uint8_t *band = upBand;
    upBand = nullptr;
    upStat = nullptr;
    heap_caps_free(band);
    for (int i = 0; i < 2; i++) {
      heap_caps_free(upStats[i]);
      upStats[i] = nullptr;
    }
    lv_obj_invalidate(lv_screen_active());
  }

  bool isAsleep() { return !backlightOn; }

  void setWakeGuard(bool on) { wakeGuard = on; }

  void onWake(void (*cb)()) { wakeCb = cb; }

  String debugJson() {
    char b[768];
    snprintf(b, sizeof(b),
             "{\"uptime_s\":%lu,\"frames\":%lu,\"skipped_fills\":%lu,\"flushes\":%lu,"
             "\"flush_timeouts\":%lu,\"max_flush_wait_ms\":%lu,\"shown\":%d,\"asleep\":%d,"
             "\"heap\":%u,\"psram\":%u,\"late_fills\":%lu,\"last_late_ms\":%lu,"
             "\"worst_gap_us\":%lu,\"now_ms\":%lu,\"worst_copy_us\":%lu,\"slow_copies\":%lu,"
             "\"last_slow_ms\":%lu,\"touch_reports\":%lu,\"wake_touches\":%lu,"
             "\"last_wake_ms\":%lu,\"last_touch\":[%lu,%lu],\"backlight_off_ms\":%lu,"
             "\"inactive_ms\":%lu,\"apply_wakes\":%lu}",
             (unsigned long)(millis() / 1000), (unsigned long)totFrames, (unsigned long)totSkips,
             (unsigned long)totFlushes, (unsigned long)totFlushTimeouts,
             (unsigned long)maxFlushWaitMs, (int)shown, (int)!backlightOn, ESP.getFreeHeap(),
             ESP.getFreePsram(), (unsigned long)lateFills, (unsigned long)lastLateMs,
             (unsigned long)worstGapUs, (unsigned long)millis(), (unsigned long)worstCopyUs,
             (unsigned long)slowCopies, (unsigned long)lastSlowMs, (unsigned long)touchReports,
             (unsigned long)wakeTouches, (unsigned long)lastWakeMs, (unsigned long)lastTouchX,
             (unsigned long)lastTouchY, (unsigned long)backlightOffMs,
             (unsigned long)lv_display_get_inactive_time(nullptr), (unsigned long)applyWakes);
    return b;
  }

  void stats(uint32_t &frames, uint32_t &skips) {
    frames = statFrames;
    skips = statSkips;
    statFrames = 0;
    statSkips = 0;
  }

  // Switching the backlight straight on (or off) jolts the 5 V rail hard enough to
  // reset the board (POWERON, and the USB serial drops with it). The expander pin is
  // only on/off, so soft-start it: pulse the enable over ~160 ms with a rising duty.
  void rampBacklight(bool on) {
    const uint8_t lit = exioState | Exio::BACKLIGHT, dark = exioState & ~Exio::BACKLIGHT;
    constexpr int STEPS = 80;
    constexpr uint32_t PERIOD_US = 2000, MIN_US = 80;  // one I2C write is ~60-100 us
    for (int i = 1; i < STEPS; i++) {
      float f = (float)i / STEPS;
      uint32_t onUs = (uint32_t)((on ? f * f : (1 - f) * (1 - f)) * PERIOD_US);
      uint32_t t0 = micros();
      if (onUs >= MIN_US) {
        exioWrite(lit);
        while (micros() - t0 < onUs) {}
      }
      if (PERIOD_US - onUs >= MIN_US) exioWrite(dark);
      while (micros() - t0 < PERIOD_US) {}
    }
    exioWrite(on ? lit : dark);
  }

  void setBacklight(bool on) {
    if (on == backlightOn) return;
    backlightOn = on;
    if (!on) backlightOffMs = millis();
    rampBacklight(on);
  }

  bool touchFound() { return gtAddr != 0; }
}

// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
#include <lvgl.h>
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_CUSTOM
#include <esp_heap_caps.h>
#include <esp_attr.h>
#include <stdlib.h>

// LVGL's objects and styles live in PSRAM. Internal RAM is kept for wifi and for the
// TLS check-in, whose SHA/AES hardware needs internal DMA buffers; with LVGL on plain
// malloc the handshake ran out ("esp-sha: Failed to allocate buf memory").
#define LV_CAPS (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)

#ifdef FL_DEBUG_PORTAL
// Debug A/B switch: set to FL_LV_INTERNAL_MAGIC and soft-restart to run LVGL on plain
// malloc (internal RAM for small blocks) instead. RTC memory survives the restart and
// flipping it needs no flash write.
#define FL_LV_INTERNAL_MAGIC 0x1A7E1A7Eu
RTC_NOINIT_ATTR uint32_t fl_lv_mem_mode;
int fl_lv_internal(void) { return fl_lv_mem_mode == FL_LV_INTERNAL_MAGIC; }
void fl_lv_set_internal(int on) { fl_lv_mem_mode = on ? FL_LV_INTERNAL_MAGIC : 0; }
#else
static inline int fl_lv_internal(void) { return 0; }
#endif

void lv_mem_init(void) {}
void lv_mem_deinit(void) {}
lv_mem_pool_t lv_mem_add_pool(void* mem, size_t bytes) {
  LV_UNUSED(mem);
  LV_UNUSED(bytes);
  return NULL;
}
void lv_mem_remove_pool(lv_mem_pool_t pool) { LV_UNUSED(pool); }

void* lv_malloc_core(size_t size) {
  if (fl_lv_internal()) return malloc(size);
  void* p = heap_caps_malloc(size, LV_CAPS);
  return p ? p : malloc(size);
}

void* lv_realloc_core(void* p, size_t new_size) {
  if (fl_lv_internal()) return realloc(p, new_size);
  void* q = heap_caps_realloc(p, new_size, LV_CAPS);
  return q ? q : realloc(p, new_size);
}

void lv_free_core(void* p) { heap_caps_free(p); }

void lv_mem_monitor_core(lv_mem_monitor_t* mon_p) { LV_UNUSED(mon_p); }
lv_result_t lv_mem_test_core(void) { return LV_RESULT_OK; }
#endif

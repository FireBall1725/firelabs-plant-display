// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (C) 2026 FireBall1725
// LVGL config. Anything not set here takes LVGL's default from lv_conf_internal.h.
#if 1
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 16

// Everything LVGL allocates goes to PSRAM (src/LvMem.c), leaving internal RAM for wifi and TLS.
#define LV_USE_STDLIB_MALLOC  LV_STDLIB_CUSTOM
#define LV_USE_STDLIB_STRING  LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB

#define LV_USE_OS LV_OS_NONE
#define LV_DEF_REFR_PERIOD 25

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0

#ifndef FL_PERF_MONITOR
#define FL_PERF_MONITOR 0
#endif
#define LV_USE_SYSMON FL_PERF_MONITOR
#define LV_USE_PERF_MONITOR FL_PERF_MONITOR
#define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT

#define LV_USE_CANVAS 1
#define LV_USE_SPAN 1

// The UI uses its own Roboto fonts (src/fonts); Montserrat 14 stays for LVGL's internals.
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(font_r14)
#define LV_FONT_DEFAULT &font_r14

#endif
#endif

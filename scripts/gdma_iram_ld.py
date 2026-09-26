# SPDX-License-Identifier: AGPL-3.0-only
# Copyright (C) 2026 FireBall1725
# PlatformIO extra script: link with a copy of the Arduino sections.ld that puts the
# GDMA control functions in IRAM.
#
# CONFIG_GDMA_CTRL_FUNC_IN_IRAM only works through ldgen, and pioarduino's hybrid
# rebuild (custom_sdkconfig) copies the rebuilt libs and memory.ld but not the
# regenerated sections.ld, so the link keeps the stock script. The RGB panel restarts
# its DMA from the vsync ISR, which also runs while a flash write has the cache off;
# gdma_stop/gdma_append and the GDMA HAL they call were still in flash, and any erase
# that overlapped a restart double-faulted into TG1WDT_SYS_RST (every OTA died).
import os

Import("env")

ANCHOR = "*libhal.a:gdma_hal_top.*(.literal.gdma_hal_clear_intr .text.gdma_hal_clear_intr)"
IRAM = [
    ("libesp_hw_support.a", "gdma", ["gdma_stop", "gdma_append"]),
    ("libhal.a", "gdma_hal_top",
     ["gdma_hal_start_with_desc", "gdma_hal_stop", "gdma_hal_append", "gdma_hal_reset"]),
    ("libhal.a", "gdma_hal_ahb_v1",
     ["gdma_ahb_hal_start_with_desc", "gdma_ahb_hal_stop", "gdma_ahb_hal_append",
      "gdma_ahb_hal_reset"]),
]

board = env.BoardConfig()
mcu = board.get("build.mcu")
memory = board.get("build.arduino.memory_type")
libs = env.PioPlatform().get_package_dir("framework-arduinoespressif32-libs")
src = os.path.join(libs, mcu, memory, "sections.ld")

with open(src) as f:
    script = f.read()
if ANCHOR not in script:
    env.Exit("gdma_iram_ld.py: anchor not found in %s; the stock script changed" % src)

# .iram0.text comes before .flash.text, and the first matching rule wins.
lines = []
for archive, obj, funcs in IRAM:
    for fn in funcs:
        lines.append("    *%s:%s.*(.literal.%s .text.%s)" % (archive, obj, fn, fn))
script = script.replace(ANCHOR, ANCHOR + "\n" + "\n".join(lines), 1)

out_dir = os.path.join(env.subst("$BUILD_DIR"), "ld")
os.makedirs(out_dir, exist_ok=True)
with open(os.path.join(out_dir, "sections.ld"), "w") as f:
    f.write(script)
env.Prepend(LIBPATH=[out_dir])

# SPDX-License-Identifier: AGPL-3.0-only
# Copyright (C) 2026 FireBall1725
# Runs PlatformIO's esptool with 256-byte packets. The board's CH343 USB-UART, on macOS's
# built-in CDC driver, drops any write much bigger than that ("Failed to write to target RAM").
import os
import sys

sys.path.insert(0, os.environ["ESPTOOL_PKG"])

import esptool
import esptool.loader
import esptool.targets.esp32s3 as s3

PACKET = 256
esptool.loader.ESPLoader.ESP_RAM_BLOCK = PACKET
s3.ESP32S3ROM.FLASH_WRITE_SIZE = PACKET
s3.ESP32S3StubLoader.FLASH_WRITE_SIZE = PACKET

sys.exit(esptool._main())

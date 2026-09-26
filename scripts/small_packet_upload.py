# SPDX-License-Identifier: AGPL-3.0-only
# Copyright (C) 2026 FireBall1725
# PlatformIO extra script: route uploads through esptool_small_packets.py.
import os

Import("env")

pkg = os.path.dirname(env.subst("$UPLOADER"))
env["ENV"]["ESPTOOL_PKG"] = pkg
env.Replace(UPLOADER=os.path.join(env.subst("$PROJECT_DIR"), "scripts", "esptool_small_packets.py"))

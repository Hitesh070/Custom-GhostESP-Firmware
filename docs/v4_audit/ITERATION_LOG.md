# Iterative Audit & Remediation Log (GhostESP God Mode V4)

## Iteration 0 — Baseline Audit & Live Failure Reproduction
- Cloned/branched `v4` from `v4-baseline-v3` (`f1bea686c`).
- Exhaustive inventory of 7,145 tracked baseline files and full inspection of `main/`, `include/`, `desktop/`, `sdkconfig`, `partitions.csv`, `CMakeLists.txt`.
- Reproduced original user-reported failures live on `COM5` (`7Semi ESP32-S3-WROOM-1-N16R8`):
  1. `bledetect` panicked inside `nimble_host` (`1024`-byte stack too small on Xtensa).
  2. Chip running warm (`52–54 C` junction) lacked a working ESP32-S3 internal thermal monitor (`TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 85)` failed).
  3. Desktop GUI lacked BLE Lab population, Timeline population, WAL persistence, thread safety, and non-blocking serial queueing.
  4. `roguedetect` flagged legitimate multi-BSSID campus Wi-Fi (`VITC-HOS2-4`) as Evil Twins.

## Iteration 1 — Firmware Hardening, Thermal Monitor & God Mode V4 Modules
- Binary built cleanly: `0x38e4e0` bytes.
- Added [`temp_monitor.c`](../../main/modules/temp_monitor.c) + `temp` CLI (`status|c|f|interval|history|reset|json`), wired boot init in [`main.c`](../../main/main.c).
- Fixed `nimble_host` stack to `6144 / sizeof(StackType_t)` + PSRAM fallback and hardened AirTag payload bounds in [`ble_manager.c`](../../main/managers/ble_manager.c).
- Hardened crypto (`wpa_crypto.c`), VLA (`ap_manager.c`), SSID bounds (`beacon_spam.c`, `ssh_scan.c`, `airtag_scan.c`), `callbacks.c`, `enum4linux_scan.c`, `wifi_manager.c`.
- Added USB-OTG mutual exclusion & `--confirm` gate on `badusb run`/`exec` + `REPEAT` buffer fix in `hid_script_parser.c` + graceful stop flag in `usb_keyboard_manager.c`.
- Aligned S3 SD/MMC pins (`1,2,4`), SD SPI (`CS=4, CLK=1, MISO=47, MOSI=2`), UART2 `esp_comm` (`17,18`), I2C bus scanner `0x08..0x77`, full 126-ch `nrf24_lab` sweep, and enterprise BSSID discrimination in `rogue_detector.c`.
- Live bench test (`v4_live_regression.json`, 55 commands): caught residual bugs — `stopscan` cleared `ap_count = 0` (`list -a` empty) and left `bledetect` active; `SPIRAM_MALLOC_ALWAYSINTERNAL=16384` starved Internal SRAM (`~7 KB` free post-scan -> `ESP_ERR_HTTPD_TASK`).

## Iteration 2 — SRAM Rebalancing, `stopscan` Cache Preservation & Final Verification
- Binary built cleanly: `0x38e610` bytes (`SHA256SUMS.txt` archived in `binaries/v4/`).
- Updated [`wifi_manager.c`](../../main/managers/wifi_manager.c), [`ap_scan.c`](../../main/scans/wifi/ap_scan.c), and [`cmd_wifi.c`](../../main/core/commands/cmd_wifi.c) so `stopscan` preserves cached `scanned_aps` and stops active BLE scans (`bledetect`, `blescan`).
- Lowered `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL` to `2048` and enabled `CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y`, `CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP=y`, `CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL=y`, `CONFIG_ESP_TASK_WDT_EN=y` in `sdkconfig`.
- Added `fflush(stdout)` in `glog_emit()` (`main/core/glog.c`).
- Flashed ESP32-S3 and ran 95 exhaustive live commands on `COM5` (`v4_exhaustive_cli.json`): **0 crashes**, **0 `ESP_ERR_HTTPD_TASK` errors**, **48–51 KB free Internal SRAM**, `scanap -> stopscan -> list -a` and `bledetect -> stopscan` verified PASS.
- Upgraded `desktop/` to modular PySide6 17-view Command Center (`7/7` `pytest` passing).

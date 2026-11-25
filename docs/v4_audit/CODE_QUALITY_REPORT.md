# Code Quality & Static Audit Report (GhostESP V4)

## 1. Key Structural & Runtime Findings Remediated
- **Xtensa FreeRTOS Stack Sizing Hazard (`ble_manager.c`):** `portSTACK_TYPE` is `uint8_t` (1 byte) on Xtensa ESP-IDF. Passing `1024` as word depth in V3 allocated a 1 KB stack for `nimble_host`, causing deterministic stack overflow crashes (`***ERROR*** A stack overflow in task nimble_host has been detected`). Fixed to `6144 / sizeof(StackType_t)`.
- **Hardware Calibration Curve Compliance (`temp_monitor.c`):** ESP32-S3 hardware ranges in `temperature_sensor_periph.c` do not support `[10, 85]`. Replaced with `[-10, 80]` plus automatic escalation to `[20, 100]` and `[50, 125]`.
- **Internal SRAM Budget on Wi-Fi+BLE Builds (`sdkconfig`):** `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=16384` and `CONFIG_MBEDTLS_INTERNAL_MEM_ALLOC=y` depleted internal DRAM down to `7 KB` after concurrent scans, triggering `ESP_ERR_HTTPD_TASK` on AP restoration. Updated to `2048` and `CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y`.
- **AP Scan Cache Preservation (`wifi_manager.c`, `ap_scan.c`, `cmd_wifi.c`):** `stopscan` drained empty Wi-Fi driver records and overwrote `ap_count = 0`, discarding valid `scanap` results (`list -a` empty) and failing to cancel active BLE scans. Both `stopscan` paths now preserve cached results and stop active BLE scans.
- **Enterprise Multi-BSSID Classification (`rogue_detector.c`) & Mutex Locking:** Separated same-authmode Enterprise/OUI infrastructure BSSIDs (`VITC-HOS2-4`) from mismatched/downgraded Evil Twins; moved `glog()` alerts outside `s_mutex`.
- **Fake Benchmark & Channel Step Removal (`nrf24_lab.c`, `self_test.c`):** Removed simulated RF PDR benchmark and `ch += 2` skip; gated sweep and SPI2 loopback verification strictly on `r.is_responsive`, releasing CS=10 (`nrf24_lab_deinit`) when hardware is absent.
- **Documentation & Help Hygiene (`cmd_help.c`):** Added `help godmode`, added missing `statusidle` header, and deduplicated `capture`, `dialconnect`, and `Attack Commands` blocks.

## 2. Build Hygiene Observations
- Existing upstream files still contain benign `-Wunused-variable` / `-Wunused-function` warnings (`wpa_crypto.c`, `reconnect_test.c`, `popup.c`, `large_builtin_icons.c`, `sd_card_manager.c`). Zero compiler errors remain and all GhostESP V4 modules compile cleanly under ESP-IDF v6.1.1 (`-Werror=all` baseline with upstream warning suppressions).
- Intentional architectural invariants retained: `CONFIG_FREERTOS_UNICORE=y` (protects legacy single-core scan globals lacking SMP spinlocks) and unmodified `partitions.csv` (preserving user NVS and OTA/factory partition offsets).

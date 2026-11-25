# FreeRTOS Concurrency, Memory & Timing Analysis

## 1. Single-Core Execution Model (`CONFIG_FREERTOS_UNICORE=y`)
- GhostESP intentionally enables `CONFIG_FREERTOS_UNICORE=y` in `sdkconfig` to serialize access to shared static scan state (`scanned_aps`, BLE scan arrays, PCAP state) across callbacks without introducing widespread SMP spinlock contention.
- Task Watchdog Timer (`CONFIG_ESP_TASK_WDT_EN=y`) and FreeRTOS runtime stats are enabled for latency monitoring.

## 2. Internal SRAM vs Octal PSRAM Partitioning (Iteration 2 Fix)
- **Previous Failure:** `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=16384` and mbedTLS internal allocations drained Internal DMA SRAM down to `36 KB` at boot and `7 KB` after scans, causing `httpd` task creation failures (`ESP_ERR_HTTPD_TASK`).
- **V4 Tuning (`sdkconfig`):**
  - `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=2048`
  - `CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y`
  - `CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP=y`
  - `CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL=y`
- **Verified Outcome:** Internal SRAM stays at `48–51 KB` free after 95 multi-protocol commands while Octal PSRAM keeps `>8.0 MB` free, restoring SoftAP + 13-handler HTTP server reliably after every `stopscan`.

## 3. Synchronization & Task Lifecycle Hardening
- **`nimble_host` Stack ([`ble_manager.c`](../../main/managers/ble_manager.c)):** Fixed `1024`-byte allocation bug to `6144 / sizeof(StackType_t)` with PSRAM stack fallback, eliminating Xtensa stack overflow panics.
- **Mutex Lock Ordering:** Moved `glog()` serial emission outside `s_mutex` in [`rogue_detector.c`](../../main/modules/rogue_detector.c) and `deauth_monitor.c`, and added unlock on error paths in `enum4linux_scan.c`.
- **Graceful USB Host Task Exit:** Replaced external `vTaskDelete` while USB host was blocking in [`usb_keyboard_manager.c`](../../main/managers/usb_keyboard_manager.c) with cooperative `s_usb_host_stop_req` signaling.
- **Thermal Sampler:** Non-blocking `esp_timer` callback records die temperature into a fixed `120`-entry ring buffer with `errors = 0`.
- **Desktop Threading:** Background `GhostSerialRX` and `GhostSerialTX` threads communicate strictly via `SerialSignals` (`Signal(str)`, `Signal(bool, str)`) to the Qt GUI thread and synchronize `serial.Serial` via `_io_lock`.

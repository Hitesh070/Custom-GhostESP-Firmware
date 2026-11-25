# Independent Repository Audit (GhostESP God Mode V4)

## 1. Repository State & Scope
- **Baseline Reference:** Tag `v4-baseline-v3` (`f1bea686c`), audited and developed on branch `v4`.
- **Total Tracked Files (`git ls-files` at baseline):** `7,145` files (`933` project-authored files across `main/`, `include/`, `desktop/`, `webui/`, `configs/`, `cmake/`, `scripts/`, and `(root)`).
- **Build & Partition Hygiene:** `partitions.csv` untouched per safety policy; build size `0x38e4e0` bytes (8% free in `0x3e0000` app partition at Iteration 1, re-verifying in Iteration 2).

## 2. Systemic Deficiencies Found vs V3 Baseline
1. **Unverified Runtime Behavior:** V3 reported `temp` and BLE scanning conceptually, while actual silicon runs panicked on `nimble_host` (`1024`-byte stack) and errored on `temp` (`temperature_sensor_install(10, 85)`).
2. **Pin & Bus Conflicts in `sdkconfig` and `settings_manager.c`:** Default config placed SD SPI on USB-JTAG `GPIO19` and unbonded `GPIO23`, GPS RX on `GPIO17`, and `esp_comm` on Display `GPIO6/7`.
3. **Internal SRAM Pressure:** Keeping allocations `<= 16 KB` and mbedTLS in internal DRAM starved Wi-Fi AP HTTP server startup (`ESP_ERR_HTTPD_TASK`) after BLE + Wi-Fi operations.
4. **Honesty Gaps in Self-Test & NRF24 Diagnostics:** External unprobed buses were previously marked `Available (Connected)` and NRF24 register polling was labeled as OTA packet benchmark PDR.
5. **Desktop Application Completeness:** Upgraded from basic 7-tab Tkinter script to a tested PySide6 multi-panel command center (`7/7` pytest unit + offscreen UI tests passing).

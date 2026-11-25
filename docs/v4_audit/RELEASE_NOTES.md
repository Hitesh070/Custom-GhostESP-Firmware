# GhostESP God Mode V4 — Release Notes

- **Baseline:** Branched from `v4-baseline-v3` (`f1bea686c`) onto branch `v4`.
- **Target Silicon:** `7Semi ESP32-S3-WROOM-1-N16R8` (16 MB Flash, 8 MB Octal PSRAM).

## Highlights
1. **New ESP32-S3 On-Chip Silicon Temperature Monitor (`temp`):** Calibrated `[-10, 80] C` HAL curve with automatic escalation (`[20, 100]` / `[50, 125]`), background `esp_timer` sampling, `120`-point history ring buffer, `[TEMP_JSON]` schema, and Desktop live Matplotlib graphing.
2. **BLE NimBLE Stability & `stopscan` Reliability:** Fixed `nimble_host` `1 KB` stack overflow crash (`6144` bytes + PSRAM fallback); preserved `scanned_aps` across `stopscan` (`list -a` retains APs); extended `stopscan` to stop active BLE scans.
3. **Internal SRAM Rebalancing:** Tuned `SPIRAM_MALLOC_ALWAYSINTERNAL=2048` + PSRAM allocations for mbedTLS, LWIP, and NimBLE — eliminated `ESP_ERR_HTTPD_TASK` and stabilized free Internal SRAM at `48–51 KB` under load.
4. **Defensive Security & Hardware Accuracy:** Fixed crypto CCM/EAPOL/SSID bounds, added BadUSB/USB-Kbd/MSC PHY exclusion + `--confirm` gate, eliminated multi-BSSID Enterprise false positives in `roguedetect`, implemented `126`-channel `nrf24lab` sweeps + real I2C bus scanning, and aligned S3 SD/UART2 pins.
5. **GhostESP Command Center V4 (`desktop/`):** Upgraded to a 17-view PySide6 desktop suite with queued serial I/O, incremental parsers, Matplotlib charts, and SQLite WAL persistence (`7/7` `pytest` tests passing).

## Binary Artifacts ([`binaries/v4/SHA256SUMS.txt`](../../binaries/v4/SHA256SUMS.txt))
```text
11A6017C7E6D5FD9B91A74A1F2F57165585013BCA95194068A798FA954E30733  bootloader.bin
8550A5BD2A1624C76532BC349E3035C65C6FCF6FABB50C8929E02EB36EE4B40E  Ghost_ESP_IDF.bin
902824F69CE7E6A0E6AA229FE4A28BC7A988CF31E0557F3313FA86FBE06422B6  partition-table.bin
```

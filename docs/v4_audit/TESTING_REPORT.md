# Testing & Validation Report (GhostESP God Mode V4)

## 1. Environment & Target Hardware
- **Board:** `7Semi ESP32-S3-WROOM-1-N16R8` (ESP32-S3 rev v0.2, 16 MB Flash, 8 MB Octal PSRAM `AP_3v3`)
- **ESP-IDF Version:** `v6.1.1` (single-core FreeRTOS configuration `CONFIG_FREERTOS_UNICORE=y`)
- **Serial Test Interface:** `COM5` (Native ESP32-S3 USB-Serial/JTAG `303A:1001`) @ `115200` baud; `COM6` (CP210x UART0 bridge) available as hardware UART backup
- **Firmware Artifact Under Test:** `binaries/v4/Ghost_ESP_IDF.bin` (`0x38e610` bytes, ~8% partition headroom in `0x3E0000` `app0`)

```text
11A6017C7E6D5FD9B91A74A1F2F57165585013BCA95194068A798FA954E30733  bootloader.bin
8550A5BD2A1624C76532BC349E3035C65C6FCF6FABB50C8929E02EB36EE4B40E  Ghost_ESP_IDF.bin
902824F69CE7E6A0E6AA229FE4A28BC7A988CF31E0557F3313FA86FBE06422B6  partition-table.bin
```

## 2. Iterative Build & Hardware Bench Verification

| Iteration | Focus | Binary Size | Outcome | Key Measured Evidence |
|---|---|---:|---|---|
| **Baseline (`v3`)** | Initial audit & failure reproduction | `~3.7 MB` | `FAIL` | `nimble_host` panic (`1024` byte stack), temp HAL range fail, `stopscan` wiped `scanned_aps`, `ESP_ERR_HTTPD_TASK` (`7 KB` SRAM free) |
| **Iteration 1** | Core fixes + Temp Monitor + Security + God Mode modules | `0x38e4e0` | `PARTIAL PASS` | Fixed NimBLE stack, BadUSB `--confirm`, Temp Monitor `52.2–54.2 C`, `roguedetect` & `deauthmon`; caught `stopscan` cache wipe & low internal SRAM warning |
| **Iteration 2** | `stopscan` AP cache + BLE stop + PSRAM alloc tuning + `fflush` | `0x38e610` | **PASS** | `95/95` live COM5 commands executed with **0 runtime panics**, **0 `ESP_ERR_HTTPD_TASK`**, and **48–51 KB** free Internal SRAM after full stress |

## 3. Exhaustive Live CLI Run Summary (Iteration 2 — `COM5`)
- **Total Commands Executed:** `95`
- **Runtime Crashes / Panics / Task Exhaustion Errors:** `0` (`1` regex hit on command `coredump`, which intentionally prints the pre-existing historical ELF dump stored at flash `0x3f0000` from Iteration 0)
- **Key Verified Flows in `v4_exhaustive_cli.json`:**
  1. **Boot & System Identity:** `chipinfo` (`Free Heap: 8321151 bytes`, `Min Heap: 8308900 bytes`), `version` (`Build: Nov 25 2026 06:03:31`, `IDF: v6.1.1`), `uuid`, `uptime`, `date`, `whoami`, `ifconfig` (`192.168.4.1`).
  2. **Thermal Telemetry (`temp`):** `temp`, `temp json`, `temp f`, `temp c`, `temp interval 1000`, `temp history`, `temp interval 2000`, `temp invalidarg` -> `[TEMP_JSON]` valid, `0` sampler errors, `48.2–54.2 C`.
  3. **Wi-Fi Discovery & `stopscan` Regression Check:** `scanap` found APs (`6`–`13` across local environment) -> `stopscan` logged `WiFi scan already complete (6 cached APs preserved).` -> `list -a` listed `[0]..[5]` accurately; `wpa3check` and `congestion` reported live channel breakdown; SoftAP & HTTP server (13 handlers on port 80) resumed cleanly with zero `ESP_ERR_HTTPD_TASK`.
  4. **Rogue AP & Deauth Monitors:** `roguedetect start/status/stop` tracked local APs (including multi-BSSID `VITC-HOS2-4`) without false-positive Evil Twin alerts; `deauthmon start/status/stop` ran cleanly with zero errors.
  5. **BLE Stability & `stopscan` Cross-Protocol Stop:** `bledetect` initialized NimBLE cleanly (no panic), discovered advertisers, and `stopscan` cleanly stopped/deinitialized BLE (`blescan -t` -> `stopscan` also verified).
  6. **Hardware Self-Test & Unplugged Peripherals:** `selftest`, `nrf24lab diag/sweep 5/benchmark 50`, `sd_config` (`CS 4, CLK 1, MISO 47, MOSI 2`), `sd status`, `irpin`, `nfctest` verified accurate `HW REQUIRED` / `HARDWARE UNRESPONSIVE` reporting when physical add-ons are absent.
  7. **BadUSB Authorization Gate:** `badusb run builtin` and `badusb exec 128` blocked execution cleanly until `--confirm` is explicitly passed.
  8. **Final Post-Stress Memory Check:** `mem` & `temp json` after 95 commands showed `48–51 KB` internal SRAM free and `>8.0 MB` Octal PSRAM free.

## 4. Desktop Application Automated Verification
- Test suite: [`desktop/tests/test_app_protocol_db.py`](../../desktop/tests/test_app_protocol_db.py)
- Command: `pytest -v desktop/tests/test_app_protocol_db.py`
- Observed Result: **`7 passed`** (ANSI sanitization, `[TEMP_JSON]` parsing + malformed JSON resilience, multi-line AP blocks, Rogue/Deauth/Profile/Self-test tables, NRF24 sweep + BLE lines, SQLite WAL roundtrip & JSON export, PySide6 offscreen 17-view smoke test).

## 5. Untested / Hardware-Gated / Policy-Excluded Cases
- **Hardware-Dependent (`HW REQUIRED`):** `nrf24lab` with live NRF24L01+ RF packets, `sd` FAT32 file writes without an inserted card, external I2C0 sensors (`GPIO8/9`), UART1 GPS fix (`GPIO44`) indoors, external host SDR (`RTL-SDR`/`HackRF`), SPI write-only ST7735S LCD readback.
- **Kconfig Disabled:** `ir*` runtime transmit (`CONFIG_HAS_INFRARED` disabled by default).
- **Non-Destructive / Policy Guarded:** Category D active RF disruption (`beaconspam`, `dhcpstarve`, `saeflood`, `blespam`, active `deauth`) and live `badusb ... --confirm` keystroke injection (which detaches native `COM5` USB CDC to switch USB descriptors).

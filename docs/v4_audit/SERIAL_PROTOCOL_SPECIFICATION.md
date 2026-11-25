# Serial Telemetry & CLI Protocol Specification

## 1. Physical Transports (7Semi ESP32-S3-WROOM-1-N16R8)
- **Port A (`COM5`):** Native ESP32-S3 USB-Serial/JTAG (`VID:PID 303A:1001`). Hardware 64-byte TX FIFO; wait-free writes ensure unplugged USB never stalls `SerialTask`. Under heavy concurrent `ESP_LOGI` + `printf`, occasional leading bytes can drop if unthrottled; `glog_emit()` calls `fflush(stdout)` to tighten frame boundaries.
- **Port B (`COM6`):** CP210x UART0 bridge @ `115200` baud with full hardware UART FIFO buffering.

## 2. Line Framing & Escaping
- Firmware emits UTF-8 text lines with optional ANSI SGR color sequences (`\x1b[...m`) and prompt `ghost> `.
- Host parser [`strip_ansi()`](../../desktop/parsers.py) strips ANSI CSI sequences and normalizes `\r\n` / `\r` line endings inside `GhostSerialTransceiver._read_loop`.

## 3. Structured & Stateful Line Grammars Parsed by `GhostTelemetryParser`
1. **Temperature JSON:** Lines containing `[TEMP_JSON] {...}` parse into `temp_json` events (`temp_c`, `temp_f`, `min_c`, `max_c`, `avg_c`, `sram_free_kb`, `psram_free_kb`, `samples`, `errors`, `range_min`, `range_max`). Text fallback matches `Current Die Temp : ...`.
2. **Chip Identity Block:** `[CHIPINFO_START]` ... `[CHIPINFO_END]` key/value telemetry.
3. **Multi-Line Wi-Fi AP Blocks (`scanap` / `list -a`):** `[N] SSID: ...`, `BSSID:`, `RSSI:`, `Channel:`, `Security|Auth|Encryption:`, `Vendor:`. Flushed eagerly on `Vendor:`, next `[N]`, `===`, or `ghost>` prompt.
4. **Pipe-Delimited Operational Tables:**
   - `roguedetect status`: `BSSID | Ch | RSSI | Security | SSID | Threat` (auto-generates HIGH `timeline` alert when Threat != `CLEAN`).
   - `nrf24lab sweep`: `CH | 2.xxx GHz | pct% | bar` (`nrf24_sweep_bin`).
   - `profile list`: `ID | Name | YES*/no | Description` (`profile_row`).
   - `deauthmon status`: `Time (s) | Type | Transmitter MAC | Receiver MAC | Reason` + summary banner (`Total Deauths`, `Disassocs`, `Live Rate`).
   - `selftest`: `Subsystem | Diagnostic Test | Result | Details` (`PASS|FAIL|WARNING|HW REQUIRED|UNTESTED`).
5. **BLE Advertisers / Trackers:** Lines containing a MAC address alongside `Advertiser`, `BLE`, `AirTag`, `Flipper`, `iBeacon`, `Skimmer`, or `RSSI` (`bledetect`, `blescan`, `listairtags`, `listflippers`).

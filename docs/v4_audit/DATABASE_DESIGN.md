# SQLite WAL Telemetry Database Design (`desktop/database.py`)

## 1. Concurrency & Durability Configuration
- Engine: Python `sqlite3` with `check_same_thread=False` guarded by `threading.Lock()` (`self._lock`) across every read/write method.
- Journaling: `PRAGMA journal_mode=WAL;` and `PRAGMA synchronous=NORMAL;` enabled at initialization so background RX parsing and UI table refreshes never hit SQLite lock contention.

## 2. Schema Definition (7 Tables)
1. **`sessions`**: `id` (PK), `port`, `started_at`, `ended_at`, `chip_info`, `firmware_version`
2. **`wifi_aps`**: `bssid` (PK), `ssid`, `channel`, `rssi`, `security`, `vendor`, `is_rogue`, `threat_details`, `last_seen` (UPSERT on `bssid`)
3. **`ble_devices`**: `mac` (PK), `name`, `rssi`, `dev_type`, `details`, `last_seen` (UPSERT on `mac`)
4. **`deauth_events`**: `id` (PK), `timestamp`, `frame_type`, `transmitter`, `receiver`, `reason`
5. **`temp_samples`**: `id` (PK), `timestamp`, `temp_c`, `temp_f`, `min_c`, `max_c`, `avg_c`, `sram_free_kb`, `psram_free_kb`
6. **`nrf24_scans`**: `id` (PK), `timestamp`, `channel`, `freq_mhz`, `activity_pct`
7. **`event_timeline`**: `id` (PK), `timestamp`, `subsystem`, `event_type`, `severity`, `details`

## 3. Export Capabilities
- `export_events_json()` writes structured `event_timeline` snapshots cleanly for offline incident review (verified in `test_database_wal_and_roundtrip`).

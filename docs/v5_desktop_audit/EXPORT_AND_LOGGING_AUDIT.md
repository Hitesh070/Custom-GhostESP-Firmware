# Export & Logging Audit

- **CSV Exports (`_export_wifi_csv`, `_export_ble_csv`, `_export_temp_csv`):** Uses `csv.writer` with UTF-8 encoding and `newline=""`. Verified exact header + row parity against `wifi_table` in `test_console_history_clear_and_exports`.
- **JSON Exports (`_export_timeline_json`, `_export_bundle_json`):** Backed by `GhostDatabase.export_events_json()` and `export_session_bundle()`. Verified valid JSON roundtrip in `test_database_wal_and_roundtrip` and `test_console_history_clear_and_exports`.
- **Error Handling:** Wrapped file I/O in `try/except OSError` with `QMessageBox.critical` + `status_bar` confirmation.

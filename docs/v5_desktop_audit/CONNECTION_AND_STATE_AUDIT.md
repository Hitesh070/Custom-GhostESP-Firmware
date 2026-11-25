# Connection & State Management Audit

- **Scenario A (Offline Startup):** App launches cleanly (`OFFLINE` badge, `--.- C` KPIs). All command buttons show `QMessageBox.information` instead of silently failing.
- **Scenario B (Connected `COM5`):** Verified live on `COM5` (`USB Serial Device`, priority `0` ahead of `COM6` CP210x priority `1`): `8,870` RX bytes parsed into `temp_json` (`35.2 C`) and `8` `profile_row` events.
- **Scenario C (Disconnect During Operation):** `_on_status_changed_ui(False, ...)` stops `temp_poll_timer`, unchecks `auto_temp_chk`, resets KPIs to `--.- C` / `-- KB` / `-- MB`, marks `temp_now_lbl` as `OFFLINE`, marks `sec_summary` as `IDLE (OFFLINE)`, and closes the SQLite `sessions` row.
- **Scenario D (Reconnect):** Opens a new `sessions` row cleanly, re-queries `temp json` (250 ms) and `profile list` (650 ms), and reuses single `GhostSerialTransceiver` worker threads without duplicate listeners.
- **Scenario E/F (Malformed JSON / Partial Prompt Lines):** Malformed `[TEMP_JSON]`, `null`/`NaN`/`Inf` fields return cleanly with zero exceptions; `serial_comm.py` flushes `ghost> ` prompts without trailing `\n` so the final AP/BLE record is never delayed.

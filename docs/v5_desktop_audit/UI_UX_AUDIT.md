# UI/UX Audit (GhostESP Command Center V5)

- **Theme Consistency:** Dark canvas (`#090d14`) / elevated surfaces (`#111824`, `#172130`) with green/cyan/amber/red semantic accents (`desktop/theme.py`).
- **Table Integrity:** All 7 `QTableWidget` instances now enforce `QAbstractItemView.EditTrigger.NoEditTriggers` so users cannot accidentally edit live telemetry cells, and `_upsert_wifi` / `_upsert_ble` disable sorting during updates to prevent half-written rows.
- **Per-Tab Feedback:** Added dedicated output panels on **Wi-Fi Discovery** (`wifi_report_log`), **BLE Lab** (`ble_log`), and **Profiles & Config** (`profile_log`), plus command-context routing for **Device & Serial** (`serial_log`) and **Memory & RTOS** (`health_log`).

# Command Console Audit

- **Autocomplete:** `QCompleter` seeded with `COMMON_COMMANDS` (`temp json`, `scanap`, `bledetect`, `roguedetect`, `deauthmon`, `nrf24lab`, `profile`, `selftest`, etc.).
- **Command History (`Up`/`Down`):** Implemented via `eventFilter` on `console_in` cycling `self.cmd_history` cleanly (verified with `QTest.keyClick` in `test_console_history_clear_and_exports`).
- **Clear Console:** Added dedicated `Clear` button wired to `console_out.clear()`.
- **Offline Handling:** `_send_cmd()` checks `self.serial.running` and alerts the user without recording a fake sent command.

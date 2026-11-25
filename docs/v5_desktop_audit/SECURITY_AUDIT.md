# Desktop Application Security Audit

- **No Shell Injection:** Zero `subprocess(..., shell=True)` or `os.system` calls in `desktop/`. Serial commands write directly to `serial.Serial` and host SDR detection uses `shutil.which()`.
- **SQL Injection Prevention:** All queries in `desktop/database.py` use parameterized `?` placeholders.
- **BadUSB Safety Gate:** `Run Builtin BadUSB` requires explicit opt-in via `badusb_confirm_chk` before sending `badusb run builtin --confirm`.
- **No Secrets / Credentials:** Zero hardcoded tokens, passwords, or API keys in `desktop/`.

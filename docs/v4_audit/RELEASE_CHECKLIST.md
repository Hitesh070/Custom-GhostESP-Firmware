# GhostESP God Mode V4 — Release Verification Checklist

| Check Item | Verification Method | Status |
|---|---|---|
| Clean ESP-IDF `v6.1.1` firmware build (`esp32s3`) | `idf.py build` (`0` errors, `0x38e610` bytes inside `0x3E0000` `app0`) | **PASS** |
| Flash artifacts & SHA-256 checksums archived | `binaries/v4/Ghost_ESP_IDF.bin`, `bootloader.bin`, `partition-table.bin`, `SHA256SUMS.txt` | **PASS** |
| Real hardware flash & boot on `7Semi ESP32-S3-WROOM-1-N16R8` | Flashed over `COM5`, boot log + `8.3 MB` total / `8.0 MB` PSRAM verified | **PASS** |
| On-chip silicon temperature monitor verified live | `temp`, `temp json`, `temp c/f`, `temp interval`, `temp history` (`48.2–54.2 C`, `0` errors) | **PASS** |
| Exhaustive live CLI stress & regression suite | `95` live COM5 commands executed with `0` crashes & `0 ESP_ERR_HTTPD_TASK` | **PASS** |
| `stopscan` AP cache & cross-protocol BLE stop | `scanap -> stopscan -> list -a` & `bledetect -> stopscan` verified live | **PASS** |
| Safety & authorization gates enforced | `--confirm` block verified live (`badusb run builtin`, `badusb exec 128`) | **PASS** |
| Honest missing-hardware gating verified | `selftest`, `nrf24lab`, `sd status`, host SDR check | **PASS** |
| Desktop Command Center PySide6 suite verified | `pytest -v desktop/tests/test_app_protocol_db.py` (`7/7` passing) | **PASS** |
| Repository hygiene & `docs/v4_audit/` suite | Leftover temporary CSV/DB cleaned; V4 docs & binaries ready on branch `v4` | **PASS** |

# Complete Project Inventory (GhostESP God Mode V4)
**Target Hardware:** 7Semi ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB Octal PSRAM)  
**ESP-IDF Version:** v6.1.1  
**Git Baseline Tag:** `v4-baseline-v3` (`f1bea686c`), active branch: `v4`  

## Top-Level Directory Breakdown (Tracked Baseline + V4 Additions)

| Directory | File Count | Measured Lines (Text Files) | Audit Scope |
|---|---:|---:|---|
| `(root)` | 48 | 65197 | Project-Authored (Full Inspection) |
| `.github` | 4 | 1093 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `LICENSES` | 4 | 285 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `cmake` | 2 | 347 | Project-Authored (Full Inspection) |
| `components` | 2596 | 2403454 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `configs` | 68 | 0 | Project-Authored (Full Inspection) |
| `desktop` | 6 | 1726 | Project-Authored (Full Inspection) |
| `docs` | 3270 | 302768 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `examples` | 29 | 95 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `firmware` | 6 | 188 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `include` | 292 | 23957 | Project-Authored (Full Inspection) |
| `main` | 467 | 308327 | Project-Authored (Full Inspection) |
| `ota_updater` | 6 | 415 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `plugins` | 297 | 56531 | Vendor / Static Docs / CI (Boundary & Manifest Audited) |
| `scripts` | 37 | 7861 | Project-Authored (Full Inspection) |
| `webui` | 13 | 8518 | Project-Authored (Full Inspection) |

## Explicit Exclusions & Justifications

- `components/` (2,596 files): Third-party SDK components (`M5GFX`, `lua`, `espressif__elf_loader`, `espressif__esp_lcd_axs15231b`, `tlv320dac3100`, `esp_audio_codec`). Boundaries (`CMakeLists.txt`, `idf_component.yml`, public headers) were audited; third-party vendor internals are excluded from line-by-line review.
- `docs/` (3,270 files): Hugo static site build output (`docs/hugo docs/public/`) and markdown user manual pages.
- `build/` (untracked generated output): CMake/Ninja/esptool artifacts verified via `idf.py size-components`, linker map, and SHA-256 checksums.
- `plugins/` (297 files) & `examples/` (29 files): SDK examples and sample app payloads; integration boundaries audited via `main/managers/plugin_manager.c` and `plugin_loader.c`.

# GhostESP God Mode V2 – Centralized Engineering Task Tracker

**Last Updated:** November 24, 2026  
**Status Key:**  
- `[x]` Completed & Verified  
- `[/]` In Progress  
- `[ ]` Pending Execution  

---

## Workstream 1: Repository Audit & Technical Debt
- [x] Inspect root files, components, and directory layout.
- [/] Agent 1: Generate `REPOSITORY_AUDIT.md` (Codebase architecture, FreeRTOS tasks, build configs).
- [/] Agent 1: Generate `EXISTING_FEATURES.md` (Categorized functional catalog).
- [/] Agent 1: Generate `TECHNICAL_DEBT.md` (Memory hazards, hardcoded pins, blocking loops).

## Workstream 2: Hardware Architecture & Pinout Authority
- [x] Verify target microcontroller: 7Semi ESP32-S3-WROOM-1-N16R8 (16MB Flash DIO, 8MB Octal PSRAM).
- [x] Verify verified display wiring: 1.8" ST7735S 128×160 on SPI3 (MOSI=15, SCK=16, CS=5, DC=6, RST=7).
- [x] Verify verified NRF24 wiring: NRF24L01+ on SPI2 (MOSI=11, MISO=13, SCK=12, CS=10, CE=14).
- [x] Verify native USB restrictions: GPIO19 (D-) and GPIO20 (D+) reserved strictly for USB-JTAG/CDC/HID.
- [/] Agent 2: Generate `HARDWARE_COMPATIBILITY.md` (Electrical analysis, power rails, level shifting).
- [/] Agent 2: Generate `PIN_CONFIGURATION.md` (Master pinout table, restriction rules).
- [/] Agent 2: Implement `include/hardware/board_config.h` and `main/hardware/board_config.c`.

## Workstream 3: Wireless & RF Subsystems Fortification
- [/] Agent 3: Audit Wi-Fi promiscuous frame sniffing and scanning in `wifi_manager.c`.
- [/] Agent 3: Audit NRF24 2.4 GHz spectrum sweep engine on dedicated `SPI2_HOST`.
- [/] Agent 3: Optimize channel dwell time, RPD sampling latching, and sweep speed.
- [/] Agent 3: Audit BLE GAP scanning, device discovery buffer, and Wi-Fi/BT coexistence.
- [/] Agent 3: Author `WIRELESS_RF_SUBSYSTEM_REPORT.md`.

## Workstream 4: Infrared, SD Card & Peripherals
- [/] Agent 4: Audit IR subsystem (`universal_ir.c`, `cmd_ir.c`) for RMT timing and protocols.
- [/] Agent 4: Enforce hardware safety: verify IR driving guidelines (transistor/MOSFET drivers).
- [/] Agent 4: Audit SD Card Manager (`sd_card_manager.c`) for SPI/SDMMC mode and graceful fallback.
- [/] Agent 4: Author `IR_AND_PERIPHERALS_REPORT.md`.

## Workstream 5: Software Architecture & Unified HAL
- [x] Create System Requirements Specification (`SRS.md`).
- [x] Create Master Project Plan (`PROJECT_PLAN.md`).
- [/] Integrate centralized board configuration into `main/CMakeLists.txt`.
- [ ] Connect NRF24, SD Card, I2C, and Display drivers to `board_config.h`.
- [ ] Implement peripheral health monitoring and graceful degradation.

## Workstream 6: UI/UX & Compact Screen Optimization
- [x] Scale design tokens in `include/gui/design_tokens.h` for screens $\le 160\text{ px}$.
- [x] Sized NRF24 frequency analyzer view with non-overlapping 2.40G/2.52G labels and dual 58 px buttons.
- [x] Scale font selection in `accessibility_fonts.c` to Montserrat 8/10.
- [ ] Review all remaining GUI views for text overflow on 128×160 display.
- [ ] Optimize status bar indicators (Wi-Fi, Bluetooth, battery, clock).

## Workstream 7: Performance & Security Hardening
- [x] Allocate minimum 4096-byte stacks for FreeRTOS background tasks.
- [x] Add 1000 ms timeout to prevent serial CLI deadlocks during task mode switches.
- [ ] Verify PSRAM usage for large buffers (packet captures, OUI database, UI framebuffers).
- [ ] Audit command line parser for bounds safety and null-pointer protection.

## Workstream 8: QA, Regression & Hardware Validation
- [x] Flash and verify basic display output on ST7735S (128×160 SPI TFT).
- [x] Verify NRF24 SPI communication (`NRF_REG_CONFIG` readback: `0x0F`).
- [x] Verify RGB LED animations on GPIO48 (Knight Rider, Police, Strobe, Rainbow).
- [ ] Run full regression suite after HAL integration.
- [ ] Produce `TESTING_REPORT.md`.

## Workstream 9: Documentation & Release Artifacts
- [x] Author `SRS.md`.
- [x] Author `PROJECT_PLAN.md`.
- [x] Author `TASKS.md`.
- [ ] Author `ARCHITECTURE.md` with Mermaid diagrams.
- [ ] Author `BUILD_GUIDE.md` and `TROUBLESHOOTING.md`.
- [ ] Update `CHANGELOG.md` with God Mode V2 architecture release.
- [ ] Build release firmware binaries (`idf.py build`) and record sizes.
- [ ] Prepare final delivery report.

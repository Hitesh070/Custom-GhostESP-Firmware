# In-Depth Technical Debt & Architectural Risk Analysis

**Target Architecture:** ESP32-S3 (Xtensa Dual-Core LX7 @ 240 MHz + LP core)  
**Hardware Profile:** 7Semi ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB Octal PSRAM)  
**Framework Version:** ESP-IDF v6.1.1  
**Audit Date:** November 24, 2026  
**Auditor:** Agent 1 - Repository & Architecture Auditor (GhostESP God Mode V2)

---

## 1. Executive Summary

This report delivers an exhaustive, critical analysis of the technical debt, structural anti-patterns, concurrency hazards, and memory safety vulnerabilities across the **Custom GhostESP Firmware** codebase.

While GhostESP is functionally rich and feature-dense, rapid multi-target expansion and historical porting from Marauder and Arduino codebases have left severe architectural liabilities:
- **Monolithic God Files:** Core translation units exceed 3,800 to 15,000 lines of C with tightly entangled concerns.
- **Concurrency & Watchdog Hazards:** Blocking `while()` loops in command handlers and priority-inverting mutexes inside high-frequency Wi-Fi promiscuous callbacks risk tripping the FreeRTOS Interrupt Watchdog Timer (IWDT) and Task Watchdog Timer (TWDT).
- **Stack Starvation:** Several FreeRTOS tasks are provisioned with stacks $\le 2048$ bytes—critically unsafe under ESP-IDF 6.1 with newlib reentrancy and FATFS operations.
- **Hardcoded GPIO Magic Numbers:** Ubiquitous bare pin numbers bypass the Kconfig and settings abstraction layer, causing pin contention between peripherals.
- **Unchecked Dynamic Allocations:** Critical heap allocations lack NULL verification, creating crash vectors under high memory pressure.

---

## 2. Hardcoded GPIO Pins & Hardware Contention

### 2.1 The Issue
Throughout `main.c`, `display_manager.c`, and vendor display drivers, GPIO numbers are hardcoded as raw integer literals rather than using Kconfig symbols (`CONFIG_*`) or board-profile hardware abstraction definitions.

### 2.2 Concrete Examples in Code

#### A. Main Bootstrap (`main/main.c`)
```c
// main/main.c:744-746
gpio_set_level(7, 1);
gpio_set_level(42, 1);

// main/main.c:788-790
gpio_reset_pin(12);
gpio_set_direction(12, GPIO_MODE_OUTPUT);
gpio_set_level(12, 1);

// main/main.c:877-884
gpio_set_direction(39, GPIO_MODE_OUTPUT);
gpio_set_level(39, 1);
gpio_set_direction(12, GPIO_MODE_OUTPUT);
gpio_set_level(12, 1);
gpio_set_direction(9, GPIO_MODE_OUTPUT);
gpio_set_level(9, 1);
gpio_set_direction(10, GPIO_MODE_OUTPUT);
gpio_set_level(10, 1);

// main/main.c:937
gpio_set_level(15, 1);
```

#### B. Display Manager (`main/managers/display_manager.c`)
```c
// main/managers/display_manager.c:2327-2329
gpio_set_level(1, 1);
gpio_set_level(2, 1);
gpio_set_level(40, 0);

// main/managers/display_manager.c:2392
gpio_set_level(38, 1);

// main/managers/display_manager.c:4915, 4953
gpio_set_level(15, 0);
gpio_set_level(15, 1);
```

#### C. Joystick Subsystem (`main/main.c`)
```c
// main/main.c:1145-1150
joystick_init(&joysticks[0], 3, HOLD_LIMIT, true);  // Left
joystick_init(&joysticks[1], 2, HOLD_LIMIT, true);  // Select
joystick_init(&joysticks[2], 0, HOLD_LIMIT, true);  // Up
joystick_init(&joysticks[3], 4, HOLD_LIMIT, true);  // Right
joystick_init(&joysticks[4], 1, HOLD_LIMIT, true);  // Down
```

### 2.3 Architectural Impact & Contention
1. **NRF24 vs. T-Deck / Display Conflicts:**  
   Pin `12` is forced HIGH in `main.c:788` for CC1101, but on our 7Semi ESP32-S3 board with NRF24L01+, **GPIO 12 is `CONFIG_NRF24_SPI_SCK_PIN`**! Hardcoded initialization code running before NRF24 setup can leave SPI clock lines in an indeterminate state.
2. **ST7735S Display Bus Contention:**  
   GPIO `15` is manipulated as a general power line in `main.c:937` and `display_manager.c:4915`, whereas on the 7Semi ST7735S TFT SPI3 bus, **GPIO 15 is `MOSI`**!
3. **Portability Breakdown:**  
   New board variants cannot be integrated simply by adding an `sdkconfig` template; developers must search for hidden `#ifdef CONFIG_BUILD_CONFIG_TEMPLATE` blocks scattered across dozens of `.c` files.

---

## 3. Concurrency, Stalling & Task Watchdog Risks

### 3.1 Synchronous While-Loops in CLI & Control Handlers
Stopping an attack or peripheral task frequently employs busy-wait polling loops with `vTaskDelay()` directly on the caller task (often `SerialTask` or the LVGL thread).

```c
// main/managers/wifi_manager.c:5031-5034
int wait_count = 0;
while (karma_task_handle != NULL && wait_count < 30) {
    vTaskDelay(pdMS_TO_TICKS(100));
    wait_count++;
}
if (karma_task_handle != NULL) {
    vTaskDelete(karma_task_handle);
    karma_task_handle = NULL;
}
```

```c
// main/features/wifi/auth_flood.c:278-282
int wait_count = 0;
while (auth_flood_task_handle != NULL && wait_count < 100) {
    vTaskDelay(pdMS_TO_TICKS(10));
    wait_count++;
}
```

#### The Hazards:
- **Serial Console Stalls:** `wifi_manager_stop_karma()` blocks the serial shell for up to **3,000 ms (3 seconds)**! During this window, UART buffers can overflow, input is dropped, and the CLI appears frozen to the operator.
- **Forced Deletion Memory Leaks:** If the task fails to exit within the timeout, `vTaskDelete(task_handle)` is called unconditionally. FreeRTOS task deletion does **not** free dynamically allocated heap buffers, unmount files, or release semaphores held by the deleted task, resulting in permanent heap leakage and deadlocked mutexes.

### 3.2 Mutex Contention Inside Wi-Fi Promiscuous RX Callbacks

In `main/core/callbacks.c`:
```c
// main/core/callbacks.c:1377-1384 (Called from wifi_eapol_scan_callback)
if (log_handshake) {
    glog("Handshake found!\nAP=%s\nPair=%s/%s\n",
         log_ap_str, msg_name(log_ap_msg), msg_name(log_sta_msg));
    char hs_payload[40];
    snprintf(hs_payload, sizeof(hs_payload), "%s|%s/%s",
             log_ap_str, msg_name(log_ap_msg), msg_name(log_sta_msg));
    ghostscript_emit_event("handshake_captured", hs_payload);
}
```

Looking inside `glog()` (`main/core/glog.c:42, 69`):
```c
static inline void glog_out_lock(void) {
    ...
    if (s_glog_out_mutex) xSemaphoreTake(s_glog_out_mutex, portMAX_DELAY);
}

static inline void glog_emit(const char *buf) {
    glog_out_lock();
    printf("%s", buf);
    glog_out_unlock();
    ...
}
```

#### The Hazard:
In ESP-IDF, 802.11 promiscuous packet callbacks run directly in the context of the high-priority `wifi` task. Calling `glog()`, which takes `s_glog_out_mutex` with `portMAX_DELAY`, causes the Wi-Fi driver task to block indefinitely if `SerialTask` is mid-way through writing a large buffer over a slow 115200 baud UART! This results in:
1. Massive packet drop in hardware MAC queues.
2. Potential Task Watchdog Timer (TWDT) reset on Core 0.

### 3.3 Broken Task Context Detection (`display_manager_is_lvgl_task`)

In `main/managers/display_manager.c:3956-3958`:
```c
bool display_manager_is_lvgl_task(void) {
  return !lvgl_task_handle || xTaskGetCurrentTaskHandle() == lvgl_task_handle;
}
```

#### Root Cause Analysis:
When running on a headless configuration where `lvgl_task_handle == NULL`, `!lvgl_task_handle` evaluates to **`true`**!  
Consequently, `display_manager_run_on_lvgl(fn, arg)` assumes it is already inside the LVGL task and executes `fn(arg)` synchronously on whatever caller task invoked it (e.g. `SerialTask`). If `fn` dereferences LVGL display structures (e.g. `lv_disp_get_default()`, `lv_layer_top()`), the firmware experiences an unhandled kernel panic (`LoadProhibited` null-pointer exception).

---

## 4. Stack Size Risks & Overflow Vulnerabilities

Tasks under ESP-IDF 6.1 require larger stack frames due to 32-bit Xtensa register window spills (`ENTRY`/`RETW`), newlib C library reentrancy structures (`struct _reent`), and expanded format string processing in `snprintf()`.

### 4.1 Vulnerable Task Allocations

| Source File | Task Name | Stack Size | Risk Level | Trigger Condition |
| :--- | :--- | :--- | :--- | :--- |
| `main/core/callbacks.c:2547` | **`pineap_logw`** | **1,792 bytes** | **CRITICAL** | Formats PineAP alerts and writes to SD card FATFS. File I/O requires $\ge 3$ KB. |
| `main/features/wifi/eapol_logoff.c:189` | **`eapol_logoff`** | **2,048 bytes** | **HIGH** | Frame building + `esp_wifi_80211_tx()` + logging. Stack overflow on verbose logs. |
| `main/features/wifi/sae_flood.c:838` | **`sae_displ`** | **2,048 bytes** | **HIGH** | Formats UI counters and calls `status_display_show_status()`. |
| `main/features/ethernet/eth_arp_poison.c:863` | **`arp_poison`** | **2,048 bytes** | **HIGH** | Raw packet transmit over SPI W5500 with stack-allocated frame structures. |
| `main/managers/display_manager.c:2704` | **`EncPoll`** | **2,048 bytes** | **MEDIUM** | Hardware input queue dispatching. Marginal headroom. |
| `main/managers/peer_ota_manager.c:863` | **`peer_ota_fin`** | **2,048 bytes** | **HIGH** | Partition validation and reboot coordination. |
| `main/vendor/m5/m5stack_keyboard.c:315` | **`tca_kbd_task`** | **2,560 bytes** | **MEDIUM** | I2C key matrix scanning and event queue dispatching. |

> [!WARNING]
> Any FreeRTOS task performing FATFS filesystem operations (`fopen`, `fprintf`, `fwrite`) or calling `snprintf()` with float/64-bit specifiers must be allocated with **a minimum stack of 4,096 bytes** (8,192 bytes recommended for PSRAM tasks).

---

## 5. Unsafe Dynamic Memory Allocations & Leaks

### 5.1 Unchecked Heap Allocations
Multiple scanning and network modules invoke `malloc()` or `heap_caps_malloc()` without validating that the returned pointer is non-NULL before dereferencing or passing to system APIs:

#### A. Active Subnet Scanner (`main/scans/wifi/arp_scan.c`)
```c
// main/scans/wifi/arp_scan.c:186
arp_tcpip_call_t *call = malloc(sizeof(arp_tcpip_call_t));
// No NULL check! Passed directly into tcpip adapter:
esp_netif_tcpip_exec(arp_send_query_tcpip, call);
```
If internal RAM is exhausted during an intense scan, `call` is `NULL`, causing an immediate kernel panic inside the LwIP TCP/IP thread.

#### B. SMB / NetBIOS Enumerator (`main/scans/wifi/enum4linux_scan.c`)
```c
// main/scans/wifi/enum4linux_scan.c:802
uint8_t *bufs = malloc(2 * SMB_MAX_BUF); // 2 * 4096 = 8192 bytes
// Unchecked dereference:
memset(bufs, 0, 2 * SMB_MAX_BUF);
```
Requesting an 8 KB contiguous buffer from internal RAM can easily fail if memory is fragmented, resulting in an instant crash.

#### C. Display View Switching (`main/managers/display_manager.c`)
```c
// main/managers/display_manager.c:3991
dm_lvgl_call_t *call = malloc(sizeof(*call));
// Unchecked call passed into async runner:
display_manager_lvgl_async_call(dm_switch_view_async_cb, call);
```

### 5.2 PSRAM vs. Internal DRAM Misallocations
- **DMA Restrictions:** Hardware peripherals (SPI2 for NRF24, SPI3 for ST7735S display, RMT for WS2812B/IR, and Wi-Fi MAC TX buffers) **cannot** read directly from external Octal PSRAM via standard DMA without specialized EDMA/GDMA configurations. Passing a PSRAM-allocated buffer to `esp_wifi_80211_tx()` or `spi_device_polling_transmit()` triggers silent data corruption or hardware DMA errors.
- **`EXT_RAM_BSS_ATTR` Overhead:** Large static buffers tagged with `EXT_RAM_BSS_ATTR` reside in PSRAM. While this saves internal DRAM, accessing PSRAM across the 40 MHz SPI bus introduces significant cache-miss latency during real-time promiscuous packet filtering.

---

## 6. Error Handling Gaps & Silent Failures

### 6.1 Ignored API Return Codes
Across raw 802.11 transmission paths, `esp_err_t` return codes from `esp_wifi_80211_tx()` are frequently ignored:

```c
// main/features/wifi/beacon_spam.c:266
esp_wifi_80211_tx(WIFI_IF_AP, packet, packet_size, false);
```
When injecting bursts at 1,000 packets/sec, internal Wi-Fi TX buffers are quickly exhausted, returning `ESP_ERR_NO_MEM`. The code ignores the error, failing to implement adaptive backoff. This causes silent packet drop and inaccurate statistics reporting.

### 6.2 SD-Card Dependency in Infrared Init
In `main/managers/infrared_manager.c:70-73`:
```c
bool infrared_manager_init(void) {
    bool ok = sd_card_manager.is_initialized;
#ifdef CONFIG_HAS_INFRARED
    if (ok && CONFIG_HAS_INFRARED) {
        gpio_reset_pin(infrared_get_tx_pin());
        ...
```
The initialization of the physical IR LED GPIO pin is conditionally gated on `sd_card_manager.is_initialized`! If the user boots a board without a MicroSD card inserted, `infrared_manager_init()` aborts, leaving the IR transmitter completely disabled even for hardcoded TV-B-Gone or CLI commands that do not touch the filesystem!

---

## 7. Monolithic Architecture & Code Duplication

### 7.1 "God Files" Exceeding Maintainability Limits
The codebase contains immense monolithic source files where business logic, hardware drivers, GUI rendering, and packet decoding are fused into single files:

1. **`main/managers/views/options_screen.c` (15,200+ Lines):**  
   Contains the entire menu tree, settings serialization, WiGLE CSV file viewers, PCAP file managers, Evil Portal pickers, and BLE trackers in a single massive file. Editing one menu item requires recompiling a 15,000-line translation unit.
2. **`main/managers/display_manager.c` (6,390+ Lines):**  
   Combines display bus initialization, LVGL tick management, backlights, hardware input dispatching, view routing, screen locking, and auto-rotation.
3. **`main/core/callbacks.c` (3,850+ Lines):**  
   Handles 802.11 promiscuous packet demuxing, EAPOL 4-way handshake detection, PMKID extraction, WiGLE CSV formatting, PineAP detection, WPS parsing, PCAP buffer pool drainage, and BLE skimmer/wardriving parsing.
4. **`main/managers/wifi_manager.c` (5,100+ Lines):**  
   Contains Wi-Fi STA, SoftAP, Evil Portal HTTP server, captive DNS server, promiscuous filters, and attack coordinator logic.

### 7.2 Duplicated Wi-Fi Attack Boilerplate
Nine separate attack implementations (`auth_flood.c`, `bad_msg.c`, `beacon_spam.c`, `channel_switch_test.c`, `dhcp_starvation.c`, `eapol_logoff.c`, `gtk_abuse.c`, `probe_request_flood.c`, `reconnect_test.c`) duplicate virtually identical architectures:
- Independent global task handles (`*_task_handle`)
- Identical `while(handle != NULL && wait_count < 100)` stopping loops
- Duplicated 802.11 header byte arrays
- Duplicate packet counter variables and status display invocations

### 7.3 Recommended Refactoring Strategy

```mermaid
flowchart TD
    subgraph Current Monolithic Debt
        CB["callbacks.c (3,850 Lines)"]
        OPT["options_screen.c (15,200 Lines)"]
        DM["display_manager.c (6,390 Lines)"]
        ATK["9x Attack Files (Duplicated Boilerplate)"]
    end

    subgraph Proposed Modular Architecture (God Mode V2)
        HAL["Hardware Abstraction Layer (Unified GPIO/Buses)"]
        AE["Unified Attack Engine (Base Class / Driver API)"]
        PE["Promiscuous Packet Pipeline (Worker Pool)"]
        MV["Modular View Registry (Individual Screen Files)"]
    end

    CB -.-> PE
    OPT -.-> MV
    DM -.-> HAL
    ATK -.-> AE
```

---

## 8. Prioritized Technical Debt Remediation Roadmap

| Priority | Issue / Risk Area | Affected Files | Remediation Action |
| :---: | :--- | :--- | :--- |
| **P0** | **Headless LVGL Context Panic** | `main/managers/display_manager.c:3956` | Update `display_manager_is_lvgl_task()` to return `false` when `lvgl_task_handle == NULL`, and verify display availability before executing callbacks. |
| **P0** | **Stack Overflow in Logger Task** | `main/core/callbacks.c:2547` | Increase `pineap_logw` task stack size from **1,792 bytes to 4,096 bytes**. |
| **P1** | **Wi-Fi Callback Mutex Lockup** | `main/core/callbacks.c:1378`, `main/core/glog.c` | Never invoke blocking `glog()` inside packet callbacks; post handshake events to a lightweight queue drained by an async worker. |
| **P1** | **CLI Stalls on Attack Stop** | `main/managers/wifi_manager.c:5031`, `auth_flood.c` | Replace blocking 100 ms while-loops with FreeRTOS task notifications (`ulTaskNotifyTake`) and 500 ms hard timeouts. |
| **P1** | **IR Init Gated on SD Mount** | `main/managers/infrared_manager.c:70` | Decouple IR LED GPIO pin setup from `sd_card_manager.is_initialized`. |
| **P2** | **Small Task Stacks ($\le 2048$ B)** | `eapol_logoff.c`, `sae_flood.c`, `eth_arp_poison.c` | Standardize all worker task stacks to a minimum of **4,096 bytes**. |
| **P2** | **Unchecked `malloc` in Scanners** | `arp_scan.c:186`, `enum4linux_scan.c:802` | Add explicit NULL checks and graceful error returns before invoking TCP/IP or memory operations. |
| **P3** | **Hardcoded GPIO Magic Numbers** | `main.c`, `display_manager.c` | Refactor all raw pin numbers into centralized board definitions in `include/core/board_pins.h`. |
| **P3** | **Monolithic God File Refactoring**| `options_screen.c`, `callbacks.c` | Split into sub-modules (e.g. `gui/menu_options/`, `core/packet_parsers/`). |

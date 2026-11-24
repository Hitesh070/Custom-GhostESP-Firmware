# GhostESP God Mode V3 – Module & Subsystem Specification

**Document Version:** 3.0.0  
**Target Hardware:** 7Semi ESP32-S3-WROOM-1-N16R8  
**Framework:** ESP-IDF v6.1.1 (FreeRTOS Dual-Core Xtensa LX7)  

---

## 1. Modular Subsystem Interface Standard

All newly engineered subsystems in God Mode V3 adhere to the standard lifecycle interface:

```c
typedef struct {
    const char *module_name;
    esp_err_t (*init)(void);
    esp_err_t (*start)(void);
    esp_err_t (*stop)(void);
    esp_err_t (*deinit)(void);
    bool      (*is_active)(void);
    void      (*dump_status)(void);
} ghost_module_t;
```

---

## 2. Core Subsystems

### 2.1 Rogue AP & Evil Twin Detection Engine (`main/modules/rogue_detector.c`)
- **Objective:** Multi-vector correlation of observed 802.11 beacon frames to detect malicious clone access points.
- **Vectors Analyzed:**
  1. *Duplicate SSID with Divergent BSSID:* Flags identical SSID broadcast from uncoordinated OUI.
  2. *Security Downgrade:* Flags network announcing lower security tier (e.g. WPA3-SAE -> Open or WPA2-PSK -> WEP).
  3. *Channel Inconsistency:* Flags network appearing on unexpected channel without 802.11h Channel Switch Announcement (CSA).
  4. *RSSI Cliff / Spike:* Sudden inexplicable +30 dBm increase in received power indicating nearby rogue transmitter.
- **Data Model:**
  ```c
  typedef struct {
      uint8_t  bssid[6];
      char     ssid[33];
      uint8_t  channel;
      int8_t   rssi;
      uint16_t security_mask;
      uint32_t first_seen_ms;
      uint32_t last_seen_ms;
      bool     is_suspicious;
      char     reason[64];
  } rogue_ap_candidate_t;
  ```

### 2.2 Passive Deauthentication / Disassociation Activity Monitor (`main/modules/deauth_monitor.c`)
- **Objective:** Detect active wireless denial-of-service and reconnection pressure in the airspace without transmitting RF.
- **Metrics Collected:**
  - Deauth Frame Count (Total & Per Second rate).
  - Disassoc Frame Count.
  - Source MAC (Transmitter BSSID / Spoofed Client).
  - Target MAC (Unicast victim or Broadcast `FF:FF:FF:FF:FF:FF`).
  - Reason Code (802.11 reason code, e.g. Reason 7: Class 3 frame received from nonassociated STA).
  - Alert trigger when rate exceeds configurable threshold (default: 5 frames/sec).

### 2.3 NRF24 RF Diagnostic & Link Benchmark Subsystem (`main/modules/nrf24_lab.c`)
- **Objective:** Provide register-level visibility and link testing using the external NRF24L01+PA+LNA transceiver on `SPI2_HOST`.
- **Capabilities:**
  1. *Register Inspector:* Decodes all 24 NRF24 configuration registers (`CONFIG`, `EN_AA`, `EN_RXADDR`, `SETUP_AW`, `SETUP_RETR`, `RF_CH`, `RF_SETUP`, `STATUS`, `OBSERVE_TX`, `RPD`, `RX_ADDR_P0-P5`, `FIFO_STATUS`, `DYNPD`, `FEATURE`).
  2. *Channel Activity Occupancy:* Measures carrier detect (RPD) across 126 channels (2.400–2.525 GHz).
  3. *Link Benchmark:* Controlled packet delivery ratio (PDR) and round-trip latency measurement between paired research nodes.

### 2.4 Configuration Profile Engine (`main/modules/profile_manager.c`)
- **Objective:** Provide instant, reproducible system configurations tailored for specific research environments.
- **Profiles Defined:**
  - `PROFILE_WIFI_RESEARCH`: Enables promiscuous RX, rogue detection, deauth monitor. Backlight 80%.
  - `PROFILE_BLE_RESEARCH`: Enables BLE scanner, GATT explorer, radar tracking.
  - `PROFILE_NRF24_LAB`: Powers up SPI2 NRF24 front-end, initiates channel occupancy sweep.
  - `PROFILE_IR_LAB`: Enables non-blocking RMT IR receiver, timing decoder.
  - `PROFILE_USB_LAB`: Prepares USB-OTG HID macro testbench.
  - `PROFILE_DIAGNOSTICS`: Runs complete hardware self-test suite.
  - `PROFILE_COMBINED`: Balanced multi-protocol monitoring.
  - `PROFILE_CUSTOM`: User-defined parameter preset in NVS.

### 2.5 Automated Firmware Self-Test Suite (`main/modules/self_test.c`)
- **Objective:** Verify physical silicon, buses, memory, and transceivers at boot or on demand.
- **Test Matrix:**
  - Internal SRAM integrity & free heap headroom.
  - 8 MB Octal PSRAM allocation test (`heap_caps_malloc(1024*1024, MALLOC_CAP_SPIRAM)`).
  - I2C0 Bus ACK verification (SDA=8, SCL=9).
  - SPI2 NRF24 register readback (`CONFIG == 0x08` or `0x0F` or `0x03`).
  - SPI3 Display driver state.
  - UART1 / UART2 loopback / presence.
  - SD Card presence & FAT mount state.
  - WS2812B RGB NeoPixel test sequence on GPIO 48.

### 2.6 Easter Eggs & Cyberpunk UI Subsystem (`main/modules/ghost_eastereggs.c`)
- **Matrix Mode:** LVGL canvas rendering animated green digital rain characters with decaying phosphorescent trails.
- **Ghost Radar:** Circular polar sweep showing relative signal strength nodes for active Wi-Fi and BLE emitters.
- **Ghost Companion:** Interactive animated mascot displaying mood, diagnostics, and milestone achievements.
- **Secret Developer Console:** Shell namespace under `ghost <command>` (`help`, `status`, `matrix`, `radar`, `diagnostics`, `easteregg`, `version`).
- **Achievement System:** Tracks milestones (First Scan, 100 APs observed, NRF24 probe passed, Self-test clean) in NVS.

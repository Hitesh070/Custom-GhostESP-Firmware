# Comprehensive Catalog of Features: Custom GhostESP Firmware

**Target Architecture:** ESP32-S3 (Xtensa Dual-Core LX7 @ 240 MHz + LP core)  
**Hardware Profile:** 7Semi ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB Octal PSRAM)  
**Framework Version:** ESP-IDF v6.1.1  
**Audit Date:** November 24, 2026  
**Auditor:** Agent 1 - Repository & Architecture Auditor (GhostESP God Mode V2)

---

## 1. Classification Methodology

Every feature within the GhostESP codebase has been inspected directly in the source code (`main/`, `include/`, `components/`) and classified under one of the following six rigorous engineering tiers:

1. **Fully Functional:** The feature is complete, compiled, tested, and works as expected on the target hardware without critical bugs or missing dependencies.
2. **Partially Functional:** The core logic exists and runs, but certain sub-commands, edge-case handlings, or complementary features are incomplete or degraded.
3. **Experimental:** Implemented as a proof-of-concept or preliminary implementation; may suffer from timing instabilities, memory leaks, or unhandled protocol edge-cases.
4. **Broken:** Code is present in the repository, but will panic, freeze, fail compilation without stubs, or abort at runtime due to known architectural or logical defects.
5. **Hardware-Dependent:** Fully functional or experimental, but requires external hardware modules beyond the baseline ESP32-S3 (e.g. NRF24L01+, CC1101, SX1262, PN532, GPS, ST7735S display, MicroSD card, Audio DAC).
6. **Missing / Not Implemented:** Features referenced in community forks, UI mockups, or related tools (e.g. Marauder / Flipper) that are either completely absent or stubbed out with empty functions in this repository.

---

## 2. Wi-Fi Subsystem (802.11 b/g/n)

### 2.1 Wi-Fi Reconnaissance & Auditing

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Access Point Scan** | `scanap` / `scan -w` | **Fully Functional** | Active & passive 2.4 GHz channel scan. Parses beacon/probe frames, captures SSID, BSSID, RSSI, channel, encryption (WPA/WPA2/WPA3), and WPS status. Stores in `scanned_aps[]`. (`main/scans/wifi/ap_scan.c`) |
| **Station (Client) Scan** | `scansta` | **Fully Functional** | Promiscuous sniffer capturing data/mgmt frames to map active client station MACs to AP BSSIDs. (`main/scans/wifi/station_scan.c`) |
| **Local Subnet Scan** | `scanlocal` | **Fully Functional** | Requires STA connection. Sends ARP requests and reverse DNS lookups across the active subnet (`/24`) to enumerate live IP/MAC/Hostnames. (`main/scans/wifi/local_scan.c`) |
| **Stop Active Scans** | `stopscan` | **Fully Functional** | Safely detaches promiscuous callbacks, terminates background tasks, and unregisters radio handlers. (`main/managers/wifi_manager.c`) |
| **Target Selection** | `select -a`, `select -s` | **Fully Functional** | Selects target APs or Stations from scan tables by index or MAC for focused attacks or tracking. (`main/core/commands/cmd_scan.c`) |
| **AP / Station Tracking** | `trackap`, `tracksta` | **Fully Functional** | Locks onto a selected BSSID/MAC, measures RSSI in real-time, and renders a live signal-strength meter. (`main/managers/wifi_manager.c`) |
| **Airspace Channel Hopping** | `hop` | **Fully Functional** | Configures automatic channel hopping (dwell time 100–1000 ms) across channels 1–14 or custom channel masks. (`main/scans/wifi/hop_profile.c`) |
| **Channel Congestion** | `congestion` | **Fully Functional** | Sweeps 2.4 GHz channels 1–13, measures frame density and packet volume per channel, identifying optimal/crowded frequencies. (`main/managers/views/channel_congestion_screen.c`) |
| **Probe Request Listener**| `listenprobes` | **Fully Functional** | Sniffs 802.11 probe requests from nearby client devices searching for preferred networks, displaying client MAC, requested SSID, and RSSI. (`main/core/callbacks.c`) |
| **WPA3 Compliance Check** | `wpa3check` | **Fully Functional** | Evaluates beacons of surrounding APs to detect WPA3-Personal (SAE), Transition Mode, and Protected Management Frames (PMF) enforcement. (`main/scans/wifi/wpa3_compliance.c`) |
| **PineAP Detection** | `pineap` | **Fully Functional** | Detects WiFi Pineapple activity by monitoring for rapid SSID karma pools, known Hak5 OUIs (`00:13:37`), and suspicious broadcast association patterns. (`main/core/callbacks.c`) |
| **WPS Detection** | Automatic in `scanap` | **Fully Functional** | Parses WPS Information Elements (IE 0xDD) in beacon frames, extracting manufacturer, model, and lock status. (`main/core/callbacks.c`) |
| **OUI MAC Resolution** | Built-in | **Fully Functional** | Resolves vendor names from MAC prefixes using an embedded compressed IEEE OUI lookup table (`core/ouis.bin`). (`main/core/ouis.c`) |

### 2.2 Wi-Fi Offensive & Security Auditing Engine

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Deauthentication Attack** | `attack -d`, `reconnect` | **Fully Functional** | Injects raw 802.11 deauthentication and disassociation frames to APs and clients using `esp_wifi_80211_tx()`. Supports targeted and broadcast modes. (`main/features/wifi/reconnect_test.c`) |
| **Beacon Spam (Random)** | `beaconspam -r` | **Fully Functional** | Rapidly transmits 802.11 beacon frames with randomized SSIDs and BSSIDs, populating nearby client device scan lists with phantom networks. (`main/features/wifi/beacon_spam.c`) |
| **Beacon Spam (Custom List)**| `beaconspam -l` | **Fully Functional** | Transmits beacon frames iterating over a user-defined list of SSIDs managed via `beaconadd`, `beaconremove`, and `beaconclear`. (`main/features/wifi/beacon_spam.c`) |
| **Beacon Spam (AP Clone)** | `beaconspam -c` | **Fully Functional** | Clones SSIDs of all scanned access points and broadcasts cloned beacons across channels to confuse client roaming. (`main/features/wifi/beacon_spam.c`) |
| **Authentication Flood** | `attack -a` | **Fully Functional** | Floods target AP with randomized 802.11 Open System Authentication frames to exhaust the AP's client association tables. (`main/features/wifi/auth_flood.c`) |
| **Probe Request Flood** | `attack -p` | **Fully Functional** | Injects bursts of directed and wildcard probe requests to overwhelm AP processing queues and trigger probe response storms. (`main/features/wifi/probe_request_flood.c`) |
| **Channel Switch Announce (CSA)**| `attack -c` | **Experimental** | Injects 802.11 Action frames with Channel Switch Announcement (CSA) IEs, attempting to force connected clients to jump to rogue frequencies. (`main/features/wifi/channel_switch_test.c`) |
| **Bad Message Fuzzing** | `attack -b` | **Experimental** | Transmits malformed 802.11 management frames (invalid lengths, illegal IE tags) to test AP/client parser stability. (`main/features/wifi/bad_msg.c`) |
| **EAPOL Logoff Attack** | `attack -e` | **Fully Functional** | Injects forged EAP-Failure / EAPOL-Logoff frames to disconnect authenticated 802.1X enterprise clients. (`main/features/wifi/eapol_logoff.c`) |
| **WPA3 SAE Commit Flood** | `saeflood` | **Partially Functional** | Transmits rapid Simultaneous Authentication of Equals (SAE) commit frames to exhaust WPA3 Dragonfly crypto coprocessors on APs. (`main/features/wifi/sae_flood.c`) |
| **DHCP Starvation** | `dhcpstarve` | **Fully Functional** | Requires association to AP. Floods the local DHCP server with `DHCPDISCOVER` packets utilizing randomized MAC addresses to exhaust the DHCP IP pool. (`main/features/wifi/dhcp_starvation.c`) |
| **Karma (SSID Lure)** | `karma start` | **Fully Functional** | Listens for client probe requests searching for remembered networks, dynamically creates matching SoftAPs, and lures clients into connecting. (`main/managers/wifi_manager.c:4738`) |
| **GTK Reinstallation Abuse** | `attack -g` | **Broken / Stubbed** | Code stubs exist (`gtk_abuse_stub.c`), but full WPA2 GTK key reinstallation attack logic is commented out due to ESP-IDF 6.1 MAC layer limitations. (`main/features/wifi/gtk_abuse.c`) |

### 2.3 Wi-Fi Infrastructure & Captive Portals

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **SoftAP Management** | `apenable`, `apcred` | **Fully Functional** | Configures and brings up an 802.11 SoftAP with configurable SSID, password, channel, and hidden status. (`main/managers/ap_manager.c`) |
| **Evil Portal Engine** | `startportal`, `stopportal` | **Hardware-Dependent** | Runs an HTTP captive portal daemon. Serves phishing login pages (Google, Router, Starbucks, etc.) from SD card (`/mnt/ghostesp/evil_portal/portals/`) or built-in flash fallback. Captures credentials. (`main/core/commands/cmd_portal.c`) |
| **UART HTML Portal Streaming**| `evilportal sethtmlstr` | **Fully Functional** | Allows streaming raw HTML portal templates into memory over serial using `[HTML/BEGIN]` and `[HTML/CLOSE]` markers, enabling portal hosting without an SD card. (`main/core/serial_manager.c`) |
| **Captive DNS Redirect** | Built-in | **Fully Functional** | High-performance multi-threaded UDP DNS server answering all domain queries with the ESP32's SoftAP IP (`192.168.4.1`) to trigger OS captive portal detection. (`main/core/dns_server.c`) |
| **DNS Sinkhole** | `sinkhole` | **Hardware-Dependent** | Uses SD card blocklists (`/mnt/ghostesp/blocklists/`) or embedded bloom filter to sinkhole telemetry and ad domains. (`main/core/dns_server.c`) |
| **HTTPS Interception Sinkhole**| `sinkhole https` | **Experimental** | TLS server intercepting port 443 requests using self-signed or CA certificates to inspect secure requests. (`main/core/sinkhole_https.c`) |
| **Wi-Fi STA Client** | `connect`, `disconnect` | **Fully Functional** | Connects to standard WPA2/WPA3 access points, obtains IP via DHCP, and enables network scanning or OTA downloads. (`main/managers/wifi_manager.c`) |

### 2.4 Packet Capture & PCAP Engine

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **SD Card PCAP Logging** | `capture -w`, `-d`, `-e`, `-p` | **Hardware-Dependent** | Captures raw 802.11 frames to MicroSD card in standard libpcap format with Radiotap headers. Supports filters for Beacons, Deauths, EAPOL, Probes, or All frames. (`main/vendor/pcap.c`) |
| **Live Wireshark Stream** | `capture -wireshark` | **Fully Functional** | Streams raw PCAP binary frames directly over USB-Serial/JTAG or UART to a host PC running `wireshark -k -i -`. (`main/vendor/pcap.c`) |
| **WPA Handshake Sniffer** | Built-in | **Fully Functional** | Tracks EAPOL 4-way handshakes (M1 through M4). Upon capturing an M1+M2 or M2+M3 pair with valid replay counters, logs completion, pulses RGB LED, and emits a GhostScript event. (`main/core/callbacks.c`) |
| **PMKID Extraction** | `capture -pmf` | **Fully Functional** | Extracts PMKID from initial EAPOL M1 association frames for offline cracking with Hashcat (mode 16800/22000) without requiring full 4-way handshake. (`main/core/callbacks.c`) |

---

## 3. Bluetooth Low Energy (BLE) Subsystem

### 3.1 BLE Reconnaissance & Threat Detection

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **BLE General Scan** | `blescan` | **Fully Functional** | Passive and active GAP scanner using Apache NimBLE. Identifies address, address type (public/random), RSSI, and raw AD payload. (`main/managers/ble_manager.c`) |
| **BLE Device Detect (Sentry)**| `bledetect` | **Fully Functional** | High-level device classifier detecting Apple AirTags, AirPods, Watches, Samsung SmartTags, Tile, Chipolo, Google Fast Pair, Ray-Ban Meta glasses, and Flipper Zero devices. (`main/scans/ble/device_detect_scan.c`) |
| **Card Skimmer Detection** | `capture -skimmer` | **Fully Functional** | Sniffs for known Bluetooth serial modules (HC-03, HC-05, HC-06, RN-42, BT-05) commonly deployed in gas pump and ATM card skimmers. Pulses red LED on detection. (`main/core/callbacks.c`) |
| **AirTag Detection & Tracking**| `listairtags`, `selectairtag` | **Fully Functional** | Detects Apple FindMy offline payload patterns (Company ID `0x004C`, Type `0x12`), logs rotating public keys, and monitors proximity. (`main/scans/ble/airtag_scan.c`) |
| **Flipper Zero Detection** | `listflippers` | **Fully Functional** | Identifies Flipper Zero devices by service UUIDs (`0x3081` Black, `0x3082` White, `0x3083` Transparent) and device names. (`main/scans/ble/flipper_scan.c`) |
| **Surveillance Camera Detect** | Built-in | **Fully Functional** | Detects Nest Weave (`0xFEAF`) and Google Matter (`0xFFF6`) commissioning advertisements emitted by smart cameras. (`main/scans/ble/device_detect_scan.c`) |
| **GATT Service Discovery** | `listgatt`, `enumgatt` | **Fully Functional** | Connects to selected BLE peripherals, enumerates GATT primary services, characteristics, and descriptors. (`main/scans/ble/gatt_scan.c`) |
| **GATT Characteristic Tracking**| `trackgatt` | **Fully Functional** | Subscribes to GATT notifications/indications or periodically polls characteristic values to track live telemetry. (`main/scans/ble/gatt_scan.c`) |
| **BLE Wardriving** | `blewardriving` | **Hardware-Dependent** | Combines BLE device discovery with GPS coordinates, formatting records into WiGLE BLE CSV format on SD card. (`main/core/callbacks.c`) |
| **Dual Wardriving (WiFi+BLE)**| `dualwd` | **Hardware-Dependent** | Concurrently runs 802.11 AP discovery and BLE device discovery alongside GPS, logging both to SD card simultaneously. (`main/core/callbacks.c`) |

### 3.2 BLE Offensive Operations & Emulation

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **BLE Spam (Apple Continuity)**| `blespam -a` | **Fully Functional** | Advertises Apple Continuity packets: Proximity Pair popups for AirPods Pro, AirPods Max, Vision Pro, AppleTV Setup, Watch, and Action modals. (`main/features/ble/ble_spam.c`) |
| **BLE Spam (Google Fast Pair)**| `blespam -g` | **Fully Functional** | Transmits Google Fast Pair model identification frames (`0xFE2C`), triggering instant setup notifications on nearby Android devices. (`main/features/ble/ble_spam.c`) |
| **BLE Spam (Samsung EasySetup)**| `blespam -s` | **Fully Functional** | Transmits Samsung EasySetup beacons (`0x0075`), prompting Galaxy devices to connect to phantom Galaxy Buds. (`main/features/ble/ble_spam.c`) |
| **BLE Spam (Microsoft SwiftPair)**| `blespam -m` | **Fully Functional** | Transmits Microsoft SwiftPair advertisements (`0x0006`), displaying pairing dialogs on Windows 10/11 machines. (`main/features/ble/ble_spam.c`) |
| **AirTag Spoofing** | `spoofairtag` | **Fully Functional** | Re-broadcasts the advertisement payload of a previously captured AirTag, cloning its beacon signal to mislead Apple FindMy location networks. (`main/scans/ble/airtag_scan.c`) |
| **Chameleon Ultra RFID CLI** | `chameleon` | **Experimental** | BLE client connecting to a Chameleon Ultra RFID emulator to change slots, dump UIDs, or trigger card emulation over BLE. (`main/managers/chameleon_manager.c`) |
| **BLE AT-Command Bridge** | `blebridge` | **Fully Functional** | Transparent bi-directional bridge routing serial console data through a custom BLE GATT characteristic. (`main/managers/ble_bridge_manager.c`) |

---

## 4. Sub-GHz & 2.4 GHz RF Operations

### 4.1 NRF24L01+ 2.4 GHz Transceiver Subsystem

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **2.4 GHz Spectrum Analyzer** | `nrf24 start`, `nrf24 stop` | **Hardware-Dependent** | Uses NRF24L01+ over SPI (`SPI2_HOST`). Sweeps channels 0–125 (2.400–2.525 GHz), samples Received Power Detector (RPD), and measures carrier activity. (`main/managers/nrf24_remote_manager.c`) |
| **Graphical Spectrum Waterfall**| UI: `NRF24 View` | **Hardware-Dependent** | Renders live 126-channel RF spectrum plot on ST7735S display with dynamic peak-hold and frequency axis labels. (`main/managers/views/nrf24_analyzer_view.c`) |
| **2.4 GHz Jamming Detection** | Built-in | **Hardware-Dependent** | Analyzes energy distribution across broad channel bands (WiFi ch 1/6/11); triggers alert if full-band continuous carrier saturation is detected. (`main/managers/views/nrf24_analyzer_view.c`) |
| **GhostLink NRF24 Stream** | `nrf24 start` (from peer) | **Hardware-Dependent** | Transmits binary NRF24 spectral sweep chunks over UART to a connected GhostLink companion display. (`main/managers/nrf24_remote_manager.c`) |
| **Mousejacking (RF Injection)**| None | **Missing** | **NOT IMPLEMENTED.** No code exists for Logitech Unifying or Nordic shockburst mouse packet injection. |
| **2.4 GHz RF Jammer** | None | **Missing** | **NOT IMPLEMENTED.** The NRF24 driver operates strictly in RX carrier detection mode; active TX carrier jamming is not implemented. |

### 4.2 CC1101 Sub-GHz Transceiver Subsystem

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Multi-Frequency Spectrum** | `subghz start` | **Hardware-Dependent** | Controls TI CC1101 over SPI. Sweeps frequencies: 315 MHz, 390 MHz, 433.92 MHz, 868.35 MHz, 915 MHz. Measures RSSI. (`main/managers/subghz_remote_manager.c`) |
| **Raw Signal Recording** | `subghz rec <file>` | **Hardware-Dependent** | Captures raw OOK pulse timings from CC1101 GDO0/GDO2 via high-speed GPIO interrupt, saving to SD card in Flipper Zero `.sub` format. (`main/managers/subghz_remote_manager.c`) |
| **Signal Replay / Transmission**| `subghz tx <file>` | **Hardware-Dependent** | Reads `.sub` pulse-width files from SD card and re-transmits via CC1101 transmitter or RMT hardware. (`main/managers/subghz_remote_manager.c`) |
| **Sub-GHz Protocol Decoders** | Built-in | **Hardware-Dependent** | Decodes fixed and rolling code signals: Princeton (PT2262), KeeLoq 64-bit, Nice Flo, Came, Linear, and Chamberlain. (`main/managers/subghz_decoders.c`) |
| **GhostLink Sub-GHz Stream** | `subghz` (from peer) | **Hardware-Dependent** | Streams Sub-GHz waterfall and RSSI buffers over GhostLink UART bus to companion display. (`main/managers/subghz_remote_manager.c`) |

---

## 5. LoRa & Mesh Networking Subsystem

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Semtech SX1262 LoRa Radio** | `lora config`, `lora start` | **Hardware-Dependent** | Configures SX1262 transceiver over SPI: frequency (433/868/915 MHz), bandwidth (125/250/500 kHz), spreading factor (SF7–SF12), coding rate (4/5–4/8). (`main/managers/lora_sx1262.c`) |
| **Meshtastic Packet Interop** | `meshtastic send`, `recv` | **Hardware-Dependent** | Decodes and transmits Protobuf-formatted Meshtastic mesh packets. Supports channel encryption with standard default keys. (`main/managers/lora_manager.c`) |
| **MeshCore Encrypted Mesh** | `meshcore start`, `mc send`| **Hardware-Dependent** | Proprietary lightweight mesh networking protocol utilizing Ed25519 public key signatures and Curve25519 key exchange for authenticated ad-hoc text messaging. (`main/managers/meshcore_manager.c`) |
| **Mesh BLE Companion Phone API**| Built-in | **Hardware-Dependent** | Emulates a BLE serial companion interface allowing mobile phone apps to send/receive mesh messages through the GhostESP radio. (`main/managers/meshcore_ble.c`) |
| **Mesh Auto-Start on Boot** | Built-in | **Fully Functional** | Restores saved mesh protocol from NVS on boot. Intentionally deferred until after display/SD memory allocation to avoid internal DRAM starvation. (`main/main.c:225`) |

---

## 6. NFC & High-Frequency RFID Subsystem

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Dual NFC Hardware Backend** | `nfc backend <auto\|pn532\|st25r>` | **Hardware-Dependent** | Abstracted hardware layer supporting both NXP PN532 (I2C/SPI) and STMicroelectronics ST25R3916 frontends with persistent NVS backend selection. (`main/managers/nfc/nfc_backend.c`) |
| **MIFARE Classic 1K/4K Read** | `nfc scan` | **Hardware-Dependent** | Reads MIFARE Classic cards using standard dictionary attacks against embedded key tables (`mf_classic_dict.nfc`). (`main/managers/nfc/mifare_classic.c`) |
| **NTAG / Ultralight (Type 2)** | `nfc scan` | **Hardware-Dependent** | Dumps memory blocks of NTAG213, NTAG215, NTAG216, and Ultralight EV1 tags. (`main/managers/nfc/ntag_t2.c`) |
| **NDEF Record Extraction** | Built-in | **Hardware-Dependent** | Parses NFC Data Exchange Format (NDEF) payloads, extracting stored URLs, plain text, vCards, or MIME records. (`main/managers/nfc/ndef.c`) |
| **NDEF URL / Text Writing** | `nfc write <url>` | **Hardware-Dependent** | Encodes and writes custom NDEF URL or Text records to blank NTAG21x tags. (`main/managers/nfc/write_ntag.c`) |
| **DESFire Identification** | `nfc scan` | **Hardware-Dependent** | Detects ISO14443-4 MIFARE DESFire cards, reads Version information and Application IDs (AIDs). (`main/managers/nfc/desfire.c`) |
| **Picopass / iClass Reading** | `nfc scan` | **Hardware-Dependent** | Reads Picopass / HID iClass tags, decrypting secure blocks via integrated optimized loclass DES cipher engine. (`main/managers/nfc/picopass.c`) |
| **EMV Bankcard Reader** | `nfc scan` | **Hardware-Dependent** | Issues APDU commands to Visa/Mastercard EMV contactless chips, parsing Primary Account Number (PAN), expiry date, and cardholder name. (`main/managers/nfc/emv.c`) |
| **Flipper Zero NFC Compatibility**| Built-in | **Hardware-Dependent** | Parses and loads 30+ card formats from Flipper Zero `.nfc` files (Amiibo, Disney Infinity, Troika, Myki, Clipper, Metromoney, Saflok, etc.). (`main/managers/nfc/flipper_nfc_compat.c`) |

---

## 7. Infrared (IR) Operations

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Infrared Pulse Transmission** | `ir tx <protocol> <addr> <cmd>` | **Hardware-Dependent** | Generates 38 kHz carrier modulated signals using the ESP32-S3 RMT peripheral on GPIO. (`main/managers/infrared_manager.c`) |
| **Infrared Protocol Decoders** | `ir rx` | **Hardware-Dependent** | Decodes demodulated 38 kHz receiver signals: NEC, Samsung, Sony (SIRC 12/15/20-bit), Philips RC5, RC6, Kaseikyo, Apple, Sharp. (`main/managers/infrared_protocols.c`) |
| **Flipper `.ir` File Replay** | `ir play <file>` | **Hardware-Dependent** | Parses standard Flipper Zero `.ir` files from SD card and transmits individual or sequential remote commands. (`main/managers/infrared_manager.c`) |
| **Signal Learning & Saving** | `ir learn <name>` | **Hardware-Dependent** | Captures raw timing sequences from an unknown remote and formats them directly into a valid `.ir` file on SD card. (`main/managers/views/infrared_view.c`) |
| **TV-B-Gone Brute Force** | `ir tvbgone` | **Hardware-Dependent** | Rapidly iterates through an embedded database of known television POWER codes across US and European manufacturer standards. (`main/managers/infrared_manager.c`) |
| **Universal Remote Presets** | UI: `Infrared View` | **Hardware-Dependent** | Graphical remote control layouts for common projectors, televisions, soundbars, and AC units. (`main/managers/views/infrared_view.c`) |

---

## 8. BadUSB & BadBLE Keystroke Injection

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **BadUSB Device Stack** | `badusb start`, `stop` | **Fully Functional** | Uses ESP32-S3 native USB OTG port via TinyUSB. Registers composite HID device: Keyboard + Mouse + Gamepad. (`main/managers/badusb_manager.c`) |
| **BadBLE GATT Keyboard** | `badble start`, `stop` | **Fully Functional** | Emulates a Bluetooth LE wireless HID keyboard via NimBLE. Pairs with mobile phones, tablets, and PCs without physical USB cables. (`main/managers/badble_manager.c`) |
| **DuckyScript v1 Parser** | `badusb exec <file>`, `badble exec`| **Hardware-Dependent** | Parses Rubber Ducky scripts: `STRING`, `DELAY`, `GUI/WINDOWS`, `ENTER`, `ALT`, `CTRL`, `SHIFT`, `REM`, `REPEAT`, key combinations. (`main/managers/hid_script_parser.c`) |
| **Mouse Jiggler** | `badusb jiggle` | **Fully Functional** | Periodically moves the mouse pointer by 1 pixel in random intervals to prevent host PC screensavers and sleep mode. (`main/managers/badusb_manager.c`) |
| **Multi-Layout Keymapping** | `badusb layout <lang>` | **Fully Functional** | Supports keyboard scancode translations for US, UK, German (DE), French (AZERTY), Spanish (ES), Italian (IT), and Russian (RU). (`main/managers/hid_script_parser.c`) |
| **USB Mass Storage (MSC)** | `usbsd start`, `stop` | **Hardware-Dependent** | Exposes the onboard MicroSD card as a standard USB Flash Drive to a connected PC for rapid file drag-and-drop. (`main/managers/usb_msc_manager.c`) |
| **USB Host Keyboard Sniffer**| `usbkbd start` | **Experimental** | Uses USB-A host port on supporting boards to capture raw scancodes from connected physical keyboards. (`main/managers/usb_keyboard_manager.c`) |

---

## 9. Display & Graphical User Interface (GUI) Engine

| Feature | Component / Source | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **LVGL v8 Graphics Pipeline**| `components/lvgl` | **Fully Functional** | Powered by LVGL v8.3.11 with custom memory allocators and double-buffering. Dispatched via dedicated `LVGL Tick Task`. (`main/managers/display_manager.c`) |
| **ST7735S 128×160 SPI TFT** | `main/managers/display_manager.c`| **Fully Functional** | Verified on SPI3 bus (`MOSI=15, SCK=16, CS=5, DC=6, RST=7`). Full refresh @ 50 Hz. Scaled compact design tokens. (`PROJECT_STATE.md`) |
| **ST7262 800×480 RGB Display**| `main/vendor/drivers/ST7262.c` | **Hardware-Dependent** | Direct GDMA zero-copy scanout with SIMD-accelerated RGB565 fills on CrowPanel 4.3" / 7" displays. |
| **AXS15231B QSPI AMOLED** | `components/axs15231b` | **Hardware-Dependent** | High-resolution QSPI interface for round and rectangular AMOLED smartwatches and dongles. |
| **CrowPanel 4.2" E-Paper** | `main/vendor/drivers/crowpanel_epaper.c`| **Hardware-Dependent** | Ultra-low-power monochrome E-Ink driver with fast differential partial refreshes for the Book Reader view. |
| **Navigation Router** | `main/gui/gui_router.c` | **Fully Functional** | Screen history stack (`push_view`, `pop_view`) with route transitions and persistent view state preservation. |
| **Compact Font Scaler** | `main/gui/accessibility_fonts.c`| **Fully Functional** | Dynamically swaps fonts (Montserrat 8, 10, 12, 14, 16) based on display resolution to eliminate text clipping on 128×160 displays. |
| **Color Palettes & Themes** | `main/gui/theme_palette.c` | **Fully Functional** | Runtime selectable color profiles: GhostESP Dark, Cyberpunk, Matrix Green, High-Contrast Light, and AMOLED Black. |
| **Rotary Encoder Input** | `main/managers/encoder_manager.c`| **Hardware-Dependent** | Debounced hardware encoder quadrature polling with acceleration curves and push-button select. |
| **Joystick / D-Pad Input** | `main/managers/joystick_manager.c`| **Hardware-Dependent** | 5-way directional button navigation (Up, Down, Left, Right, Center/Enter) with hold-repeat detection. |
| **Touchscreen Gestures** | `main/managers/display_manager.c`| **Hardware-Dependent** | Supports I2C touch controllers (FT6336, GT911, CST816) with edge swipe detection for quick menu invocation. |
| **Full On-Screen Keyboard** | `main/managers/views/keyboard_screen.c`| **Fully Functional** | Interactive QWERTY touch/joystick virtual keyboard for entering SSIDs, passwords, and shell commands. |
| **Flappy Ghost Mini-Game** | `main/managers/views/flappy_ghost.c`| **Fully Functional** | Full physics-based Flappy Bird arcade game clone rendered in LVGL with persistent high-score tracking. |

---

## 10. Audio & Sound FX Subsystem

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **TI TLV320DAC3100 I2S Audio**| Built-in | **Hardware-Dependent** | I2C control and I2S audio stream configuration for TI low-power stereo DAC. (`components/tlv320dac3100`) |
| **I2S DAC / Speaker Driver** | Built-in | **Hardware-Dependent** | Generic I2S audio output supporting internal DACs, NS4168, and AW88298 amplifiers. (`main/managers/audio_i2s_output.c`) |
| **SD Card Audio Player** | `audio play <file.wav>` | **Hardware-Dependent** | Streams and plays uncompressed WAV, FLAC, and MP3 files from SD card. (`main/managers/audio_stream_manager.c`) |
| **UI Sound Effects & Tones** | Built-in | **Hardware-Dependent** | Low-latency audio feedback clicks, navigation beeps, error alerts, and Flipper Zero ringtones. (`main/managers/audio_stream_manager.c`) |
| **Desktop USB Rave Visualizer**| `rave`, `raveport` | **Fully Functional** | Receives real-time 15-band FFT desktop audio spectral frames over USB serial and renders high-frame-rate music visualizers. (`main/managers/views/music_visualizer.c`) |
| **ES7210 Microphone & Goertzel**| `mic_cal` | **Hardware-Dependent** | Captures ambient audio via ES7210 I2S microphone, executes Goertzel algorithm to detect dual-tone frequencies. (`main/managers/microphone/mic_goertzel.c`) |

---

## 11. GPS & Wardriving Subsystem

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Hardware NMEA GPS Engine**| `gpsinfo`, `gpspin`, `gpsbaud` | **Hardware-Dependent** | UART GPS driver supporting 9600–115200 baud. Parses `$GPGGA`, `$GPRMC`, extracting Lat, Lon, Alt, Fix Quality, and HDOP. (`main/managers/gps_manager.c`) |
| **WiGLE Wi-Fi Wardriving** | `startwd` | **Hardware-Dependent** | Concurrently hops channels, sniffs AP beacons, queries GPS coordinates, and appends compliant WiGLE CSV records to SD card. (`main/core/callbacks.c`) |
| **WiGLE BLE Wardriving** | `blewardriving` | **Hardware-Dependent** | Sniffs BLE advertisements, records MAC, RSSI, device name, and GPS coordinates into WiGLE BLE CSV schema on SD card. (`main/core/callbacks.c`) |
| **Live GPS Stream over GhostLink**| `wdstream` | **Fully Functional** | Re-broadcasts live NMEA / parsed GPS coordinates from one board to a companion board over GhostLink UART. (`main/core/commands/cmd_wdstream.c`) |

---

## 12. Power & Battery Management Subsystem

| Feature | Driver / Manager | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **AXP2101 / AXP2102 PMU** | `main/vendor/drivers/apx2102.c` | **Hardware-Dependent** | I2C power management unit controlling DCDC/LDO voltage rails, battery charging current, and power-key events. |
| **IP5306 Power Management** | `main/managers/ip5306_manager.c`| **Hardware-Dependent** | I2C battery fuel gauge monitoring charge percentage, boost converter state, and VBUS presence. |
| **ADC Battery Fuel Gauge** | `main/managers/fuel_gauge_manager.c`| **Fully Functional** | Calibrated ESP32-S3 ADC reading across a 2:1 resistor divider to calculate battery percentage curve. |
| **Display Auto-Sleep & Dimming**| `main/managers/settings_manager.c`| **Fully Functional** | Configurable inactivity timeout (15s to 5m) that steps backlight to 10% and shuts off LCD power rails to save battery. |
| **Deep Sleep & Light Sleep** | `main/main.c:900` | **Fully Functional** | Suspends CPU, retains RTC domain memory, and wakes up via GPIO button interrupt (EXT0/EXT1) or timer. |

---

## 13. Core System, CLI & Automation Subsystems

### 13.1 Interactive CLI Shell & Commands

| Feature | CLI Command | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **Dual-Console CLI Engine** | `UART0` & `USB-JTAG` | **Fully Functional** | Thread-safe command line interface with VT100 ANSI arrow-key cursor positioning, tab completion, and multi-line command history. (`main/core/serial_manager.c`) |
| **System Info & Telemetry** | `status`, `chipinfo`, `mem`, `devices`, `uptime`, `version`, `uuid`, `macaddr` | **Fully Functional** | Complete suite of introspection commands reporting CPU clock, internal SRAM watermarks, Octal PSRAM usage, and active tasks. (`main/core/commands/`) |
| **Unix Shell Utilities** | `echo`, `clear`, `banner`, `alias`, `history`, `ps`, `top`, `df`, `tail`, `grep`, `tee`, `watch`, `reboot`, `crash` | **Fully Functional** | Rich embedded POSIX-like utilities allowing output piping, command aliasing, background task monitoring (`top`), and file inspection. (`main/core/commands/`) |
| **Network IP Utilities** | `ifconfig`, `ping` | **Fully Functional** | Reports IP addresses, netmask, and gateway; sends ICMP Echo requests to verify remote connectivity. (`main/core/commands/`) |
| **Clock & RTC Synchronization**| `time`, `settime`, `timezone` | **Fully Functional** | Syncs system time from external I2C PCF8563 RTC, NTP over WiFi, or manual CLI input. Manages UTC POSIX timezone strings. (`main/core/commands/`) |

### 13.2 Storage & File Systems

| Feature | Manager / Driver | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **MicroSD Card (SDMMC / SPI)**| `main/managers/sd_card_manager.c`| **Hardware-Dependent** | Mounts FAT32 SD cards using SDMMC 1-line/4-line or SPI with Just-In-Time bus sharing arbitration between display and SD. |
| **Virtual SD / Wear-Levelling**| `main/managers/sd_card_manager.c`| **Fully Functional** | Allocates an internal wear-levelling FAT partition in SPI flash when no physical MicroSD card is installed, preserving file paths. |
| **Settings NVS Engine** | `main/managers/settings_manager.c`| **Fully Functional** | Persistent key-value store in NVS partition `nvs` with atomic flush and validation guards. |
| **SD Settings Auto-Backup** | `main/managers/settings_sd_backup.c`| **Hardware-Dependent** | Serializes the entire NVS settings database to a structured JSON file on SD card (`/mnt/ghostesp/settings_backup.json`) and restores on demand. |

### 13.3 Extensibility, Scripting & Virtual Pet

| Feature | Subsystem / Manager | Classification | Technical Description & Source Reference |
| :--- | :--- | :--- | :--- |
| **GhostChi Virtual Pet** | `main/managers/ghostchi_manager.c`| **Fully Functional** | Interactive Cyber-Tamagotchi companion pet living on the screen/status bar. Gains XP and levels up as you perform WiFi/BLE scans and attacks. Has 12 moods and feeding mechanics. |
| **GhostScript (Lua 5.4 Engine)**| `main/managers/ghostscript_runtime.c`| **Fully Functional** | Sandboxed Lua 5.4 interpreter exposing internal GhostESP APIs (`wifi.scan`, `deauth.start`, `rgb.set`, `display.print`) for headless automated penetration scripts. |
| **Native ELF Plugin Loader** | `main/managers/plugin_manager.c`| **Fully Functional** | Relocates and executes compiled `.gapp` ELF binaries from SD card using `elf_loader`, providing a sandboxed canvas and hardware API without reflashing firmware. |
| **App Gallery & Storefront** | `main/managers/views/app_gallery_screen.c`| **Fully Functional** | Graphical browser and launcher for native plugins installed under `/mnt/ghostesp/apps/`. |
| **Over-The-Air (OTA) Updates** | `main/managers/ota_manager.c` | **Fully Functional** | Dual-partition A/B rollback-safe OTA flashing from remote HTTP/HTTPS servers or local web console. (`partitions_ota_16mb.csv`) |
| **GhostLink Peer OTA Flashing**| `main/managers/peer_ota_manager.c`| **Fully Functional** | Allows one ESP32 (e.g. Master with screen) to flash new firmware directly into a connected secondary ESP32 peer over UART. |
| **Single-Page Web Console** | `webui/` | **Fully Functional** | Embedded web application served by the ESP32 HTTP server over SoftAP/STA, providing web-based CLI, scans, file management, and screen mirroring. |

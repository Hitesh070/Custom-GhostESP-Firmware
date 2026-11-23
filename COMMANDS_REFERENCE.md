# Custom GhostESP Firmware – Complete CLI Commands Reference

> **Hardware:** 7Semi ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB Octal PSRAM)  
> **Firmware Target:** ESP32-S3 Headless Build (`v1.0.0+`)  
> **Primary Interface:** Native USB-Serial / JTAG on `COM5` (115200 baud)  
> **Last Updated:** November 23, 2026  
> **Maintenance Protocol:** This document tracks all active, compiled, and registered commands for the ESP32-S3 headless configuration. It is continuously maintained as new commands and options are added.

---

## Quick Navigation Index

1. [System & Hardware Management](#1-system--hardware-management)
2. [Headless Shell & Terminal Utilities](#2-headless-shell--terminal-utilities)
3. [RGB & NeoPixel LED Subsystem](#3-rgb--neopixel-led-subsystem)
4. [Wi-Fi Reconnaissance & Auditing](#4-wi-fi-reconnaissance--auditing)
5. [Wi-Fi Attack & Security Testing Engine](#5-wi-fi-attack--security-testing-engine)
6. [Wi-Fi Station & AP Management](#6-wi-fi-station--ap-management)
7. [Packet Capture & PCAP Tools](#7-packet-capture--pcap-tools)
8. [Evil Portal Engine](#8-evil-portal-engine)
9. [LAN & Network Scanning (IP/ARP/Port)](#9-lan--network-scanning-iparpport)
10. [IoT, Smart Home & Gadgets](#10-iot-smart-home--gadgets)
11. [Bluetooth Low Energy (BLE) Operations](#11-bluetooth-low-energy-ble-operations)
12. [GPS & Wardriving / WiGLE](#12-gps--wardriving--wigle)
13. [GhostLink & Peer-to-Peer Communication](#13-ghostlink--peer-to-peer-communication)
14. [Over-The-Air (OTA) Peer Flashing](#14-over-the-air-ota-peer-flashing)
15. [SD Card & USB Mass Storage (MSC)](#15-sd-card--usb-mass-storage-msc)
16. [BadUSB & BadBLE HID Emulation](#16-badusb--badble-hid-emulation)
17. [GhostScript Automation Engine](#17-ghostscript-automation-engine)
18. [Aerial / Drone Remote ID Tracking & Spoofing](#18-aerial--drone-remote-id-tracking--spoofing)
19. [Flock Safety ALPR Camera Detection](#19-flock-safety-alpr-camera-detection)
20. [Chameleon Ultra RFID / NFC Integration](#20-chameleon-ultra-rfid--nfc-integration)
21. [NRF24L01+ 2.4 GHz RF Transceiver](#21-nrf24l01-24-ghz-rf-transceiver)
22. [Sub-GHz, LoRa & Meshtastic](#22-sub-ghz-lora--meshtastic)

---

## 1. System & Hardware Management

### `chipinfo`
- **Description:** Displays low-level ESP32-S3 silicon details, hardware revision, CPU core count, feature flags, embedded flash size, and ESP-IDF version.
- **Syntax:** `chipinfo`
- **Example:**
  ```text
  GhostESP> chipinfo
  ```

### `devices`
- **Description:** Enumerate all enabled hardware peripherals, bus mappings, active GPIO pins, and their operational statuses (e.g., RGB LED, SPI, I2C, UART).
- **Syntax:** `devices`
- **Example:**
  ```text
  GhostESP> devices
  ```

### `status`
- **Description:** Prints an overarching system summary including WiFi state, active attacks/scanners, free internal heap, free Octal PSRAM, FreeRTOS tasks, and uptime.
- **Syntax:** `status`
- **Example:**
  ```text
  GhostESP> status
  ```

### `mem`
- **Description:** Detailed heap and memory breakdown. Displays total, free, and minimum free watermark for both internal SRAM and external 8 MB Octal PSRAM.
- **Syntax:** `mem`
- **Example:**
  ```text
  GhostESP> mem
  ```

### `uptime`
- **Description:** Displays the duration since last system boot/reset in days, hours, minutes, and seconds.
- **Syntax:** `uptime`
- **Example:**
  ```text
  GhostESP> uptime
  ```

### `version`
- **Description:** Prints GhostESP software version, build date, compile timestamp, git commit hash, and ESP-IDF toolchain version.
- **Syntax:** `version`
- **Example:**
  ```text
  GhostESP> version
  ```

### `uuid`
- **Description:** Reads and prints the factory-fused eFuse unique chip identifier (UUID).
- **Syntax:** `uuid`
- **Example:**
  ```text
  GhostESP> uuid
  ```

### `macaddr`
- **Description:** Outputs all factory burned and runtime MAC addresses for Station (STA), SoftAP, Bluetooth LE (BLE), and Ethernet.
- **Syntax:** `macaddr`
- **Example:**
  ```text
  GhostESP> macaddr
  ```

### `reboot`
- **Description:** Performs a clean software restart of the ESP32-S3 processor.
- **Syntax:** `reboot`
- **Example:**
  ```text
  GhostESP> reboot
  ```

### `crash`
- **Description:** Intentionally triggers a kernel panic / illegal dereference for debugging coredumps and stack tracing.
- **Syntax:** `crash`
- **Example:**
  ```text
  GhostESP> crash
  ```

### `coredump`
- **Description:** Manages crash coredumps stored in dedicated flash partitions.
- **Syntax:** `coredump [dump|erase]`
- **Options:**
  - `(no args)`: Displays summary (partition size and whether a dump is present).
  - `dump`: Streams base64-encoded coredump out of serial console.
  - `erase`: Wipes the coredump flash partition.
- **Example:**
  ```text
  GhostESP> coredump
  GhostESP> coredump dump
  GhostESP> coredump erase
  ```

### `loglevel`
- **Description:** Dynamically adjust the global ESP-IDF serial logging verbosity on the fly without recompilation.
- **Syntax:** `loglevel [none|error|warn|info|debug|verbose]`
- **Example:**
  ```text
  GhostESP> loglevel debug
  GhostESP> loglevel info
  ```

### `identify`
- **Description:** Flashes the onboard RGB NeoPixel LED in an alert pattern to locate and physically identify the board.
- **Syntax:** `identify`
- **Example:**
  ```text
  GhostESP> identify
  ```

### `stop`
- **Description:** Universal panic command that halts any currently running background Wi-Fi attacks, BLE spam, wardriving, or scanners.
- **Syntax:** `stop`
- **Example:**
  ```text
  GhostESP> stop
  ```

---

## 2. Headless Shell & Terminal Utilities

### `echo`
- **Description:** Prints string to CLI. Supports escape characters `\n` and `\t`.
- **Syntax:** `echo <text>`
- **Example:**
  ```text
  GhostESP> echo Hello GhostESP\nWelcome
  ```

### `clear`
- **Description:** Clears terminal screen via ANSI escape sequences (`\033[2J\033[H`).
- **Syntax:** `clear`
- **Example:**
  ```text
  GhostESP> clear
  ```

### `hostname`
- **Description:** View or change the prompt hostname displayed in the serial CLI.
- **Syntax:** `hostname [new_name]`
- **Example:**
  ```text
  GhostESP> hostname
  GhostESP> hostname S3-Ghost
  ```

### `color` / `cli_color`
- **Description:** Changes ANSI color palette of the serial console text.
- **Syntax:** `color [name|0-255|off]`
- **Options:** `red`, `green`, `blue`, `yellow`, `cyan`, `magenta`, `white`, `off`
- **Example:**
  ```text
  GhostESP> color cyan
  GhostESP> color off
  ```

### `banner`
- **Description:** Toggles or queries the ASCII GhostESP boot banner on CLI connection.
- **Syntax:** `banner [on|off|status]`
- **Example:**
  ```text
  GhostESP> banner status
  GhostESP> banner off
  ```

### `alias` & `unalias`
- **Description:** Create or delete persistent command aliases/shortcuts in NVS.
- **Syntax:**
  - `alias [alias_name command_string]`
  - `unalias <alias_name|all>`
- **Example:**
  ```text
  GhostESP> alias quickscan scanap 5
  GhostESP> quickscan
  GhostESP> unalias quickscan
  ```

### `history`
- **Description:** View recent CLI command history or clear the history buffer.
- **Syntax:** `history [-c]`
- **Example:**
  ```text
  GhostESP> history
  GhostESP> history -c
  ```

### `didyoumean`
- **Description:** Toggle or inspect the Levenshtein distance typo suggestion engine for unrecognized commands.
- **Syntax:** `didyoumean [on|off|status]`
- **Example:**
  ```text
  GhostESP> didyoumean on
  ```

### `ps` / `top`
- **Description:** Displays FreeRTOS task status table: Task Name, State (`Running`, `Blocked`, `Ready`, `Suspended`), Priority, Stack High Water Mark (free bytes), and Task ID.
- **Syntax:** `ps` or `top`
- **Example:**
  ```text
  GhostESP> ps
  ```

### `df`
- **Description:** Displays disk usage and free space on the mounted FAT/LittleFS storage (e.g. `/mnt` for SD card or SPIFFS).
- **Syntax:** `df`
- **Example:**
  ```text
  GhostESP> df
  ```

### `tail`
- **Description:** Prints the last lines of a file located on the SD card storage.
- **Syntax:** `tail <filepath> [num_lines]`
- **Example:**
  ```text
  GhostESP> tail /mnt/ghostesp/gps/wardrive.csv 20
  ```

### `grep`
- **Description:** Searches a text file on the storage medium for lines matching a regex or substring.
- **Syntax:** `grep <pattern> <filepath>`
- **Example:**
  ```text
  GhostESP> grep "TargetAP" /mnt/ghostesp/scans/aplist.txt
  ```

### `source`
- **Description:** Executes a script file containing CLI commands sequentially line-by-line from storage.
- **Syntax:** `source <filepath>`
- **Example:**
  ```text
  GhostESP> source /mnt/ghostesp/scripts/startup.txt
  ```

### `tee`
- **Description:** Appends text to an SD card file while simultaneously echoing it to the serial console.
- **Syntax:** `tee <filepath> <text>`
- **Example:**
  ```text
  GhostESP> tee /mnt/notes.txt "Test completed on channel 6"
  ```

### `env` & `export`
- **Description:** View or persist simple environment variables in memory and NVS.
- **Syntax:**
  - `env`
  - `export <KEY>=<VALUE>`
- **Example:**
  ```text
  GhostESP> export TARGET_BSSID=AA:BB:CC:DD:EE:FF
  GhostESP> env
  ```

### `watch`
- **Description:** Periodically executes a specified command every $N$ seconds. Halt with `watch stop`.
- **Syntax:**
  - `watch <seconds> <command>`
  - `watch stop`
- **Example:**
  ```text
  GhostESP> watch 3 mem
  GhostESP> watch stop
  ```

### `settings`
- **Description:** Inspect, alter, or reset non-volatile storage (NVS) settings variables.
- **Syntax:** `settings <list|get|set|reset|help> [key] [value]`
- **Example:**
  ```text
  GhostESP> settings list
  GhostESP> settings get ap_ssid
  GhostESP> settings set ap_ssid MyGhostAP
  GhostESP> settings reset
  ```

---

## 3. RGB & NeoPixel LED Subsystem

> **Hardware Configuration:** 1 × WS2812B NeoPixel on **GPIO48** (GRB byte order, 800 kHz RMT).

### `rgbmode` / `setrgbmode`
- **Description:** Sets the active RGB LED visual animation mode or static color.
- **Syntax:** `rgbmode <rainbow|police|strobe|knight|off|red|green|blue|white|cyan|magenta|yellow>`
- **Supported Modes:**
  - `rainbow`: Smooth spectrum color wave with live real-time brightness scaling.
  - `knight`: Single-pixel smooth sine breathing Cylon eye (red).
  - `police`: High-visibility siren burst (3 rapid red flashes $\to$ pause $\to$ 3 rapid blue flashes).
  - `strobe`: Rapid 10 Hz high-intensity white flash bursts.
  - `off`: Shuts off the LED and clears the RMT driver.
  - `<color>`: Static primary or secondary color.
- **Example:**
  ```text
  GhostESP> rgbmode rainbow
  GhostESP> rgbmode knight
  GhostESP> rgbmode police
  GhostESP> rgbmode off
  ```

### `setneopixelbrightness`
- **Description:** Sets the global maximum brightness scaling percentage (0% to 100%). Dynamically updates static colors, Knight Rider breathing, and Rainbow modes in real time.
- **Syntax:** `setneopixelbrightness <0-100>`
- **Example:**
  ```text
  GhostESP> setneopixelbrightness 20
  GhostESP> setneopixelbrightness 100
  ```

### `getneopixelbrightness`
- **Description:** Queries and prints the currently active NeoPixel brightness percentage.
- **Syntax:** `getneopixelbrightness`
- **Example:**
  ```text
  GhostESP> getneopixelbrightness
  ```

### `setrgbpins`
- **Description:** Configures the hardware GPIO pin used for the NeoPixel / RGB LED strip. (On 1-wire addressable WS2812, assign the same pin to all three arguments).
- **Syntax:** `setrgbpins <red_pin> <green_pin> <blue_pin>`
- **Example:**
  ```text
  GhostESP> setrgbpins 48 48 48
  ```

### `setrgbcount`
- **Description:** Configures the total count of chained LEDs on the NeoPixel strip (defaults to 1 on this board).
- **Syntax:** `setrgbcount <1-512>`
- **Example:**
  ```text
  GhostESP> setrgbcount 1
  ```

### `rave` & `raveport`
- **Description:** Starts sound/frequency reactive LED rave pulsing.
- **Syntax:**
  - `rave <on|off>`
  - `raveport <port>`
- **Example:**
  ```text
  GhostESP> rave on
  GhostESP> rave off
  ```

---

## 4. Wi-Fi Reconnaissance & Auditing

### `scanap`
- **Description:** Performs an active and passive scan of all nearby 2.4 GHz 802.11 Access Points across legal regulatory channels.
- **Syntax:** `scanap [seconds]`
- **Example:**
  ```text
  GhostESP> scanap 5
  ```

### `scansta`
- **Description:** Puts the Wi-Fi transceiver into promiscuous channel-hopping mode to detect active client stations (STAs) transmitting frames.
- **Syntax:** `scansta`
- **Example:**
  ```text
  GhostESP> scansta
  ```

### `stopscan`
- **Description:** Immediately terminates any active `scanap` or `scansta` process.
- **Syntax:** `stopscan`
- **Example:**
  ```text
  GhostESP> stopscan
  ```

### `list`
- **Description:** Prints formatted tables of discovered targets from memory buffers.
- **Syntax:** `list <-a|-s|-airtags>`
- **Options:**
  - `-a`: List discovered Access Points (SSID, BSSID, RSSI, Channel, Encryption).
  - `-s`: List discovered Station clients with their associated AP BSSID.
  - `-airtags`: List discovered Apple AirTags (BLE).
- **Example:**
  ```text
  GhostESP> list -a
  GhostESP> list -s
  ```

### `select`
- **Description:** Designates target AP(s) or Station(s) by index for subsequent tracking, testing, attacks, or handshake capture.
- **Syntax:**
  - `select -a <index[,index,...]>` (supports multi-target comma-separated list)
  - `select -s <index>`
  - `select -airtag <index>`
- **Example:**
  ```text
  GhostESP> select -a 1
  GhostESP> select -a 2,4,7
  GhostESP> select -s 0
  ```

### `trackap` & `tracksta`
- **Description:** Continuously monitors and prints live signal strength (RSSI) updates for the currently selected AP or Station. Stop with `stop`.
- **Syntax:**
  - `trackap`
  - `tracksta`
- **Example:**
  ```text
  GhostESP> select -a 3
  GhostESP> trackap
  ```

### `scanall`
- **Description:** Runs a unified consecutive sweep for both Access Points and connected Stations.
- **Syntax:** `scanall [seconds]`
- **Example:**
  ```text
  GhostESP> scanall 10
  ```

### `congestion`
- **Description:** Scans the 2.4 GHz spectrum and renders an ASCII visual bar chart representing channel utilization and AP distribution across channels 1–14.
- **Syntax:** `congestion`
- **Example:**
  ```text
  GhostESP> congestion
  ```

### `wpa3check`
- **Description:** Performs passive security analysis on selected AP(s) or all discovered APs, auditing for WPA3 SAE, Transition Mode vulnerabilities, PMF (Protected Management Frames) enforcement, and cipher suites.
- **Syntax:** `wpa3check`
- **Example:**
  ```text
  GhostESP> select -a 2
  GhostESP> wpa3check
  ```

### `listenprobes`
- **Description:** Enters promiscuous mode to capture and log 802.11 Probe Request frames emitted by smartphones and laptops seeking known SSIDs.
- **Syntax:** `listenprobes [channel] [stop]`
- **Example:**
  ```text
  GhostESP> listenprobes
  GhostESP> listenprobes 6
  GhostESP> listenprobes stop
  ```

### `hop`
- **Description:** View or customize the channel hopping schedule and plan used during scans, monitoring, and promiscuous capture.
- **Syntax:** `hop [auto|all|basic|custom <channels>|<channels>]`
- **Example:**
  ```text
  GhostESP> hop
  GhostESP> hop 1,6,11
  GhostESP> hop all
  GhostESP> hop auto
  ```

### `setcountry`
- **Description:** Sets the active Wi-Fi regulatory domain code. Adjusts allowed channels and max transmit power (e.g. unlocks channel 12-14 where legal).
- **Syntax:** `setcountry <CC>` (e.g., `US`, `JP`, `EU`, `01` worldwide safe)
- **Example:**
  ```text
  GhostESP> setcountry US
  GhostESP> setcountry JP
  ```

### `pineap`
- **Description:** Scans for operational signatures characteristic of Hak5 WiFi Pineapple devices (e.g., rogue Karma responses, spoofed beacon structures).
- **Syntax:** `pineap [-s]`
- **Example:**
  ```text
  GhostESP> pineap
  GhostESP> pineap -s
  ```

---

## 5. Wi-Fi Attack & Security Testing Engine

> **Notice:** These commands are designed exclusively for authorized penetration testing, vulnerability analysis, and network defense validation.

### `reconnect` (alias: `deauth`)
- **Description:** Transmits IEEE 802.11 Deauthentication and Disassociation frames to the selected AP(s) and client(s) to evaluate network recovery and reconnection handling.
- **Syntax:** `reconnect [-hsd]`
- **Options:**
  - `(no flag)`: Sends reconnection deauth burst.
  - `-hsd`: Forces client re-association and captures 4-way WPA/WPA2 EAPOL handshake to storage.
- **Example:**
  ```text
  GhostESP> select -a 1
  GhostESP> reconnect
  GhostESP> reconnect -hsd
  ```

### `stopreconnect` (alias: `stopdeauth`)
- **Description:** Stops active deauth / reconnection tests.
- **Syntax:** `stopreconnect`
- **Example:**
  ```text
  GhostESP> stopreconnect
  ```

### `wifitest` (alias: `attack`)
- **Description:** Comprehensive 802.11 protocol testing suite supporting multiple targets simultaneously.
- **Syntax:** `wifitest <option>`
- **Arguments:**
  - `-d`: Client reconnection / deauth test.
  - `-hsd`: Handshake capture combined with client reconnection test.
  - `-c`: Channel switch announcement test (CSA).
  - `-e`: EAPOL logoff frame injection test.
  - `-p`: Probe request flood test.
  - `-b`: Malformed EAPOL key frame injection (bad MIC).
  - `-a`: Authentication frame flooding test.
  - `-s`: SAE commit flood test (WPA3).
- **Example:**
  ```text
  GhostESP> select -a 1,3
  GhostESP> wifitest -d
  GhostESP> wifitest -hsd
  GhostESP> wifitest -p
  ```

### `beaconspam`
- **Description:** Transmits high-volume 802.11 Beacon management frames to advertise fake or custom networks.
- **Syntax:** `beaconspam [OPTION] [SSID]`
- **Arguments:**
  - `-r`: Generates randomized pseudo-random SSIDs.
  - `-rr`: Rickroll lyric SSID flood.
  - `-l`: Spams SSIDs gathered from scan results list.
  - `[SSID]`: Advertises a specific single SSID repeatedly.
- **Example:**
  ```text
  GhostESP> beaconspam -r
  GhostESP> beaconspam -rr
  GhostESP> beaconspam "Free Public WiFi"
  GhostESP> stopspam
  ```

### `stopspam`
- **Description:** Stops running beacon spam transmission.
- **Syntax:** `stopspam`
- **Example:**
  ```text
  GhostESP> stopspam
  ```

### Custom Beacon List Suite (`beaconadd`, `beaconremove`, `beaconclear`, `beaconshow`, `beaconspamlist`)
- **Description:** Create and manage a customized target list of network SSIDs to spam.
- **Syntax:**
  - `beaconadd <SSID>`: Adds an SSID to list.
  - `beaconremove <SSID>`: Removes an SSID from list.
  - `beaconclear`: Wipes custom list.
  - `beaconshow`: Prints active list.
  - `beaconspamlist`: Starts spamming all SSIDs in the list.
- **Example:**
  ```text
  GhostESP> beaconadd Staff_Guest
  GhostESP> beaconadd Conference_Room
  GhostESP> beaconshow
  GhostESP> beaconspamlist
  GhostESP> stopspam
  ```

### `karma`
- **Description:** Rogue AP attack that dynamically listens for client 802.11 Probe Requests and instantly spawns fake APs matching whatever network the client is searching for.
- **Syntax:**
  - `karma start [SSID1 SSID2 ...]`
  - `karma stop`
- **Example:**
  ```text
  GhostESP> karma start
  GhostESP> karma start Starbucks AirportWiFi
  GhostESP> karma stop
  ```

### `dhcpstarve`
- **Description:** Floods the local network with forged DHCP Discover requests with randomized MAC addresses to exhaust the DHCP server's IP address lease pool.
- **Syntax:** `dhcpstarve <start|stop|display> [threads]`
- **Example:**
  ```text
  GhostESP> dhcpstarve start 4
  GhostESP> dhcpstarve display
  GhostESP> dhcpstarve stop
  ```

### `saeflood` & `stopsaeflood`
- **Description:** Tests WPA3-Personal SAE (Simultaneous Authentication of Equals) Dragonfly handshake resistance against resource exhaustion.
- **Syntax:**
  - `saeflood <password>`
  - `stopsaeflood`
  - `saefloodhelp`
- **Example:**
  ```text
  GhostESP> select -a 2
  GhostESP> saeflood testpassword123
  GhostESP> stopsaeflood
  ```

---

## 6. Wi-Fi Station & AP Management

### `connect`
- **Description:** Connects the ESP32-S3 as a client station to an existing 2.4 GHz Wi-Fi network and obtains an IP via DHCP.
- **Syntax:** `connect <SSID> [Password]`
- **Example:**
  ```text
  GhostESP> connect HomeNetwork MySecurePass
  ```

### `disconnect`
- **Description:** Disconnects from the current Wi-Fi Access Point and releases DHCP leases.
- **Syntax:** `disconnect`
- **Example:**
  ```text
  GhostESP> disconnect
  ```

### `forgetwifi`
- **Description:** Deletes saved Wi-Fi credentials from NVS memory.
- **Syntax:** `forgetwifi`
- **Example:**
  ```text
  GhostESP> forgetwifi
  ```

### `wifistatus`
- **Description:** Displays connection status, RSSI, current channel, IP address, gateway, subnet mask, and DNS servers.
- **Syntax:** `wifistatus`
- **Example:**
  ```text
  GhostESP> wifistatus
  ```

### `autoreconnect`
- **Description:** Toggles automatic reconnection logic if the station is involuntarily dropped from an AP.
- **Syntax:** `autoreconnect <on|off>`
- **Example:**
  ```text
  GhostESP> autoreconnect on
  ```

### `apcred`
- **Description:** Modify or restore the credentials for GhostESP's local Access Point (`GhostNet`).
- **Syntax:**
  - `apcred <SSID> <Password>` (Password min 8 chars)
  - `apcred -r` (resets to default: `GhostNet` / `GhostNet`)
- **Example:**
  ```text
  GhostESP> apcred GhostAdmin Secret1234
  GhostESP> apcred -r
  ```

### `apenable`
- **Description:** Configures whether the management SoftAP automatically starts on system boot.
- **Syntax:** `apenable <on|off>`
- **Example:**
  ```text
  GhostESP> apenable on
  ```

### `webauth`
- **Description:** Enables or disables password protection for the built-in HTTP WebUI.
- **Syntax:** `webauth [on|off|toggle|status]`
- **Example:**
  ```text
  GhostESP> webauth on
  ```

### `webuiap`
- **Description:** Configures WebUI network binding and port settings.
- **Syntax:** `webuiap`
- **Example:**
  ```text
  GhostESP> webuiap
  ```

### `sinkhole`
- **Description:** Starts a rogue DNS server that resolves all domain name queries to the ESP32's local IP address.
- **Syntax:** `sinkhole <start|stop|status>`
- **Example:**
  ```text
  GhostESP> sinkhole start
  GhostESP> sinkhole stop
  ```

---

## 7. Packet Capture & PCAP Tools

### `capture`
- **Description:** High-speed promiscuous packet capture engine with live streaming or SD storage in standard `.pcap` format.
- **Syntax:** `capture [OPTION] [-channel <n>|-c <n>]`
- **Capture Modes:**
  - `-probe`: Captures 802.11 Probe Request frames.
  - `-beacon`: Captures 802.11 Beacon management frames.
  - `-deauth`: Captures 802.11 Deauth and Disassociation frames.
  - `-eapol`: Captures 4-way WPA/WPA2 authentication handshakes and PMKID.
  - `-raw`: Captures all raw 802.11 frames promiscuously.
  - `-wps`: Captures Wi-Fi Protected Setup packets.
  - `-pwn`: Captures Pwnagotchi discovery beacons.
  - `-ble`: Captures Bluetooth Low Energy advertisements.
  - `-skimmer`: Captures and flags ATM/gas pump BLE skimmer packets.
  - `-channel <n>`: Locks the capture to a specific channel (1–14).
- **Storage & Streaming Tools:**
  - `-list`: Displays saved PCAP files on the SD card with indicator for captured handshakes (`+`).
  - `-export <pcap_file>`: Converts captured EAPOL handshakes to Hashcat `hc22000` PMKID/EAPOL format for hash cracking.
  - `-wireshark`: Streams live raw 802.11 PCAP over USB-Serial in real time directly into Wireshark pipe.
  - `-wiresharkble`: Streams live BLE PCAP to USB-Serial.
  - `-stop`: Terminates the active capture session.
- **Examples:**
  ```text
  GhostESP> capture -eapol -channel 6
  GhostESP> capture -raw -channel 11
  GhostESP> capture -list
  GhostESP> capture -export /mnt/ghostesp/pcaps/handshake_1.pcap
  GhostESP> capture -stop
  ```

---

## 8. Evil Portal Engine

### `startportal`
- **Description:** Launches a captive portal web server and rogue DHCP/DNS redirection to capture credentials entered on mobile devices and laptops.
- **Syntax:** `startportal [FilePath|default] [SSID] [Password]`
- **Arguments:**
  - `FilePath`: Path to custom HTML template on SD card (or `default` for embedded page).
  - `SSID`: Network name to broadcast.
  - `Password`: Optional WPA2 key (omit for open captive portal).
- **Example:**
  ```text
  GhostESP> startportal default "Free Airport Wi-Fi"
  GhostESP> startportal /mnt/ghostesp/portals/google.html "Google Starbucks"
  ```

### `stopportal`
- **Description:** Halts the running captive portal web server and DNS hijack service.
- **Syntax:** `stopportal`
- **Example:**
  ```text
  GhostESP> stopportal
  ```

### `listportals`
- **Description:** Lists all available Evil Portal HTML templates stored on the SD card (`/mnt/ghostesp/portals/`).
- **Syntax:** `listportals`
- **Example:**
  ```text
  GhostESP> listportals
  ```

### `evilportal`
- **Description:** Upload custom HTML pages directly over the serial UART buffer without requiring an SD card reader.
- **Syntax:** `evilportal -c sethtmlstr`
- **Protocol:**
  1. Send: `evilportal -c sethtmlstr`
  2. Send marker: `[HTML/BEGIN]`
  3. Stream HTML file content
  4. Send marker: `[HTML/CLOSE]`
  5. Launch via: `startportal`

---

## 9. LAN & Network Scanning (IP/ARP/Port)

> **Prerequisite:** Connect to a network first using `connect <SSID> <Pass>`.

### `ifconfig`
- **Description:** Displays IP addresses, subnet mask, default gateway, and MAC addresses for active interfaces.
- **Syntax:** `ifconfig`
- **Example:**
  ```text
  GhostESP> ifconfig
  ```

### `ping`
- **Description:** Transmits ICMP echo request packets to a remote host or IP to verify reachability and measure round-trip latency.
- **Syntax:** `ping <host_or_ip> [count]`
- **Example:**
  ```text
  GhostESP> ping 192.168.1.1 4
  GhostESP> ping google.com
  ```

### `scanarp`
- **Description:** Broadcasts ARP requests across the local subnet (`/24`) to identify all active online devices and map their IP to MAC addresses.
- **Syntax:** `scanarp [stop|monitor [seconds]]`
- **Example:**
  ```text
  GhostESP> scanarp
  GhostESP> scanarp monitor 30
  ```

### `scanlocal`
- **Description:** Performs an automated host discovery sweep combining ARP, NetBIOS name queries, mDNS, and SSDP to identify hostnames, device vendors, and operating systems.
- **Syntax:** `scanlocal [stop]`
- **Example:**
  ```text
  GhostESP> scanlocal
  ```

### `scanports`
- **Description:** Scans TCP ports on a target IP or the entire local subnet.
- **Syntax:**
  - `scanports local` (scans common ports on discovered LAN hosts)
  - `scanports <IP> [all|start-end]`
- **Example:**
  ```text
  GhostESP> scanports 192.168.1.1
  GhostESP> scanports 192.168.1.50 20-1000
  GhostESP> scanports 192.168.1.1 all
  ```

### `scanssh`
- **Description:** Scans target IP or subnet on port 22 and extracts SSH version banner strings.
- **Syntax:** `scanssh <IP>`
- **Example:**
  ```text
  GhostESP> scanssh 192.168.1.100
  ```

### `netbiosscan`
- **Description:** Queries port 137 UDP to grab NetBIOS computer names, workgroups, and domain names from Windows machines and Samba servers.
- **Syntax:**
  - `netbiosscan <IP>`
  - `netbiosscan subnet <a.b.c.0>`
- **Example:**
  ```text
  GhostESP> netbiosscan 192.168.1.15
  GhostESP> netbiosscan subnet 192.168.1.0
  ```

### `httpbannerscan`
- **Description:** Connects to HTTP/HTTPS ports (80, 443, 8080, 8443) on targets and grabs web server identification headers (`Server:` header).
- **Syntax:**
  - `httpbannerscan <IP>`
  - `httpbannerscan subnet <a.b.c.0>`
- **Example:**
  ```text
  GhostESP> httpbannerscan 192.168.1.1
  ```

### `snmpprobe`
- **Description:** Probes SNMP services (UDP 161) with common community strings (`public`, `private`) and performs OID walks.
- **Syntax:**
  - `snmpprobe <IP>`
  - `snmpprobe subnet <a.b.c.0>`
  - `snmpprobe walk <IP> [OID]`
  - `snmpprobe communities <comm1,comm2>`
- **Example:**
  ```text
  GhostESP> snmpprobe 192.168.1.1
  GhostESP> snmpprobe walk 192.168.1.1 1.3.6.1.2.1.1
  ```

### `mdnssniff`
- **Description:** Passively sniffs mDNS (Bonjour), LLMNR, SSDP, and NetBIOS broadcasts to detect network hostnames and published services.
- **Syntax:** `mdnssniff <IP|all|stop>`
- **Example:**
  ```text
  GhostESP> mdnssniff all
  GhostESP> mdnssniff stop
  ```

### `certscan` (alias: `trustscan`)
- **Description:** Establishes TLS handshakes with target hosts on HTTPS (port 443) and extracts certificate subject, issuer, validity period, and SANs.
- **Syntax:** `certscan <IP_or_hostname> [port]`
- **Example:**
  ```text
  GhostESP> certscan 192.168.1.1 443
  GhostESP> certscan google.com
  ```

### `enumscan`
- **Description:** Comprehensive multi-stage automated recon combining ARP, port scanning, banner grabbing, and NetBIOS/mDNS resolution for a target IP.
- **Syntax:** `enumscan <IP>`
- **Example:**
  ```text
  GhostESP> enumscan 192.168.1.1
  ```

---

## 10. IoT, Smart Home & Gadgets

### `wol`
- **Description:** Sends Wake-on-LAN magic packet over UDP port 9 to remotely power on target PC or NAS.
- **Syntax:** `wol <MAC_Address> [broadcast_IP]`
- **Example:**
  ```text
  GhostESP> wol 00:11:22:33:44:55
  GhostESP> wol 00:11:22:33:44:55 192.168.1.255
  ```

### `powerprinter`
- **Description:** Connects to raw JetDirect printer ports (TCP 9100) on network printers and prints custom plain text.
- **Syntax:** `powerprinter <Printer_IP> <Text> <FontSize> <Alignment>`
- **Alignments:** `CM` (Center Middle), `TL` (Top Left), `TR` (Top Right), `BL` (Bottom Left), `BR` (Bottom Right)
- **Example:**
  ```text
  GhostESP> powerprinter 192.168.1.200 "Security Audit In Progress" 24 CM
  ```

### `dialconnect`
- **Description:** Uses the DIAL (Discovery and Launch) SSDP protocol to trigger the YouTube app on all smart TVs and streaming sticks across the local LAN.
- **Syntax:** `dialconnect`
- **Example:**
  ```text
  GhostESP> dialconnect
  ```

### `govee`
- **Description:** Controls Govee smart lights on the local network via their local UDP API (port 4003).
- **Syntax:**
  - `govee scan`
  - `govee <IP> <on|off|brightness <0-100>|color <RRGGBB>>`
- **Example:**
  ```text
  GhostESP> govee scan
  GhostESP> govee 192.168.1.85 on
  GhostESP> govee 192.168.1.85 brightness 50
  GhostESP> govee 192.168.1.85 color FF0000
  ```

### `tplinktest`
- **Description:** Scans for and controls TP-Link Kasa smart plugs/switches using port 9999 XOR encryption.
- **Syntax:** `tplinktest <scan|IP> [command]`
- **Example:**
  ```text
  GhostESP> tplinktest scan
  ```

---

## 11. Bluetooth Low Energy (BLE) Operations

### `blescan`
- **Description:** Multifunction Bluetooth Low Energy scanner.
- **Syntax:** `blescan [OPTION]`
- **Arguments:**
  - `-f`: "Find the Flippers" — filters specifically for Flipper Zero white/black/clear devices.
  - `-ds`: Detects BLE spam attacks originating from nearby devices.
  - `-a`: Scans for Apple AirTags and Find My trackers.
  - `-adv`: Parses and prints detailed advertiser payloads (manufacturer data, service UUIDs).
  - `-oui <hex_prefix>`: Filters advertisers by IEEE OUI MAC prefix.
  - `-vendor <name>`: Filters advertisers by vendor name.
  - `-g`: Scans for connectable GATT peripheral servers.
  - `-r`: Dumps raw BLE advertisement packets to console.
  - `-s`: Stops the BLE scanner.
- **Example:**
  ```text
  GhostESP> blescan -f
  GhostESP> blescan -a
  GhostESP> blescan -adv
  GhostESP> blescan -s
  ```

### `bledetect`
- **Description:** Signature-based scanner for trackers, gas pump skimmers, and beacon devices.
- **Syntax:** `bledetect [-s|-l|-c|-i|-t <idx>|-u|-sp <idx>]`
- **Options:**
  - `(no flag)`: Starts tracker/skimmer detection scan.
  - `-s`: Stops scan, retaining discovered devices.
  - `-l`: Lists detected devices with RSSI and identification.
  - `-t <index>`: Tracks a specific device with continuous live RSSI.
  - `-u`: Stops following tracked device.
  - `-c`: Clears stored scan results.
- **Example:**
  ```text
  GhostESP> bledetect
  GhostESP> bledetect -l
  GhostESP> bledetect -t 0
  GhostESP> bledetect -s
  ```

### `blespam`
- **Description:** Generates high-volume BLE advertisement packets that trigger pairing popups and notifications on nearby mobile devices.
- **Syntax:** `blespam [OPTION] [-name <text>]`
- **Options:**
  - `-apple`: Triggers iOS popups (AirPods, AppleTV setup, AirTags).
  - `-ms`: Microsoft Swift Pair popups on Windows 10/11 laptops.
  - `-samsung`: Samsung Galaxy Buds/Watch quick-pair popups.
  - `-google`: Android Google Fast Pair popups.
  - `-random`: Continuously rotates through all ecosystem payloads.
  - `-name <text>`: Assigns custom advertised device name.
  - `-s`: Stops BLE advertisement spam.
- **Example:**
  ```text
  GhostESP> blespam -apple
  GhostESP> blespam -google -name "Test Earbuds"
  GhostESP> blespam -random
  GhostESP> blespam -s
  ```

### `listairtags` & `selectairtag`
- **Description:** Lists and selects discovered Apple Find My AirTag beacons.
- **Syntax:**
  - `listairtags` (or `list -airtags`)
  - `selectairtag <index>`
- **Example:**
  ```text
  GhostESP> listairtags
  GhostESP> selectairtag 0
  ```

### `spoofairtag` & `stopspoof`
- **Description:** Broadcasts BLE advertisement payloads cloning a selected or custom Apple AirTag.
- **Syntax:**
  - `spoofairtag`
  - `stopspoof`
- **Example:**
  ```text
  GhostESP> selectairtag 0
  GhostESP> spoofairtag
  GhostESP> stopspoof
  ```

### Flipper Zero Discovery Suite (`listflippers`, `selectflipper`)
- **Description:** Inspects and interacts with nearby Flipper Zero devices discovered via BLE.
- **Syntax:**
  - `listflippers`: Lists detected Flippers.
  - `selectflipper <index>`: Selects target Flipper.
- **Example:**
  ```text
  GhostESP> blescan -f
  GhostESP> listflippers
  GhostESP> selectflipper 0
  ```

### GATT Interaction Suite (`listgatt`, `selectgatt`, `enumgatt`, `trackgatt`)
- **Description:** Explores and maps BLE GATT services, characteristics, and descriptors on connectable peripherals.
- **Syntax:**
  - `listgatt`: Lists discoverable connectable GATT servers.
  - `selectgatt <index>`: Connects to peripheral.
  - `enumgatt`: Enumerate all services, characteristics, UUIDs, and read/write flags.
  - `trackgatt`: Monitors live connection RSSI.
- **Example:**
  ```text
  GhostESP> listgatt
  GhostESP> selectgatt 1
  GhostESP> enumgatt
  ```

### `listadv`
- **Description:** Displays the parsed advertisement buffer populated by `blescan -adv`.
- **Syntax:** `listadv`
- **Example:**
  ```text
  GhostESP> listadv
  ```

---

## 12. GPS & Wardriving / WiGLE

> **Hardware:** Connect external NMEA GPS module (e.g. NEO-6M/NEO-8M) to configurable UART pins.

### `gpsinfo`
- **Description:** Displays real-time GPS telemetry: latitude, longitude, altitude, speed, HDOP, fix quality, and visible satellites.
- **Syntax:** `gpsinfo [-s]`
- **Example:**
  ```text
  GhostESP> gpsinfo
  GhostESP> gpsinfo -s
  ```

### `gpspin`
- **Description:** Configures the hardware GPIO pin used as the UART RX input for external GPS modules.
- **Syntax:** `gpspin <pin>`
- **Example:**
  ```text
  GhostESP> gpspin 1
  ```

### `gpsbaud`
- **Description:** Sets the baud rate for the GPS serial interface or auto-negotiates it.
- **Syntax:** `gpsbaud <auto|4800|9600|19200|38400|57600|115200>`
- **Example:**
  ```text
  GhostESP> gpsbaud 9600
  GhostESP> gpsbaud auto
  ```

### `startwd`
- **Description:** Begins automated Wi-Fi wardriving. Pairs scanned 802.11 APs with GPS coordinates and logs to WiGLE-compliant CSV files on the SD card.
- **Syntax:** `startwd [-s] [--active|--monitor] [--channels <csv>] [--hop <ms>]`
- **Options:**
  - `(no flag)`: Starts wardriving logger.
  - `-s`: Stops wardriving.
  - `--monitor`: Operates in passive monitor mode instead of active scan.
- **Example:**
  ```text
  GhostESP> startwd
  GhostESP> startwd -s
  ```

### `blewardriving`
- **Description:** Logs discovered Bluetooth LE devices, beacons, and AirTags with GPS coordinates to CSV.
- **Syntax:** `blewardriving [-s]`
- **Example:**
  ```text
  GhostESP> blewardriving
  GhostESP> blewardriving -s
  ```

### `dualwd`
- **Description:** Dual-radio wardriving engine. Simultaneously sweeps and logs both Wi-Fi and BLE devices alongside GPS data (uses 8 MB Octal PSRAM buffers).
- **Syntax:** `dualwd [start|-s]`
- **Example:**
  ```text
  GhostESP> dualwd start
  GhostESP> dualwd -s
  ```

### `wdstream`
- **Description:** Streams live wardriving observations over the serial port for companion mobile apps.
- **Syntax:**
  - `wdstream start [-wifi] [-ble] [-i <ms>] [-ch auto|1|1,6,11]`
  - `wdstream stop`
  - `wdstream status`
- **Example:**
  ```text
  GhostESP> wdstream start -wifi -ble -i 100
  GhostESP> wdstream status
  GhostESP> wdstream stop
  ```

### `sweep`
- **Description:** Comprehensive single-shot RF sweep: scans Wi-Fi APs, stations, and BLE peripherals, compiling a complete CSV report to SD card (`/mnt/ghostesp/sweeps/`).
- **Syntax:** `sweep [-w <wifi_sec>] [-b <ble_sec>]`
- **Example:**
  ```text
  GhostESP> sweep -w 10 -b 10
  ```

### `wigle`
- **Description:** Manages automated uploading of wardrive CSV logs directly to the WiGLE.net database via Wi-Fi.
- **Syntax:** `wigle <API|auto|donate|show|list|files|upload|stats>`
- **Options:**
  - `wigle API <name:token>`: Sets base64 API key credentials.
  - `wigle auto <on|off>`: Automatically uploads pending files when Wi-Fi connects.
  - `wigle donate <on|off>`: Sets WiGLE donation flag.
  - `wigle files`: Lists pending CSV files in `/mnt/ghostesp/gps/`.
  - `wigle upload <filename>`: Uploads a specific CSV.
  - `wigle upload all`: Uploads all queued logs.
  - `wigle stats`: Queries account stats and rank from WiGLE server.
- **Example:**
  ```text
  GhostESP> wigle API user_auth_token_here
  GhostESP> wigle files
  GhostESP> wigle upload all
  ```

---

## 13. GhostLink & Peer-to-Peer Communication

GhostLink allows two or more GhostESP boards to form a high-speed encrypted control link over hardware UART or BLE.

### `commdiscovery`
- **Description:** Checks for active GhostLink peer announcements and displays available peer nodes.
- **Syntax:** `commdiscovery`
- **Example:**
  ```text
  GhostESP> commdiscovery
  ```

### `commconnect` & `commdisconnect`
- **Description:** Connects to or disconnects from a named GhostESP peer node.
- **Syntax:**
  - `commconnect <peer_name>`
  - `commdisconnect`
- **Example:**
  ```text
  GhostESP> commconnect Ghost_Node_2
  GhostESP> commdisconnect
  ```

### `commsend`
- **Description:** Dispatches a CLI command to execute remotely on the connected peer ESP32.
- **Syntax:** `commsend <command> [data]`
- **Example:**
  ```text
  GhostESP> commsend scanap 5
  GhostESP> commsend rgbmode police
  ```

### `commstatus`
- **Description:** Displays connection status, link latency, packets sent/received, and dropped frames.
- **Syntax:** `commstatus`
- **Example:**
  ```text
  GhostESP> commstatus
  ```

### `commsetpins`
- **Description:** Changes the UART TX and RX GPIO pins used for the inter-board GhostLink connection.
- **Syntax:** `commsetpins <tx_pin> <rx_pin>`
- **Example:**
  ```text
  GhostESP> commsetpins 4 5
  ```

### `glbench`
- **Description:** Benchmarks raw bi-directional GhostLink data throughput and latency across the physical link.
- **Syntax:** `glbench [send|recv|both] [kb]`
- **Example:**
  ```text
  GhostESP> glbench both 512
  ```

### `blebridge`
- **Description:** Bridges the GhostLink communication bus wirelessly over Bluetooth Low Energy.
- **Syntax:** `blebridge [start|stop|status|pair <peer_name>]`
- **Example:**
  ```text
  GhostESP> blebridge start
  GhostESP> blebridge status
  GhostESP> blebridge stop
  ```

---

## 14. Over-The-Air (OTA) Peer Flashing

> Enabled on this board (`CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y`). Allows remote updating of peer nodes.

### `otainfo`
- **Description:** Displays the active partition table, currently running partition (`ota_0` or `ota_1`), boot partition, and firmware version.
- **Syntax:** `otainfo`
- **Example:**
  ```text
  GhostESP> otainfo
  ```

### `otarecv`
- **Description:** Puts the device into OTA receive mode to accept a binary firmware stream transmitted from a GhostLink peer node.
- **Syntax:** `otarecv <size> [hash]`
- **Example:**
  ```text
  GhostESP> otarecv 2097152
  ```

### `otastatus`
- **Description:** Queries progress percentage, bytes written, and verification status of an ongoing OTA update.
- **Syntax:** `otastatus`
- **Example:**
  ```text
  GhostESP> otastatus
  ```

### `otaabort`
- **Description:** Cancels an active OTA flashing process, clears flash staging buffers, and marks the previous firmware partition as healthy.
- **Syntax:** `otaabort`
- **Example:**
  ```text
  GhostESP> otaabort
  ```

---

## 15. SD Card & USB Mass Storage (MSC)

### `sd`
- **Description:** Full-featured command-line filesystem suite for managing MicroSD cards.
- **Syntax:** `sd <subcommand> [args]`
- **Subcommands:**
  - `sd status`: Mount state, card type (SDSC/SDHC), total capacity, and used space.
  - `sd list [path]`: Lists files and directories with assigned numerical indices.
  - `sd info <index|path>`: Displays file size, timestamps, and permissions.
  - `sd size <index|path>`: Outputs exact byte size.
  - `sd read <index|path> [offset] [length] [--base64]`: Reads raw or base64 file data.
  - `sd write <path> <base64>`: Creates/overwrites a file with base64 data.
  - `sd append <path> <base64>`: Appends base64 data to an existing file.
  - `sd mkdir <path>`: Creates a directory.
  - `sd rm <index|path>`: Deletes a file or empty directory.
  - `sd tree [path] [depth]`: Recursive directory hierarchy view.
- **Examples:**
  ```text
  GhostESP> sd status
  GhostESP> sd list /mnt/ghostesp
  GhostESP> sd mkdir /mnt/ghostesp/captures
  GhostESP> sd rm /mnt/ghostesp/temp.txt
  ```

### `sd_config`
- **Description:** Displays current SD card GPIO pin mapping and mode (SPI vs. SDMMC 1-line/4-line).
- **Syntax:** `sd_config`
- **Example:**
  ```text
  GhostESP> sd_config
  ```

### `sd_pins_spi` & `sd_pins_mmc`
- **Description:** Customizes GPIO pin mappings for SPI or SDMMC card interfaces.
- **Syntax:**
  - `sd_pins_spi <cs> <clk> <miso> <mosi>`
  - `sd_pins_mmc <clk> <cmd> <d0> <d1> <d2> <d3>`
- **Example:**
  ```text
  GhostESP> sd_pins_spi 10 12 13 11
  ```

### `sd_save_config`
- **Description:** Commits the current SD card pin configuration to NVS memory.
- **Syntax:** `sd_save_config`
- **Example:**
  ```text
  GhostESP> sd_save_config
  ```

### `usbsd`
- **Description:** Enables USB Mass Storage (MSC) passthrough over the native USB port. The MicroSD card mounts directly as a USB flash drive on Windows/Linux/macOS for rapid file transfer.
- **Syntax:** `usbsd [on|off|status]`
- **Example:**
  ```text
  GhostESP> usbsd status
  GhostESP> usbsd on
  GhostESP> usbsd off
  ```

---

## 16. BadUSB & BadBLE HID Emulation

Emulates a USB or Bluetooth HID keyboard/mouse to inject keystrokes and execute payloads.

### `badusb`
- **Description:** Native USB HID payload executor using DuckyScript syntax.
- **Syntax:** `badusb <subcommand> [args]`
- **Subcommands:**
  - `badusb run <filename>`: Executes a DuckyScript located in `/mnt/ghostesp/badusb/`.
  - `badusb list`: Lists available scripts on SD card.
  - `badusb stop`: Aborts active script execution.
  - `badusb type <text>`: Injects plain text keystrokes directly.
  - `badusb keysend <modifier> <keycode>`: Sends raw modifier + HID scancode.
  - `badusb set_vid <hex>`: Spoofs USB Vendor ID (e.g. `0x046D` Logitech).
  - `badusb set_pid <hex>`: Spoofs USB Product ID.
  - `badusb set_mfr <text>`: Custom manufacturer string.
  - `badusb set_prod <text>`: Custom product descriptor string.
  - `badusb set_rand <0|1>`: Randomizes USB serial number.
  - `badusb set_layout <n>`: Changes keyboard layout mapping.
  - `badusb jiggle_start`: Simulates periodic mouse movement to prevent PC lock screen.
  - `badusb jiggle_stop`: Halts mouse jiggler.
  - `badusb trackpad_move <dx> <dy>`: Injects relative mouse movement.
  - `badusb trackpad_button <mask>`: Clicks mouse buttons.
  - `badusb trackpad_wheel <delta>`: Scrolls mouse wheel.
- **Example:**
  ```text
  GhostESP> badusb list
  GhostESP> badusb run payload.txt
  GhostESP> badusb type "cmd.exe\n"
  GhostESP> badusb jiggle_start
  ```

### `usbkbd`
- **Description:** Enables, disables, or inspects the USB HID keyboard emulation stack on the ESP32-S3.
- **Syntax:** `usbkbd <on|off|status>`
- **Example:**
  ```text
  GhostESP> usbkbd status
  GhostESP> usbkbd on
  ```

### `badble`
- **Description:** Wireless BadUSB variant. Pairs with phones, tablets, or computers as a Bluetooth HID keyboard to inject keystroke scripts wirelessly.
- **Syntax:** `badble <status|list|run|stop|type|keysend|set_name> [args]`
- **Example:**
  ```text
  GhostESP> badble status
  GhostESP> badble list
  GhostESP> badble set_name "Logitech MX Keys"
  GhostESP> badble run mobile_payload.txt
  ```

---

## 17. GhostScript Automation Engine

### `script`
- **Description:** Automation script manager for executing custom sequences of GhostESP CLI commands stored in text files on the SD card (`/mnt/ghostesp/scripts/`).
- **Syntax:** `script <list|run|status|stop> [index]`
- **Subcommands:**
  - `script list`: Scans SD card and displays numbered index of all runnable scripts.
  - `script run <index>`: Launches the script at the specified numerical index in a background task.
  - `script status`: Displays current script line execution and progress.
  - `script stop`: Halts running script.
- **Example:**
  ```text
  GhostESP> script list
  GhostESP> script run 0
  GhostESP> script status
  GhostESP> script stop
  ```

---

## 18. Aerial / Drone Remote ID Tracking & Spoofing

### `aerialscan`
- **Description:** Scans for FAA / ASTM standard 2.4 GHz Drone Remote ID broadcast beacons transmitted by DJI, Autel, and DIY drones.
- **Syntax:** `aerialscan`
- **Example:**
  ```text
  GhostESP> aerialscan
  ```

### `aeriallist`
- **Description:** Displays table of detected drones: Serial Number / ID, Operator Location, Drone GPS coordinates, altitude, velocity, and RSSI.
- **Syntax:** `aeriallist`
- **Example:**
  ```text
  GhostESP> aeriallist
  ```

### `aerialtrack`
- **Description:** Continuously tracks a specific drone by index or MAC address, reporting real-time distance and flight telemetry.
- **Syntax:** `aerialtrack <index|MAC_Address>`
- **Example:**
  ```text
  GhostESP> aeriallist
  GhostESP> aerialtrack 0
  ```

### `aerialstop`
- **Description:** Stops drone tracking / scanner.
- **Syntax:** `aerialstop`
- **Example:**
  ```text
  GhostESP> aerialstop
  ```

### `aerialspoof` & `aerialspoofstop`
- **Description:** Broadcasts synthetic FAA Remote ID beacons with custom coordinates, serial numbers, and altitudes.
- **Syntax:**
  - `aerialspoof [device_id latitude longitude altitude]`
  - `aerialspoofstop`
- **Example:**
  ```text
  GhostESP> aerialspoof DJI-Mavic3-9999 37.7749 -122.4194 120
  GhostESP> aerialspoofstop
  ```

---

## 19. Flock Safety ALPR Camera Detection

### `flockscan`
- **Description:** Scans 2.4 GHz Wi-Fi and BLE probe signatures to identify nearby Flock Safety Automated License Plate Reader (ALPR) surveillance cameras.
- **Syntax:** `flockscan`
- **Example:**
  ```text
  GhostESP> flockscan
  ```

### `flocklist`
- **Description:** Displays all detected Flock Safety ALPR units, MAC addresses, signal strengths, and first/last seen timestamps.
- **Syntax:** `flocklist`
- **Example:**
  ```text
  GhostESP> flocklist
  ```

### `flockstop`
- **Description:** Stops the Flock camera detection scanner.
- **Syntax:** `flockstop`
- **Example:**
  ```text
  GhostESP> flockstop
  ```

---

## 20. Chameleon Ultra RFID / NFC Integration

> Integrates with Proxmark / Chameleon Ultra devices over BLE.

### `chameleon`
- **Description:** Comprehensive control client for Chameleon Ultra RFID/NFC emulators.
- **Syntax:** `chameleon <connect|disconnect|status|scanhf|scanlf|battery|reader|emulator> [args]`
- **Subcommands:**
  - `chameleon connect [timeout] [pin]`: Connects via BLE.
  - `chameleon disconnect`: Disconnects BLE session.
  - `chameleon status`: Displays connection state and firmware version.
  - `chameleon scanhf`: Scans 13.56 MHz High-Frequency RFID tags (Mifare, ISO14443A).
  - `chameleon scanlf`: Scans 125 kHz Low-Frequency RFID tags (EM410x, HID Prox).
  - `chameleon battery`: Queries Chameleon battery level.
  - `chameleon reader`: Places Chameleon into reader mode.
  - `chameleon emulator`: Configures Chameleon into card emulation mode.
- **Example:**
  ```text
  GhostESP> chameleon connect 10
  GhostESP> chameleon scanhf
  GhostESP> chameleon scanlf
  GhostESP> chameleon disconnect
  ```

---

## 21. NRF24L01+ 2.4 GHz RF Transceiver

> **SPI Hardware Bus Configuration:**  
> - Host: `SPI2_HOST` (FSPI)  
> - MOSI: `GPIO 11` | MISO: `GPIO 13` | SCK: `GPIO 12` | CSN: `GPIO 10` | CE: `GPIO 14`

### `nrf24`
- **Description:** Manages the NRF24L01+ transceiver for 2.4 GHz RF operations, carrier detection, and spectrum analysis.
- **Syntax:** `nrf24 <start|stop|pause|resume|status|state>`
- **Subcommands:**
  - `nrf24 start`: Initializes the SPI hardware bus and starts 2.4 GHz spectrum scanning task across channels 0–125 (2400 to 2525 MHz).
  - `nrf24 stop`: Shuts down the scanner and releases the SPI bus.
  - `nrf24 pause`: Temporarily suspends channel sweeps.
  - `nrf24 resume`: Resumes paused sweeps.
  - `nrf24 status`: Displays current task state, error messages, and configured SPI pin assignments.
- **Example:**
  ```text
  GhostESP> nrf24 status
  GhostESP> nrf24 start
  GhostESP> nrf24 stop
  ```

---

## 22. Sub-GHz, LoRa & Meshtastic

### `subghz`
- **Description:** Interfaces with external CC1101 Sub-GHz transceiver modules for 300–928 MHz signals.
- **Syntax:** `subghz <start|stop|status|capture|save|load|list|replay>`
- **Example:**
  ```text
  GhostESP> subghz status
  ```

### `lora` / `meshtastic` / `meshcore` / `mc` / `mesh`
- **Description:** Long-range packet radio communication suite supporting Meshtastic and MeshCore protocols over SX1262/SX1276 modules.
- **Syntax:**
  - `mesh`: Shows active mesh protocol and status.
  - `mesh switch <meshtastic|meshcore>`: Switches between protocols.
  - `mesh on [protocol]` / `mesh off`: Starts or stops mesh radio.
  - `lora status`: Displays radio status, frequency, spreading factor, and bandwidth.
  - `lora send <text>`: Transmits public broadcast message.
  - `lora dm <node_id> <text>`: Sends encrypted direct message.
  - `lora nodes`: Lists heard nearby mesh nodes.
  - `lora pubkey`: Displays node public key.
- **Example:**
  ```text
  GhostESP> mesh
  GhostESP> lora status
  GhostESP> lora nodes
  GhostESP> lora send "Hello from GhostESP"
  ```

---

## 23. Clock, Display & Input Injection

### `timezone`
- **Description:** Configures POSIX timezone string for system clock formatting.
- **Syntax:** `timezone <TZ_STRING>` (e.g. `UTC0`, `EST5EDT`, `GMT-5:30`)
- **Example:**
  ```text
  GhostESP> timezone "IST-5:30"
  ```

### `time` / `settime` / `date`
- **Description:** View or manually set system real-time clock (RTC).
- **Syntax:**
  - `date`
  - `time`
  - `settime <YYYY> <MM> <DD> <HH> <MM> <SS>`
- **Example:**
  ```text
  GhostESP> date
  GhostESP> settime 2026 11 23 18 00 00
  ```

### `privacy`
- **Description:** Redacts sensitive Wi-Fi and BLE SSIDs and MAC addresses in console logs.
- **Syntax:** `privacy [on|off|toggle|status]`
- **Example:**
  ```text
  GhostESP> privacy on
  ```

### `input` & `iobtn`
- **Description:** Virtual button and GPIO trigger injection for testing headless firmware actions without physical buttons.
- **Syntax:**
  - `input <left|right|up|down|select>`
  - `iobtn <1|2|3> [command]`
- **Example:**
  ```text
  GhostESP> input select
  GhostESP> iobtn 1 "scanap 5"
  ```

### `fav` / `favorites`
- **Description:** Manages shortcut list of favorite commands.
- **Syntax:** `fav [list|add <cmd>|remove <idx>|run <idx>]`
- **Example:**
  ```text
  GhostESP> fav list
  GhostESP> fav add "rgbmode rainbow"
  ```

### `loadconfig`
- **Description:** Forces reload of user configuration from SD card or NVS flash.
- **Syntax:** `loadconfig`
- **Example:**
  ```text
  GhostESP> loadconfig
  ```

---

## 24. Document Maintenance Checklist

Whenever new firmware capabilities or subcommands are added:
1. Locate the registration point in `main/core/commandline.c`.
2. Inspect the handler function in `main/core/commands/cmd_<name>.c`.
3. Add the command to the appropriate section above with:
   - Syntax and arguments.
   - Exact functionality and hardware prerequisites.
   - Realistic terminal input/output examples.
4. Update the **Last Updated** header timestamp.

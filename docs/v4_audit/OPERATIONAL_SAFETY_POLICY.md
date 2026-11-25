# Operational Safety & Research Policy (GhostESP V4)

1. **Authorized Research Only:** All 802.11 monitoring, Rogue AP radar, Deauth monitoring, BLE scanning, NRF24 spectrum analysis, and USB HID laboratory capabilities must only be operated against hardware and networks owned by or explicitly authorized in writing to the researcher.
2. **Passive-By-Default Operation:** Boot initializes passive telemetry (`temp_monitor`), SoftAP (`Hitesh_ESP`), and monitoring readiness without transmitting deauthentication frames, beacon floods, DHCP starvation, or HID keystrokes.
3. **Explicit BadUSB Authorization Guard:** `badusb run` and `badusb exec` refuse execution unless explicitly invoked with `--confirm` (or confirmed via the Desktop Command Center authorization checkbox), warning users that TinyUSB claims `GPIO19/20` and drops `COM5`.
4. **Non-Destructive Partition Handling:** `partitions.csv` is locked against silent modification to protect user NVS calibration and app partitions.

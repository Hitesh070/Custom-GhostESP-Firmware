# Passive Wi-Fi Security Research Guide (Authorized Lab Use Only)

> **Policy Reminder:** Only monitor networks you own or have explicit written authorization to test. Active disruption commands (`attack`, `wifitest`, `beaconspam`, `dhcpstarve`, `saeflood`, `reconnect`) fall under [`OPERATIONAL_SAFETY_POLICY.md`](OPERATIONAL_SAFETY_POLICY.md).

## 1. Non-Destructive Passive Audit Workflow
1. **Scan Environment:** `scanap` scans channels and caches discovered APs (`[0]..[N]`).
2. **Stop & Preserve Results:** `stopscan` stops active scanning while preserving `scanned_aps` for inspection via `list -a`, `wpa3check`, and `congestion`.
3. **Evil Twin / Rogue AP Radar:**
   - `roguedetect start` monitors beacons passively.
   - Legitimate multi-BSSID Enterprise deployments sharing an SSID and security suite (e.g., `VITC-HOS2-4`) are classified `CLEAN` rather than false-positive Evil Twins.
   - `roguedetect status` displays tracked APs and flagged threats (`Channel Jump`, security downgrade, rogue clone).
   - `roguedetect stop` returns Wi-Fi gracefully.
4. **Deauthentication / Disassociation Flood Detection:**
   - `deauthmon start` counts deauth/disassoc management frames passively and decays live fps over a `2 s` window.
   - `deauthmon status` reports totals, live fps, and TX/RX MAC pairs with reason codes.

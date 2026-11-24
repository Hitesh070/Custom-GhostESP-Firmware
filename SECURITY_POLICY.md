# GhostESP God Mode V3 – Security & Authorization Policy

**Scope:** Wireless Security Research, Protocol Auditing, and Embedded Engineering  
**Authority:** Autonomous Security Engineering Organization  
**Version:** 3.0.0  

---

## 1. Ethical & Authorized Research Principles

GhostESP is strictly engineered for defensive security research, protocol analysis, RF auditing, and authorized security assessments.

### Mandatory Rules of Engagement:
1. **Separation of Passive Monitoring & Active Testing:**
   - Passive observation (Wi-Fi scanning, beacon capture, probe monitoring, deauth detection, BLE sniffing, NRF24 carrier scanning, IR receiving) is non-intrusive and emits zero RF.
   - Passive tools never silently trigger or initiate RF transmissions.
2. **Explicit User Confirmation for Active Operations:**
   - Any operation that transmits RF or emits USB keystrokes requires explicit user confirmation via CLI prompt or UI dialogue.
3. **Authorized Lab Profile (`PROFILE_AUTHORIZED_LAB`):**
   - Active testing is locked unless an authorized research profile is enabled with configured target identifiers (SSID, BSSID, or BLE address).
4. **No Destructive Capabilities:**
   - No credential harvesting, malware delivery, unauthorized deauthentication transmission, or persistent covert implants are implemented.
5. **Transparency & Visibility:**
   - The contents and purpose of all test macros (such as USB HID test patterns) are fully inspectable and visible to the user before dispatch.
6. **Cancellation & Emergency Stop:**
   - A global stop command (`stop` / `cancel` / physical BACK button) immediately ceases all background tasks and sets RF front-ends to safe idle states.
7. **Zero Hardcoded Secrets:**
   - No private keys, passwords, or cloud credentials are hardcoded into the firmware binary.

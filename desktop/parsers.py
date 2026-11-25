"""Stateful and line-oriented telemetry parsers for GhostESP serial output."""

from __future__ import annotations

import json
import re
from dataclasses import dataclass, field
from typing import Any, Dict, List, Optional

ANSI_RE = re.compile(r"\x1b\[[0-9;]*[a-zA-Z]")
MAC_RE = re.compile(r"\b([0-9A-Fa-f]{2}(?::[0-9A-Fa-f]{2}){5})\b")


def strip_ansi(text: str) -> str:
    """Remove ANSI escape sequences and trailing whitespace."""
    return ANSI_RE.sub("", text or "").strip()


@dataclass
class ParsedEvent:
    kind: str
    data: Dict[str, Any] = field(default_factory=dict)


class GhostTelemetryParser:
    """Incrementally parses GhostESP CLI lines into structured events."""

    def __init__(self) -> None:
        self._current_ap: Optional[Dict[str, Any]] = None
        self._current_ble: Optional[Dict[str, Any]] = None
        self._ap_block_idx = 0
        self._in_ble_table = False

    def flush_pending_ap(self) -> Optional[ParsedEvent]:
        """Flush buffered multi-line AP entry (e.g., on prompt or next block)."""
        if self._current_ap and self._current_ap.get("bssid"):
            ap = dict(self._current_ap)
            self._current_ap = None
            return ParsedEvent("wifi_ap", ap)
        self._current_ap = None
        return None

    def flush_pending_ble(self) -> Optional[ParsedEvent]:
        """Flush buffered multi-line BLE entry (`bledetect` / `listadv`)."""
        if self._current_ble and self._current_ble.get("mac"):
            ble = dict(self._current_ble)
            self._current_ble = None
            return ParsedEvent("ble_device", ble)
        self._current_ble = None
        return None

    def feed_line(self, raw_line: str) -> List[ParsedEvent]:
        """Parse one incoming serial line and emit 0..N structured events."""
        clean = strip_ansi(raw_line)
        events: List[ParsedEvent] = []
        if not clean:
            return events

        # Flush pending AP/BLE blocks if we hit the prompt or an unrelated section banner
        if clean.startswith("ghost>") or clean.startswith("ghost >") or clean.startswith("==="):
            flushed = self.flush_pending_ap()
            if flushed:
                events.append(flushed)
            flushed_ble = self.flush_pending_ble()
            if flushed_ble:
                events.append(flushed_ble)

        # 1. Structured [TEMP_JSON] telemetry
        if "[TEMP_JSON]" in clean:
            idx = clean.find("{")
            if idx != -1:
                try:
                    payload = json.loads(clean[idx:])
                    events.append(ParsedEvent("temp_json", payload))
                    return events
                except json.JSONDecodeError:
                    pass

        # Text fallback for `temp` command
        m_temp = re.search(r"Current Die Temp\s*:\s*(-?\d+(?:\.\d+)?)\s*C\s*\((-?\d+(?:\.\d+)?)\s*F\)\s*\[([^\]]+)\]", clean)
        if m_temp:
            events.append(ParsedEvent("temp_text", {
                "temp_c": float(m_temp.group(1)),
                "temp_f": float(m_temp.group(2)),
                "band": m_temp.group(3).strip(),
            }))
            return events

        # 2. Multi-line `scanap` / `list -a` AP block parsing
        m_ssid = re.match(r"^\[\d+\]\s*SSID:\s*(.*?)(?:,\s*)?$", clean)
        if m_ssid:
            flushed = self.flush_pending_ap()
            if flushed:
                events.append(flushed)
            ssid = m_ssid.group(1).strip().rstrip(",")
            self._current_ap = {
                "bssid": "",
                "ssid": ssid or "<Hidden>",
                "channel": 0,
                "rssi": -100,
                "security": "UNKNOWN",
                "vendor": "",
                "threat": "CLEAN",
            }
            return events

        if self._current_ap is not None:
            m_bssid = re.search(r"BSSID:\s*([0-9A-Fa-f:]{17})", clean)
            if m_bssid:
                self._current_ap["bssid"] = m_bssid.group(1).upper()
                return events
            m_rssi = re.search(r"RSSI:\s*(-?\d+)", clean)
            if m_rssi:
                self._current_ap["rssi"] = int(m_rssi.group(1))
                return events
            m_ch = re.search(r"Channel:\s*(\d+)", clean)
            if m_ch:
                self._current_ap["channel"] = int(m_ch.group(1))
                return events
            m_sec = re.search(r"(?:Security|Auth|Encryption):\s*([^,]+)", clean, re.IGNORECASE)
            if m_sec:
                self._current_ap["security"] = m_sec.group(1).strip().rstrip(",")
                return events
            m_vnd = re.search(r"Vendor:\s*(.+)", clean)
            if m_vnd:
                self._current_ap["vendor"] = m_vnd.group(1).strip().rstrip(",")
                flushed = self.flush_pending_ap()
                if flushed:
                    events.append(flushed)
                return events

        # 3. `roguedetect status` table rows: BSSID | Ch | RSSI | Security | SSID | Threat
        if "|" in clean:
            parts = [p.strip() for p in clean.split("|")]
            if len(parts) >= 6 and MAC_RE.fullmatch(parts[0]):
                try:
                    ch_val = int(re.sub(r"[^0-9]", "", parts[1]) or "0")
                except ValueError:
                    ch_val = 0
                try:
                    rssi_val = int(re.sub(r"[^0-9-]", "", parts[2]) or "-100")
                except ValueError:
                    rssi_val = -100
                threat = parts[5] or "CLEAN"
                ap_data = {
                    "bssid": parts[0].upper(),
                    "channel": ch_val,
                    "rssi": rssi_val,
                    "security": parts[3] or "UNKNOWN",
                    "ssid": parts[4] or "<Hidden>",
                    "vendor": "",
                    "threat": threat,
                }
                events.append(ParsedEvent("wifi_ap", ap_data))
                if threat.upper() not in ("CLEAN", "NONE", "IDLE", "OK", "-"):
                    events.append(ParsedEvent("timeline", {
                        "subsystem": "WIFI_ROGUE",
                        "severity": "HIGH",
                        "details": f"{ap_data['ssid']} [{ap_data['bssid']}] {threat}",
                    }))
                return events

            # 4. `nrf24lab sweep` row: CH | 2.xxx GHz | pct% | bar
            m_sweep = re.match(r"^\s*(\d+)\s*\|\s*2\.(\d+)\s*GHz\s*\|\s*(\d+)%", clean)
            if m_sweep:
                events.append(ParsedEvent("nrf24_sweep_bin", {
                    "channel": int(m_sweep.group(1)),
                    "freq_mhz": 2000 + int(m_sweep.group(2)),
                    "activity_pct": int(m_sweep.group(3)),
                }))
                return events

            # 5. `profile list` row: ID | Name | YES * / no | Desc
            if parts[0].isdigit() and len(parts) >= 4 and parts[2].upper().startswith(("YES", "NO")):
                events.append(ParsedEvent("profile_row", {
                    "id": int(parts[0]),
                    "name": parts[1],
                    "active": "YES" in parts[2].upper(),
                    "description": parts[3],
                }))
                return events

            # 6. `deauthmon status` row: Time (s) | Type | Transmitter MAC | Receiver MAC | Reason
            if len(parts) >= 5 and MAC_RE.search(parts[2]) and MAC_RE.search(parts[3]):
                events.append(ParsedEvent("deauth_event", {
                    "time_s": parts[0],
                    "frame_type": parts[1],
                    "transmitter": MAC_RE.search(parts[2]).group(1).upper(),
                    "receiver": MAC_RE.search(parts[3]).group(1).upper(),
                    "reason": parts[4],
                }))
                return events

            # 7. `selftest` row: Subsystem | Diagnostic Test | Result | Details
            if len(parts) >= 4 and parts[2] in ("PASS", "FAIL", "WARNING", "HW REQUIRED", "UNTESTED"):
                events.append(ParsedEvent("selftest_row", {
                    "subsystem": parts[0],
                    "test": parts[1],
                    "result": parts[2],
                    "details": parts[3],
                }))
                return events

        # 8. `roguedetect status` summary header
        m_rogue_sum = re.search(r"Status:\s*(ACTIVE[^|]*|IDLE)\s*\|\s*Tracked APs:\s*(\d+)\s*\|\s*Threats Detected:\s*(\d+)", clean)
        if m_rogue_sum:
            events.append(ParsedEvent("rogue_summary", {
                "status": m_rogue_sum.group(1).strip(),
                "tracked_aps": int(m_rogue_sum.group(2)),
                "threats": int(m_rogue_sum.group(3)),
            }))
            return events

        # 9. `deauthmon status` summary header
        m_deauth_sum = re.search(r"Status:\s*(ACTIVE|IDLE)\s*\|\s*Total Deauths:\s*(\d+)\s*\|\s*Disassocs:\s*(\d+)\s*\|\s*Live Rate:\s*(\d+)\s*fps", clean)
        if m_deauth_sum:
            events.append(ParsedEvent("deauth_summary", {
                "status": m_deauth_sum.group(1).strip(),
                "total_deauths": int(m_deauth_sum.group(2)),
                "disassocs": int(m_deauth_sum.group(3)),
                "rate_fps": int(m_deauth_sum.group(4)),
            }))
            return events

        # 10. Multi-line & single-line BLE device discoveries (`bledetect`, `blescan`, `listadv`, `listairtags`, `listflippers`)
        if re.match(r"^[IWEVD]\s*\(\d+\)\s*(?:BLE_INIT|BLE_MANAGER|NimBLE|wifi|ap_manager):", clean):
            return events

        m_ble_hdr = re.match(r"^\[\d+\]\s+(?:(BLE Advertiser)|(.+?)\s+found)\s*$", clean)
        if m_ble_hdr:
            flushed_ble = self.flush_pending_ble()
            if flushed_ble:
                events.append(flushed_ble)
            dtype = "BLE Advertiser" if m_ble_hdr.group(1) else m_ble_hdr.group(2).strip()
            self._current_ble = {
                "mac": "",
                "name": dtype,
                "rssi": -90,
                "dev_type": dtype,
                "details": clean,
            }
            return events

        if self._current_ble is not None:
            m_bmac = re.match(r"^MAC:\s*([0-9A-Fa-f:]{17})", clean)
            if m_bmac:
                self._current_ble["mac"] = m_bmac.group(1).upper()
                return events
            m_brssi = re.search(r"RSSI:\s*(-?\d+)\s*dBm", clean)
            if m_brssi:
                self._current_ble["rssi"] = int(m_brssi.group(1))
                return events
            m_bname = re.match(r"^(?:Name|Variant|Manufacturer):\s*(.+)", clean)
            if m_bname:
                val = m_bname.group(1).strip()
                if val:
                    self._current_ble["name"] = val
                    if clean.startswith("Variant:"):
                        self._current_ble["dev_type"] = val
                return events

        # Bare `MAC: xx:xx:xx:xx:xx:xx` lines from `blescan -t`
        m_bare_mac = re.match(r"^MAC:\s*([0-9A-Fa-f:]{17})\b", clean)
        if m_bare_mac:
            events.append(ParsedEvent("ble_device", {
                "mac": m_bare_mac.group(1).upper(),
                "name": "BLE Raw",
                "rssi": 0,
                "dev_type": "BLE Raw",
                "details": clean,
            }))
            return events

        m_mac = MAC_RE.search(clean)
        if m_mac and any(tok in clean for tok in ("Advertiser", "AirTag", "Flipper", "iBeacon", "Skimmer", "Fast Pair", "Apple Watch")):
            mac = m_mac.group(1).upper()
            rssi_m = re.search(r"(-?\d+)\s*dBm", clean)
            if not rssi_m:
                rssi_m = re.search(r"RSSI[:\s]+(-?\d+)", clean)
            rssi_val = int(rssi_m.group(1)) if rssi_m else -90
            if "|" in clean:
                bparts = [p.strip() for p in clean.split("|")]
                dev_type = bparts[0] if bparts else "BLE"
                name = bparts[3] if len(bparts) > 3 else (bparts[-1] if len(bparts) > 1 else "Unknown")
            else:
                name_m = re.search(r"Name:\s*([^,\r\n]+)", clean)
                name = name_m.group(1).strip() if name_m else "BLE Node"
                dev_type = "AirTag" if "AirTag" in clean else ("Flipper" if "Flipper" in clean else "BLE")
            events.append(ParsedEvent("ble_device", {
                "mac": mac,
                "name": name or "Unknown",
                "rssi": rssi_val,
                "dev_type": dev_type,
                "details": clean,
            }))
            return events

        # 11. Memory & NRF24 lines
        m_sram = re.search(r"Internal SRAM\s*:\s*(\d+)\s*KB free", clean)
        if m_sram:
            events.append(ParsedEvent("memory", {"sram_free_kb": int(m_sram.group(1))}))
            return events
        m_psram = re.search(r"Octal PSRAM\s*:\s*(\d+)\s*(KB|MB)\s*(?:total )?free", clean)
        if m_psram:
            val = int(m_psram.group(1))
            kb = val * 1024 if m_psram.group(2) == "MB" else val
            events.append(ParsedEvent("memory", {"psram_free_kb": kb}))
            return events

        if "HARDWARE UNRESPONSIVE" in clean or "MODULE CONNECTED & RESPONSIVE" in clean or "0x00 CONFIG" in clean:
            events.append(ParsedEvent("nrf24_line", {"text": clean}))

        # 12. High-signal timeline notifications
        if "ROGUE ALERT" in clean or "Evil Twin" in clean:
            events.append(ParsedEvent("timeline", {"subsystem": "WIFI_ROGUE", "severity": "HIGH", "details": clean}))
        elif "DEAUTH STORM" in clean:
            events.append(ParsedEvent("timeline", {"subsystem": "WIFI_DEAUTH", "severity": "CRITICAL", "details": clean}))
        elif "WiFi Scan started" in clean or "WiFi scan stopped" in clean or ("Found" in clean and "access points" in clean):
            events.append(ParsedEvent("timeline", {"subsystem": "WIFI_SCAN", "severity": "INFO", "details": clean}))
        elif "Profile" in clean and ("applied" in clean.lower() or "switched" in clean.lower()):
            events.append(ParsedEvent("timeline", {"subsystem": "PROFILE", "severity": "INFO", "details": clean}))
        elif "Self-Test Summary" in clean:
            events.append(ParsedEvent("timeline", {"subsystem": "SELFTEST", "severity": "INFO", "details": clean}))

        return events

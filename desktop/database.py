"""Thread-safe SQLite WAL persistence layer for GhostESP Command Center."""

from __future__ import annotations

import json
import sqlite3
import threading
import time
from typing import Any, Dict, List, Optional


class GhostDatabase:
    """Thread-safe SQLite telemetry store using WAL journaling."""

    def __init__(self, db_path: str = "ghost_telemetry.db") -> None:
        self.db_path = db_path
        self._lock = threading.Lock()
        self._conn = sqlite3.connect(self.db_path, check_same_thread=False)
        self._conn.row_factory = sqlite3.Row
        with self._lock:
            self._conn.execute("PRAGMA journal_mode=WAL;")
            self._conn.execute("PRAGMA synchronous=NORMAL;")
            self._create_tables()

    def _create_tables(self) -> None:
        self._conn.executescript(
            """
            CREATE TABLE IF NOT EXISTS sessions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                port TEXT NOT NULL,
                started_at TEXT NOT NULL,
                ended_at TEXT,
                chip_info TEXT,
                firmware_version TEXT
            );

            CREATE TABLE IF NOT EXISTS wifi_aps (
                bssid TEXT PRIMARY KEY,
                ssid TEXT,
                channel INTEGER,
                rssi INTEGER,
                security TEXT,
                vendor TEXT,
                is_rogue INTEGER DEFAULT 0,
                threat_details TEXT,
                last_seen TEXT
            );

            CREATE TABLE IF NOT EXISTS ble_devices (
                mac TEXT PRIMARY KEY,
                name TEXT,
                rssi INTEGER,
                dev_type TEXT,
                details TEXT,
                last_seen TEXT
            );

            CREATE TABLE IF NOT EXISTS deauth_events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                frame_type TEXT,
                transmitter TEXT,
                receiver TEXT,
                reason TEXT
            );

            CREATE TABLE IF NOT EXISTS temp_samples (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                temp_c REAL,
                temp_f REAL,
                min_c REAL,
                max_c REAL,
                avg_c REAL,
                sram_free_kb INTEGER,
                psram_free_kb INTEGER
            );

            CREATE TABLE IF NOT EXISTS nrf24_scans (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                channel INTEGER,
                freq_mhz INTEGER,
                activity_pct INTEGER
            );

            CREATE TABLE IF NOT EXISTS event_timeline (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                subsystem TEXT NOT NULL,
                event_type TEXT NOT NULL,
                severity TEXT NOT NULL,
                details TEXT
            );
            """
        )
        self._conn.commit()

    @staticmethod
    def _now_iso() -> str:
        return time.strftime("%Y-%m-%d %H:%M:%S")

    def start_session(self, port: str, chip_info: str = "", firmware_version: str = "") -> int:
        with self._lock:
            cur = self._conn.execute(
                "INSERT INTO sessions (port, started_at, chip_info, firmware_version) VALUES (?, ?, ?, ?)",
                (port, self._now_iso(), chip_info, firmware_version),
            )
            self._conn.commit()
            return int(cur.lastrowid)

    def end_session(self, session_id: Optional[int]) -> None:
        if session_id is None:
            return
        with self._lock:
            self._conn.execute(
                "UPDATE sessions SET ended_at = ? WHERE id = ?",
                (self._now_iso(), session_id),
            )
            self._conn.commit()

    def log_ap(
        self,
        bssid: str,
        ssid: str,
        channel: int,
        rssi: int,
        security: str = "UNKNOWN",
        is_rogue: int = 0,
        threat_details: str = "CLEAN",
        vendor: str = "",
    ) -> None:
        if not bssid:
            return
        with self._lock:
            self._conn.execute(
                """
                INSERT INTO wifi_aps
                    (bssid, ssid, channel, rssi, security, vendor, is_rogue, threat_details, last_seen)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
                ON CONFLICT(bssid) DO UPDATE SET
                    ssid = COALESCE(NULLIF(excluded.ssid, ''), wifi_aps.ssid),
                    channel = CASE WHEN excluded.channel > 0 THEN excluded.channel ELSE wifi_aps.channel END,
                    rssi = excluded.rssi,
                    security = CASE WHEN excluded.security != 'UNKNOWN' THEN excluded.security ELSE wifi_aps.security END,
                    vendor = COALESCE(NULLIF(excluded.vendor, ''), wifi_aps.vendor),
                    is_rogue = excluded.is_rogue,
                    threat_details = excluded.threat_details,
                    last_seen = excluded.last_seen
                """,
                (bssid.upper(), ssid, channel, rssi, security, vendor, is_rogue, threat_details, self._now_iso()),
            )
            self._conn.commit()

    def log_ble_device(self, mac: str, name: str, rssi: int, dev_type: str = "BLE", details: str = "") -> None:
        if not mac:
            return
        with self._lock:
            self._conn.execute(
                """
                INSERT INTO ble_devices (mac, name, rssi, dev_type, details, last_seen)
                VALUES (?, ?, ?, ?, ?, ?)
                ON CONFLICT(mac) DO UPDATE SET
                    name = COALESCE(NULLIF(excluded.name, 'Unknown'), ble_devices.name),
                    rssi = excluded.rssi,
                    dev_type = excluded.dev_type,
                    details = excluded.details,
                    last_seen = excluded.last_seen
                """,
                (mac.upper(), name, rssi, dev_type, details, self._now_iso()),
            )
            self._conn.commit()

    def log_deauth_event(self, frame_type: str, transmitter: str, receiver: str, reason: str) -> None:
        with self._lock:
            self._conn.execute(
                "INSERT INTO deauth_events (timestamp, frame_type, transmitter, receiver, reason) VALUES (?, ?, ?, ?, ?)",
                (self._now_iso(), frame_type, transmitter.upper(), receiver.upper(), reason),
            )
            self._conn.commit()

    def log_temp_sample(
        self,
        temp_c: float,
        temp_f: float,
        min_c: float = 0.0,
        max_c: float = 0.0,
        avg_c: float = 0.0,
        sram_free_kb: int = 0,
        psram_free_kb: int = 0,
    ) -> None:
        with self._lock:
            self._conn.execute(
                """
                INSERT INTO temp_samples
                    (timestamp, temp_c, temp_f, min_c, max_c, avg_c, sram_free_kb, psram_free_kb)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                """,
                (self._now_iso(), temp_c, temp_f, min_c, max_c, avg_c, sram_free_kb, psram_free_kb),
            )
            self._conn.commit()

    def log_nrf24_bin(self, channel: int, freq_mhz: int, activity_pct: int) -> None:
        with self._lock:
            self._conn.execute(
                "INSERT INTO nrf24_scans (timestamp, channel, freq_mhz, activity_pct) VALUES (?, ?, ?, ?)",
                (self._now_iso(), channel, freq_mhz, activity_pct),
            )
            self._conn.commit()

    def log_event(self, subsystem: str, event_type: str, severity: str, details: str) -> None:
        with self._lock:
            self._conn.execute(
                "INSERT INTO event_timeline (timestamp, subsystem, event_type, severity, details) VALUES (?, ?, ?, ?, ?)",
                (self._now_iso(), subsystem, event_type, severity, details),
            )
            self._conn.commit()

    def get_wifi_aps(self) -> List[Dict[str, Any]]:
        with self._lock:
            rows = self._conn.execute("SELECT * FROM wifi_aps ORDER BY last_seen DESC").fetchall()
            return [dict(r) for r in rows]

    def get_ble_devices(self) -> List[Dict[str, Any]]:
        with self._lock:
            rows = self._conn.execute("SELECT * FROM ble_devices ORDER BY last_seen DESC").fetchall()
            return [dict(r) for r in rows]

    def get_temp_samples(self, limit: int = 120) -> List[Dict[str, Any]]:
        with self._lock:
            rows = self._conn.execute(
                "SELECT * FROM temp_samples ORDER BY id DESC LIMIT ?", (limit,)
            ).fetchall()
            return [dict(r) for r in reversed(rows)]

    def get_events(self, limit: int = 500) -> List[Dict[str, Any]]:
        with self._lock:
            rows = self._conn.execute(
                "SELECT * FROM event_timeline ORDER BY id DESC LIMIT ?", (limit,)
            ).fetchall()
            return [dict(r) for r in rows]

    def export_events_json(self, filepath: str) -> int:
        events = self.get_events(limit=5000)
        with open(filepath, "w", encoding="utf-8") as fh:
            json.dump(events, fh, indent=2)
        return len(events)

    def export_session_bundle(self, filepath: str) -> None:
        bundle = {
            "exported_at": self._now_iso(),
            "wifi_aps": self.get_wifi_aps(),
            "ble_devices": self.get_ble_devices(),
            "temp_samples": self.get_temp_samples(500),
            "events": self.get_events(2000),
        }
        with open(filepath, "w", encoding="utf-8") as fh:
            json.dump(bundle, fh, indent=2)

    def clear_telemetry(self) -> None:
        with self._lock:
            for tbl in ("wifi_aps", "ble_devices", "deauth_events", "temp_samples", "nrf24_scans", "event_timeline"):
                self._conn.execute(f"DELETE FROM {tbl}")
            self._conn.commit()

    def close(self) -> None:
        with self._lock:
            if self._conn:
                self._conn.close()
                self._conn = None  # type: ignore[assignment]

"""
GhostESP God Mode V3 - Laptop Command Center Local Database Storage
Author: Autonomous Desktop Application Engineering Organization
"""

import sqlite3
import os
import json
import csv
from datetime import datetime

class GhostDatabase:
    def __init__(self, db_path="ghostesp_sessions.db"):
        self.db_path = db_path
        self._init_db()

    def _get_connection(self):
        return sqlite3.connect(self.db_path)

    def _init_db(self):
        with self._get_connection() as conn:
            cursor = conn.cursor()
            # Session Table
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS sessions (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    start_time TEXT,
                    target_hw TEXT,
                    firmware_ver TEXT
                )
            """)
            # Wi-Fi AP Table
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS wifi_aps (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    session_id INTEGER,
                    bssid TEXT,
                    ssid TEXT,
                    channel INTEGER,
                    rssi INTEGER,
                    authmode TEXT,
                    is_rogue INTEGER,
                    threat_reason TEXT,
                    timestamp TEXT
                )
            """)
            # Deauth Events Table
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS deauth_events (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    session_id INTEGER,
                    transmitter TEXT,
                    receiver TEXT,
                    reason_code INTEGER,
                    rate_fps INTEGER,
                    timestamp TEXT
                )
            """)
            # BLE Nodes Table
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS ble_devices (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    session_id INTEGER,
                    address TEXT,
                    name TEXT,
                    rssi INTEGER,
                    uuid TEXT,
                    timestamp TEXT
                )
            """)
            # Event Timeline Table
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS event_timeline (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    session_id INTEGER,
                    subsystem TEXT,
                    event_type TEXT,
                    severity TEXT,
                    message TEXT,
                    timestamp TEXT
                )
            """)
            conn.commit()

    def log_event(self, subsystem, event_type, severity, message, session_id=1):
        with self._get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute("""
                INSERT INTO event_timeline (session_id, subsystem, event_type, severity, message, timestamp)
                VALUES (?, ?, ?, ?, ?, ?)
            """, (session_id, subsystem, event_type, severity, message, datetime.now().isoformat()))
            conn.commit()

    def log_ap(self, bssid, ssid, channel, rssi, authmode, is_rogue=0, threat="", session_id=1):
        with self._get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute("""
                INSERT INTO wifi_aps (session_id, bssid, ssid, channel, rssi, authmode, is_rogue, threat_reason, timestamp)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
            """, (session_id, bssid, ssid, channel, rssi, authmode, is_rogue, threat, datetime.now().isoformat()))
            conn.commit()

    def export_events_json(self, filepath):
        with self._get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute("SELECT subsystem, event_type, severity, message, timestamp FROM event_timeline ORDER BY id DESC")
            rows = cursor.fetchall()
            data = [{"subsystem": r[0], "event_type": r[1], "severity": r[2], "message": r[3], "timestamp": r[4]} for r in rows]
            with open(filepath, "w") as f:
                json.dump(data, f, indent=2)

    def export_aps_csv(self, filepath):
        with self._get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute("SELECT bssid, ssid, channel, rssi, authmode, is_rogue, threat_reason, timestamp FROM wifi_aps ORDER BY id DESC")
            rows = cursor.fetchall()
            with open(filepath, "w", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                writer.writerow(["BSSID", "SSID", "Channel", "RSSI", "AuthMode", "IsRogue", "ThreatReason", "Timestamp"])
                writer.writerows(rows)

    def close(self):
        pass


"""Unit and integration tests for GhostESP Command Center parsers & SQLite persistence."""

from __future__ import annotations

import json
import sys
from pathlib import Path

import pytest

DESKTOP_DIR = Path(__file__).resolve().parents[1]
if str(DESKTOP_DIR) not in sys.path:
    sys.path.insert(0, str(DESKTOP_DIR))

from database import GhostDatabase
from parsers import GhostTelemetryParser, strip_ansi


def test_strip_ansi():
    raw = "\x1b[38;5;36mghost\x1b[0m> scanap\r\n"
    assert strip_ansi(raw) == "ghost> scanap"


def test_parse_temp_json_and_text():
    parser = GhostTelemetryParser()
    line = (
        '[TEMP_JSON] {"sensor":"ESP32 CHIP INTERNAL TEMPERATURE","init":true,"valid":true,'
        '"temp_c":46.5,"temp_f":115.7,"min_c":44.0,"max_c":48.0,"avg_c":46.0,'
        '"unit":"C","interval_ms":2000,"samples":12,"errors":0,"last_ms":24000,'
        '"range_min":-10,"range_max":80,"sram_free_kb":64,"psram_free_kb":7900}'
    )
    events = parser.feed_line(line)
    assert len(events) == 1
    assert events[0].kind == "temp_json"
    assert events[0].data["temp_c"] == pytest.approx(46.5)
    assert events[0].data["sram_free_kb"] == 64

    # Malformed JSON must not raise
    bad = parser.feed_line("[TEMP_JSON] {not-valid-json")
    assert bad == []


def test_parse_scanap_multiline_block():
    parser = GhostTelemetryParser()
    lines = [
        "[0] SSID: VITC-HOS2-4,",
        "     BSSID: 70:3A:0E:E7:F6:01,",
        "     RSSI: -74,",
        "     Channel: 1,",
        "     Vendor: Ruckus Wireless",
        "[1] SSID: OpenHotspot,",
        "     BSSID: aa:bb:cc:dd:ee:ff,",
        "     RSSI: -52,",
        "     Channel: 6,",
        "ghost> ",
    ]
    all_events = []
    for ln in lines:
        all_events.extend(parser.feed_line(ln))

    aps = [e.data for e in all_events if e.kind == "wifi_ap"]
    assert len(aps) == 2
    assert aps[0]["ssid"] == "VITC-HOS2-4"
    assert aps[0]["bssid"] == "70:3A:0E:E7:F6:01"
    assert aps[0]["rssi"] == -74
    assert aps[0]["channel"] == 1
    assert aps[0]["vendor"] == "Ruckus Wireless"
    assert aps[1]["bssid"] == "AA:BB:CC:DD:EE:FF"


def test_parse_roguedetect_deauth_profiles_and_selftest():
    parser = GhostTelemetryParser()
    ev1 = parser.feed_line(" 70:3A:0E:E7:F6:01 | 1  | -74  | WPA2_PSK       | CorpWiFi             | EVIL_TWIN")
    kinds1 = {e.kind for e in ev1}
    assert "wifi_ap" in kinds1
    assert "timeline" in kinds1

    ev2 = parser.feed_line(" Status: ACTIVE | Total Deauths: 15 | Disassocs: 2 | Live Rate: 7 fps")
    assert ev2[0].kind == "deauth_summary"
    assert ev2[0].data["total_deauths"] == 15
    assert ev2[0].data["rate_fps"] == 7

    ev3 = parser.feed_line("  1 | WiFi-Research   | YES *  | Dedicated 802.11 monitor")
    assert ev3[0].kind == "profile_row"
    assert ev3[0].data["active"] is True
    assert ev3[0].data["id"] == 1

    ev4 = parser.feed_line(" RF Front-End| NRF24L01+ on SPI2 (CS10)    | HW REQUIRED | Disconnected / Unpowered")
    assert ev4[0].kind == "selftest_row"
    assert ev4[0].data["result"] == "HW REQUIRED"


def test_parse_nrf24_sweep_and_ble():
    parser = GhostTelemetryParser()
    ev_sw = parser.feed_line("  37 | 2.437 GHz |  80% | ####################     ")
    assert ev_sw[0].kind == "nrf24_sweep_bin"
    assert ev_sw[0].data["channel"] == 37
    assert ev_sw[0].data["freq_mhz"] == 2437
    assert ev_sw[0].data["activity_pct"] == 80

    ev_ble = parser.feed_line("[0] Advertiser | AA:BB:CC:11:22:33 | -61 dBm | Flipper_Test")
    assert ev_ble[0].kind == "ble_device"
    assert ev_ble[0].data["mac"] == "AA:BB:CC:11:22:33"
    assert ev_ble[0].data["rssi"] == -61


def test_database_wal_and_roundtrip(tmp_path):
    db_file = tmp_path / "test_ghost.db"
    db = GhostDatabase(str(db_file))
    try:
        sid = db.start_session("COM5", "ESP32-S3", "V4")
        db.log_ap("11:22:33:44:55:66", "TestAP", 6, -55, "WPA2_PSK", 0, "CLEAN", "Espressif")
        db.log_ap("11:22:33:44:55:66", "TestAP", 6, -49, "WPA2_PSK", 1, "Channel Jump", "Espressif")
        db.log_ble_device("AA:BB:CC:DD:EE:01", "Beacon1", -68, "iBeacon", "raw")
        db.log_temp_sample(45.2, 113.36, 44.0, 47.0, 45.1, 70, 7900)
        db.log_deauth_event("Deauth", "AA:AA:AA:AA:AA:AA", "BB:BB:BB:BB:BB:BB", "Unspecified")
        db.log_event("SYSTEM", "CONNECT", "INFO", "Session opened")
        db.end_session(sid)

        aps = db.get_wifi_aps()
        assert len(aps) == 1
        assert aps[0]["rssi"] == -49
        assert aps[0]["is_rogue"] == 1

        bles = db.get_ble_devices()
        assert len(bles) == 1
        assert bles[0]["name"] == "Beacon1"

        temps = db.get_temp_samples()
        assert len(temps) == 1
        assert temps[0]["temp_c"] == pytest.approx(45.2)

        out_json = tmp_path / "events.json"
        cnt = db.export_events_json(str(out_json))
        assert cnt == 1
        loaded = json.loads(out_json.read_text(encoding="utf-8"))
        assert loaded[0]["subsystem"] == "SYSTEM"
    finally:
        db.close()


def test_pyside6_window_offscreen_smoke(tmp_path, monkeypatch):
    monkeypatch.setenv("QT_QPA_PLATFORM", "offscreen")
    from PySide6.QtWidgets import QApplication
    from app import GhostCommandCenterWindow

    qapp = QApplication.instance() or QApplication([])
    win = GhostCommandCenterWindow(db_path=str(tmp_path / "ui_test.db"))
    try:
        assert win.stack.count() == 18
        win._on_serial_line_ui(
            '[TEMP_JSON] {"sensor":"ESP32 CHIP INTERNAL TEMPERATURE","init":true,"valid":true,'
            '"temp_c":49.2,"temp_f":120.6,"min_c":45.0,"max_c":50.1,"avg_c":47.5,'
            '"unit":"C","interval_ms":2000,"samples":5,"errors":0,"last_ms":10000,'
            '"range_min":-10,"range_max":80,"sram_free_kb":88,"psram_free_kb":7850}'
        )
        assert win.kpi_temp.text() == "49.2 C"
        assert win.kpi_sram.text() == "88 KB"

        win._on_serial_line_ui("[0] Advertiser | AA:BB:CC:DD:EE:11 | -55 dBm | Pixel_9")
        assert win.ble_table.rowCount() == 1
        assert win.kpi_ble.text() == "1"

        win._on_serial_line_ui(" Thermal     | On-Chip Die Temp Sensor     | PASS        | 49.2 C (Verified)")
        assert win.selftest_table.rowCount() == 1

        # ESP-IDF BLE_INIT log must NOT become a scanned BLE device with timestamp as RSSI
        win._on_serial_line_ui("I (88734) BLE_INIT: Bluetooth MAC: 44:bd:8d:68:db:66")
        assert win.ble_table.rowCount() == 1
        # Bare `MAC:` lines from `blescan -t` must populate BLE Lab
        win._on_serial_line_ui("MAC: 65:42:60:3c:15:61")
        win._on_serial_line_ui("MAC: d6:3e:82:00:d7:56")
        assert win.ble_table.rowCount() == 3

        # Sorting-safe Wi-Fi upsert must not produce empty rows
        win._upsert_wifi({"bssid": "ZZ:11:22:33:44:55", "ssid": "Z_Net", "channel": 6, "rssi": -60, "security": "WPA2", "vendor": "V1", "threat": "CLEAN"})
        win._upsert_wifi({"bssid": "AA:11:22:33:44:55", "ssid": "A_Net", "channel": 1, "rssi": -70, "security": "OPEN", "vendor": "V2", "threat": "CLEAN"})
        for r in range(win.wifi_table.rowCount()):
            assert win.wifi_table.item(r, 1).text() in ("Z_Net", "A_Net")
    finally:
        win.close()
        qapp.processEvents()

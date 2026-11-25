"""Exhaustive V5 functional, UI, calculation, parser, export & state tests for GhostESP Command Center."""

from __future__ import annotations

import csv
import json
import math
import sys
from pathlib import Path

import pytest

DESKTOP_DIR = Path(__file__).resolve().parents[1]
if str(DESKTOP_DIR) not in sys.path:
    sys.path.insert(0, str(DESKTOP_DIR))

from PySide6.QtCore import Qt
from PySide6.QtTest import QTest
from PySide6.QtWidgets import QApplication, QFileDialog, QMessageBox, QPushButton, QTableWidget

from app import GhostCommandCenterWindow
from database import GhostDatabase
from parsers import GhostTelemetryParser
from serial_comm import GhostSDRInterface, GhostSerialTransceiver


@pytest.fixture
def qapp():
    return QApplication.instance() or QApplication([])


def _make_window(tmp_path, monkeypatch):
    monkeypatch.setenv("QT_QPA_PLATFORM", "offscreen")
    win = GhostCommandCenterWindow(db_path=str(tmp_path / "v5_test.db"))
    sent: list[str] = []
    win.serial.running = True
    win.serial.current_port_name = "MOCK_COM5"
    win.serial.send_command = lambda cmd: (sent.append(cmd), True)[1]  # type: ignore[method-assign]
    return win, sent


def test_navigation_all_17_views(tmp_path, monkeypatch, qapp):
    win, _ = _make_window(tmp_path, monkeypatch)
    try:
        assert win.stack.count() == 19
        assert len(win.nav_buttons) == 19
        for i, btn in enumerate(win.nav_buttons):
            btn.click()
            assert win.stack.currentIndex() == i
            assert btn.property("active") == "true"
    finally:
        win.close()
        qapp.processEvents()


def test_every_command_button_connected_and_offline_safe(tmp_path, monkeypatch, qapp):
    win, sent = _make_window(tmp_path, monkeypatch)
    warnings: list[str] = []
    monkeypatch.setattr(QMessageBox, "information", lambda *a: warnings.append(str(a[2])))
    monkeypatch.setattr(QMessageBox, "warning", lambda *a: warnings.append(str(a[2])))
    monkeypatch.setattr(QFileDialog, "getSaveFileName", lambda *a, **kw: ("", ""))
    try:
        # Click all action buttons on every page while connected (skipping export/screenshot file dialogs)
        all_buttons = win.findChildren(QPushButton)
        assert len(all_buttons) >= 60
        for btn in all_buttons:
            txt = btn.text()
            if btn in win.nav_buttons or txt in ("CONNECT", "DISCONNECT", "Refresh", "Screenshot", "Color Picker") or "Export" in txt:
                continue
            btn.click()
        # Verify representative commands were queued
        for expected in ("temp json", "selftest", "scanap", "stopscan", "list -a", "wpa3check",
                         "congestion", "roguedetect start", "deauthmon start", "bledetect",
                         "blescan -t", "listairtags", "listflippers", "nrf24lab diag",
                         "profile list", "mem", "ps", "df", "coredump", "badusb list"):
            assert expected in sent, f"Missing sent command: {expected}"
        # BadUSB builtin must have been blocked by unchecked checkbox
        assert "badusb run builtin --confirm" not in sent
        assert any("confirm" in w.lower() for w in warnings)

        # Now test offline state blocks commands cleanly without crash
        sent.clear()
        win.serial.running = False
        win._send_cmd("scanap")
        assert sent == []
    finally:
        win.close()
        qapp.processEvents()


def test_deterministic_calculations_and_null_nan_resilience(tmp_path, monkeypatch, qapp):
    win, _ = _make_window(tmp_path, monkeypatch)
    try:
        # Known C -> F vectors (firmware supplies both; verify display & rounding)
        for c, f in [(-10.0, 14.0), (0.0, 32.0), (25.0, 77.0), (52.2, 126.0), (80.0, 176.0)]:
            win._dispatch_event("temp_json", {
                "temp_c": c, "temp_f": f, "min_c": c, "max_c": c, "avg_c": c,
                "sram_free_kb": 50, "psram_free_kb": 8192, "valid": True, "samples": 1,
            })
            assert win.kpi_temp.text() == f"{c:.1f} C"
            assert win.kpi_psram.text() == "8.0 MB"

        # Null / NaN / Inf / invalid types must NOT raise
        for bad in (
            {"temp_c": None, "temp_f": None, "sram_free_kb": None, "psram_free_kb": None, "valid": True},
            {"temp_c": float("nan"), "temp_f": float("inf"), "sram_free_kb": "bad", "psram_free_kb": -1, "valid": True},
        ):
            win._dispatch_event("temp_json", bad)
    finally:
        win.close()
        qapp.processEvents()


def test_nrf24_freq_precision_and_ble_filtering():
    p = GhostTelemetryParser()
    # 2.45 GHz -> 2450 MHz (not 2045 MHz)
    ev = p.feed_line("  50 | 2.45 GHz | 42% | #########")
    assert len(ev) == 1
    assert ev[0].data["freq_mhz"] == 2450

    ev2 = p.feed_line("  37 | 2.437 GHz | 80% | ################")
    assert ev2[0].data["freq_mhz"] == 2437

    # System BLE_INIT log must be ignored
    assert p.feed_line("I (88734) BLE_INIT: Bluetooth MAC: 44:bd:8d:68:db:66") == []

    # Multi-line bledetect block
    block = [
        "[0] Fast Pair found",
        "     MAC: 55:b4:26:75:69:d2",
        "     RSSI: -93 dBm (Out of Range)",
        "     Variant: Fast Pair",
        "ghost> ",
    ]
    out = []
    for ln in block:
        out.extend(p.feed_line(ln))
    assert len(out) == 1
    assert out[0].kind == "ble_device"
    assert out[0].data["mac"] == "55:B4:26:75:69:D2"
    assert out[0].data["rssi"] == -93


def test_disconnect_clears_stale_live_indicators(tmp_path, monkeypatch, qapp):
    win, _ = _make_window(tmp_path, monkeypatch)
    try:
        win._dispatch_event("temp_json", {
            "temp_c": 51.2, "temp_f": 124.2, "min_c": 50.0, "max_c": 52.0, "avg_c": 51.0,
            "sram_free_kb": 48, "psram_free_kb": 8010, "valid": True, "samples": 10,
        })
        assert win.kpi_temp.text() == "51.2 C"
        win._on_status_changed_ui(False, "Disconnected")
        assert win.conn_badge.text() == "OFFLINE"
        assert "OFFLINE" in win.kpi_temp.text() or win.kpi_temp.text() == "--.- C"
        assert "IDLE" in win.sec_summary.text() or "OFFLINE" in win.sec_summary.text()
    finally:
        win.close()
        qapp.processEvents()


def test_console_history_clear_and_exports(tmp_path, monkeypatch, qapp):
    win, sent = _make_window(tmp_path, monkeypatch)
    try:
        win.console_in.setText("temp json")
        win._send_console_input()
        win.console_in.setText("mem")
        win._send_console_input()
        assert win.cmd_history == ["temp json", "mem"]

        # Up arrow recalls previous command
        QTest.keyClick(win.console_in, Qt.Key.Key_Up)
        assert win.console_in.text() == "mem"
        QTest.keyClick(win.console_in, Qt.Key.Key_Up)
        assert win.console_in.text() == "temp json"

        # Verify CSV export content matches table
        win._upsert_wifi({"bssid": "AA:BB:CC:DD:EE:FF", "ssid": "LabAP", "channel": 6, "rssi": -55, "security": "WPA2", "vendor": "Test", "threat": "CLEAN"})
        csv_path = tmp_path / "aps.csv"
        win._export_table_csv(win.wifi_table, str(csv_path))
        rows = list(csv.reader(csv_path.open(encoding="utf-8")))
        assert rows[0] == ["BSSID", "SSID", "Ch", "RSSI", "Security", "Vendor", "Threat"]
        assert rows[1] == ["AA:BB:CC:DD:EE:FF", "LabAP", "6", "-55", "WPA2", "Test", "CLEAN"]

        # Verify JSON bundle export
        bundle_path = tmp_path / "bundle.json"
        win.db.export_session_bundle(str(bundle_path))
        bundle = json.loads(bundle_path.read_text(encoding="utf-8"))
        assert bundle["wifi_aps"][0]["bssid"] == "AA:BB:CC:DD:EE:FF"

        # Verify all telemetry tables are read-only
        for tbl in win.findChildren(QTableWidget):
            assert tbl.editTriggers() == QTableWidget.EditTrigger.NoEditTriggers
    finally:
        win.close()
        qapp.processEvents()


def test_honest_sdr_and_requirements_sync():
    sdr = GhostSDRInterface().get_info()
    assert "ESP32-S3 is NOT an SDR" in sdr["hardware"] or "External" in sdr["driver"]
    reqs = (DESKTOP_DIR / "requirements.txt").read_text(encoding="utf-8").lower()
    assert "matplotlib" in reqs
    assert "pyside6" in reqs
    assert "pyserial" in reqs


def test_screen_mirror_binary_decode_and_controls(tmp_path, monkeypatch, qapp):
    import struct
    from parsers import MIRROR_END_BYTES, MIRROR_HDR_FMT, MIRROR_MAGIC_BYTES, extract_mirror_packets

    # Pack INFO (240x320) + 2x1 RGB444 (cmd=0x06) packet
    info_hdr = struct.pack(MIRROR_HDR_FMT, 0x47455350, 0x01, 240, 320, 16, 0, 0)
    # Two pixels: white (0xFFF) and red (0xF00) -> 3 bytes: 0xFF, 0xFF, 0x00
    px_payload = bytes([0xFF, 0xFF, 0x00])
    frame_hdr = struct.pack(MIRROR_HDR_FMT, 0x47455350, 0x06, 0, 0, 1, 0, len(px_payload))
    buf = bytearray(info_hdr + frame_hdr + px_payload + MIRROR_END_BYTES)
    pkts = extract_mirror_packets(buf)
    assert len(pkts) == 2
    assert len(buf) == 0

    win, sent = _make_window(tmp_path, monkeypatch)
    try:
        for pkt in pkts:
            win._on_mirror_packet_ui(*pkt)
        assert win.mirror_w == 240
        assert win.mirror_h == 320
        assert win.mirror_frames == 1
        assert win.mirror_img.pixelColor(0, 0).red() == 255
        assert win.mirror_img.pixelColor(1, 0).red() == 255
        assert win.mirror_img.pixelColor(1, 0).green() == 0
    finally:
        win.close()
        qapp.processEvents()

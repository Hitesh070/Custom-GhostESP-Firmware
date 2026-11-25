"""GhostESP Command Center — Industry-Grade PySide6 Engineering Suite."""

from __future__ import annotations

import csv
import json
import sys
import time
from collections import deque
from pathlib import Path
from typing import Dict, List, Optional

from PySide6.QtCore import QObject, Qt, QStringListModel, QTimer, Signal
from PySide6.QtGui import QColor, QFont
from PySide6.QtWidgets import (
    QApplication,
    QCheckBox,
    QComboBox,
    QCompleter,
    QFileDialog,
    QFrame,
    QGridLayout,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QPlainTextEdit,
    QPushButton,
    QScrollArea,
    QSpinBox,
    QStackedWidget,
    QStatusBar,
    QTableWidget,
    QTableWidgetItem,
    QVBoxLayout,
    QWidget,
)

from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from database import GhostDatabase
from parsers import GhostTelemetryParser
from serial_comm import GhostSDRInterface, GhostSerialTransceiver
from theme import APP_STYLESHEET, PALETTE


PINOUT_ROWS = [
    ("GPIO 1 / 2 / 4 / 47", "SD Card (SPI/SDMMC)", "Optional Slot (CS=4, CLK=1, MOSI=2, MISO=47)", "Board Config"),
    ("GPIO 5 / 6 / 7 / 15 / 16", "SPI3 Display LCD", "1.8\" ST7735S TFT (CS=5, DC=6, RST=7, MOSI=15, SCK=16)", "Locked"),
    ("GPIO 8 / 9", "I2C0 Primary", "External I2C Bus (SDA=8, SCL=9)", "Configured"),
    ("GPIO 10 / 11 / 12 / 13 / 14", "SPI2 RF Front-End", "NRF24L01+PA+LNA (CS=10, MOSI=11, SCK=12, MISO=13, CE=14)", "HW Dependent"),
    ("GPIO 17 / 18", "UART2 / ESP Comm", "Internal Peer UART2 (TX=17, RX=18)", "Relocated V4"),
    ("GPIO 19 / 20", "USB-OTG Native PHY", "USB-Serial-JTAG (COM5) & TinyUSB BadUSB/MSC", "Silicon Locked"),
    ("GPIO 27 - 32", "Octal/Quad Flash", "16 MB Internal SPI Flash", "Silicon Locked"),
    ("GPIO 33 - 38", "Octal PSRAM", "8 MB Internal Octal PSRAM (AP_3v3)", "Silicon Locked"),
    ("GPIO 43 / 44", "UART1 Telemetry", "CP210x Bridge (COM6) / GPS RX=44", "Active"),
    ("GPIO 48", "Onboard RGB", "WS2812B NeoPixel Status LED", "Active"),
]

COMMON_COMMANDS = [
    "help", "help all", "help godmode", "chipinfo", "temp", "temp json", "temp history",
    "temp c", "temp f", "temp interval 2000", "temp reset", "selftest", "version", "mem",
    "ps", "df", "ifconfig", "uptime", "scanap", "stopscan", "list -a", "wpa3check",
    "congestion", "roguedetect start", "roguedetect status", "roguedetect stop",
    "roguedetect clear", "deauthmon start", "deauthmon status", "deauthmon stop",
    "deauthmon clear", "blescan", "bledetect", "listairtags", "listflippers",
    "nrf24lab diag", "nrf24lab sweep 10", "nrf24lab benchmark 100",
    "profile list", "profile apply 0", "profile apply 1", "profile apply 2",
    "ghost status", "ghost radar", "ghost achievements", "ghost version",
    "sd status", "sd list", "badusb list", "usbkbd status", "irpin", "nfctest", "coredump",
]


class SerialSignals(QObject):
    """Thread-safe Qt signal bridge from serial background threads to UI."""
    line_received = Signal(str)
    status_changed = Signal(bool, str)


class MplChartWidget(QWidget):
    """Dark-themed Matplotlib canvas wrapper."""

    def __init__(self, title: str, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.figure = Figure(figsize=(6, 2.8), dpi=100, facecolor=PALETTE["bg_surface"])
        self.canvas = FigureCanvas(self.figure)
        self.ax = self.figure.add_subplot(111)
        self.title = title
        self._style_axes()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.canvas)

    def _style_axes(self) -> None:
        self.ax.set_facecolor(PALETTE["bg_console"])
        self.ax.set_title(self.title, color=PALETTE["accent_cyan"], fontsize=10, fontweight="bold")
        self.ax.tick_params(colors=PALETTE["text_secondary"], labelsize=8)
        for spine in self.ax.spines.values():
            spine.set_color(PALETTE["border_subtle"])
        self.ax.grid(True, color=PALETTE["border_subtle"], linestyle="--", linewidth=0.5, alpha=0.7)


def make_card(title: str) -> tuple[QFrame, QVBoxLayout]:
    card = QFrame()
    card.setObjectName("Card")
    layout = QVBoxLayout(card)
    layout.setContentsMargins(12, 10, 12, 10)
    layout.setSpacing(6)
    lbl = QLabel(title.upper())
    lbl.setObjectName("SectionTitle")
    layout.addWidget(lbl)
    return card, layout


def make_kpi(label: str, default_val: str = "--", accent: str = PALETTE["text_primary"]) -> tuple[QFrame, QLabel]:
    card = QFrame()
    card.setObjectName("Card")
    layout = QVBoxLayout(card)
    layout.setContentsMargins(12, 10, 12, 10)
    layout.setSpacing(3)
    hdr = QLabel(label.upper())
    hdr.setObjectName("SectionTitle")
    val = QLabel(default_val)
    val.setObjectName("MetricValue")
    val.setStyleSheet(f"color: {accent};")
    layout.addWidget(hdr)
    layout.addWidget(val)
    return card, val


class GhostCommandCenterWindow(QMainWindow):
    """Main GhostESP Command Center desktop window."""

    def __init__(self, db_path: str = "ghost_telemetry.db") -> None:
        super().__init__()
        self.setWindowTitle("GhostESP Command Center V4 // 7Semi ESP32-S3-WROOM-1-N16R8")
        self.resize(1460, 900)
        self.setStyleSheet(APP_STYLESHEET)

        self.db = GhostDatabase(db_path=db_path)
        self.parser = GhostTelemetryParser()
        self.sdr = GhostSDRInterface()
        self.signals = SerialSignals()
        self.signals.line_received.connect(self._on_serial_line_ui)
        self.signals.status_changed.connect(self._on_status_changed_ui)

        self.serial = GhostSerialTransceiver(
            on_line_callback=self.signals.line_received.emit,
            on_status_callback=self.signals.status_changed.emit,
        )

        self.session_id: Optional[int] = None
        self.known_aps: Dict[str, int] = {}
        self.known_bles: Dict[str, int] = {}
        self.temp_points: deque = deque(maxlen=120)
        self.sweep_bins: Dict[int, int] = {}
        self.cmd_history: List[str] = []
        self.nav_buttons: List[QPushButton] = []

        self.temp_poll_timer = QTimer(self)
        self.temp_poll_timer.timeout.connect(self._poll_temperature)

        self._build_ui()
        self._refresh_ports()
        self._load_persisted_snapshot()

    def _build_ui(self) -> None:
        central = QWidget()
        self.setCentralWidget(central)
        root_layout = QVBoxLayout(central)
        root_layout.setContentsMargins(0, 0, 0, 0)
        root_layout.setSpacing(0)

        # Top connection & hardware identity bar
        top_bar = QFrame()
        top_bar.setObjectName("TopBar")
        top_bar.setFixedHeight(52)
        tb = QHBoxLayout(top_bar)
        tb.setContentsMargins(14, 6, 14, 6)
        tb.setSpacing(10)

        brand = QLabel("GHOSTESP // COMMAND V4")
        brand.setStyleSheet(f"color: {PALETTE['accent_green']}; font-size: 15px; font-weight: 800; letter-spacing: 1px;")
        tb.addWidget(brand)

        hw_lbl = QLabel("7Semi ESP32-S3 N16R8")
        hw_lbl.setStyleSheet(f"color: {PALETTE['text_secondary']}; font-size: 11px;")
        tb.addWidget(hw_lbl)
        tb.addStretch(1)

        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(140)
        tb.addWidget(QLabel("Port:"))
        tb.addWidget(self.port_combo)

        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["115200", "460800", "921600"])
        tb.addWidget(QLabel("Baud:"))
        tb.addWidget(self.baud_combo)

        btn_refresh = QPushButton("Refresh")
        btn_refresh.clicked.connect(self._refresh_ports)
        tb.addWidget(btn_refresh)

        self.btn_connect = QPushButton("CONNECT")
        self.btn_connect.setObjectName("PrimaryBtn")
        self.btn_connect.clicked.connect(self._toggle_connect)
        tb.addWidget(self.btn_connect)

        self.conn_badge = QLabel("OFFLINE")
        self.conn_badge.setObjectName("BadgeOffline")
        tb.addWidget(self.conn_badge)
        root_layout.addWidget(top_bar)

        # Workspace splitter: Left sidebar + Stacked Views
        body = QHBoxLayout()
        body.setContentsMargins(0, 0, 0, 0)
        body.setSpacing(0)

        sidebar = QFrame()
        sidebar.setObjectName("Sidebar")
        sidebar.setFixedWidth(220)
        sb_scroll = QScrollArea()
        sb_scroll.setWidgetResizable(True)
        sb_scroll.setFrameShape(QFrame.Shape.NoFrame)
        sb_inner = QWidget()
        sb_inner.setStyleSheet(f"background-color: {PALETTE['bg_sidebar']};")
        self.sb_layout = QVBoxLayout(sb_inner)
        self.sb_layout.setContentsMargins(0, 8, 0, 8)
        self.sb_layout.setSpacing(2)
        sb_scroll.setWidget(sb_inner)
        sb_wrap = QVBoxLayout(sidebar)
        sb_wrap.setContentsMargins(0, 0, 0, 0)
        sb_wrap.addWidget(sb_scroll)
        body.addWidget(sidebar)

        self.stack = QStackedWidget()
        body.addWidget(self.stack, 1)
        root_layout.addLayout(body, 1)

        # Build all 17 views cleanly
        views = [
            ("Dashboard", self._view_dashboard()),
            ("Device & Serial", self._view_serial()),
            ("Hardware Pinout", self._view_hardware()),
            ("Temperature Monitor", self._view_temperature()),
            ("Wi-Fi Discovery", self._view_wifi()),
            ("Rogue & Deauth", self._view_security()),
            ("BLE Lab", self._view_ble()),
            ("NRF24 RF Spectrum", self._view_nrf24()),
            ("SDR Interface", self._view_sdr()),
            ("IR / NFC / Sub-GHz", self._view_rf_misc()),
            ("BadUSB / USB HID", self._view_usb()),
            ("SD & PCAP Storage", self._view_storage()),
            ("Profiles & Config", self._view_profiles()),
            ("Memory & RTOS", self._view_system_health()),
            ("Timeline & Alerts", self._view_timeline()),
            ("Command Console", self._view_console()),
            ("Help & V4 Audit", self._view_help()),
        ]
        for idx, (name, widget) in enumerate(views):
            btn = QPushButton(f"  {idx + 1:02d}. {name}")
            btn.setObjectName("NavBtn")
            btn.clicked.connect(lambda _=False, i=idx: self._switch_page(i))
            self.sb_layout.addWidget(btn)
            self.nav_buttons.append(btn)
            self.stack.addWidget(widget)
        self.sb_layout.addStretch(1)
        self._switch_page(0)

        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("Ready | Select serial port (COM5 USB-JTAG or COM6 UART) and click CONNECT.")

    def _switch_page(self, index: int) -> None:
        self.stack.setCurrentIndex(index)
        for i, btn in enumerate(self.nav_buttons):
            btn.setProperty("active", "true" if i == index else "false")
            btn.style().unpolish(btn)
            btn.style().polish(btn)

    # ---------------- 17 Structured Engineering Views ----------------

    def _view_dashboard(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        lay.setSpacing(12)

        grid = QGridLayout()
        grid.setSpacing(10)
        c1, self.kpi_temp = make_kpi("Silicon Die Temp", "--.- C", PALETTE["accent_amber"])
        c2, self.kpi_sram = make_kpi("Internal SRAM Free", "-- KB", PALETTE["accent_green"])
        c3, self.kpi_psram = make_kpi("Octal PSRAM Free", "-- MB", PALETTE["accent_cyan"])
        c4, self.kpi_aps = make_kpi("Discovered APs", "0", PALETTE["text_primary"])
        c5, self.kpi_ble = make_kpi("BLE Advertisers", "0", PALETTE["accent_purple"])
        c6, self.kpi_threats = make_kpi("Rogue / Deauth Alerts", "0 / 0 fps", PALETTE["accent_red"])
        for col, card in enumerate((c1, c2, c3, c4, c5, c6)):
            grid.addWidget(card, 0, col)
        lay.addLayout(grid)

        act_card, act_lay = make_card("Quick Hardware & Security Workflows")
        row = QHBoxLayout()
        for label, cmd, obj in [
            ("Read Die Temp (JSON)", "temp json", "PrimaryBtn"),
            ("Run Self-Test", "selftest", ""),
            ("Start Wi-Fi Scan", "scanap", ""),
            ("Stop Scan", "stopscan", ""),
            ("Start BLE Detect", "bledetect", ""),
            ("Rogue Status", "roguedetect status", ""),
            ("Deauth Status", "deauthmon status", ""),
            ("Profiles List", "profile list", ""),
        ]:
            b = QPushButton(label)
            if obj:
                b.setObjectName(obj)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            row.addWidget(b)
        act_lay.addLayout(row)
        lay.addWidget(act_card)

        feed_card, feed_lay = make_card("Live Operational Telemetry Feed")
        self.dash_log = QPlainTextEdit()
        self.dash_log.setReadOnly(True)
        self.dash_log.setMaximumBlockCount(400)
        feed_lay.addWidget(self.dash_log)
        lay.addWidget(feed_card, 1)
        return w

    def _view_serial(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("Serial Link Diagnostics & Silicon Identity")
        btns = QHBoxLayout()
        for label, cmd in [
            ("chipinfo", "chipinfo"),
            ("version", "version"),
            ("uuid", "uuid"),
            ("macaddr", "macaddr"),
            ("uptime", "uptime"),
            ("Toggle DTR/RTS Low", "__dtr_low"),
        ]:
            b = QPushButton(label)
            if cmd == "__dtr_low":
                b.clicked.connect(lambda: self.serial.set_control_lines(False, False))
            else:
                b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            btns.addWidget(b)
        cl.addLayout(btns)
        self.serial_log = QPlainTextEdit()
        self.serial_log.setReadOnly(True)
        self.serial_log.setMaximumBlockCount(1000)
        cl.addWidget(self.serial_log)
        lay.addWidget(card)
        return w

    def _view_hardware(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)

        p_card, pl = make_card("7Semi ESP32-S3-WROOM-1-N16R8 Verified Pin Authority")
        tbl = QTableWidget(len(PINOUT_ROWS), 4)
        tbl.setHorizontalHeaderLabels(["GPIO Pins", "Subsystem", "Assignment", "Lock Status"])
        tbl.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        for r, row in enumerate(PINOUT_ROWS):
            for c, val in enumerate(row):
                tbl.setItem(r, c, QTableWidgetItem(val))
        pl.addWidget(tbl)
        lay.addWidget(p_card, 1)

        s_card, sl = make_card("Live Automated Self-Test (`selftest`)")
        b = QPushButton("Execute Live Hardware Self-Test")
        b.setObjectName("PrimaryBtn")
        b.clicked.connect(lambda: self._send_cmd("selftest"))
        sl.addWidget(b)
        self.selftest_table = QTableWidget(0, 4)
        self.selftest_table.setHorizontalHeaderLabels(["Subsystem", "Diagnostic Test", "Result", "Details"])
        self.selftest_table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        sl.addWidget(self.selftest_table)
        lay.addWidget(s_card, 1)
        return w

    def _view_temperature(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        lay.setSpacing(10)

        ctl_card, ctl_lay = make_card("ESP32 Chip Internal Temperature Monitor")
        row = QHBoxLayout()
        self.temp_now_lbl = QLabel("--.- C (--.- F)")
        self.temp_now_lbl.setStyleSheet(f"color: {PALETTE['accent_amber']}; font-size: 20px; font-weight: 800; font-family: Consolas;")
        row.addWidget(self.temp_now_lbl)
        row.addStretch(1)

        b_sample = QPushButton("Sample Now")
        b_sample.setObjectName("PrimaryBtn")
        b_sample.clicked.connect(lambda: self._send_cmd("temp json"))
        row.addWidget(b_sample)

        b_c = QPushButton("Unit: C")
        b_c.clicked.connect(lambda: self._send_cmd("temp c"))
        b_f = QPushButton("Unit: F")
        b_f.clicked.connect(lambda: self._send_cmd("temp f"))
        b_rst = QPushButton("Reset Stats")
        b_rst.clicked.connect(self._reset_temp)
        b_exp = QPushButton("Export CSV")
        b_exp.clicked.connect(self._export_temp_csv)
        row.addWidget(b_c)
        row.addWidget(b_f)
        row.addWidget(b_rst)
        row.addWidget(b_exp)

        self.auto_temp_chk = QCheckBox("Auto-Poll")
        self.auto_temp_chk.toggled.connect(self._toggle_temp_poll)
        self.temp_poll_spin = QSpinBox()
        self.temp_poll_spin.setRange(500, 60000)
        self.temp_poll_spin.setValue(2000)
        self.temp_poll_spin.setSuffix(" ms")
        row.addWidget(self.auto_temp_chk)
        row.addWidget(self.temp_poll_spin)
        ctl_lay.addLayout(row)

        self.temp_stats_lbl = QLabel("Min: -- | Max: -- | Avg: -- | HW Range: -10..80 C | Samples: 0")
        self.temp_stats_lbl.setStyleSheet(f"color: {PALETTE['text_secondary']}; font-family: Consolas;")
        ctl_lay.addWidget(self.temp_stats_lbl)
        lay.addWidget(ctl_card)

        chart_card, chart_lay = make_card("Silicon Die Thermal Trend (Real On-Chip Telemetry)")
        self.temp_chart = MplChartWidget("ESP32-S3 Die Temperature (C)")
        chart_lay.addWidget(self.temp_chart)
        lay.addWidget(chart_card, 1)
        return w

    def _view_wifi(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)

        card, cl = make_card("802.11 Access Point Discovery & Security Analyzer")
        bar = QHBoxLayout()
        for label, cmd, obj in [
            ("Start AP Scan", "scanap", "PrimaryBtn"),
            ("Stop Scan", "stopscan", "DangerBtn"),
            ("List Cached APs", "list -a", ""),
            ("WPA3 Check", "wpa3check", ""),
            ("Channel Congestion", "congestion", ""),
            ("Clear Table", "__clear", ""),
            ("Export CSV", "__csv", ""),
        ]:
            b = QPushButton(label)
            if obj:
                b.setObjectName(obj)
            if cmd == "__clear":
                b.clicked.connect(self._clear_wifi)
            elif cmd == "__csv":
                b.clicked.connect(self._export_wifi_csv)
            else:
                b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            bar.addWidget(b)
        cl.addLayout(bar)

        self.wifi_table = QTableWidget(0, 7)
        self.wifi_table.setHorizontalHeaderLabels(["BSSID", "SSID", "Ch", "RSSI", "Security", "Vendor", "Threat"])
        self.wifi_table.setSortingEnabled(True)
        self.wifi_table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        cl.addWidget(self.wifi_table)
        lay.addWidget(card)
        return w

    def _view_security(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)

        bar = QHBoxLayout()
        for label, cmd in [
            ("Rogue Start", "roguedetect start"),
            ("Rogue Status", "roguedetect status"),
            ("Rogue Stop", "roguedetect stop"),
            ("DeauthMon Start", "deauthmon start"),
            ("DeauthMon Status", "deauthmon status"),
            ("DeauthMon Stop", "deauthmon stop"),
        ]:
            b = QPushButton(label)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            bar.addWidget(b)
        lay.addLayout(bar)

        self.sec_summary = QLabel("Rogue Detector: IDLE | Deauth Monitor: IDLE (0 fps)")
        self.sec_summary.setStyleSheet(f"color: {PALETTE['accent_cyan']}; font-family: Consolas; font-weight: bold;")
        lay.addWidget(self.sec_summary)

        card, cl = make_card("Deauthentication / Disassociation Frame Log")
        self.deauth_table = QTableWidget(0, 5)
        self.deauth_table.setHorizontalHeaderLabels(["Time (s)", "Type", "Transmitter MAC", "Receiver MAC", "Reason"])
        self.deauth_table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        cl.addWidget(self.deauth_table)
        lay.addWidget(card, 1)
        return w

    def _view_ble(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)

        card, cl = make_card("BLE Lab & Advertiser Telemetry (Fixed NimBLE 6KB Stack)")
        bar = QHBoxLayout()
        for label, cmd, obj in [
            ("BLE Detect (bledetect)", "bledetect", "PrimaryBtn"),
            ("BLE Scan -t", "blescan -t", ""),
            ("Stop BLE Scan", "stopscan", "DangerBtn"),
            ("AirTags", "listairtags", ""),
            ("Flippers", "listflippers", ""),
            ("Clear", "__clear", ""),
            ("Export CSV", "__csv", ""),
        ]:
            b = QPushButton(label)
            if obj:
                b.setObjectName(obj)
            if cmd == "__clear":
                b.clicked.connect(self._clear_ble)
            elif cmd == "__csv":
                b.clicked.connect(self._export_ble_csv)
            else:
                b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            bar.addWidget(b)
        cl.addLayout(bar)

        self.ble_table = QTableWidget(0, 5)
        self.ble_table.setHorizontalHeaderLabels(["MAC", "Name", "RSSI (dBm)", "Type", "Raw Details"])
        self.ble_table.setSortingEnabled(True)
        self.ble_table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        cl.addWidget(self.ble_table)
        lay.addWidget(card)
        return w

    def _view_nrf24(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)

        card, cl = make_card("NRF24L01+PA+LNA 2.4 GHz RF Spectrum Analyzer (SPI2)")
        top = QHBoxLayout()
        self.nrf_badge = QLabel("HW DEPENDENT / UNPROBED")
        self.nrf_badge.setObjectName("BadgeSimulated")
        top.addWidget(self.nrf_badge)
        top.addStretch(1)
        for label, cmd in [
            ("Inspect Registers", "nrf24lab diag"),
            ("126-Ch Sweep", "nrf24lab sweep 10"),
            ("SPI2 Echo Diagnostic", "nrf24lab benchmark 100"),
        ]:
            b = QPushButton(label)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            top.addWidget(b)
        cl.addLayout(top)

        self.nrf_chart = MplChartWidget("2.4 GHz Carrier Activity (2400 - 2525 MHz)")
        cl.addWidget(self.nrf_chart, 2)
        self.nrf_log = QPlainTextEdit()
        self.nrf_log.setReadOnly(True)
        self.nrf_log.setMaximumBlockCount(400)
        cl.addWidget(self.nrf_log, 1)
        lay.addWidget(card)
        return w

    def _view_sdr(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("External Host SDR Interface")
        badge = QLabel("EXTERNAL HARDWARE REQUIRED / NOT AN SDR MCU")
        badge.setObjectName("BadgeSimulated")
        cl.addWidget(badge, alignment=Qt.AlignmentFlag.AlignLeft)

        info = self.sdr.get_info()
        txt = QPlainTextEdit()
        txt.setReadOnly(True)
        txt.setPlainText(
            f"Driver        : {info['driver']}\n"
            f"Hardware      : {info['hardware']}\n"
            f"Status        : {info['status']}\n"
            f"Tuning Range  : {info['tuning_range']}\n\n"
            "Architecture Notice:\n"
            "  The 7Semi ESP32-S3 + NRF24L01+ front-end operates on 802.11 / BLE / ISM 2.4 GHz packets.\n"
            "  True wideband I/Q demodulation requires an external USB SDR (RTL-SDR, HackRF One, BladeRF)\n"
            "  attached to the host PC."
        )
        cl.addWidget(txt)
        lay.addWidget(card)
        return w

    def _view_rf_misc(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("Infrared / Sub-GHz / NFC Peripheral Controls")
        badge = QLabel("EXTERNAL MODULES (HW DEPENDENT)")
        badge.setObjectName("BadgeSimulated")
        cl.addWidget(badge, alignment=Qt.AlignmentFlag.AlignLeft)
        row = QHBoxLayout()
        for label, cmd in [("Check IR Pin Config", "irpin"), ("Run NFC Self-Test", "nfctest"), ("GPS Info", "gpsinfo -s")]:
            b = QPushButton(label)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            row.addWidget(b)
        cl.addLayout(row)
        self.rf_misc_log = QPlainTextEdit()
        self.rf_misc_log.setReadOnly(True)
        cl.addWidget(self.rf_misc_log)
        lay.addWidget(card)
        return w

    def _view_usb(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("BadUSB & USB Host Keyboard Laboratory (GPIO19/20 Shared PHY)")
        warn = QLabel("WARNING: Activating BadUSB or USB Host Keyboard detaches Native USB-Serial-JTAG (COM5). Monitor over CP210x UART (COM6).")
        warn.setStyleSheet(f"color: {PALETTE['accent_amber']}; font-weight: bold;")
        cl.addWidget(warn)

        self.badusb_confirm_chk = QCheckBox("Explicitly confirm active HID USB-OTG execution (--confirm)")
        cl.addWidget(self.badusb_confirm_chk)

        row = QHBoxLayout()
        for label, cmd in [
            ("BadUSB List", "badusb list"),
            ("USB Kbd Status", "usbkbd status"),
            ("Run Builtin BadUSB", "__badusb_builtin"),
            ("Stop BadUSB", "badusb stop"),
        ]:
            b = QPushButton(label)
            if cmd == "__badusb_builtin":
                b.clicked.connect(self._run_badusb_builtin)
            else:
                b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            row.addWidget(b)
        cl.addLayout(row)
        self.usb_log = QPlainTextEdit()
        self.usb_log.setReadOnly(True)
        cl.addWidget(self.usb_log)
        lay.addWidget(card)
        return w

    def _view_storage(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("SD Card (FATFS) & PCAP Capture Browser")
        row = QHBoxLayout()
        for label, cmd in [
            ("SD Config", "sd_config"),
            ("List SD Root", "sd list"),
            ("Capture List", "capture -list"),
            ("Stop Capture", "capture -stop"),
        ]:
            b = QPushButton(label)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            row.addWidget(b)
        cl.addLayout(row)
        self.storage_log = QPlainTextEdit()
        self.storage_log.setReadOnly(True)
        cl.addWidget(self.storage_log)
        lay.addWidget(card)
        return w

    def _view_profiles(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("Research Profiles & System Settings")
        row = QHBoxLayout()
        b_list = QPushButton("Refresh Profiles")
        b_list.setObjectName("PrimaryBtn")
        b_list.clicked.connect(lambda: self._send_cmd("profile list"))
        row.addWidget(b_list)
        for pid, pname in [(0, "Combined"), (1, "WiFi-Research"), (2, "BLE-Research"), (3, "NRF24-Lab"), (6, "Diagnostics")]:
            bp = QPushButton(f"Apply {pid}: {pname}")
            bp.clicked.connect(lambda _=False, i=pid: self._send_cmd(f"profile apply {i}"))
            row.addWidget(bp)
        cl.addLayout(row)

        self.profile_table = QTableWidget(0, 4)
        self.profile_table.setHorizontalHeaderLabels(["ID", "Profile Name", "Active", "Description"])
        self.profile_table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        cl.addWidget(self.profile_table)
        lay.addWidget(card)
        return w

    def _view_system_health(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("FreeRTOS Task Telemetry, Memory & Coredump Audit")
        row = QHBoxLayout()
        for label, cmd in [("Memory (mem)", "mem"), ("Tasks (ps)", "ps"), ("Filesystems (df)", "df"), ("Coredump", "coredump")]:
            b = QPushButton(label)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            row.addWidget(b)
        cl.addLayout(row)
        self.health_log = QPlainTextEdit()
        self.health_log.setReadOnly(True)
        cl.addWidget(self.health_log)
        lay.addWidget(card)
        return w

    def _view_timeline(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("Real-Time Security & System Timeline")
        row = QHBoxLayout()
        b_clear = QPushButton("Clear View")
        b_clear.clicked.connect(lambda: self.timeline_table.setRowCount(0))
        b_exp_j = QPushButton("Export Timeline JSON")
        b_exp_j.clicked.connect(self._export_timeline_json)
        b_exp_b = QPushButton("Export Full Session Bundle")
        b_exp_b.setObjectName("PrimaryBtn")
        b_exp_b.clicked.connect(self._export_bundle_json)
        row.addWidget(b_clear)
        row.addStretch(1)
        row.addWidget(b_exp_j)
        row.addWidget(b_exp_b)
        cl.addLayout(row)

        self.timeline_table = QTableWidget(0, 4)
        self.timeline_table.setHorizontalHeaderLabels(["Timestamp", "Subsystem", "Severity", "Details"])
        self.timeline_table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        cl.addWidget(self.timeline_table)
        lay.addWidget(card)
        return w

    def _view_console(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("ANSI-Cleaned Serial Console & Command Autocomplete")
        self.console_out = QPlainTextEdit()
        self.console_out.setReadOnly(True)
        self.console_out.setMaximumBlockCount(5000)
        cl.addWidget(self.console_out, 1)

        in_row = QHBoxLayout()
        in_row.addWidget(QLabel("ghost>"))
        self.console_in = QLineEdit()
        self.console_in.setPlaceholderText("Enter CLI command (Tab/type for autocomplete, e.g., temp json, scanap, bledetect)...")
        completer = QCompleter()
        completer.setModel(QStringListModel(COMMON_COMMANDS))
        completer.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        self.console_in.setCompleter(completer)
        self.console_in.returnPressed.connect(self._send_console_input)
        in_row.addWidget(self.console_in, 1)
        btn = QPushButton("SEND")
        btn.setObjectName("PrimaryBtn")
        btn.clicked.connect(self._send_console_input)
        in_row.addWidget(btn)
        cl.addLayout(in_row)
        lay.addWidget(card)
        return w

    def _view_help(self) -> QWidget:
        w = QWidget()
        lay = QVBoxLayout(w)
        lay.setContentsMargins(14, 14, 14, 14)
        card, cl = make_card("GhostESP Firmware Reference & God Mode V4 Notes")
        row = QHBoxLayout()
        for label, cmd in [("help", "help"), ("help godmode", "help godmode"), ("help wifi", "help wifi"), ("help ble", "help ble"), ("ghost version", "ghost version")]:
            b = QPushButton(label)
            b.clicked.connect(lambda _=False, c=cmd: self._send_cmd(c))
            row.addWidget(b)
        cl.addLayout(row)
        self.help_log = QPlainTextEdit()
        self.help_log.setReadOnly(True)
        self.help_log.setPlainText(
            "GhostESP Command Center V4 (PySide6 Edition)\n"
            "- Thread-safe serial transport with queued writes & WAL SQLite persistence\n"
            "- Integrated ESP32-S3 Internal Die Temperature Monitor ([TEMP_JSON])\n"
            "- Honest hardware gating for unplugged peripherals (NRF24, SD Card, Host SDR)\n"
            "- Explicit --confirm authorization guard on BadUSB execution\n"
        )
        cl.addWidget(self.help_log)
        lay.addWidget(card)
        return w

    # ---------------- Actions & Serial Handlers ----------------

    def _refresh_ports(self) -> None:
        self.port_combo.clear()
        ports = GhostSerialTransceiver.list_available_ports()
        if not ports:
            self.port_combo.addItem("No Ports Detected")
            return
        for p in ports:
            self.port_combo.addItem(f"{p['device']} — {p['description']}", userData=p["device"])

    def _toggle_connect(self) -> None:
        if self.serial.running:
            self.temp_poll_timer.stop()
            self.auto_temp_chk.setChecked(False)
            self.serial.disconnect()
            return
        port = self.port_combo.currentData() or self.port_combo.currentText()
        if not port or "No Ports" in str(port):
            QMessageBox.warning(self, "Serial Port", "No valid serial port selected.")
            return
        baud = int(self.baud_combo.currentText())
        ok, msg = self.serial.connect(str(port), baudrate=baud)
        if not ok:
            QMessageBox.critical(self, "Connection Failed", msg)

    def _on_status_changed_ui(self, connected: bool, message: str) -> None:
        self.status_bar.showMessage(message)
        if connected:
            self.btn_connect.setText("DISCONNECT")
            self.btn_connect.setObjectName("DangerBtn")
            self.conn_badge.setText("LIVE")
            self.conn_badge.setObjectName("BadgeLive")
            self.session_id = self.db.start_session(str(self.serial.current_port_name or ""))
            self.db.log_event("SYSTEM", "CONNECT", "INFO", message)
            self._append_timeline("SYSTEM", "INFO", message)
            QTimer.singleShot(250, lambda: self._send_cmd("temp json"))
        else:
            self.btn_connect.setText("CONNECT")
            self.btn_connect.setObjectName("PrimaryBtn")
            self.conn_badge.setText("OFFLINE")
            self.conn_badge.setObjectName("BadgeOffline")
            self.db.end_session(self.session_id)
            self.session_id = None
            self.db.log_event("SYSTEM", "DISCONNECT", "INFO", message)
            self._append_timeline("SYSTEM", "INFO", message)

        for widget in (self.btn_connect, self.conn_badge):
            widget.style().unpolish(widget)
            widget.style().polish(widget)

    def _send_cmd(self, cmd: str) -> None:
        if not self.serial.running:
            QMessageBox.information(self, "Offline", f"Connect to ESP32-S3 before sending `{cmd}`.")
            return
        self.serial.send_command(cmd)
        self.console_out.appendPlainText(f"> {cmd}")
        self.cmd_history.append(cmd)

    def _send_console_input(self) -> None:
        cmd = self.console_in.text().strip()
        if cmd:
            self._send_cmd(cmd)
            self.console_in.clear()

    def _poll_temperature(self) -> None:
        if self.serial.running:
            self.serial.send_command("temp json")

    def _toggle_temp_poll(self, enabled: bool) -> None:
        if enabled:
            self.temp_poll_timer.start(self.temp_poll_spin.value())
        else:
            self.temp_poll_timer.stop()

    def _reset_temp(self) -> None:
        self.temp_points.clear()
        self._redraw_temp_chart()
        if self.serial.running:
            self._send_cmd("temp reset")

    def _run_badusb_builtin(self) -> None:
        if not self.badusb_confirm_chk.isChecked():
            QMessageBox.warning(self, "Confirmation Required", "Check the explicit BadUSB confirmation box first.")
            return
        self._send_cmd("badusb run builtin --confirm")

    def _on_serial_line_ui(self, raw_line: str) -> None:
        from parsers import strip_ansi
        clean = strip_ansi(raw_line)
        self.console_out.appendPlainText(clean)
        if clean:
            self.dash_log.appendPlainText(clean)
            if any(k in clean for k in ("IDF", "Chip", "ESP32", "MAC", "Uptime")):
                self.serial_log.appendPlainText(clean)
            if any(k in clean for k in ("Free", "Heap", "Task", "SPIFFS", "Core")):
                self.health_log.appendPlainText(clean)
            if any(k in clean for k in ("SD", "PCAP", "/mnt")):
                self.storage_log.appendPlainText(clean)
            if any(k in clean for k in ("IR", "NFC", "GPS")):
                self.rf_misc_log.appendPlainText(clean)
            if any(k in clean for k in ("BadUSB", "USB", "usbkbd")):
                self.usb_log.appendPlainText(clean)
            if any(k in clean for k in ("Usage:", "Commands:", "Description:")):
                self.help_log.appendPlainText(clean)

        for ev in self.parser.feed_line(raw_line):
            self._dispatch_event(ev.kind, ev.data)

        self.status_bar.showMessage(
            f"{self.serial.current_port_name or 'Disconnected'} | RX: {self.serial.rx_bytes} B | TX: {self.serial.tx_bytes} B"
        )

    def _dispatch_event(self, kind: str, d: dict) -> None:
        if kind == "temp_json":
            c = float(d.get("temp_c", 0.0))
            f = float(d.get("temp_f", 0.0))
            sram = int(d.get("sram_free_kb", 0))
            psram = int(d.get("psram_free_kb", 0))
            self.kpi_temp.setText(f"{c:.1f} C")
            self.kpi_sram.setText(f"{sram} KB")
            self.kpi_psram.setText(f"{psram / 1024.0:.1f} MB")
            self.temp_now_lbl.setText(f"{c:.1f} C ({f:.1f} F)")
            self.temp_stats_lbl.setText(
                f"Min: {d.get('min_c', 0):.1f} C | Max: {d.get('max_c', 0):.1f} C | Avg: {d.get('avg_c', 0):.1f} C | "
                f"HW Range: {d.get('range_min', -10)}..{d.get('range_max', 80)} C | Samples: {d.get('samples', 0)}"
            )
            if d.get("valid"):
                self.temp_points.append(c)
                self._redraw_temp_chart()
                self.db.log_temp_sample(
                    c, f, float(d.get("min_c", 0)), float(d.get("max_c", 0)),
                    float(d.get("avg_c", 0)), sram, psram,
                )
        elif kind == "temp_text":
            self.kpi_temp.setText(f"{d['temp_c']:.1f} C")
            self.temp_now_lbl.setText(f"{d['temp_c']:.1f} C ({d['temp_f']:.1f} F) [{d['band']}]")
            self.temp_points.append(float(d["temp_c"]))
            self._redraw_temp_chart()
        elif kind == "wifi_ap":
            self._upsert_wifi(d)
        elif kind == "ble_device":
            self._upsert_ble(d)
        elif kind == "nrf24_line":
            self.nrf_log.appendPlainText(d["text"])
            if "UNRESPONSIVE" in d["text"]:
                self.nrf_badge.setText("DISCONNECTED / UNPOWERED")
                self.nrf_badge.setObjectName("BadgeOffline")
            elif "RESPONSIVE" in d["text"]:
                self.nrf_badge.setText("NRF24L01+ CONNECTED")
                self.nrf_badge.setObjectName("BadgeLive")
            self.nrf_badge.style().unpolish(self.nrf_badge)
            self.nrf_badge.style().polish(self.nrf_badge)
        elif kind == "nrf24_sweep_bin":
            self.sweep_bins[d["channel"]] = d["activity_pct"]
            self.db.log_nrf24_bin(d["channel"], d["freq_mhz"], d["activity_pct"])
            self._redraw_nrf_chart()
        elif kind == "profile_row":
            self._upsert_profile(d)
        elif kind == "selftest_row":
            self._upsert_selftest(d)
        elif kind == "rogue_summary":
            self.sec_summary.setText(f"Rogue Detector: {d['status']} | APs: {d['tracked_aps']} | Threats: {d['threats']}")
        elif kind == "deauth_summary":
            self.kpi_threats.setText(f"{d['total_deauths']} / {d['rate_fps']} fps")
            self.sec_summary.setText(
                f"DeauthMon: {d['status']} | Deauths: {d['total_deauths']} | Disassocs: {d['disassocs']} | Rate: {d['rate_fps']} fps"
            )
        elif kind == "deauth_event":
            r = self.deauth_table.rowCount()
            self.deauth_table.insertRow(r)
            for idx, key in enumerate(("time_s", "frame_type", "transmitter", "receiver", "reason")):
                self.deauth_table.setItem(r, idx, QTableWidgetItem(str(d[key])))
            self.db.log_deauth_event(d["frame_type"], d["transmitter"], d["receiver"], d["reason"])
        elif kind == "timeline":
            self.db.log_event(d["subsystem"], "EVENT", d["severity"], d["details"])
            self._append_timeline(d["subsystem"], d["severity"], d["details"])

    def _upsert_wifi(self, d: dict) -> None:
        bssid = d["bssid"]
        vals = [bssid, d["ssid"], str(d["channel"]), str(d["rssi"]), d["security"], d["vendor"], d["threat"]]
        if bssid in self.known_aps:
            row = self.known_aps[bssid]
        else:
            row = self.wifi_table.rowCount()
            self.wifi_table.insertRow(row)
            self.known_aps[bssid] = row
        for c, v in enumerate(vals):
            self.wifi_table.setItem(row, c, QTableWidgetItem(v))
        self.kpi_aps.setText(str(len(self.known_aps)))
        self.db.log_ap(
            bssid, d["ssid"], int(d["channel"]), int(d["rssi"]), d["security"],
            0 if d["threat"] in ("CLEAN", "NONE") else 1, d["threat"], d["vendor"],
        )

    def _upsert_ble(self, d: dict) -> None:
        mac = d["mac"]
        vals = [mac, d["name"], str(d["rssi"]), d["dev_type"], d["details"]]
        if mac in self.known_bles:
            row = self.known_bles[mac]
        else:
            row = self.ble_table.rowCount()
            self.ble_table.insertRow(row)
            self.known_bles[mac] = row
        for c, v in enumerate(vals):
            self.ble_table.setItem(row, c, QTableWidgetItem(v))
        self.kpi_ble.setText(str(len(self.known_bles)))
        self.db.log_ble_device(mac, d["name"], int(d["rssi"]), d["dev_type"], d["details"])

    def _upsert_profile(self, d: dict) -> None:
        pid = d["id"]
        while self.profile_table.rowCount() <= pid:
            self.profile_table.insertRow(self.profile_table.rowCount())
        for c, val in enumerate([str(pid), d["name"], "ACTIVE *" if d["active"] else "no", d["description"]]):
            self.profile_table.setItem(pid, c, QTableWidgetItem(val))

    def _upsert_selftest(self, d: dict) -> None:
        r = self.selftest_table.rowCount()
        self.selftest_table.insertRow(r)
        for c, key in enumerate(("subsystem", "test", "result", "details")):
            item = QTableWidgetItem(str(d[key]))
            if d["result"] == "PASS":
                item.setForeground(QColor(PALETTE["accent_green"]))
            elif d["result"] == "FAIL":
                item.setForeground(QColor(PALETTE["accent_red"]))
            elif d["result"] in ("WARNING", "HW REQUIRED"):
                item.setForeground(QColor(PALETTE["accent_amber"]))
            self.selftest_table.setItem(r, c, item)

    def _append_timeline(self, subsystem: str, severity: str, details: str) -> None:
        self.timeline_table.insertRow(0)
        ts = time.strftime("%H:%M:%S")
        for c, v in enumerate((ts, subsystem, severity, details)):
            self.timeline_table.setItem(0, c, QTableWidgetItem(v))

    def _redraw_temp_chart(self) -> None:
        self.temp_chart.ax.clear()
        self.temp_chart._style_axes()
        if self.temp_points:
            xs = list(range(len(self.temp_points)))
            ys = list(self.temp_points)
            self.temp_chart.ax.plot(xs, ys, color=PALETTE["accent_amber"], linewidth=2.0, marker="o", markersize=3)
            self.temp_chart.ax.set_ylabel("C", color=PALETTE["text_secondary"], fontsize=8)
        self.temp_chart.canvas.draw_idle()

    def _redraw_nrf_chart(self) -> None:
        self.nrf_chart.ax.clear()
        self.nrf_chart._style_axes()
        if self.sweep_bins:
            chs = sorted(self.sweep_bins.keys())
            pcts = [self.sweep_bins[c] for c in chs]
            self.nrf_chart.ax.bar(chs, pcts, color=PALETTE["accent_green"], width=0.8)
            self.nrf_chart.ax.set_ylim(0, 100)
            self.nrf_chart.ax.set_xlim(0, 125)
        self.nrf_chart.canvas.draw_idle()

    def _load_persisted_snapshot(self) -> None:
        for ev in self.db.get_events(limit=50):
            self.timeline_table.insertRow(self.timeline_table.rowCount())
            r = self.timeline_table.rowCount() - 1
            for c, k in enumerate(("timestamp", "subsystem", "severity", "details")):
                self.timeline_table.setItem(r, c, QTableWidgetItem(str(ev[k] or "")))

    def _clear_wifi(self) -> None:
        self.wifi_table.setRowCount(0)
        self.known_aps.clear()
        self.kpi_aps.setText("0")

    def _clear_ble(self) -> None:
        self.ble_table.setRowCount(0)
        self.known_bles.clear()
        self.kpi_ble.setText("0")

    def _export_wifi_csv(self) -> None:
        path, _ = QFileDialog.getSaveFileName(self, "Export Wi-Fi APs", "wifi_aps.csv", "CSV Files (*.csv)")
        if path:
            self._export_table_csv(self.wifi_table, path)

    def _export_ble_csv(self) -> None:
        path, _ = QFileDialog.getSaveFileName(self, "Export BLE Devices", "ble_devices.csv", "CSV Files (*.csv)")
        if path:
            self._export_table_csv(self.ble_table, path)

    def _export_temp_csv(self) -> None:
        path, _ = QFileDialog.getSaveFileName(self, "Export Temperature Samples", "temp_samples.csv", "CSV Files (*.csv)")
        if path:
            rows = self.db.get_temp_samples(limit=5000)
            with open(path, "w", newline="", encoding="utf-8") as fh:
                writer = csv.writer(fh)
                writer.writerow(["timestamp", "temp_c", "temp_f", "min_c", "max_c", "avg_c", "sram_free_kb", "psram_free_kb"])
                for r in rows:
                    writer.writerow([r[k] for k in ("timestamp", "temp_c", "temp_f", "min_c", "max_c", "avg_c", "sram_free_kb", "psram_free_kb")])

    @staticmethod
    def _export_table_csv(tbl: QTableWidget, path: str) -> None:
        headers = [tbl.horizontalHeaderItem(c).text() for c in range(tbl.columnCount())]
        with open(path, "w", newline="", encoding="utf-8") as fh:
            writer = csv.writer(fh)
            writer.writerow(headers)
            for r in range(tbl.rowCount()):
                writer.writerow([tbl.item(r, c).text() if tbl.item(r, c) else "" for c in range(tbl.columnCount())])

    def _export_timeline_json(self) -> None:
        path, _ = QFileDialog.getSaveFileName(self, "Export Timeline", "timeline.json", "JSON Files (*.json)")
        if path:
            self.db.export_events_json(path)

    def _export_bundle_json(self) -> None:
        path, _ = QFileDialog.getSaveFileName(self, "Export Session Bundle", "ghostesp_bundle.json", "JSON Files (*.json)")
        if path:
            self.db.export_session_bundle(path)

    def closeEvent(self, event) -> None:
        self.temp_poll_timer.stop()
        self.serial.disconnect()
        self.db.close()
        super().closeEvent(event)


def main() -> int:
    app = QApplication(sys.argv)
    win = GhostCommandCenterWindow()
    win.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())

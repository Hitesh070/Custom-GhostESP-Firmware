#!/usr/bin/env python3
"""
GhostESP God Mode V3 - Laptop Command Center
Author: Autonomous Desktop Application Engineering Organization
Architecture: Multi-Tab Cyberpunk Operations Suite with Real-Time Serial Stream Telemetry
"""

import sys
import os
import time
import re
import csv
import tkinter as tk
from tkinter import ttk, messagebox, filedialog

# Add current folder to sys.path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from serial_comm import GhostSerialTransceiver
from database import GhostDatabase
from modules.sdr_abstraction import SDRManager

THEME = {
    "bg_dark": "#0a0d12",
    "bg_card": "#131821",
    "bg_input": "#1a2230",
    "accent_green": "#00ff66",
    "accent_cyan": "#00e5ff",
    "accent_red": "#ff3366",
    "accent_yellow": "#ffcc00",
    "text_main": "#e0e6ed",
    "text_dim": "#7e8b9b",
    "border": "#243042"
}

class GhostCommandCenter(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("GHOSTESP GOD MODE V3 // LAPTOP COMMAND CENTER")
        self.geometry("1120x740")
        self.minsize(980, 620)
        self.configure(bg=THEME["bg_dark"])

        self.db = GhostDatabase()
        self.sdr = SDRManager()
        self.serial = GhostSerialTransceiver(
            on_line_received=self._on_serial_line,
            on_status_changed=self._on_connection_status
        )

        # Internal state tracking for streams
        self.current_ap = {}
        self.known_aps = {}     # bssid -> item_id
        self.known_bles = {}    # mac -> item_id

        self._setup_styles()
        self._build_header()
        self._build_tabs()
        self._build_statusbar()

        self._refresh_com_ports()

    def _setup_styles(self):
        style = ttk.Style(self)
        style.theme_use("clam")
        style.configure("TNotebook", background=THEME["bg_dark"], borderwidth=0)
        style.configure("TNotebook.Tab", background=THEME["bg_card"], foreground=THEME["text_dim"],
                        padding=[14, 8], font=("Consolas", 10, "bold"))
        style.map("TNotebook.Tab",
                  background=[("selected", THEME["bg_input"])],
                  foreground=[("selected", THEME["accent_green"])])

        # Style Treeviews
        style.configure("Treeview",
                        background=THEME["bg_card"],
                        foreground=THEME["text_main"],
                        fieldbackground=THEME["bg_card"],
                        font=("Consolas", 9),
                        rowheight=24)
        style.configure("Treeview.Heading",
                        background=THEME["bg_input"],
                        foreground=THEME["accent_cyan"],
                        font=("Consolas", 9, "bold"))
        style.map("Treeview",
                  background=[("selected", THEME["bg_input"])],
                  foreground=[("selected", THEME["accent_green"])])

    def _build_header(self):
        header = tk.Frame(self, bg=THEME["bg_card"], height=60, bd=1, relief="flat", highlightbackground=THEME["border"], highlightthickness=1)
        header.pack(fill="x", padx=10, pady=8)

        # Title Brand
        title_box = tk.Frame(header, bg=THEME["bg_card"])
        title_box.pack(side="left", padx=15, pady=8)
        lbl_title = tk.Label(title_box, text="⚡ GHOSTESP COMMAND CENTER", font=("Consolas", 14, "bold"), fg=THEME["accent_green"], bg=THEME["bg_card"])
        lbl_title.pack(anchor="w")
        lbl_sub = tk.Label(title_box, text="God Mode V3 // Target: 7Semi ESP32-S3-WROOM-1-N16R8", font=("Consolas", 8), fg=THEME["text_dim"], bg=THEME["bg_card"])
        lbl_sub.pack(anchor="w")

        # Serial Connection Box
        conn_box = tk.Frame(header, bg=THEME["bg_card"])
        conn_box.pack(side="right", padx=15, pady=8)

        tk.Label(conn_box, text="PORT:", font=("Consolas", 9, "bold"), fg=THEME["text_dim"], bg=THEME["bg_card"]).pack(side="left", padx=4)
        self.port_combo = ttk.Combobox(conn_box, width=15, state="readonly")
        self.port_combo.pack(side="left", padx=4)

        btn_refresh = tk.Button(conn_box, text="↻", command=self._refresh_com_ports, bg=THEME["bg_input"], fg=THEME["accent_cyan"], relief="flat", padx=6)
        btn_refresh.pack(side="left", padx=2)

        self.btn_connect = tk.Button(conn_box, text="CONNECT", command=self._toggle_connect, bg=THEME["accent_green"], fg="#000000", font=("Consolas", 9, "bold"), relief="flat", padx=10)
        self.btn_connect.pack(side="left", padx=6)

    def _build_tabs(self):
        self.notebook = ttk.Notebook(self)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=2)

        self.tab_dashboard = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_wifi = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_ble = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_nrf24 = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_timeline = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_sdr = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_terminal = tk.Frame(self.notebook, bg=THEME["bg_dark"])

        self.notebook.add(self.tab_dashboard, text="[ DASHBOARD ]")
        self.notebook.add(self.tab_wifi, text="[ WI-FI LAB ]")
        self.notebook.add(self.tab_ble, text="[ BLE LAB ]")
        self.notebook.add(self.tab_nrf24, text="[ NRF24 RF ]")
        self.notebook.add(self.tab_timeline, text="[ TIMELINE ]")
        self.notebook.add(self.tab_sdr, text="[ SDR INTERFACE ]")
        self.notebook.add(self.tab_terminal, text="[ CONSOLE ]")

        self._init_dashboard_tab()
        self._init_wifi_tab()
        self._init_ble_tab()
        self._init_nrf24_tab()
        self._init_timeline_tab()
        self._init_sdr_tab()
        self._init_terminal_tab()

    def _init_dashboard_tab(self):
        grid_frame = tk.Frame(self.tab_dashboard, bg=THEME["bg_dark"])
        grid_frame.pack(fill="both", expand=True, padx=10, pady=10)

        # Left Info Panel
        info_card = tk.LabelFrame(grid_frame, text=" HARDWARE & SUBSYSTEM STATUS ", font=("Consolas", 10, "bold"), fg=THEME["accent_cyan"], bg=THEME["bg_card"], bd=1, relief="flat", highlightbackground=THEME["border"], highlightthickness=1)
        info_card.pack(side="left", fill="both", expand=True, padx=5, pady=5)

        self.dash_status_text = tk.Text(info_card, bg=THEME["bg_card"], fg=THEME["text_main"], font=("Consolas", 10), bd=0, padx=10, pady=10)
        self.dash_status_text.pack(fill="both", expand=True)
        self.dash_status_text.insert("end",
            "Target: 7Semi ESP32-S3-WROOM-1-N16R8\n"
            "CPU: Xtensa LX7 Dual-Core @ 240 MHz\n"
            "Flash: 16 MB SPI Flash (DIO @ 80 MHz)\n"
            "PSRAM: 8 MB Octal PSRAM (AP_3v3 @ 40 MHz DDR)\n"
            "----------------------------------------------------\n"
            "Bus Architecture:\n"
            "  • Primary I2C:   GPIO8 (SDA), GPIO9 (SCL)\n"
            "  • Dedicated RF:  SPI2_HOST (CS10, MOSI11, SCK12, MISO13, CE14)\n"
            "  • Dedicated LCD: SPI3_HOST (CS5, DC6, RST7, MOSI15, SCK16)\n"
            "  • Serial Stream: UART1 (TX43, RX44)\n"
            "  • Native USB:    GPIO19 (D-), GPIO20 (D+) [LOCKED]\n"
            "----------------------------------------------------\n"
            "Active Research Subsystems:\n"
            "  [✔] Rogue AP & Evil Twin Detection Engine\n"
            "  [✔] Passive Deauth Storm Activity Monitor\n"
            "  [✔] NRF24L01+PA+LNA 2.4GHz Register Inspector\n"
            "  [✔] Matrix & Cyberpunk Radar Systems\n"
        )
        self.dash_status_text.configure(state="disabled")

        # Quick Action Controls
        ctrl_card = tk.LabelFrame(grid_frame, text=" FAST COMMAND DISPATCH ", font=("Consolas", 10, "bold"), fg=THEME["accent_green"], bg=THEME["bg_card"], bd=1, relief="flat", highlightbackground=THEME["border"], highlightthickness=1)
        ctrl_card.pack(side="right", fill="both", padx=5, pady=5, ipadx=10)

        actions = [
            ("⚡ Run Full Self-Test", "selftest"),
            ("📡 Start Rogue AP Detection", "roguedetect start"),
            ("🛑 Stop Rogue AP Detection", "roguedetect stop"),
            ("⚡ Start Deauth Monitor", "deauthmon start"),
            ("📊 Inspect NRF24 Registers", "nrf24lab dump"),
            ("📶 NRF24 126-Ch Spectrum Sweep", "nrf24lab sweep 10"),
            ("👾 Trigger Matrix Mode", "ghost matrix"),
            ("👻 Summon Ghost Companion", "ghost companion"),
            ("🎯 Dump Achievements", "ghost achievements"),
        ]

        for text, cmd in actions:
            btn = tk.Button(ctrl_card, text=text, command=lambda c=cmd: self._send_quick_cmd(c),
                            bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9),
                            anchor="w", relief="flat", padx=10, pady=4)
            btn.pack(fill="x", padx=10, pady=4)

    def _init_wifi_tab(self):
        frame = tk.Frame(self.tab_wifi, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        # Toolbar
        tbar = tk.Frame(frame, bg=THEME["bg_card"], height=40)
        tbar.pack(fill="x", pady=4)
        tk.Button(tbar, text="▶ Start Live AP Scan", command=lambda: self._send_quick_cmd("scanap"), bg=THEME["accent_green"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="⏹ Stop Scan", command=lambda: self._send_quick_cmd("stopscan"), bg=THEME["accent_red"], fg="#fff", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="⚠️ Check Rogue / Evil Twins", command=lambda: self._send_quick_cmd("roguedetect status"), bg=THEME["accent_cyan"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="🗑 Clear Table", command=self._clear_wifi_table, bg=THEME["bg_input"], fg=THEME["text_dim"], font=("Consolas", 9)).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="💾 Export APs to CSV", command=self._export_aps_csv, bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="right", padx=5, pady=4)

        # Treeview
        cols = ("BSSID", "SSID", "CH", "RSSI", "SECURITY", "VENDOR", "THREAT")
        self.wifi_tree = ttk.Treeview(frame, columns=cols, show="headings")
        for c in cols:
            self.wifi_tree.heading(c, text=c)
            self.wifi_tree.column(c, width=100)
        self.wifi_tree.column("BSSID", width=140)
        self.wifi_tree.column("SSID", width=220)
        self.wifi_tree.column("VENDOR", width=140)
        self.wifi_tree.column("THREAT", width=120)

        scrollbar = ttk.Scrollbar(frame, orient="vertical", command=self.wifi_tree.yview)
        self.wifi_tree.configure(yscrollcommand=scrollbar.set)
        scrollbar.pack(side="right", fill="y")
        self.wifi_tree.pack(fill="both", expand=True, pady=4)

    def _init_ble_tab(self):
        frame = tk.Frame(self.tab_ble, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        # Toolbar
        tbar = tk.Frame(frame, bg=THEME["bg_card"], height=40)
        tbar.pack(fill="x", pady=4)
        tk.Button(tbar, text="▶ Start BLE Scan", command=lambda: self._send_quick_cmd("blescan"), bg=THEME["accent_green"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="⏹ Stop Scan", command=lambda: self._send_quick_cmd("stop"), bg=THEME["accent_red"], fg="#fff", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="🍎 Scan AirTags", command=lambda: self._send_quick_cmd("listairtags"), bg=THEME["accent_cyan"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="🐬 Scan Flippers", command=lambda: self._send_quick_cmd("listflippers"), bg=THEME["accent_yellow"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="🔍 BLE Detect", command=lambda: self._send_quick_cmd("bledetect"), bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="🗑 Clear Table", command=self._clear_ble_table, bg=THEME["bg_input"], fg=THEME["text_dim"], font=("Consolas", 9)).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="💾 Export BLE to CSV", command=self._export_ble_csv, bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="right", padx=5, pady=4)

        # Treeview
        cols = ("MAC", "NAME", "RSSI", "TYPE", "DETAILS")
        self.ble_tree = ttk.Treeview(frame, columns=cols, show="headings")
        for c in cols:
            self.ble_tree.heading(c, text=c)
            self.ble_tree.column(c, width=120)
        self.ble_tree.column("MAC", width=150)
        self.ble_tree.column("NAME", width=200)
        self.ble_tree.column("DETAILS", width=350)

        scrollbar = ttk.Scrollbar(frame, orient="vertical", command=self.ble_tree.yview)
        self.ble_tree.configure(yscrollcommand=scrollbar.set)
        scrollbar.pack(side="right", fill="y")
        self.ble_tree.pack(fill="both", expand=True, pady=4)

    def _init_nrf24_tab(self):
        frame = tk.Frame(self.tab_nrf24, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        tbar = tk.Frame(frame, bg=THEME["bg_card"])
        tbar.pack(fill="x", pady=4)
        tk.Button(tbar, text="🔍 Inspect Registers (SPI2)", command=lambda: self._send_quick_cmd("nrf24lab dump"), bg=THEME["accent_cyan"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="📶 Run 126-Ch Sweep", command=lambda: self._send_quick_cmd("nrf24lab sweep 12"), bg=THEME["accent_green"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="⚡ Run RF Benchmark", command=lambda: self._send_quick_cmd("nrf24lab bench 100"), bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="left", padx=5, pady=4)

        self.nrf_text = tk.Text(frame, bg=THEME["bg_card"], fg=THEME["accent_green"], font=("Consolas", 9), bd=0, padx=8, pady=8)
        self.nrf_text.pack(fill="both", expand=True)

    def _init_timeline_tab(self):
        frame = tk.Frame(self.tab_timeline, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        tbar = tk.Frame(frame, bg=THEME["bg_card"])
        tbar.pack(fill="x", pady=4)
        tk.Button(tbar, text="🗑 Clear Timeline", command=self._clear_timeline, bg=THEME["bg_input"], fg=THEME["text_dim"], font=("Consolas", 9)).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="💾 Export Timeline (JSON)", command=self._export_timeline_json, bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="right", padx=5, pady=4)

        cols = ("TIME", "SUBSYSTEM", "SEVERITY", "EVENT DETAILS")
        self.timeline_tree = ttk.Treeview(frame, columns=cols, show="headings")
        for c in cols:
            self.timeline_tree.heading(c, text=c)
        self.timeline_tree.column("TIME", width=90)
        self.timeline_tree.column("SUBSYSTEM", width=120)
        self.timeline_tree.column("SEVERITY", width=100)
        self.timeline_tree.column("EVENT DETAILS", width=600)

        scrollbar = ttk.Scrollbar(frame, orient="vertical", command=self.timeline_tree.yview)
        self.timeline_tree.configure(yscrollcommand=scrollbar.set)
        scrollbar.pack(side="right", fill="y")
        self.timeline_tree.pack(fill="both", expand=True)

    def _init_sdr_tab(self):
        frame = tk.Frame(self.tab_sdr, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        info = self.sdr.get_info()
        card = tk.LabelFrame(frame, text=" EXTENSIBLE SDR HARDWARE ABSTRACTION ", font=("Consolas", 10, "bold"), fg=THEME["accent_cyan"], bg=THEME["bg_card"], bd=1, relief="flat", highlightbackground=THEME["border"], highlightthickness=1)
        card.pack(fill="both", expand=True, padx=10, pady=10)

        txt = tk.Text(card, bg=THEME["bg_card"], fg=THEME["text_main"], font=("Consolas", 10), bd=0, padx=10, pady=10)
        txt.pack(fill="both", expand=True)
        txt.insert("end",
            f"Active Driver:  {info['driver']}\n"
            f"Hardware Mode:  {info['hardware']}\n"
            f"Device Status:  {info['status']}\n"
            f"Tuning Limits:  {info['tuning_range']}\n"
            "----------------------------------------------------------------------\n"
            "Architecture Standard:\n"
            "  1. The 7Semi ESP32-S3 and NRF24L01+ are NOT software-defined radios.\n"
            "  2. Wideband I/Q digitizing is isolated strictly to external host SDRs\n"
            "     (RTL-SDR v4, HackRF One, LimeSDR, BladeRF).\n"
            "  3. GhostESP telemetry provides correlated 2.4 GHz ground-truth context.\n"
        )
        txt.configure(state="disabled")

    def _init_terminal_tab(self):
        frame = tk.Frame(self.tab_terminal, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        self.term_text = tk.Text(frame, bg="#05070a", fg="#00ff66", font=("Consolas", 9), bd=0, padx=8, pady=8)
        self.term_text.pack(fill="both", expand=True)

        cmd_box = tk.Frame(frame, bg=THEME["bg_card"])
        cmd_box.pack(fill="x", pady=4)

        tk.Label(cmd_box, text="CMD >", font=("Consolas", 9, "bold"), fg=THEME["accent_green"], bg=THEME["bg_card"]).pack(side="left", padx=6)
        self.cmd_entry = tk.Entry(cmd_box, bg=THEME["bg_input"], fg="#ffffff", font=("Consolas", 10), insertbackground="#00ff66")
        self.cmd_entry.pack(side="left", fill="x", expand=True, padx=6, pady=4)
        self.cmd_entry.bind("<Return>", lambda e: self._send_cmd_entry())

        btn_send = tk.Button(cmd_box, text="SEND", command=self._send_cmd_entry, bg=THEME["bg_input"], fg=THEME["accent_green"], font=("Consolas", 9, "bold"))
        btn_send.pack(side="right", padx=6)

    def _build_statusbar(self):
        self.statusbar = tk.Label(self, text="Status: Disconnected | Ready to connect to 7Semi ESP32-S3", bd=1, relief="flat", anchor="w", font=("Consolas", 9), bg=THEME["bg_card"], fg=THEME["text_dim"])
        self.statusbar.pack(side="bottom", fill="x")

    def _refresh_com_ports(self):
        ports = GhostSerialTransceiver.list_available_ports()
        p_list = [p["device"] for p in ports]
        self.port_combo["values"] = p_list
        if p_list:
            for p in ports:
                if p["is_likely_esp"]:
                    self.port_combo.set(p["device"])
                    break
            if not self.port_combo.get():
                self.port_combo.current(0)
        else:
            self.port_combo.set("No Ports Detected")

    def _toggle_connect(self):
        if self.serial.running:
            self.serial.disconnect()
        else:
            port = self.port_combo.get()
            if not port or "No Ports" in port:
                messagebox.showerror("Error", "Please select a valid COM port.")
                return
            ok, msg = self.serial.connect(port)
            if not ok:
                messagebox.showerror("Connection Failed", msg)

    def _on_connection_status(self, is_connected, message):
        self.after(0, lambda: self._update_conn_ui(is_connected, message))

    def _update_conn_ui(self, is_connected, message):
        self.statusbar.config(text=f"Status: {message}")
        if is_connected:
            self.btn_connect.config(text="DISCONNECT", bg=THEME["accent_red"], fg="#ffffff")
            self.db.log_event("SYSTEM", "CONNECT", "INFO", f"Connected to {self.serial.current_port_name}")
            self._add_timeline_entry("SYSTEM", "INFO", f"Serial connection established on {self.serial.current_port_name}")
        else:
            self.btn_connect.config(text="CONNECT", bg=THEME["accent_green"], fg="#000000")
            self._add_timeline_entry("SYSTEM", "INFO", "Serial connection closed")

    def _send_quick_cmd(self, cmd_text):
        if not self.serial.running:
            messagebox.showwarning("Not Connected", "Please connect to the ESP32-S3 serial port first.")
            return
        self.serial.send_command(cmd_text)
        self.term_text.insert("end", f"> {cmd_text}\n")
        self.term_text.see("end")

    def _send_cmd_entry(self):
        cmd = self.cmd_entry.get().strip()
        if cmd:
            self._send_quick_cmd(cmd)
            self.cmd_entry.delete(0, "end")

    def _on_serial_line(self, line):
        self.after(0, lambda: self._handle_incoming_line(line))

    def _handle_incoming_line(self, line):
        # 1. Append raw line to terminal
        self.term_text.insert("end", line + "\n")
        self.term_text.see("end")

        # Strip ANSI escape codes
        clean = re.sub(r'\x1b\[[0-9;]*[a-zA-Z]', '', line).strip()
        if not clean:
            return

        # 2. Check for Wi-Fi AP scan block lines
        # Pattern 1: [idx] SSID: <ssid>,
        m_ssid = re.search(r'\[\d+\]\s*SSID:\s*(.*?)(?:,|$)', clean)
        if m_ssid:
            if self.current_ap.get("bssid"):
                self._commit_ap(self.current_ap)
            self.current_ap = {
                "ssid": m_ssid.group(1).strip(),
                "bssid": "",
                "ch": "",
                "rssi": "",
                "sec": "WPA2",
                "vendor": "",
                "threat": "CLEAN"
            }
            return

        # Pattern 2: BSSID: <mac>,
        m_bssid = re.search(r'BSSID:\s*([0-9A-Fa-f:]{17})', clean)
        if m_bssid and "current_ap" in self.__dict__ and self.current_ap:
            self.current_ap["bssid"] = m_bssid.group(1).upper()
            return

        # Pattern 3: RSSI: <rssi>,
        m_rssi = re.search(r'RSSI:\s*(-?\d+)', clean)
        if m_rssi and "current_ap" in self.__dict__ and self.current_ap:
            self.current_ap["rssi"] = m_rssi.group(1)
            return

        # Pattern 4: Channel: <ch>,
        m_ch = re.search(r'Channel:\s*(\d+)', clean)
        if m_ch and "current_ap" in self.__dict__ and self.current_ap:
            self.current_ap["ch"] = m_ch.group(1)
            if self.current_ap.get("bssid"):
                self._commit_ap(self.current_ap)
            return

        # Pattern 5: Vendor: <vendor>
        m_vnd = re.search(r'Vendor:\s*(.+)', clean)
        if m_vnd and "current_ap" in self.__dict__ and self.current_ap:
            self.current_ap["vendor"] = m_vnd.group(1).strip()
            if self.current_ap.get("bssid"):
                self._commit_ap(self.current_ap)
            return

        # Pattern 6: Radar status table line (BSSID | Ch | RSSI | Security | SSID | Threat)
        if "|" in clean and re.search(r'[0-9A-Fa-f:]{17}', clean):
            parts = [p.strip() for p in clean.split("|")]
            if len(parts) >= 6 and re.match(r'^[0-9A-Fa-f:]{17}$', parts[0]):
                ap_data = {
                    "bssid": parts[0].upper(),
                    "ch": parts[1],
                    "rssi": parts[2],
                    "sec": parts[3],
                    "ssid": parts[4],
                    "vendor": "",
                    "threat": parts[5]
                }
                self._commit_ap(ap_data)
                if parts[5] not in ("CLEAN", "NONE", "IDLE"):
                    self._add_timeline_entry("WIFI_ROGUE", "HIGH", f"Threat Detected: {parts[4]} ({parts[0]}) - {parts[5]}")
                return

        # 3. Check for BLE Advertisers & Devices
        # Format: [<idx>] Advertiser | <MAC> | <RSSI> dBm | <Name>
        if re.search(r'\[\d+\]\s*(?:Advertiser|iBeacon|BLE|Flipper|AirTag)', clean, re.IGNORECASE):
            mac_m = re.search(r'([0-9A-Fa-f:]{17})', clean)
            if mac_m:
                mac = mac_m.group(1).upper()
                rssi_m = re.search(r'(-?\d+)\s*dBm', clean)
                rssi = rssi_m.group(1) if rssi_m else ""
                parts = [p.strip() for p in clean.split("|")]
                dev_type = parts[0] if len(parts) > 0 else "BLE"
                name = parts[3] if len(parts) > 3 else "Unknown"
                details = clean
                self._commit_ble({"mac": mac, "name": name, "rssi": rssi, "type": dev_type, "details": details})
                return

        # Format: Tracking BLE advertiser: RSSI <rssi> ... MAC: <mac> Name: <name>
        if "Tracking BLE" in clean or "BLE Device" in clean:
            mac_m = re.search(r'([0-9A-Fa-f:]{17})', clean)
            if mac_m:
                mac = mac_m.group(1).upper()
                name_m = re.search(r'Name:\s*([^\n\r,]+)', clean)
                name = name_m.group(1).strip() if name_m else "BLE Device"
                rssi_m = re.search(r'RSSI\s*(-?\d+)', clean)
                rssi = rssi_m.group(1) if rssi_m else ""
                self._commit_ble({"mac": mac, "name": name, "rssi": rssi, "type": "Advertiser", "details": clean})
                return

        # 4. Check for NRF24 Telemetry
        if "NRF24" in clean or "Register" in clean or "RF_CH" in clean or "RPD" in clean:
            self.nrf_text.insert("end", clean + "\n")
            self.nrf_text.see("end")

        # 5. Check for Timeline Events & Alerts
        if "WiFi Scan started" in clean:
            self._add_timeline_entry("WIFI_SCAN", "INFO", "Wi-Fi scan initiated (hopping channels)")
        elif "Found" in clean and "access points" in clean:
            self._add_timeline_entry("WIFI_SCAN", "INFO", clean)
        elif "WiFi scan stopped" in clean:
            if self.current_ap.get("bssid"):
                self._commit_ap(self.current_ap)
            self._add_timeline_entry("WIFI_SCAN", "INFO", "Wi-Fi scan completed")
        elif "DEAUTH STORM" in clean:
            self.db.log_event("WIFI_DEAUTH", "ALERT", "CRITICAL", clean)
            self._add_timeline_entry("WIFI_DEAUTH", "CRITICAL", clean)
        elif "ROGUE ALERT" in clean or "Evil Twin" in clean or "ROGUE AP DETECTED" in clean:
            self.db.log_event("WIFI_ROGUE", "ALERT", "HIGH", clean)
            self._add_timeline_entry("WIFI_ROGUE", "HIGH", clean)
        elif "GHOSTESP RESEARCH CONFIGURATION PROFILES" in clean or "Active | Description" in clean:
            self._add_timeline_entry("PROFILE", "INFO", "Inspecting configuration profiles")
        elif "profile apply" in clean or "Profile switched" in clean:
            self._add_timeline_entry("PROFILE", "INFO", clean)
        elif "SELF-TEST" in clean or "Self-Test" in clean:
            self._add_timeline_entry("SELFTEST", "INFO", clean)

    def _commit_ap(self, ap_data):
        bssid = ap_data.get("bssid", "")
        if not bssid:
            return
        ssid = ap_data.get("ssid", "<Hidden>")
        ch = ap_data.get("ch", "?")
        rssi = ap_data.get("rssi", "?")
        sec = ap_data.get("sec", "WPA2")
        vendor = ap_data.get("vendor", "")
        threat = ap_data.get("threat", "CLEAN")

        # Insert or update into wifi_tree
        vals = (bssid, ssid, ch, rssi, sec, vendor, threat)
        if bssid in self.known_aps:
            item_id = self.known_aps[bssid]
            self.wifi_tree.item(item_id, values=vals)
        else:
            item_id = self.wifi_tree.insert("", "end", values=vals)
            self.known_aps[bssid] = item_id
            self.db.log_ap(bssid, ssid, int(ch) if ch.isdigit() else 0, int(rssi) if rssi.lstrip('-').isdigit() else 0, sec, 1 if threat != "CLEAN" else 0, threat)
            self._add_timeline_entry("WIFI_AP", "INFO", f"Discovered AP: {ssid} [{bssid}] Ch:{ch} RSSI:{rssi}dBm")

    def _commit_ble(self, ble_data):
        mac = ble_data.get("mac", "")
        if not mac:
            return
        name = ble_data.get("name", "Unknown")
        rssi = ble_data.get("rssi", "?")
        dev_type = ble_data.get("type", "BLE")
        details = ble_data.get("details", "")

        vals = (mac, name, rssi, dev_type, details)
        if mac in self.known_bles:
            item_id = self.known_bles[mac]
            self.ble_tree.item(item_id, values=vals)
        else:
            item_id = self.ble_tree.insert("", "end", values=vals)
            self.known_bles[mac] = item_id
            self.db.log_event("BLE", "DEVICE", "INFO", f"BLE: {name} [{mac}] {rssi}dBm")
            self._add_timeline_entry("BLE", "INFO", f"BLE Device: {name} [{mac}] {rssi}dBm")

    def _add_timeline_entry(self, subsystem, severity, details):
        ts = time.strftime("%H:%M:%S")
        self.timeline_tree.insert("", 0, values=(ts, subsystem, severity, details))

    def _clear_wifi_table(self):
        for item in self.wifi_tree.get_children():
            self.wifi_tree.delete(item)
        self.known_aps.clear()

    def _clear_ble_table(self):
        for item in self.ble_tree.get_children():
            self.ble_tree.delete(item)
        self.known_bles.clear()

    def _clear_timeline(self):
        for item in self.timeline_tree.get_children():
            self.timeline_tree.delete(item)

    def _export_aps_csv(self):
        f = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("CSV files", "*.csv")])
        if f:
            with open(f, "w", newline="", encoding="utf-8") as out:
                writer = csv.writer(out)
                writer.writerow(["BSSID", "SSID", "CH", "RSSI", "SECURITY", "VENDOR", "THREAT"])
                for item in self.wifi_tree.get_children():
                    writer.writerow(self.wifi_tree.item(item)["values"])
            messagebox.showinfo("Exported", f"Saved Wi-Fi AP database to {f}")

    def _export_ble_csv(self):
        f = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("CSV files", "*.csv")])
        if f:
            with open(f, "w", newline="", encoding="utf-8") as out:
                writer = csv.writer(out)
                writer.writerow(["MAC", "NAME", "RSSI", "TYPE", "DETAILS"])
                for item in self.ble_tree.get_children():
                    writer.writerow(self.ble_tree.item(item)["values"])
            messagebox.showinfo("Exported", f"Saved BLE devices to {f}")

    def _export_timeline_json(self):
        f = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if f:
            self.db.export_events_json(f)
            messagebox.showinfo("Exported", f"Saved Timeline Events to {f}")

if __name__ == "__main__":
    app = GhostCommandCenter()
    app.mainloop()

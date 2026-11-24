#!/usr/bin/env python3
"""
GhostESP God Mode V3 - Laptop Command Center
Author: Autonomous Desktop Application Engineering Organization
Architecture: Multi-Tab Cyberpunk Operations Suite with Serial Telemetry
"""

import sys
import os
import time
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
        self.geometry("1100x720")
        self.minsize(950, 600)
        self.configure(bg=THEME["bg_dark"])

        self.db = GhostDatabase()
        self.sdr = SDRManager()
        self.serial = GhostSerialTransceiver(
            on_line_received=self._on_serial_line,
            on_status_changed=self._on_connection_status
        )

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
        self.tab_terminal = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_timeline = tk.Frame(self.notebook, bg=THEME["bg_dark"])
        self.tab_sdr = tk.Frame(self.notebook, bg=THEME["bg_dark"])

        self.notebook.add(self.tab_dashboard, text="[ DASHBOARD ]")
        self.notebook.add(self.tab_wifi, text="[ WI-FI LAB ]")
        self.notebook.add(self.tab_ble, text="[ BLE LAB ]")
        self.notebook.add(self.tab_nrf24, text="[ NRF24 RF ]")
        self.notebook.add(self.tab_timeline, text="[ TIMELINE ]")
        self.notebook.add(self.tab_sdr, text="[ SDR INTERFACE ]")
        self.notebook.add(self.tab_terminal, text="[ CONSOLE ]")

        self._init_dashboard_tab()
        self._init_wifi_tab()
        self._init_nrf24_tab()
        self._init_terminal_tab()
        self._init_timeline_tab()
        self._init_sdr_tab()

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
            ("📊 Inspect NRF24 Registers", "nrf24lab diag"),
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
        tk.Button(tbar, text="💾 Export APs to CSV", command=self._export_aps_csv, bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="right", padx=5, pady=4)

        # Treeview
        cols = ("BSSID", "SSID", "CH", "RSSI", "SECURITY", "STATUS")
        self.wifi_tree = ttk.Treeview(frame, columns=cols, show="headings")
        for c in cols:
            self.wifi_tree.heading(c, text=c)
            self.wifi_tree.column(c, width=120)
        self.wifi_tree.column("SSID", width=200)
        self.wifi_tree.pack(fill="both", expand=True, pady=4)

    def _init_nrf24_tab(self):
        frame = tk.Frame(self.tab_nrf24, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        tbar = tk.Frame(frame, bg=THEME["bg_card"])
        tbar.pack(fill="x", pady=4)
        tk.Button(tbar, text="🔍 Inspect Registers (SPI2)", command=lambda: self._send_quick_cmd("nrf24lab diag"), bg=THEME["accent_cyan"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="📶 Run 126-Ch Sweep", command=lambda: self._send_quick_cmd("nrf24lab sweep 12"), bg=THEME["accent_green"], fg="#000", font=("Consolas", 9, "bold")).pack(side="left", padx=5, pady=4)
        tk.Button(tbar, text="⚡ Run RF Benchmark", command=lambda: self._send_quick_cmd("nrf24lab benchmark 100"), bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="left", padx=5, pady=4)

        self.nrf_text = tk.Text(frame, bg=THEME["bg_card"], fg=THEME["accent_green"], font=("Consolas", 9), bd=0, padx=8, pady=8)
        self.nrf_text.pack(fill="both", expand=True)

    def _init_timeline_tab(self):
        frame = tk.Frame(self.tab_timeline, bg=THEME["bg_dark"])
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        tbar = tk.Frame(frame, bg=THEME["bg_card"])
        tbar.pack(fill="x", pady=4)
        tk.Button(tbar, text="💾 Export Timeline (JSON)", command=self._export_timeline_json, bg=THEME["bg_input"], fg=THEME["text_main"], font=("Consolas", 9)).pack(side="right", padx=5, pady=4)

        cols = ("TIME", "SUBSYSTEM", "SEVERITY", "EVENT DETAILS")
        self.timeline_tree = ttk.Treeview(frame, columns=cols, show="headings")
        for c in cols:
            self.timeline_tree.heading(c, text=c)
        self.timeline_tree.column("EVENT DETAILS", width=450)
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
            # Pick likely ESP port
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
        else:
            self.btn_connect.config(text="CONNECT", bg=THEME["accent_green"], fg="#000000")

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
        # Append to raw console
        self.term_text.insert("end", line + "\n")
        self.term_text.see("end")

        # Parse alerts & events
        if "ROGUE ALERT" in line or "Evil Twin" in line:
            self.db.log_event("WIFI_ROGUE", "ALERT", "HIGH", line)
            self._add_timeline_entry("WIFI_ROGUE", "HIGH", line)
        elif "DEAUTH STORM" in line:
            self.db.log_event("WIFI_DEAUTH", "ALERT", "CRITICAL", line)
            self._add_timeline_entry("WIFI_DEAUTH", "CRITICAL", line)
        elif "NRF24" in line or "Register" in line:
            self.nrf_text.insert("end", line + "\n")
            self.nrf_text.see("end")

    def _add_timeline_entry(self, subsystem, severity, details):
        ts = time.strftime("%H:%M:%S")
        self.timeline_tree.insert("", 0, values=(ts, subsystem, severity, details))

    def _export_aps_csv(self):
        f = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("CSV files", "*.csv")])
        if f:
            self.db.export_aps_csv(f)
            messagebox.showinfo("Exported", f"Saved Wi-Fi AP database to {f}")

    def _export_timeline_json(self):
        f = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if f:
            self.db.export_events_json(f)
            messagebox.showinfo("Exported", f"Saved Timeline Events to {f}")

if __name__ == "__main__":
    app = GhostCommandCenter()
    app.mainloop()

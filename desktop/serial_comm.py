"""
GhostESP God Mode V3 - Background Serial Transceiver & Auto-Discovery Engine
Author: Autonomous Desktop Application Engineering Organization
"""

import threading
import time
import serial
import serial.tools.list_ports

class GhostSerialTransceiver:
    def __init__(self, baud_rate=115200, on_line_received=None, on_status_changed=None):
        self.baud_rate = baud_rate
        self.on_line_received = on_line_received
        self.on_status_changed = on_status_changed
        self.serial_port = None
        self.running = False
        self.thread = None
        self.current_port_name = None

    @staticmethod
    def list_available_ports():
        ports = serial.tools.list_ports.comports()
        detected = []
        for p in ports:
            is_esp = any(id_str in (p.description or "").lower() for id_str in ["ch340", "cp210", "usb-serial", "jtag", "esp"])
            detected.append({
                "device": p.device,
                "description": p.description,
                "is_likely_esp": is_esp
            })
        return detected

    def connect(self, port_name):
        self.disconnect()
        try:
            self.serial_port = serial.Serial(port_name, self.baud_rate, timeout=0.5)
            self.current_port_name = port_name
            self.running = True
            self.thread = threading.Thread(target=self._read_worker, daemon=True)
            self.thread.start()
            if self.on_status_changed:
                self.on_status_changed(True, f"Connected to {port_name} @ {self.baud_rate} baud")
            return True, f"Connected to {port_name}"
        except Exception as e:
            if self.on_status_changed:
                self.on_status_changed(False, f"Connection failed: {str(e)}")
            return False, str(e)

    def disconnect(self):
        self.running = False
        if self.thread and self.thread.is_alive():
            self.thread.join(timeout=1.0)
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.close()
            except Exception:
                pass
        self.serial_port = None
        self.current_port_name = None
        if self.on_status_changed:
            self.on_status_changed(False, "Disconnected")

    def send_command(self, cmd_text):
        if not self.serial_port or not self.serial_port.is_open:
            return False, "Not connected"
        try:
            if not cmd_text.endswith("\n"):
                cmd_text += "\n"
            self.serial_port.write(cmd_text.encode("utf-8", errors="ignore"))
            return True, "Command sent"
        except Exception as e:
            return False, str(e)

    def _read_worker(self):
        buffer = ""
        while self.running and self.serial_port and self.serial_port.is_open:
            try:
                data = self.serial_port.read(256)
                if data:
                    buffer += data.decode("utf-8", errors="replace")
                    while "\n" in buffer:
                        line, buffer = buffer.split("\n", 1)
                        line = line.strip("\r")
                        if self.on_line_received:
                            self.on_line_received(line)
                else:
                    time.sleep(0.02)
            except Exception as e:
                if self.running and self.on_status_changed:
                    self.on_status_changed(False, f"Serial error: {str(e)}")
                break
        self.running = False

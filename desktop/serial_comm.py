"""Thread-safe serial transport & host SDR detection abstraction."""

from __future__ import annotations

import queue
import shutil
import threading
import time
from typing import Callable, Dict, List, Optional, Tuple

import serial
import serial.tools.list_ports


class GhostSerialTransceiver:
    """Thread-safe serial I/O manager with queued writes and error recovery."""

    def __init__(
        self,
        on_line_callback: Optional[Callable[[str], None]] = None,
        on_status_callback: Optional[Callable[[bool, str], None]] = None,
    ) -> None:
        self.ser: Optional[serial.Serial] = None
        self.running = False
        self.auto_reconnect = False
        self._rx_thread: Optional[threading.Thread] = None
        self._tx_thread: Optional[threading.Thread] = None
        self._io_lock = threading.Lock()
        self._tx_queue: "queue.Queue[Optional[str]]" = queue.Queue()
        self.on_line = on_line_callback
        self.on_status = on_status_callback
        self.current_port_name: Optional[str] = None
        self.current_baudrate: int = 115200
        self.rx_bytes = 0
        self.tx_bytes = 0

    @staticmethod
    def list_available_ports() -> List[Dict[str, object]]:
        """Enumerate system COM/tty ports with ESP32-S3 likelihood scoring."""
        ports = []
        for p in serial.tools.list_ports.comports():
            desc = (p.description or "").lower()
            hwid = (p.hwid or "").lower()
            is_esp_jtag = "303a:1001" in hwid or "usb serial device" in desc
            is_uart_bridge = any(
                k in desc or k in hwid for k in ("cp210", "ch340", "ftdi", "espressif", "esp32")
            )
            ports.append({
                "device": p.device,
                "description": p.description or p.device,
                "hwid": p.hwid or "",
                "is_likely_esp": bool(is_esp_jtag or is_uart_bridge),
                "priority": 0 if is_esp_jtag else (1 if is_uart_bridge else 2),
            })
        ports.sort(key=lambda item: (item["priority"], str(item["device"])))
        return ports

    def connect(
        self,
        port: str,
        baudrate: int = 115200,
        dtr: bool = False,
        rts: bool = False,
        auto_reconnect: bool = False,
    ) -> Tuple[bool, str]:
        """Open serial port and start background RX and TX worker threads."""
        self.disconnect()
        self.auto_reconnect = auto_reconnect
        try:
            ser = serial.Serial(
                port=port,
                baudrate=baudrate,
                timeout=0.1,
                write_timeout=1.0,
            )
            ser.dtr = dtr
            ser.rts = rts
        except Exception as exc:
            msg = f"Failed to open {port}: {exc}"
            if self.on_status:
                self.on_status(False, msg)
            return False, msg

        with self._io_lock:
            self.ser = ser
            self.current_port_name = port
            self.current_baudrate = baudrate
            self.running = True
            self.rx_bytes = 0
            self.tx_bytes = 0
            while not self._tx_queue.empty():
                try:
                    self._tx_queue.get_nowait()
                except queue.Empty:
                    break

        if self.on_status:
            self.on_status(True, f"Connected to {port} @ {baudrate} bps")

        self._rx_thread = threading.Thread(target=self._read_loop, name="GhostSerialRX", daemon=True)
        self._tx_thread = threading.Thread(target=self._write_loop, name="GhostSerialTX", daemon=True)
        self._rx_thread.start()
        self._tx_thread.start()
        return True, "Connected"

    def disconnect(self) -> None:
        """Stop worker threads and close serial handle cleanly."""
        self.auto_reconnect = False
        was_running = self.running
        self.running = False
        self._tx_queue.put(None)

        if self._rx_thread and self._rx_thread.is_alive() and threading.current_thread() != self._rx_thread:
            self._rx_thread.join(timeout=0.5)
        if self._tx_thread and self._tx_thread.is_alive() and threading.current_thread() != self._tx_thread:
            self._tx_thread.join(timeout=0.5)

        with self._io_lock:
            if self.ser is not None:
                try:
                    if self.ser.is_open:
                        self.ser.close()
                except Exception:
                    pass
                self.ser = None

        if was_running and self.on_status:
            self.on_status(False, "Disconnected")

    def send_command(self, cmd: str) -> bool:
        """Enqueue a CLI command to send asynchronously."""
        if not self.running:
            return False
        text = cmd if cmd.endswith("\n") else (cmd + "\n")
        self._tx_queue.put(text)
        return True

    def set_control_lines(self, dtr: bool, rts: bool) -> bool:
        """Set DTR/RTS lines safely (for reset or UART mode)."""
        with self._io_lock:
            if not self.ser or not self.ser.is_open:
                return False
            try:
                self.ser.dtr = dtr
                self.ser.rts = rts
                return True
            except Exception:
                return False

    def _write_loop(self) -> None:
        while self.running:
            try:
                item = self._tx_queue.get(timeout=0.2)
            except queue.Empty:
                continue
            if item is None:
                break
            payload = item.encode("utf-8", errors="replace")
            with self._io_lock:
                if self.ser and self.ser.is_open:
                    try:
                        written = self.ser.write(payload)
                        self.ser.flush()
                        self.tx_bytes += int(written or len(payload))
                    except Exception as exc:
                        if self.on_status:
                            self.on_status(False, f"Serial TX Error: {exc}")
                        break

    def _read_loop(self) -> None:
        buffer = b""
        while self.running:
            chunk = b""
            with self._io_lock:
                ser = self.ser
            if ser is None or not ser.is_open:
                break
            try:
                waiting = ser.in_waiting
                chunk = ser.read(waiting if waiting > 0 else 1)
            except Exception as exc:
                self.running = False
                if self.on_status:
                    self.on_status(False, f"Serial Disconnected: {exc}")
                break

            if not chunk:
                continue
            self.rx_bytes += len(chunk)
            buffer += chunk.replace(b"\r\n", b"\n").replace(b"\r", b"\n")
            while b"\n" in buffer:
                line_bytes, buffer = buffer.split(b"\n", 1)
                line = line_bytes.decode("utf-8", errors="replace")
                if self.on_line:
                    self.on_line(line)


class GhostSDRInterface:
    """Honest host-side SDR abstraction checking external CLI backends."""

    def get_info(self) -> Dict[str, str]:
        has_rtl = shutil.which("rtl_power") is not None or shutil.which("rtl_test") is not None
        has_hackrf = shutil.which("hackrf_sweep") is not None or shutil.which("hackrf_info") is not None
        if has_hackrf:
            return {
                "driver": "HackRF CLI (External Host)",
                "hardware": "HackRF One",
                "status": "CLI Detected (Host Connected Check Pending)",
                "tuning_range": "1 MHz - 6000 MHz",
            }
        if has_rtl:
            return {
                "driver": "RTL-SDR CLI (External Host)",
                "hardware": "RTL-SDR Dongle",
                "status": "CLI Detected (Host Connected Check Pending)",
                "tuning_range": "24 MHz - 1766 MHz",
            }
        return {
            "driver": "Unconnected (External SDR Required)",
            "hardware": "None on Host (ESP32-S3 is NOT an SDR)",
            "status": "Hardware Not Attached (Simulation / Context Mode)",
            "tuning_range": "N/A until RTL-SDR / HackRF / BladeRF is attached",
        }

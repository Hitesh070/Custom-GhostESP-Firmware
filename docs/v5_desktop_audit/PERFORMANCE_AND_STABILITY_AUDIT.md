# Performance & Stability Audit

- **Non-Blocking Serial I/O:** Dedicated `GhostSerialRX` and `GhostSerialTX` daemon threads with `queue.Queue` decouple serial latency from the Qt UI event loop.
- **Bounded Log Widgets:** `QPlainTextEdit.setMaximumBlockCount(300..5000)` prevents unbounded RAM growth during long captures.
- **Bounded Chart Buffers:** `self.temp_points = deque(maxlen=120)` keeps Matplotlib redraw cost `O(1)` over long runs.
- **Database Contention:** SQLite `WAL` mode + `threading.Lock()` allows concurrent writes and UI queries without lock errors.
- **Limitation Noted:** Sustained 30-minute continuous live soak was not executed during this session; short live `COM5` burst (`8,870` bytes) and `pytest` offscreen stress ran cleanly (`1.40s` total).

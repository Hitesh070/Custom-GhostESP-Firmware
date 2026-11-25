# Technical Debt & Future Roadmap (GhostESP V4)

| Item | Area | Description & Rationale | Recommended Follow-Up |
|---|---|---|---|
| TD-01 | `sdkconfig` / SMP | `CONFIG_FREERTOS_UNICORE=y` is preserved because shared scan tables (`scanned_aps`, BLE/AP arrays) lack SMP spinlocks | Audit all global scan tables under spinlocks before evaluating dual-core migration |
| TD-02 | `partitions.csv` | `0x3e0000` app0 partition has ~8% headroom (`0x519f0` bytes) on 16 MB flash | Plan an explicit, user-approved partition expansion during a major OTA/wipe release |
| TD-03 | Upstream Compiler Warnings | Upstream files (`wpa_crypto.c`, `reconnect_test.c`, `popup.c`, `large_builtin_icons.c`, `sd_card_manager.c`) have unused helper warnings suppressed | Clean remaining unused static helpers upstream |
| TD-04 | USB-Serial/JTAG Secondary Console | ESP-IDF Secondary USB-Serial/JTAG uses a 64-byte hardware FIFO | Use CP210x UART (`COM6`) for heavy continuous streaming (`raw` / debug verbose) alongside `COM5` |
| TD-05 | Disconnected Hardware Verification | NRF24L01+, microSD card, external I2C sensors, IR LED, and host SDR were absent on bench | Validate sweep/storage/IR against physical attached modules when hardware is connected |

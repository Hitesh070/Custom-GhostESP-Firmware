# Source Code Inventory (`main/`, `include/`, `desktop/`)

Total C/C++/Header/Python modules in `main/`, `include/`, and `desktop/`: **770** files.

| File | Ext | Role | Lines | V4 State |
|---|---|---|---:|---|
| `desktop/README.md` | `.md` | Desktop Application (GhostESP Command Center) | 43 | AUDITED |
| `desktop/app.py` | `.py` | Desktop Application (GhostESP Command Center) | 1118 | FIXED_V4 |
| `desktop/database.py` | `.py` | Desktop Application (GhostESP Command Center) | 270 | FIXED_V4 |
| `desktop/modules/sdr_abstraction.py` | `.py` | Desktop Application (GhostESP Command Center) | 69 | AUDITED |
| `desktop/parsers.py` | `.py` | Desktop Application (GhostESP Command Center) | 264 | FIXED_V4 |
| `desktop/requirements.txt` | `.txt` | Desktop Application (GhostESP Command Center) | 3 | AUDITED |
| `desktop/serial_comm.py` | `.py` | Desktop Application (GhostESP Command Center) | 223 | FIXED_V4 |
| `desktop/tests/test_app_protocol_db.py` | `.py` | Desktop Application (GhostESP Command Center) | 168 | FIXED_V4 |
| `desktop/theme.py` | `.py` | Desktop Application (GhostESP Command Center) | 200 | FIXED_V4 |
| `include/core/ca_fingerprints.h` | `.h` | Firmware Public Header | 66 | AUDITED |
| `include/core/ca_known_certs.h` | `.h` | Firmware Public Header | 52 | AUDITED |
| `include/core/callbacks.h` | `.h` | Firmware Public Header | 125 | AUDITED |
| `include/core/cert_check.h` | `.h` | Firmware Public Header | 86 | AUDITED |
| `include/core/chip_info.h` | `.h` | Firmware Public Header | 45 | AUDITED |
| `include/core/commandline.h` | `.h` | Firmware Public Header | 32 | AUDITED |
| `include/core/commands.h` | `.h` | Firmware Public Header | 319 | AUDITED |
| `include/core/dns_server.h` | `.h` | Firmware Public Header | 171 | AUDITED |
| `include/core/esp_comm_manager.h` | `.h` | Firmware Public Header | 113 | AUDITED |
| `include/core/ghostchi_identity.h` | `.h` | Firmware Public Header | 21 | AUDITED |
| `include/core/ghostesp_version.h` | `.h` | Firmware Public Header | 10 | AUDITED |
| `include/core/ghostlink_bench.h` | `.h` | Firmware Public Header | 32 | AUDITED |
| `include/core/glog.h` | `.h` | Firmware Public Header | 43 | AUDITED |
| `include/core/memory_debug.h` | `.h` | Firmware Public Header | 39 | AUDITED |
| `include/core/network_constants.h` | `.h` | Firmware Public Header | 109 | AUDITED |
| `include/core/ouis.h` | `.h` | Firmware Public Header | 29 | AUDITED |
| `include/core/pcap_pool.h` | `.h` | Firmware Public Header | 88 | AUDITED |
| `include/core/scan_saver.h` | `.h` | Firmware Public Header | 38 | AUDITED |
| `include/core/screen_mirror.h` | `.h` | Firmware Public Header | 35 | AUDITED |
| `include/core/serial_manager.h` | `.h` | Firmware Public Header | 68 | AUDITED |
| `include/core/shell.h` | `.h` | Firmware Public Header | 52 | AUDITED |
| `include/core/sinkhole_https.h` | `.h` | Firmware Public Header | 162 | AUDITED |
| `include/core/system_manager.h` | `.h` | Firmware Public Header | 65 | AUDITED |
| `include/core/uart_share.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/core/universal_ir.h` | `.h` | Firmware Public Header | 41 | AUDITED |
| `include/core/utils.h` | `.h` | Firmware Public Header | 317 | AUDITED |
| `include/core/wpa_crypto.h` | `.h` | Firmware Public Header | 75 | AUDITED |
| `include/features/ble/ble_spam.h` | `.h` | Firmware Public Header | 98 | AUDITED |
| `include/features/ethernet/eth_arp_poison.h` | `.h` | Firmware Public Header | 52 | AUDITED |
| `include/features/wifi/auth_flood.h` | `.h` | Firmware Public Header | 25 | AUDITED |
| `include/features/wifi/bad_msg.h` | `.h` | Firmware Public Header | 26 | AUDITED |
| `include/features/wifi/beacon_spam.h` | `.h` | Firmware Public Header | 65 | AUDITED |
| `include/features/wifi/channel_switch_test.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/features/wifi/dhcp_starvation.h` | `.h` | Firmware Public Header | 33 | AUDITED |
| `include/features/wifi/eapol_logoff.h` | `.h` | Firmware Public Header | 32 | AUDITED |
| `include/features/wifi/gtk_abuse.h` | `.h` | Firmware Public Header | 26 | AUDITED |
| `include/features/wifi/probe_request_flood.h` | `.h` | Firmware Public Header | 25 | AUDITED |
| `include/features/wifi/reconnect_test.h` | `.h` | Firmware Public Header | 73 | AUDITED |
| `include/features/wifi/sae_flood.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/gui/accessibility_fonts.h` | `.h` | Firmware Public Header | 14 | AUDITED |
| `include/gui/asset_pack.h` | `.h` | Firmware Public Header | 62 | AUDITED |
| `include/gui/design_tokens.h` | `.h` | Firmware Public Header | 129 | AUDITED |
| `include/gui/detail_view.h` | `.h` | Firmware Public Header | 63 | AUDITED |
| `include/gui/gui_anim.h` | `.h` | Firmware Public Header | 34 | AUDITED |
| `include/gui/gui_router.h` | `.h` | Firmware Public Header | 53 | AUDITED |
| `include/gui/ios_toggle.h` | `.h` | Firmware Public Header | 40 | AUDITED |
| `include/gui/large_builtin_icons.h` | `.h` | Firmware Public Header | 10 | AUDITED |
| `include/gui/live_chart.h` | `.h` | Firmware Public Header | 82 | AUDITED |
| `include/gui/lv_draw_ppa_v8.h` | `.h` | Firmware Public Header | 9 | AUDITED |
| `include/gui/lvgl_safe.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/gui/main_menu_layout.h` | `.h` | Firmware Public Header | 88 | AUDITED |
| `include/gui/menu_catalog.h` | `.h` | Firmware Public Header | 43 | AUDITED |
| `include/gui/menu_config.h` | `.h` | Firmware Public Header | 34 | AUDITED |
| `include/gui/menu_item_style.h` | `.h` | Firmware Public Header | 35 | AUDITED |
| `include/gui/native_canvas_scale.h` | `.h` | Firmware Public Header | 54 | AUDITED |
| `include/gui/native_canvas_touch.h` | `.h` | Firmware Public Header | 15 | AUDITED |
| `include/gui/nav_history.h` | `.h` | Firmware Public Header | 24 | AUDITED |
| `include/gui/options_view.h` | `.h` | Firmware Public Header | 93 | AUDITED |
| `include/gui/paged_menu.h` | `.h` | Firmware Public Header | 46 | AUDITED |
| `include/gui/popup.h` | `.h` | Firmware Public Header | 95 | AUDITED |
| `include/gui/progress_bar_view.h` | `.h` | Firmware Public Header | 23 | AUDITED |
| `include/gui/rssi_meter.h` | `.h` | Firmware Public Header | 55 | AUDITED |
| `include/gui/scan_status.h` | `.h` | Firmware Public Header | 27 | AUDITED |
| `include/gui/screen_layout.h` | `.h` | Firmware Public Header | 70 | AUDITED |
| `include/gui/select_overlay.h` | `.h` | Firmware Public Header | 48 | AUDITED |
| `include/gui/theme_palette_api.h` | `.h` | Firmware Public Header | 70 | AUDITED |
| `include/gui/toast.h` | `.h` | Firmware Public Header | 26 | AUDITED |
| `include/gui/touch_bar.h` | `.h` | Firmware Public Header | 102 | AUDITED |
| `include/gui/ui_capabilities.h` | `.h` | Firmware Public Header | 38 | AUDITED |
| `include/gui/view_input.h` | `.h` | Firmware Public Header | 52 | AUDITED |
| `include/hardware/board_config.h` | `.h` | 7Semi ESP32-S3 Board HAL & Pin Authority | 383 | FIXED_V4 |
| `include/managers/aerial_detector_manager.h` | `.h` | Firmware Public Header | 126 | AUDITED |
| `include/managers/ap_manager.h` | `.h` | Firmware Public Header | 63 | AUDITED |
| `include/managers/auth_digest.h` | `.h` | Firmware Public Header | 23 | AUDITED |
| `include/managers/badble_manager.h` | `.h` | Firmware Public Header | 36 | AUDITED |
| `include/managers/badusb_builtin_script.h` | `.h` | Firmware Public Header | 63 | AUDITED |
| `include/managers/badusb_manager.h` | `.h` | Firmware Public Header | 74 | AUDITED |
| `include/managers/ble_bridge_manager.h` | `.h` | Firmware Public Header | 14 | AUDITED |
| `include/managers/ble_manager.h` | `.h` | Firmware Public Header | 91 | AUDITED |
| `include/managers/bmi270_driver.h` | `.h` | Firmware Public Header | 12 | AUDITED |
| `include/managers/camera_stream_manager.h` | `.h` | Firmware Public Header | 30 | AUDITED |
| `include/managers/chameleon_manager.h` | `.h` | Firmware Public Header | 299 | AUDITED |
| `include/managers/cloud_store_manager.h` | `.h` | Firmware Public Header | 82 | AUDITED |
| `include/managers/config_manager.h` | `.h` | Firmware Public Header | 26 | AUDITED |
| `include/managers/crash_reporter.h` | `.h` | Firmware Public Header | 32 | AUDITED |
| `include/managers/default_portal.h` | `.h` | Firmware Public Header | 167 | AUDITED |
| `include/managers/dial_manager.h` | `.h` | Firmware Public Header | 41 | AUDITED |
| `include/managers/display_manager.h` | `.h` | Firmware Public Header | 309 | AUDITED |
| `include/managers/encoder_manager.h` | `.h` | Firmware Public Header | 68 | AUDITED |
| `include/managers/espnow_manager.h` | `.h` | Firmware Public Header | 45 | AUDITED |
| `include/managers/ethernet/eth_comm_handler.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/managers/ethernet/eth_fingerprint.h` | `.h` | Firmware Public Header | 46 | AUDITED |
| `include/managers/ethernet/eth_http.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/ethernet/eth_scan_async.h` | `.h` | Firmware Public Header | 70 | AUDITED |
| `include/managers/ethernet/eth_utils.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/ethernet_manager.h` | `.h` | Firmware Public Header | 40 | AUDITED |
| `include/managers/flock_detector_manager.h` | `.h` | Firmware Public Header | 91 | AUDITED |
| `include/managers/fuel_gauge_manager.h` | `.h` | Firmware Public Header | 73 | AUDITED |
| `include/managers/ghost_esp_site_gz.h` | `.h` | Firmware Public Header | 2624 | AUDITED |
| `include/managers/ghost_raw_radio.h` | `.h` | Firmware Public Header | 70 | AUDITED |
| `include/managers/ghost_sta_diag.h` | `.h` | Firmware Public Header | 98 | AUDITED |
| `include/managers/ghostchi_activity.h` | `.h` | Firmware Public Header | 40 | AUDITED |
| `include/managers/ghostchi_manager.h` | `.h` | Firmware Public Header | 56 | AUDITED |
| `include/managers/ghostchi_mood.h` | `.h` | Firmware Public Header | 53 | AUDITED |
| `include/managers/ghostscript_manager.h` | `.h` | Firmware Public Header | 71 | AUDITED |
| `include/managers/ghostscript_runtime.h` | `.h` | Firmware Public Header | 56 | AUDITED |
| `include/managers/gps_manager.h` | `.h` | Firmware Public Header | 60 | AUDITED |
| `include/managers/haptic_manager.h` | `.h` | Firmware Public Header | 31 | AUDITED |
| `include/managers/hid_script_parser.h` | `.h` | Firmware Public Header | 39 | AUDITED |
| `include/managers/http_proxy.h` | `.h` | Firmware Public Header | 22 | AUDITED |
| `include/managers/infrared_common.h` | `.h` | Firmware Public Header | 214 | AUDITED |
| `include/managers/infrared_decoder.h` | `.h` | Firmware Public Header | 49 | AUDITED |
| `include/managers/infrared_manager.h` | `.h` | Firmware Public Header | 111 | AUDITED |
| `include/managers/infrared_protocols.h` | `.h` | Firmware Public Header | 22 | AUDITED |
| `include/managers/infrared_timings.h` | `.h` | Firmware Public Header | 143 | AUDITED |
| `include/managers/ip5306_manager.h` | `.h` | Firmware Public Header | 31 | AUDITED |
| `include/managers/joystick_manager.h` | `.h` | Firmware Public Header | 77 | AUDITED |
| `include/managers/lora_ble.h` | `.h` | Firmware Public Header | 59 | AUDITED |
| `include/managers/lora_channels.h` | `.h` | Firmware Public Header | 81 | AUDITED |
| `include/managers/lora_crypto.h` | `.h` | Firmware Public Header | 51 | AUDITED |
| `include/managers/lora_manager.h` | `.h` | Firmware Public Header | 227 | AUDITED |
| `include/managers/lora_mesh.h` | `.h` | Firmware Public Header | 365 | AUDITED |
| `include/managers/lora_modem.h` | `.h` | Firmware Public Header | 130 | AUDITED |
| `include/managers/lora_pb.h` | `.h` | Firmware Public Header | 299 | AUDITED |
| `include/managers/lora_phoneapi.h` | `.h` | Firmware Public Header | 105 | AUDITED |
| `include/managers/lora_pki.h` | `.h` | Firmware Public Header | 133 | AUDITED |
| `include/managers/lora_store.h` | `.h` | Firmware Public Header | 35 | AUDITED |
| `include/managers/lora_sx1262.h` | `.h` | Firmware Public Header | 70 | AUDITED |
| `include/managers/m5_audio_codec.h` | `.h` | Firmware Public Header | 22 | AUDITED |
| `include/managers/meshcore_alloc.h` | `.h` | Firmware Public Header | 37 | AUDITED |
| `include/managers/meshcore_ble.h` | `.h` | Firmware Public Header | 36 | AUDITED |
| `include/managers/meshcore_companion.h` | `.h` | Firmware Public Header | 63 | AUDITED |
| `include/managers/meshcore_config.h` | `.h` | Firmware Public Header | 266 | AUDITED |
| `include/managers/meshcore_crypto.h` | `.h` | Firmware Public Header | 74 | AUDITED |
| `include/managers/meshcore_identity.h` | `.h` | Firmware Public Header | 58 | AUDITED |
| `include/managers/meshcore_manager.h` | `.h` | Firmware Public Header | 94 | AUDITED |
| `include/managers/meshcore_mesh.h` | `.h` | Firmware Public Header | 173 | AUDITED |
| `include/managers/meshcore_packet.h` | `.h` | Firmware Public Header | 86 | AUDITED |
| `include/managers/meshcore_store.h` | `.h` | Firmware Public Header | 88 | AUDITED |
| `include/managers/microphone/mic_driver.h` | `.h` | Firmware Public Header | 77 | AUDITED |
| `include/managers/microphone/mic_goertzel.h` | `.h` | Firmware Public Header | 69 | AUDITED |
| `include/managers/microphone/mic_visualizer.h` | `.h` | Firmware Public Header | 42 | AUDITED |
| `include/managers/motion_detector_manager.h` | `.h` | Firmware Public Header | 40 | AUDITED |
| `include/managers/nfc/desfire.h` | `.h` | Firmware Public Header | 105 | AUDITED |
| `include/managers/nfc/emv.h` | `.h` | Firmware Public Header | 106 | AUDITED |
| `include/managers/nfc/flipper_nfc_compat.h` | `.h` | Firmware Public Header | 360 | AUDITED |
| `include/managers/nfc/loclass/optimized_cipher.h` | `.h` | Firmware Public Header | 66 | AUDITED |
| `include/managers/nfc/loclass/optimized_cipherutils.h` | `.h` | Firmware Public Header | 55 | AUDITED |
| `include/managers/nfc/loclass/optimized_elite.h` | `.h` | Firmware Public Header | 35 | AUDITED |
| `include/managers/nfc/loclass/optimized_ikeys.h` | `.h` | Firmware Public Header | 32 | AUDITED |
| `include/managers/nfc/loclass/standalone_des.h` | `.h` | Firmware Public Header | 41 | AUDITED |
| `include/managers/nfc/mifare_classic.h` | `.h` | Firmware Public Header | 85 | AUDITED |
| `include/managers/nfc/mifare_test.h` | `.h` | Firmware Public Header | 16 | AUDITED |
| `include/managers/nfc/ndef.h` | `.h` | Firmware Public Header | 21 | AUDITED |
| `include/managers/nfc/ndef_builder.h` | `.h` | Firmware Public Header | 31 | AUDITED |
| `include/managers/nfc/ndef_tag_gen.h` | `.h` | Firmware Public Header | 26 | AUDITED |
| `include/managers/nfc/nfc_backend.h` | `.h` | Firmware Public Header | 27 | AUDITED |
| `include/managers/nfc/ntag_t2.h` | `.h` | Firmware Public Header | 86 | AUDITED |
| `include/managers/nfc/picopass.h` | `.h` | Firmware Public Header | 152 | AUDITED |
| `include/managers/nfc/pn532_compat.h` | `.h` | Firmware Public Header | 29 | AUDITED |
| `include/managers/nfc/write_ntag.h` | `.h` | Firmware Public Header | 39 | AUDITED |
| `include/managers/nrf24_remote_manager.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/ota_manager.h` | `.h` | Firmware Public Header | 136 | AUDITED |
| `include/managers/p4_slave_ota_manager.h` | `.h` | Firmware Public Header | 18 | AUDITED |
| `include/managers/peer_ota_manager.h` | `.h` | Firmware Public Header | 80 | AUDITED |
| `include/managers/peer_storage_manager.h` | `.h` | Firmware Public Header | 118 | AUDITED |
| `include/managers/plugin_api.h` | `.h` | Firmware Public Header | 878 | AUDITED |
| `include/managers/plugin_icon.h` | `.h` | Firmware Public Header | 20 | AUDITED |
| `include/managers/plugin_installer.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/managers/plugin_loader.h` | `.h` | Firmware Public Header | 45 | AUDITED |
| `include/managers/plugin_manager.h` | `.h` | Firmware Public Header | 142 | AUDITED |
| `include/managers/rgb_effects/rgb_effect_helpers.h` | `.h` | Firmware Public Header | 108 | AUDITED |
| `include/managers/rgb_manager.h` | `.h` | Firmware Public Header | 144 | AUDITED |
| `include/managers/sd_card_manager.h` | `.h` | Firmware Public Header | 175 | AUDITED |
| `include/managers/self_ota_manager.h` | `.h` | Firmware Public Header | 55 | AUDITED |
| `include/managers/settings_manager.h` | `.h` | Firmware Public Header | 749 | AUDITED |
| `include/managers/settings_sd_backup.h` | `.h` | Firmware Public Header | 23 | AUDITED |
| `include/managers/status_anim_utils.h` | `.h` | Firmware Public Header | 65 | AUDITED |
| `include/managers/status_display_animations.h` | `.h` | Firmware Public Header | 25 | AUDITED |
| `include/managers/status_display_manager.h` | `.h` | Firmware Public Header | 20 | AUDITED |
| `include/managers/subghz_decoders.h` | `.h` | Firmware Public Header | 117 | AUDITED |
| `include/managers/subghz_remote_manager.h` | `.h` | Firmware Public Header | 53 | AUDITED |
| `include/managers/usb_keyboard_manager.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/usb_msc_manager.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/managers/views/accelerometer_screen.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/views/airspace_monitor_screen.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/views/app_gallery_screen.h` | `.h` | Firmware Public Header | 14 | AUDITED |
| `include/managers/views/audio_player_screen.h` | `.h` | Firmware Public Header | 12 | AUDITED |
| `include/managers/views/badble_view.h` | `.h` | Firmware Public Header | 8 | AUDITED |
| `include/managers/views/badusb_view.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/managers/views/book_reader_view.h` | `.h` | Firmware Public Header | 16 | AUDITED |
| `include/managers/views/channel_congestion_screen.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/views/clock_screen.h` | `.h` | Firmware Public Header | 14 | AUDITED |
| `include/managers/views/cloud_store_screen.h` | `.h` | Firmware Public Header | 16 | AUDITED |
| `include/managers/views/compass_screen.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/views/crowpanel_audio_view.h` | `.h` | Firmware Public Header | 8 | AUDITED |
| `include/managers/views/crowpanel_p4_camera_view.h` | `.h` | Firmware Public Header | 16 | AUDITED |
| `include/managers/views/enviii_screen.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/views/error_popup.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/managers/views/ethernet_screen.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/managers/views/favorites_manager_screen.h` | `.h` | Firmware Public Header | 20 | AUDITED |
| `include/managers/views/file_browser.h` | `.h` | Firmware Public Header | 107 | AUDITED |
| `include/managers/views/flappy_ghost_screen.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/managers/views/ghostchi_screen.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/views/ghostscript_browser_view.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/views/ghostscript_runner_view.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/managers/views/hop_profile_screen.h` | `.h` | Firmware Public Header | 9 | AUDITED |
| `include/managers/views/infrared_view.h` | `.h` | Firmware Public Header | 30 | AUDITED |
| `include/managers/views/keyboard_screen.h` | `.h` | Firmware Public Header | 20 | AUDITED |
| `include/managers/views/lockscreen.h` | `.h` | Firmware Public Header | 46 | AUDITED |
| `include/managers/views/lora_view.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/views/main_menu_screen.h` | `.h` | Firmware Public Header | 27 | AUDITED |
| `include/managers/views/menu_editor_screen.h` | `.h` | Firmware Public Header | 6 | AUDITED |
| `include/managers/views/music_visualizer.h` | `.h` | Firmware Public Header | 32 | AUDITED |
| `include/managers/views/nfc_view.h` | `.h` | Firmware Public Header | 51 | AUDITED |
| `include/managers/views/nrf24_analyzer_view.h` | `.h` | Firmware Public Header | 15 | AUDITED |
| `include/managers/views/number_pad_screen.h` | `.h` | Firmware Public Header | 24 | AUDITED |
| `include/managers/views/options_screen.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/managers/views/packet_monitor_screen.h` | `.h` | Firmware Public Header | 13 | AUDITED |
| `include/managers/views/plugin_runner_view.h` | `.h` | Firmware Public Header | 28 | AUDITED |
| `include/managers/views/sd_browser_screen.h` | `.h` | Firmware Public Header | 12 | AUDITED |
| `include/managers/views/sentry_view.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/managers/views/settings_screen.h` | `.h` | Firmware Public Header | 14 | AUDITED |
| `include/managers/views/setup_wizard_screen.h` | `.h` | Firmware Public Header | 10 | AUDITED |
| `include/managers/views/splash_screen.h` | `.h` | Firmware Public Header | 47 | AUDITED |
| `include/managers/views/subghz_view.h` | `.h` | Firmware Public Header | 17 | AUDITED |
| `include/managers/views/tdongle_status_screen.h` | `.h` | Firmware Public Header | 15 | AUDITED |
| `include/managers/views/terminal_screen.h` | `.h` | Firmware Public Header | 57 | AUDITED |
| `include/managers/views/trackpad_view.h` | `.h` | Firmware Public Header | 16 | AUDITED |
| `include/managers/views/wardriving_screen.h` | `.h` | Firmware Public Header | 15 | AUDITED |
| `include/managers/wifi_manager.h` | `.h` | Firmware Public Header | 355 | AUDITED |
| `include/managers/wigle_manager.h` | `.h` | Firmware Public Header | 114 | AUDITED |
| `include/managers/zigbee_manager.h` | `.h` | Firmware Public Header | 43 | AUDITED |
| `include/modules/deauth_monitor.h` | `.h` | God Mode Telemetry & Security Module | 77 | AUDITED |
| `include/modules/ghost_eastereggs.h` | `.h` | God Mode Telemetry & Security Module | 65 | AUDITED |
| `include/modules/nrf24_lab.h` | `.h` | God Mode Telemetry & Security Module | 72 | FIXED_V4 |
| `include/modules/profile_manager.h` | `.h` | God Mode Telemetry & Security Module | 65 | AUDITED |
| `include/modules/rogue_detector.h` | `.h` | God Mode Telemetry & Security Module | 87 | AUDITED |
| `include/modules/self_test.h` | `.h` | God Mode Telemetry & Security Module | 39 | AUDITED |
| `include/modules/temp_monitor.h` | `.h` | God Mode Telemetry & Security Module | 92 | FIXED_V4 |
| `include/scans/ble/advertiser_scan.h` | `.h` | Firmware Public Header | 67 | AUDITED |
| `include/scans/ble/airtag_scan.h` | `.h` | Firmware Public Header | 81 | AUDITED |
| `include/scans/ble/device_detect_scan.h` | `.h` | Firmware Public Header | 61 | AUDITED |
| `include/scans/ble/flipper_scan.h` | `.h` | Firmware Public Header | 85 | AUDITED |
| `include/scans/ble/gatt_scan.h` | `.h` | Firmware Public Header | 111 | AUDITED |
| `include/scans/common/scan_types.h` | `.h` | Firmware Public Header | 52 | AUDITED |
| `include/scans/scans.h` | `.h` | Firmware Public Header | 31 | AUDITED |
| `include/scans/wifi/airspace_monitor.h` | `.h` | Firmware Public Header | 101 | AUDITED |
| `include/scans/wifi/ap_scan.h` | `.h` | Firmware Public Header | 192 | AUDITED |
| `include/scans/wifi/arp_scan.h` | `.h` | Firmware Public Header | 77 | AUDITED |
| `include/scans/wifi/enum4linux_scan.h` | `.h` | Firmware Public Header | 67 | AUDITED |
| `include/scans/wifi/govee_scan.h` | `.h` | Firmware Public Header | 34 | AUDITED |
| `include/scans/wifi/hop_profile.h` | `.h` | Firmware Public Header | 46 | AUDITED |
| `include/scans/wifi/http_banner_scan.h` | `.h` | Firmware Public Header | 44 | AUDITED |
| `include/scans/wifi/local_scan.h` | `.h` | Firmware Public Header | 70 | AUDITED |
| `include/scans/wifi/name_sniff.h` | `.h` | Firmware Public Header | 34 | AUDITED |
| `include/scans/wifi/netbios_scan.h` | `.h` | Firmware Public Header | 46 | AUDITED |
| `include/scans/wifi/port_scan.h` | `.h` | Firmware Public Header | 176 | AUDITED |
| `include/scans/wifi/reconnect_pmf_note.h` | `.h` | Firmware Public Header | 34 | AUDITED |
| `include/scans/wifi/snmp_scan.h` | `.h` | Firmware Public Header | 88 | AUDITED |
| `include/scans/wifi/ssh_scan.h` | `.h` | Firmware Public Header | 55 | AUDITED |
| `include/scans/wifi/station_scan.h` | `.h` | Firmware Public Header | 140 | AUDITED |
| `include/scans/wifi/wardrive_observation.h` | `.h` | Firmware Public Header | 78 | AUDITED |
| `include/scans/wifi/wardrive_policy.h` | `.h` | Firmware Public Header | 26 | AUDITED |
| `include/scans/wifi/wardrive_scan.h` | `.h` | Firmware Public Header | 20 | AUDITED |
| `include/scans/wifi/wifi_channels.h` | `.h` | Firmware Public Header | 134 | AUDITED |
| `include/scans/wifi/wpa3_compliance.h` | `.h` | Firmware Public Header | 58 | AUDITED |
| `include/suppress_noisy_logs.h` | `.h` | Firmware Public Header | 11 | AUDITED |
| `include/vendor/GPS/MicroNMEA.h` | `.h` | Firmware Public Header | 323 | AUDITED |
| `include/vendor/GPS/gps_logger.h` | `.h` | Firmware Public Header | 83 | AUDITED |
| `include/vendor/GPS/minmea.h` | `.h` | Firmware Public Header | 320 | AUDITED |
| `include/vendor/GPS/minmea_soft.h` | `.h` | Firmware Public Header | 40 | AUDITED |
| `include/vendor/dial_client.h` | `.h` | Firmware Public Header | 46 | AUDITED |
| `include/vendor/drivers/CH422G.h` | `.h` | Firmware Public Header | 113 | AUDITED |
| `include/vendor/drivers/ST7262.h` | `.h` | Firmware Public Header | 100 | AUDITED |
| `include/vendor/drivers/aw9523.h` | `.h` | Firmware Public Header | 36 | AUDITED |
| `include/vendor/drivers/axp2101.h` | `.h` | Firmware Public Header | 66 | AUDITED |
| `include/vendor/drivers/axs15231b.h` | `.h` | Firmware Public Header | 31 | AUDITED |
| `include/vendor/drivers/crowpanel_epaper.h` | `.h` | Firmware Public Header | 38 | AUDITED |
| `include/vendor/drivers/crowpanel_p4_display.h` | `.h` | Firmware Public Header | 25 | AUDITED |
| `include/vendor/drivers/pcf8563.h` | `.h` | Firmware Public Header | 101 | AUDITED |
| `include/vendor/keyboard_handler.h` | `.h` | Firmware Public Header | 150 | AUDITED |
| `include/vendor/led/led_strip.h` | `.h` | Firmware Public Header | 127 | AUDITED |
| `include/vendor/led/led_strip_interface.h` | `.h` | Firmware Public Header | 105 | AUDITED |
| `include/vendor/led/led_strip_rmt.h` | `.h` | Firmware Public Header | 61 | AUDITED |
| `include/vendor/led/led_strip_spi.h` | `.h` | Firmware Public Header | 53 | AUDITED |
| `include/vendor/led/led_strip_types.h` | `.h` | Firmware Public Header | 56 | AUDITED |
| `include/vendor/m5/m5_keyboard_def.h` | `.h` | Firmware Public Header | 151 | AUDITED |
| `include/vendor/m5/m5gfx_wrapper.h` | `.h` | Firmware Public Header | 21 | AUDITED |
| `include/vendor/pcap.h` | `.h` | Firmware Public Header | 109 | AUDITED |
| `include/vendor/printer.h` | `.h` | Firmware Public Header | 14 | AUDITED |
| `include/vendor/radiotap.h` | `.h` | Firmware Public Header | 105 | AUDITED |
| `include/vendor/wifi_l2.h` | `.h` | Firmware Public Header | 118 | AUDITED |
| `main/CMakeLists.txt` | `.txt` | Build / Manifest / Documentation | 390 | AUDITED |
| `main/Kconfig.projbuild` | `.projbuild` | Build / Manifest / Documentation | 2346 | AUDITED |
| `main/boot_banner_text.h` | `.h` | Build / Manifest / Documentation | 79 | AUDITED |
| `main/core/ca_fingerprints.c` | `.c` | Core System / Dispatcher / Crypto | 75 | AUDITED |
| `main/core/ca_known_certs.c` | `.c` | Core System / Dispatcher / Crypto | 95 | AUDITED |
| `main/core/callbacks.c` | `.c` | Core System / Dispatcher / Crypto | 4307 | FIXED_V4 |
| `main/core/cert_check.c` | `.c` | Core System / Dispatcher / Crypto | 177 | AUDITED |
| `main/core/chip_info.c` | `.c` | Core System / Dispatcher / Crypto | 400 | AUDITED |
| `main/core/commandline.c` | `.c` | Core System / Dispatcher / Crypto | 649 | AUDITED |
| `main/core/commands/cmd_aerial.c` | `.c` | CLI Command Handler | 268 | AUDITED |
| `main/core/commands/cmd_apcred.c` | `.c` | CLI Command Handler | 112 | AUDITED |
| `main/core/commands/cmd_audio.c` | `.c` | CLI Command Handler | 82 | AUDITED |
| `main/core/commands/cmd_badble.c` | `.c` | CLI Command Handler | 165 | AUDITED |
| `main/core/commands/cmd_badusb.c` | `.c` | CLI Command Handler | 341 | FIXED_V4 |
| `main/core/commands/cmd_ble.c` | `.c` | CLI Command Handler | 782 | AUDITED |
| `main/core/commands/cmd_camera.c` | `.c` | CLI Command Handler | 181 | AUDITED |
| `main/core/commands/cmd_capture.c` | `.c` | CLI Command Handler | 524 | AUDITED |
| `main/core/commands/cmd_comm.c` | `.c` | CLI Command Handler | 239 | AUDITED |
| `main/core/commands/cmd_diagnostics.c` | `.c` | CLI Command Handler | 714 | AUDITED |
| `main/core/commands/cmd_ethernet.c` | `.c` | CLI Command Handler | 1898 | AUDITED |
| `main/core/commands/cmd_gadgets.c` | `.c` | CLI Command Handler | 168 | AUDITED |
| `main/core/commands/cmd_godmode.c` | `.c` | CLI Command Handler | 136 | FIXED_V4 |
| `main/core/commands/cmd_gps.c` | `.c` | CLI Command Handler | 205 | AUDITED |
| `main/core/commands/cmd_help.c` | `.c` | CLI Command Handler | 949 | FIXED_V4 |
| `main/core/commands/cmd_input.c` | `.c` | CLI Command Handler | 99 | AUDITED |
| `main/core/commands/cmd_ir.c` | `.c` | CLI Command Handler | 916 | AUDITED |
| `main/core/commands/cmd_karma.c` | `.c` | CLI Command Handler | 47 | AUDITED |
| `main/core/commands/cmd_lora.c` | `.c` | CLI Command Handler | 629 | AUDITED |
| `main/core/commands/cmd_mesh.c` | `.c` | CLI Command Handler | 343 | AUDITED |
| `main/core/commands/cmd_meshcore.c` | `.c` | CLI Command Handler | 289 | AUDITED |
| `main/core/commands/cmd_netscan.c` | `.c` | CLI Command Handler | 532 | AUDITED |
| `main/core/commands/cmd_nrf24.c` | `.c` | CLI Command Handler | 139 | AUDITED |
| `main/core/commands/cmd_ota.c` | `.c` | CLI Command Handler | 26 | AUDITED |
| `main/core/commands/cmd_p4_slave_ota.c` | `.c` | CLI Command Handler | 31 | AUDITED |
| `main/core/commands/cmd_portal.c` | `.c` | CLI Command Handler | 1265 | AUDITED |
| `main/core/commands/cmd_rgb.c` | `.c` | CLI Command Handler | 329 | AUDITED |
| `main/core/commands/cmd_scan.c` | `.c` | CLI Command Handler | 490 | AUDITED |
| `main/core/commands/cmd_script.c` | `.c` | CLI Command Handler | 320 | AUDITED |
| `main/core/commands/cmd_sd.c` | `.c` | CLI Command Handler | 802 | AUDITED |
| `main/core/commands/cmd_settings.c` | `.c` | CLI Command Handler | 832 | AUDITED |
| `main/core/commands/cmd_shell.c` | `.c` | CLI Command Handler | 895 | AUDITED |
| `main/core/commands/cmd_subghz.c` | `.c` | CLI Command Handler | 294 | AUDITED |
| `main/core/commands/cmd_system.c` | `.c` | CLI Command Handler | 911 | AUDITED |
| `main/core/commands/cmd_usb_msc.c` | `.c` | CLI Command Handler | 48 | AUDITED |
| `main/core/commands/cmd_wdstream.c` | `.c` | CLI Command Handler | 627 | AUDITED |
| `main/core/commands/cmd_wifi.c` | `.c` | CLI Command Handler | 692 | FIXED_V4 |
| `main/core/commands/cmd_wigle.c` | `.c` | CLI Command Handler | 137 | AUDITED |
| `main/core/dns_server.c` | `.c` | Core System / Dispatcher / Crypto | 1902 | AUDITED |
| `main/core/esp_comm_manager.c` | `.c` | Core System / Dispatcher / Crypto | 2267 | AUDITED |
| `main/core/ghostchi_identity.c` | `.c` | Core System / Dispatcher / Crypto | 42 | AUDITED |
| `main/core/ghostlink_bench.c` | `.c` | Core System / Dispatcher / Crypto | 474 | AUDITED |
| `main/core/glog.c` | `.c` | Core System / Dispatcher / Crypto | 193 | FIXED_V4 |
| `main/core/memory_debug.c` | `.c` | Core System / Dispatcher / Crypto | 338 | AUDITED |
| `main/core/network_constants.c` | `.c` | Core System / Dispatcher / Crypto | 416 | AUDITED |
| `main/core/ouis.bin` | `.bin` | Core System / Dispatcher / Crypto | 99 | AUDITED |
| `main/core/ouis.c` | `.c` | Core System / Dispatcher / Crypto | 197 | AUDITED |
| `main/core/ouis.json` | `.json` | Core System / Dispatcher / Crypto | 10123 | AUDITED |
| `main/core/scan_saver.c` | `.c` | Core System / Dispatcher / Crypto | 216 | AUDITED |
| `main/core/screen_mirror.c` | `.c` | Core System / Dispatcher / Crypto | 474 | AUDITED |
| `main/core/serial_manager.c` | `.c` | Core System / Dispatcher / Crypto | 1429 | AUDITED |
| `main/core/sinkhole_https.c` | `.c` | Core System / Dispatcher / Crypto | 1349 | AUDITED |
| `main/core/system_manager.c` | `.c` | Core System / Dispatcher / Crypto | 152 | AUDITED |
| `main/core/uart_share.c` | `.c` | Core System / Dispatcher / Crypto | 159 | AUDITED |
| `main/core/universal_ir.c` | `.c` | Core System / Dispatcher / Crypto | 239 | AUDITED |
| `main/core/utils.c` | `.c` | Core System / Dispatcher / Crypto | 780 | AUDITED |
| `main/core/wpa_crypto.c` | `.c` | Core System / Dispatcher / Crypto | 488 | FIXED_V4 |
| `main/features/ble/ble_spam.c` | `.c` | Security Research / Active Test Feature | 1083 | AUDITED |
| `main/features/ethernet/eth_arp_poison.c` | `.c` | Security Research / Active Test Feature | 979 | AUDITED |
| `main/features/wifi/auth_flood.c` | `.c` | Security Research / Active Test Feature | 302 | AUDITED |
| `main/features/wifi/bad_msg.c` | `.c` | Security Research / Active Test Feature | 318 | AUDITED |
| `main/features/wifi/beacon_spam.c` | `.c` | Security Research / Active Test Feature | 657 | FIXED_V4 |
| `main/features/wifi/channel_switch_test.c` | `.c` | Security Research / Active Test Feature | 253 | AUDITED |
| `main/features/wifi/dhcp_starvation.c` | `.c` | Security Research / Active Test Feature | 190 | AUDITED |
| `main/features/wifi/eapol_logoff.c` | `.c` | Security Research / Active Test Feature | 247 | AUDITED |
| `main/features/wifi/gtk_abuse.c` | `.c` | Security Research / Active Test Feature | 615 | AUDITED |
| `main/features/wifi/gtk_abuse_stub.c` | `.c` | Security Research / Active Test Feature | 21 | AUDITED |
| `main/features/wifi/probe_request_flood.c` | `.c` | Security Research / Active Test Feature | 255 | AUDITED |
| `main/features/wifi/reconnect_test.c` | `.c` | Security Research / Active Test Feature | 916 | AUDITED |
| `main/features/wifi/sae_flood.c` | `.c` | Security Research / Active Test Feature | 948 | AUDITED |
| `main/gui/accessibility_fonts.c` | `.c` | LVGL Embedded Display UI Component | 193 | AUDITED |
| `main/gui/asset_pack.c` | `.c` | LVGL Embedded Display UI Component | 1844 | AUDITED |
| `main/gui/detail_view.c` | `.c` | LVGL Embedded Display UI Component | 1379 | AUDITED |
| `main/gui/gui_anim.c` | `.c` | LVGL Embedded Display UI Component | 329 | AUDITED |
| `main/gui/gui_router.c` | `.c` | LVGL Embedded Display UI Component | 185 | AUDITED |
| `main/gui/ios_toggle.c` | `.c` | LVGL Embedded Display UI Component | 182 | AUDITED |
| `main/gui/large_builtin_icons.c` | `.c` | LVGL Embedded Display UI Component | 165 | AUDITED |
| `main/gui/live_chart.c` | `.c` | LVGL Embedded Display UI Component | 952 | AUDITED |
| `main/gui/lv_draw_ppa_v8.c` | `.c` | LVGL Embedded Display UI Component | 299 | AUDITED |
| `main/gui/lvgl_safe.c` | `.c` | LVGL Embedded Display UI Component | 23 | AUDITED |
| `main/gui/main_menu_layout.c` | `.c` | LVGL Embedded Display UI Component | 274 | AUDITED |
| `main/gui/menu_catalog.c` | `.c` | LVGL Embedded Display UI Component | 341 | AUDITED |
| `main/gui/menu_config.c` | `.c` | LVGL Embedded Display UI Component | 80 | AUDITED |
| `main/gui/menu_item_style.c` | `.c` | LVGL Embedded Display UI Component | 192 | AUDITED |
| `main/gui/nav_history.c` | `.c` | LVGL Embedded Display UI Component | 47 | AUDITED |
| `main/gui/options_view.c` | `.c` | LVGL Embedded Display UI Component | 552 | AUDITED |
| `main/gui/paged_menu.c` | `.c` | LVGL Embedded Display UI Component | 236 | AUDITED |
| `main/gui/popup.c` | `.c` | LVGL Embedded Display UI Component | 865 | AUDITED |
| `main/gui/progress_bar_view.c` | `.c` | LVGL Embedded Display UI Component | 307 | AUDITED |
| `main/gui/rssi_meter.c` | `.c` | LVGL Embedded Display UI Component | 393 | AUDITED |
| `main/gui/scan_status.c` | `.c` | LVGL Embedded Display UI Component | 231 | AUDITED |
| `main/gui/screen_layout.c` | `.c` | LVGL Embedded Display UI Component | 456 | AUDITED |
| `main/gui/select_overlay.c` | `.c` | LVGL Embedded Display UI Component | 306 | AUDITED |
| `main/gui/theme_palette.c` | `.c` | LVGL Embedded Display UI Component | 305 | AUDITED |
| `main/gui/toast.c` | `.c` | LVGL Embedded Display UI Component | 381 | AUDITED |
| `main/gui/touch_bar.c` | `.c` | LVGL Embedded Display UI Component | 197 | AUDITED |
| `main/gui/view_input.c` | `.c` | LVGL Embedded Display UI Component | 54 | AUDITED |
| `main/hardware/board_config.c` | `.c` | 7Semi ESP32-S3 Board HAL & Pin Authority | 755 | FIXED_V4 |
| `main/idf_component.yml` | `.yml` | Build / Manifest / Documentation | 50 | AUDITED |
| `main/main.c` | `.c` | Build / Manifest / Documentation | 1460 | FIXED_V4 |
| `main/managers/aerial_detector_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1366 | AUDITED |
| `main/managers/ap_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2284 | FIXED_V4 |
| `main/managers/audio_i2s_output.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 590 | AUDITED |
| `main/managers/audio_i2s_output.h` | `.h` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 82 | AUDITED |
| `main/managers/audio_receiver_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 660 | AUDITED |
| `main/managers/audio_receiver_manager.h` | `.h` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 92 | AUDITED |
| `main/managers/audio_stream_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1678 | AUDITED |
| `main/managers/audio_stream_manager.h` | `.h` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 179 | AUDITED |
| `main/managers/auth_digest.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 141 | AUDITED |
| `main/managers/badble_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 964 | AUDITED |
| `main/managers/badusb_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1049 | FIXED_V4 |
| `main/managers/ble_bridge_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1295 | AUDITED |
| `main/managers/ble_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1579 | FIXED_V4 |
| `main/managers/bmi270_driver.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 232 | AUDITED |
| `main/managers/camera_stream_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 425 | AUDITED |
| `main/managers/chameleon_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 3624 | AUDITED |
| `main/managers/cloud_store_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1598 | AUDITED |
| `main/managers/config_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 191 | AUDITED |
| `main/managers/crash_reporter.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 150 | AUDITED |
| `main/managers/dial_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1472 | AUDITED |
| `main/managers/display_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 6394 | AUDITED |
| `main/managers/encoder_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 255 | AUDITED |
| `main/managers/espnow_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 420 | AUDITED |
| `main/managers/espnow_manager_stub.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 53 | AUDITED |
| `main/managers/ethernet/eth_comm_handler.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 467 | AUDITED |
| `main/managers/ethernet/eth_fingerprint.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 560 | AUDITED |
| `main/managers/ethernet/eth_http.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 174 | AUDITED |
| `main/managers/ethernet/eth_scan_async.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 550 | AUDITED |
| `main/managers/ethernet/eth_utils.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 71 | AUDITED |
| `main/managers/ethernet_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 726 | AUDITED |
| `main/managers/flock_detector_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 739 | AUDITED |
| `main/managers/fuel_gauge_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1027 | AUDITED |
| `main/managers/ghost_raw_radio.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 212 | AUDITED |
| `main/managers/ghostchi_activity.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 48 | AUDITED |
| `main/managers/ghostchi_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1260 | AUDITED |
| `main/managers/ghostchi_mood.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 245 | AUDITED |
| `main/managers/ghostscript_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 460 | AUDITED |
| `main/managers/ghostscript_runtime.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 3042 | AUDITED |
| `main/managers/ghostscript_runtime_stub.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 41 | AUDITED |
| `main/managers/gps_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1565 | AUDITED |
| `main/managers/haptic_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 377 | AUDITED |
| `main/managers/hid_script_parser.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 661 | FIXED_V4 |
| `main/managers/http_proxy.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 68 | AUDITED |
| `main/managers/infrared_common.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 133 | AUDITED |
| `main/managers/infrared_decoder.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1200 | AUDITED |
| `main/managers/infrared_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1193 | FIXED_V4 |
| `main/managers/infrared_protocol_encoders.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 227 | AUDITED |
| `main/managers/infrared_protocols.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 357 | AUDITED |
| `main/managers/infrared_rx_gpio.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 429 | AUDITED |
| `main/managers/ip5306_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 187 | AUDITED |
| `main/managers/joystick_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 170 | AUDITED |
| `main/managers/lora_ble.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 886 | AUDITED |
| `main/managers/lora_channels.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 372 | AUDITED |
| `main/managers/lora_crypto.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 128 | AUDITED |
| `main/managers/lora_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2166 | AUDITED |
| `main/managers/lora_mesh.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 3204 | AUDITED |
| `main/managers/lora_modem.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 313 | AUDITED |
| `main/managers/lora_pb.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1062 | AUDITED |
| `main/managers/lora_phoneapi.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2520 | AUDITED |
| `main/managers/lora_pki.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 514 | AUDITED |
| `main/managers/lora_store.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 93 | AUDITED |
| `main/managers/lora_sx1262.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 951 | AUDITED |
| `main/managers/lora_unishox.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 530 | AUDITED |
| `main/managers/lora_unishox.h` | `.h` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 35 | AUDITED |
| `main/managers/lora_x25519_donna.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 860 | AUDITED |
| `main/managers/m5_audio_codec.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 429 | AUDITED |
| `main/managers/meshcore_ble.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 374 | AUDITED |
| `main/managers/meshcore_companion.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1192 | AUDITED |
| `main/managers/meshcore_crypto.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 125 | AUDITED |
| `main/managers/meshcore_crypto_port.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 111 | AUDITED |
| `main/managers/meshcore_identity.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 163 | AUDITED |
| `main/managers/meshcore_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 690 | AUDITED |
| `main/managers/meshcore_mesh.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1431 | AUDITED |
| `main/managers/meshcore_packet.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 111 | AUDITED |
| `main/managers/meshcore_store.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 173 | AUDITED |
| `main/managers/microphone/mic_driver.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 334 | AUDITED |
| `main/managers/microphone/mic_goertzel.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 426 | AUDITED |
| `main/managers/microphone/mic_visualizer.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 290 | AUDITED |
| `main/managers/motion_detector_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 719 | AUDITED |
| `main/managers/nfc/desfire.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1032 | AUDITED |
| `main/managers/nfc/emv.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 910 | AUDITED |
| `main/managers/nfc/flipper_nfc_compat.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 685 | AUDITED |
| `main/managers/nfc/flipper_parsers/aime.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 163 | AUDITED |
| `main/managers/nfc/flipper_parsers/all_in_one.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 107 | AUDITED |
| `main/managers/nfc/flipper_parsers/bip.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 340 | AUDITED |
| `main/managers/nfc/flipper_parsers/charliecard.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1269 | AUDITED |
| `main/managers/nfc/flipper_parsers/clipper.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 585 | AUDITED |
| `main/managers/nfc/flipper_parsers/csc.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 142 | AUDITED |
| `main/managers/nfc/flipper_parsers/disney_infinity.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 122 | AUDITED |
| `main/managers/nfc/flipper_parsers/emv_card.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 280 | AUDITED |
| `main/managers/nfc/flipper_parsers/gallagher.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 98 | AUDITED |
| `main/managers/nfc/flipper_parsers/gallagher_util.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 60 | AUDITED |
| `main/managers/nfc/flipper_parsers/hi.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 228 | AUDITED |
| `main/managers/nfc/flipper_parsers/hid.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 147 | AUDITED |
| `main/managers/nfc/flipper_parsers/hworld.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 240 | AUDITED |
| `main/managers/nfc/flipper_parsers/itso.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 122 | AUDITED |
| `main/managers/nfc/flipper_parsers/kazan.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 407 | AUDITED |
| `main/managers/nfc/flipper_parsers/metromoney.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 185 | AUDITED |
| `main/managers/nfc/flipper_parsers/microel.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 227 | AUDITED |
| `main/managers/nfc/flipper_parsers/mizip.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 259 | AUDITED |
| `main/managers/nfc/flipper_parsers/myki.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 126 | AUDITED |
| `main/managers/nfc/flipper_parsers/opal.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 211 | AUDITED |
| `main/managers/nfc/flipper_parsers/plantain.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 399 | AUDITED |
| `main/managers/nfc/flipper_parsers/saflok.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 453 | AUDITED |
| `main/managers/nfc/flipper_parsers/skylanders.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 909 | AUDITED |
| `main/managers/nfc/flipper_parsers/smartrider.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 335 | AUDITED |
| `main/managers/nfc/flipper_parsers/social_moscow.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 311 | AUDITED |
| `main/managers/nfc/flipper_parsers/sonicare.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 111 | AUDITED |
| `main/managers/nfc/flipper_parsers/troika.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 216 | AUDITED |
| `main/managers/nfc/flipper_parsers/trt.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 97 | AUDITED |
| `main/managers/nfc/flipper_parsers/two_cities.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 185 | AUDITED |
| `main/managers/nfc/flipper_parsers/umarsh.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 149 | AUDITED |
| `main/managers/nfc/flipper_parsers/ventra.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 292 | AUDITED |
| `main/managers/nfc/flipper_parsers/washcity.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 190 | AUDITED |
| `main/managers/nfc/flipper_parsers/zolotaya_korona.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 208 | AUDITED |
| `main/managers/nfc/flipper_parsers/zolotaya_korona_online.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 139 | AUDITED |
| `main/managers/nfc/loclass/optimized_cipher.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 215 | AUDITED |
| `main/managers/nfc/loclass/optimized_cipherutils.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 95 | AUDITED |
| `main/managers/nfc/loclass/optimized_elite.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 142 | AUDITED |
| `main/managers/nfc/loclass/optimized_ikeys.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 175 | AUDITED |
| `main/managers/nfc/loclass/standalone_des.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 219 | AUDITED |
| `main/managers/nfc/mifare_classic.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2918 | AUDITED |
| `main/managers/nfc/ndef.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 668 | AUDITED |
| `main/managers/nfc/ndef_builder.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 231 | AUDITED |
| `main/managers/nfc/ndef_tag_gen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 161 | AUDITED |
| `main/managers/nfc/nfc_backend.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 82 | AUDITED |
| `main/managers/nfc/nfc_cli.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1639 | AUDITED |
| `main/managers/nfc/ntag_t2.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 407 | AUDITED |
| `main/managers/nfc/picopass.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 476 | AUDITED |
| `main/managers/nfc/write_ntag.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 180 | AUDITED |
| `main/managers/nrf24_remote_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 461 | AUDITED |
| `main/managers/ota_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1243 | AUDITED |
| `main/managers/p4_slave_ota_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 164 | AUDITED |
| `main/managers/peer_ota_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 891 | AUDITED |
| `main/managers/peer_storage_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 760 | AUDITED |
| `main/managers/plugin_api.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2962 | AUDITED |
| `main/managers/plugin_api_canvas.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 912 | AUDITED |
| `main/managers/plugin_api_hw.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 121 | AUDITED |
| `main/managers/plugin_api_internal.h` | `.h` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 127 | AUDITED |
| `main/managers/plugin_api_lowlevel.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1787 | AUDITED |
| `main/managers/plugin_api_ui.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 602 | AUDITED |
| `main/managers/plugin_api_widgets.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 782 | AUDITED |
| `main/managers/plugin_icon.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 135 | AUDITED |
| `main/managers/plugin_installer.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 661 | AUDITED |
| `main/managers/plugin_loader.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 350 | AUDITED |
| `main/managers/plugin_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 859 | AUDITED |
| `main/managers/rgb_effects/rgb_effect_helpers.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 151 | AUDITED |
| `main/managers/rgb_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2336 | AUDITED |
| `main/managers/sd_card_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2315 | FIXED_V4 |
| `main/managers/self_ota_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 482 | AUDITED |
| `main/managers/settings_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2972 | FIXED_V4 |
| `main/managers/settings_sd_backup.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 577 | AUDITED |
| `main/managers/status_display_animations/anim_bouncing_text.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 102 | AUDITED |
| `main/managers/status_display_animations/anim_falling_leaves.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 195 | AUDITED |
| `main/managers/status_display_animations/anim_flying_toaster.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 108 | AUDITED |
| `main/managers/status_display_animations/anim_game_of_life.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 134 | AUDITED |
| `main/managers/status_display_animations/anim_ghost.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 71 | AUDITED |
| `main/managers/status_display_animations/anim_hud.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 218 | AUDITED |
| `main/managers/status_display_animations/anim_matrix.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 147 | AUDITED |
| `main/managers/status_display_animations/anim_spiral.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 73 | AUDITED |
| `main/managers/status_display_animations/anim_starfield.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 143 | AUDITED |
| `main/managers/status_display_animations/status_anim_utils.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 193 | AUDITED |
| `main/managers/status_display_animations/status_display_animations.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 90 | AUDITED |
| `main/managers/status_display_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1212 | AUDITED |
| `main/managers/subghz_decoders.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 3207 | AUDITED |
| `main/managers/subghz_remote_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2352 | AUDITED |
| `main/managers/usb_keyboard_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 542 | FIXED_V4 |
| `main/managers/usb_msc_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 284 | AUDITED |
| `main/managers/views/accelerometer_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 720 | AUDITED |
| `main/managers/views/airspace_monitor_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 529 | AUDITED |
| `main/managers/views/app_gallery_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2083 | AUDITED |
| `main/managers/views/audio_player_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1498 | AUDITED |
| `main/managers/views/badble_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 750 | AUDITED |
| `main/managers/views/badusb_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1272 | AUDITED |
| `main/managers/views/book_reader_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1963 | AUDITED |
| `main/managers/views/channel_congestion_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 301 | AUDITED |
| `main/managers/views/clock_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 783 | AUDITED |
| `main/managers/views/cloud_store_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1003 | AUDITED |
| `main/managers/views/compass_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 732 | AUDITED |
| `main/managers/views/crowpanel_audio_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 361 | AUDITED |
| `main/managers/views/crowpanel_p4_camera_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1073 | AUDITED |
| `main/managers/views/enviii_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1074 | AUDITED |
| `main/managers/views/error_popup.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 278 | AUDITED |
| `main/managers/views/ethernet_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1795 | AUDITED |
| `main/managers/views/favorites_manager_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 718 | AUDITED |
| `main/managers/views/flappy_ghost.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 617 | AUDITED |
| `main/managers/views/ghostchi_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1552 | AUDITED |
| `main/managers/views/ghostscript_browser_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 275 | AUDITED |
| `main/managers/views/ghostscript_runner_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 728 | AUDITED |
| `main/managers/views/hop_profile_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 312 | AUDITED |
| `main/managers/views/infrared_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 3697 | AUDITED |
| `main/managers/views/keyboard_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2201 | AUDITED |
| `main/managers/views/lockscreen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2338 | AUDITED |
| `main/managers/views/lora_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 2204 | AUDITED |
| `main/managers/views/main_menu_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1744 | AUDITED |
| `main/managers/views/menu_editor_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 269 | AUDITED |
| `main/managers/views/music_visualizer.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 443 | AUDITED |
| `main/managers/views/nfc_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 6131 | AUDITED |
| `main/managers/views/nrf24_analyzer_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1535 | AUDITED |
| `main/managers/views/number_pad_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 467 | AUDITED |
| `main/managers/views/options_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 16067 | AUDITED |
| `main/managers/views/packet_monitor_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 637 | AUDITED |
| `main/managers/views/plugin_runner_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 804 | AUDITED |
| `main/managers/views/sd_browser_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1077 | AUDITED |
| `main/managers/views/sentry_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1618 | AUDITED |
| `main/managers/views/setup_wizard_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1561 | AUDITED |
| `main/managers/views/splash_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 370 | AUDITED |
| `main/managers/views/subghz_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 4715 | AUDITED |
| `main/managers/views/tdongle_status_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 178 | AUDITED |
| `main/managers/views/terminal_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1334 | AUDITED |
| `main/managers/views/trackpad_view.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 546 | AUDITED |
| `main/managers/views/wardriving_screen.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1219 | AUDITED |
| `main/managers/wifi_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 5341 | FIXED_V4 |
| `main/managers/wigle_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 1596 | AUDITED |
| `main/managers/zigbee_manager.c` | `.c` | Subsystem Manager (WiFi/BLE/USB/SD/IR/UI) | 275 | AUDITED |
| `main/mf_classic_dict.nfc` | `.nfc` | Build / Manifest / Documentation | 3217 | AUDITED |
| `main/modules/deauth_monitor.c` | `.c` | God Mode Telemetry & Security Module | 230 | FIXED_V4 |
| `main/modules/ghost_eastereggs.c` | `.c` | God Mode Telemetry & Security Module | 217 | FIXED_V4 |
| `main/modules/nrf24_lab.c` | `.c` | God Mode Telemetry & Security Module | 327 | FIXED_V4 |
| `main/modules/profile_manager.c` | `.c` | God Mode Telemetry & Security Module | 170 | AUDITED |
| `main/modules/rogue_detector.c` | `.c` | God Mode Telemetry & Security Module | 294 | FIXED_V4 |
| `main/modules/self_test.c` | `.c` | God Mode Telemetry & Security Module | 121 | FIXED_V4 |
| `main/modules/temp_monitor.c` | `.c` | God Mode Telemetry & Security Module | 359 | FIXED_V4 |
| `main/scans/ble/advertiser_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 992 | AUDITED |
| `main/scans/ble/airtag_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 559 | FIXED_V4 |
| `main/scans/ble/device_detect_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 975 | AUDITED |
| `main/scans/ble/flipper_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 502 | AUDITED |
| `main/scans/ble/gatt_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1302 | AUDITED |
| `main/scans/device_detect_scan_stub.c` | `.c` | Wireless/Network Reconnaissance Scanner | 155 | AUDITED |
| `main/scans/wifi/airspace_monitor.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1571 | AUDITED |
| `main/scans/wifi/ap_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1183 | FIXED_V4 |
| `main/scans/wifi/arp_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1034 | AUDITED |
| `main/scans/wifi/enum4linux_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1317 | FIXED_V4 |
| `main/scans/wifi/govee_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 216 | AUDITED |
| `main/scans/wifi/hop_profile.c` | `.c` | Wireless/Network Reconnaissance Scanner | 138 | AUDITED |
| `main/scans/wifi/http_banner_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 289 | AUDITED |
| `main/scans/wifi/local_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 777 | AUDITED |
| `main/scans/wifi/name_sniff.c` | `.c` | Wireless/Network Reconnaissance Scanner | 409 | AUDITED |
| `main/scans/wifi/netbios_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 401 | AUDITED |
| `main/scans/wifi/port_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 851 | AUDITED |
| `main/scans/wifi/reconnect_pmf_note.c` | `.c` | Wireless/Network Reconnaissance Scanner | 53 | AUDITED |
| `main/scans/wifi/snmp_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1108 | AUDITED |
| `main/scans/wifi/ssh_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 234 | FIXED_V4 |
| `main/scans/wifi/station_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 1006 | AUDITED |
| `main/scans/wifi/wardrive_policy.c` | `.c` | Wireless/Network Reconnaissance Scanner | 109 | AUDITED |
| `main/scans/wifi/wardrive_scan.c` | `.c` | Wireless/Network Reconnaissance Scanner | 191 | AUDITED |
| `main/scans/wifi/wifi_channels.c` | `.c` | Wireless/Network Reconnaissance Scanner | 388 | AUDITED |
| `main/scans/wifi/wpa3_compliance.c` | `.c` | Wireless/Network Reconnaissance Scanner | 289 | AUDITED |
| `main/vendor/GPS/MicroNMEA.c` | `.c` | Embedded Protocol / Driver Helper | 1203 | AUDITED |
| `main/vendor/GPS/gps_logger.c` | `.c` | Embedded Protocol / Driver Helper | 1396 | AUDITED |
| `main/vendor/GPS/minmea.c` | `.c` | Embedded Protocol / Driver Helper | 685 | AUDITED |
| `main/vendor/GPS/minmea_soft.c` | `.c` | Embedded Protocol / Driver Helper | 1068 | AUDITED |
| `main/vendor/crowpanel_p4_display.c` | `.c` | Embedded Protocol / Driver Helper | 415 | AUDITED |
| `main/vendor/dial_client.c` | `.c` | Embedded Protocol / Driver Helper | 134 | AUDITED |
| `main/vendor/drivers/CH422G.c` | `.c` | Embedded Protocol / Driver Helper | 169 | AUDITED |
| `main/vendor/drivers/ST7262.c` | `.c` | Embedded Protocol / Driver Helper | 761 | AUDITED |
| `main/vendor/drivers/apx2102.c` | `.c` | Embedded Protocol / Driver Helper | 210 | AUDITED |
| `main/vendor/drivers/aw9523.c` | `.c` | Embedded Protocol / Driver Helper | 181 | AUDITED |
| `main/vendor/drivers/axs15231b.c` | `.c` | Embedded Protocol / Driver Helper | 41 | AUDITED |
| `main/vendor/drivers/crowpanel_epaper.c` | `.c` | Embedded Protocol / Driver Helper | 926 | AUDITED |
| `main/vendor/drivers/pcf8563.c` | `.c` | Embedded Protocol / Driver Helper | 344 | AUDITED |
| `main/vendor/ed25519/add_scalar.c` | `.c` | Embedded Protocol / Driver Helper | 69 | AUDITED |
| `main/vendor/ed25519/ed_25519.h` | `.h` | Embedded Protocol / Driver Helper | 41 | AUDITED |
| `main/vendor/ed25519/fe.c` | `.c` | Embedded Protocol / Driver Helper | 1491 | AUDITED |
| `main/vendor/ed25519/fe.h` | `.h` | Embedded Protocol / Driver Helper | 41 | AUDITED |
| `main/vendor/ed25519/fixedint.h` | `.h` | Embedded Protocol / Driver Helper | 72 | AUDITED |
| `main/vendor/ed25519/ge.c` | `.c` | Embedded Protocol / Driver Helper | 467 | AUDITED |
| `main/vendor/ed25519/ge.h` | `.h` | Embedded Protocol / Driver Helper | 74 | AUDITED |
| `main/vendor/ed25519/key_exchange.c` | `.c` | Embedded Protocol / Driver Helper | 79 | AUDITED |
| `main/vendor/ed25519/keypair.c` | `.c` | Embedded Protocol / Driver Helper | 23 | AUDITED |
| `main/vendor/ed25519/license.txt` | `.txt` | Embedded Protocol / Driver Helper | 16 | AUDITED |
| `main/vendor/ed25519/precomp_data.h` | `.h` | Embedded Protocol / Driver Helper | 1391 | AUDITED |
| `main/vendor/ed25519/sc.c` | `.c` | Embedded Protocol / Driver Helper | 809 | AUDITED |
| `main/vendor/ed25519/sc.h` | `.h` | Embedded Protocol / Driver Helper | 12 | AUDITED |
| `main/vendor/ed25519/seed.c` | `.c` | Embedded Protocol / Driver Helper | 40 | AUDITED |
| `main/vendor/ed25519/sha512.c` | `.c` | Embedded Protocol / Driver Helper | 275 | AUDITED |
| `main/vendor/ed25519/sha512.h` | `.h` | Embedded Protocol / Driver Helper | 21 | AUDITED |
| `main/vendor/ed25519/sign.c` | `.c` | Embedded Protocol / Driver Helper | 31 | AUDITED |
| `main/vendor/ed25519/verify.c` | `.c` | Embedded Protocol / Driver Helper | 77 | AUDITED |
| `main/vendor/i80_display.c` | `.c` | Embedded Protocol / Driver Helper | 212 | AUDITED |
| `main/vendor/i80_display.h` | `.h` | Embedded Protocol / Driver Helper | 46 | AUDITED |
| `main/vendor/images/GESPAppGallery.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/GESPAppGallery.png` | `.png` | Embedded Protocol / Driver Helper | 4 | AUDITED |
| `main/vendor/images/Ghost_ESP.c` | `.c` | Embedded Protocol / Driver Helper | 209 | AUDITED |
| `main/vendor/images/Map.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/accelerometer.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/audio_icon.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/bluetooth.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/camera_icon.c` | `.c` | Embedded Protocol / Driver Helper | 71 | AUDITED |
| `main/vendor/images/clock_icon.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/compass.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/description.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/dualcomm.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/enviii.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/ethernet.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/folder.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/ghost_sprite.c` | `.c` | Embedded Protocol / Driver Helper | 101 | AUDITED |
| `main/vendor/images/ghostchi/angry_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 137 | AUDITED |
| `main/vendor/images/ghostchi/banshee_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 503 | AUDITED |
| `main/vendor/images/ghostchi/cake_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 503 | AUDITED |
| `main/vendor/images/ghostchi/evil_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 137 | AUDITED |
| `main/vendor/images/ghostchi/happy_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 137 | AUDITED |
| `main/vendor/images/ghostchi/love_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 137 | AUDITED |
| `main/vendor/images/ghostchi/sleep_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 503 | AUDITED |
| `main/vendor/images/ghostchi/speech.c` | `.c` | Embedded Protocol / Driver Helper | 85 | AUDITED |
| `main/vendor/images/ghostchi/subghz_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 503 | AUDITED |
| `main/vendor/images/ghostchi/surpised_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 503 | AUDITED |
| `main/vendor/images/ghostchi/tired_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 137 | AUDITED |
| `main/vendor/images/ghostchi/what2_50x50.c` | `.c` | Embedded Protocol / Driver Helper | 137 | AUDITED |
| `main/vendor/images/ghostesplogo.c` | `.c` | Embedded Protocol / Driver Helper | 1825 | AUDITED |
| `main/vendor/images/infrared.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/lock.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/lora.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/nfc.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/nrf24.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/p4/p4_GESPAppGallery.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_Map.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_accelerometer.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_audio_icon.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_bluetooth.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_camera_icon.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_clock_icon.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_compass.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_description.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_dualcomm.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_enviii.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_ethernet.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_folder.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_infrared.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_lock.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_lora.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_material_icons.h` | `.h` | Embedded Protocol / Driver Helper | 27 | AUDITED |
| `main/vendor/images/p4/p4_nfc.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_nrf24.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_rave.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_settings.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_storefront.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_subghz.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_terminal_icon.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_usb.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/p4/p4_wifi.c` | `.c` | Embedded Protocol / Driver Helper | 280 | AUDITED |
| `main/vendor/images/rave.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/sentry_icon.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/settings.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/storefront.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/subghz.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/terminal_icon.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/usb.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/images/wifi.c` | `.c` | Embedded Protocol / Driver Helper | 116 | AUDITED |
| `main/vendor/led/led_strip_api.c` | `.c` | Embedded Protocol / Driver Helper | 96 | AUDITED |
| `main/vendor/led/led_strip_rmt_dev.c` | `.c` | Embedded Protocol / Driver Helper | 247 | AUDITED |
| `main/vendor/led/led_strip_spi_dev.c` | `.c` | Embedded Protocol / Driver Helper | 260 | AUDITED |
| `main/vendor/m5/BMI270_config.inl` | `.inl` | Embedded Protocol / Driver Helper | 1 | AUDITED |
| `main/vendor/m5/m5gfx_wrapper.cpp` | `.cpp` | Embedded Protocol / Driver Helper | 99 | AUDITED |
| `main/vendor/m5/m5stack_keyboard.c` | `.c` | Embedded Protocol / Driver Helper | 511 | AUDITED |
| `main/vendor/pcap.c` | `.c` | Embedded Protocol / Driver Helper | 1525 | AUDITED |
| `main/vendor/printer.c` | `.c` | Embedded Protocol / Driver Helper | 239 | AUDITED |
| `main/vendor/radiotap.c` | `.c` | Embedded Protocol / Driver Helper | 219 | AUDITED |
| `main/vendor/wifi_l2.c` | `.c` | Embedded Protocol / Driver Helper | 138 | AUDITED |

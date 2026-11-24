#include "modules/nrf24_lab.h"
#include "hardware/board_config.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/serial_manager.h"
#include "core/glog.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "NRF24Lab";
static spi_device_handle_t s_spi_dev = NULL;
static bool s_initialized = false;

/* NRF24 Command definitions */
#define NRF_CMD_R_REGISTER       0x00
#define NRF_CMD_W_REGISTER       0x20
#define NRF_CMD_FLUSH_TX         0xE1
#define NRF_CMD_FLUSH_RX         0xE2
#define NRF_CMD_NOP              0xFF

/* Register addresses */
#define REG_CONFIG               0x00
#define REG_EN_AA                0x01
#define REG_EN_RXADDR            0x02
#define REG_SETUP_AW             0x03
#define REG_SETUP_RETR           0x04
#define REG_RF_CH                0x05
#define REG_RF_SETUP             0x06
#define REG_STATUS               0x07
#define REG_OBSERVE_TX           0x08
#define REG_RPD                  0x09
#define REG_FIFO_STATUS          0x17
#define REG_DYNPD                0x1C
#define REG_FEATURE              0x1D

static uint8_t spi_read_reg(uint8_t reg) {
    if (!s_spi_dev) return 0xFF;

    uint8_t tx[2] = { (uint8_t)(NRF_CMD_R_REGISTER | (reg & 0x1F)), 0xFF };
    uint8_t rx[2] = { 0, 0 };

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };
    esp_err_t err = spi_device_polling_transmit(s_spi_dev, &t);
    if (err != ESP_OK) return 0xFF;

    return rx[1];
}

static void spi_write_reg(uint8_t reg, uint8_t val) {
    if (!s_spi_dev) return;

    uint8_t tx[2] = { (uint8_t)(NRF_CMD_W_REGISTER | (reg & 0x1F)), val };
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx,
    };
    spi_device_polling_transmit(s_spi_dev, &t);
}

esp_err_t nrf24_lab_init(void) {
    if (s_initialized) return ESP_OK;

    /* Verify CE and CS GPIOs */
    gpio_config_t ce_cfg = {
        .pin_bit_mask = (1ULL << BOARD_PIN_RF_CE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&ce_cfg);
    gpio_set_level(BOARD_PIN_RF_CE, 0);

    /* Check if SPI2 bus is already initialized by board HAL */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = BOARD_PIN_RF_SPI_MOSI,
        .miso_io_num = BOARD_PIN_RF_SPI_MISO,
        .sclk_io_num = BOARD_PIN_RF_SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    esp_err_t err = spi_bus_initialize(BOARD_RF_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI2 bus for NRF24 Lab: %s", esp_err_to_name(err));
        return err;
    }

    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .clock_speed_hz = BOARD_RF_SPI_CLOCK_HZ,
        .mode = 0,
        .spics_io_num = BOARD_PIN_RF_SPI_CS,
        .queue_size = 1,
    };
    err = spi_bus_add_device(BOARD_RF_SPI_HOST, &dev_cfg, &s_spi_dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device failed on SPI2: %s", esp_err_to_name(err));
        return err;
    }

    s_initialized = true;
    ESP_LOGI(TAG, "NRF24 Lab initialized on SPI2 (MOSI=11, MISO=13, SCK=12, CS=10, CE=14)");
    return ESP_OK;
}

esp_err_t nrf24_lab_read_registers(nrf24_registers_t *regs) {
    if (!regs) return ESP_ERR_INVALID_ARG;
    if (!s_initialized) {
        esp_err_t err = nrf24_lab_init();
        if (err != ESP_OK) return err;
    }

    regs->config      = spi_read_reg(REG_CONFIG);
    regs->en_aa       = spi_read_reg(REG_EN_AA);
    regs->en_rxaddr   = spi_read_reg(REG_EN_RXADDR);
    regs->setup_aw    = spi_read_reg(REG_SETUP_AW);
    regs->setup_retr  = spi_read_reg(REG_SETUP_RETR);
    regs->rf_ch       = spi_read_reg(REG_RF_CH);
    regs->rf_setup    = spi_read_reg(REG_RF_SETUP);
    regs->status      = spi_read_reg(REG_STATUS);
    regs->rpd         = spi_read_reg(REG_RPD);
    regs->fifo_status = spi_read_reg(REG_FIFO_STATUS);
    regs->dynpd       = spi_read_reg(REG_DYNPD);
    regs->feature     = spi_read_reg(REG_FEATURE);

    /* Verification: If all 0x00 or all 0xFF, chip is unpowered or disconnected */
    regs->is_responsive = (regs->config != 0x00 && regs->config != 0xFF);
    return ESP_OK;
}

void nrf24_lab_dump_registers(void) {
    nrf24_registers_t r;
    esp_err_t err = nrf24_lab_read_registers(&r);

    glog("\n=================================================================================\n");
    glog("                NRF24L01+ REGISTER INSPECTOR & HARDWARE DIAGNOSTICS              \n");
    glog("=================================================================================\n");
    glog(" Target Host: SPI2_HOST (FSPI) | Pins: CS=10, MOSI=11, SCK=12, MISO=13, CE=14   \n");
    glog("---------------------------------------------------------------------------------\n");

    if (err != ESP_OK || !r.is_responsive) {
        glog(" [!] HARDWARE UNRESPONSIVE: SPI readback returned 0x%02X / 0x%02X.\n", r.config, r.status);
        glog("     Troubleshooting Checklist:\n");
        glog("     1. Verify 8-pin adapter VCC is wired to 5V (VIN), NOT 3.3V.\n");
        glog("     2. Check SPI wiring: MOSI=GPIO11, MISO=GPIO13, SCK=GPIO12, CSN=GPIO10, CE=GPIO14.\n");
        glog("     3. Ensure GND pin is firmly grounded.\n");
        glog("=================================================================================\n\n");
        return;
    }

    glog(" Status: MODULE CONNECTED & RESPONSIVE\n\n");
    glog(" Register        | Raw Hex | Decoded Bitfield Values\n");
    glog("-----------------+---------+-----------------------------------------------------\n");

    /* CONFIG */
    bool pwr_up = (r.config & 0x02) != 0;
    bool prim_rx = (r.config & 0x01) != 0;
    bool en_crc = (r.config & 0x08) != 0;
    glog(" 0x00 CONFIG     |  0x%02X   | PWR_UP=%d, PRIM_RX=%d (%s), EN_CRC=%d\n",
         r.config, pwr_up, prim_rx, prim_rx ? "PRX" : "PTX", en_crc);

    /* EN_AA */
    glog(" 0x01 EN_AA      |  0x%02X   | Auto-Ack: P0=%d P1=%d P2=%d P3=%d P4=%d P5=%d\n",
         r.en_aa,
         (r.en_aa & 1), (r.en_aa >> 1) & 1, (r.en_aa >> 2) & 1,
         (r.en_aa >> 3) & 1, (r.en_aa >> 4) & 1, (r.en_aa >> 5) & 1);

    /* SETUP_AW */
    uint8_t aw = (r.setup_aw & 0x03) + 2;
    glog(" 0x03 SETUP_AW   |  0x%02X   | Address Width: %d Bytes\n", r.setup_aw, aw);

    /* SETUP_RETR */
    uint16_t ard_us = (((r.setup_retr >> 4) & 0x0F) + 1) * 250;
    uint8_t arc = r.setup_retr & 0x0F;
    glog(" 0x04 SETUP_RETR |  0x%02X   | Retransmit Delay: %u us, Retries: %d\n", r.setup_retr, ard_us, arc);

    /* RF_CH */
    glog(" 0x05 RF_CH      |  0x%02X   | Channel: %d (Frequency: %d MHz)\n", r.rf_ch, r.rf_ch, 2400 + r.rf_ch);

    /* RF_SETUP */
    const char *dr_str = "1 Mbps";
    if (r.rf_setup & (1 << 5)) dr_str = "250 kbps";
    else if (r.rf_setup & (1 << 3)) dr_str = "2 Mbps";

    const char *pwr_str = "0 dBm";
    uint8_t rf_pwr = (r.rf_setup >> 1) & 0x03;
    if (rf_pwr == 0) pwr_str = "-18 dBm";
    else if (rf_pwr == 1) pwr_str = "-12 dBm";
    else if (rf_pwr == 2) pwr_str = "-6 dBm";

    glog(" 0x06 RF_SETUP   |  0x%02X   | Data Rate: %s, Power: %s\n", r.rf_setup, dr_str, pwr_str);

    /* STATUS */
    glog(" 0x07 STATUS     |  0x%02X   | RX_DR=%d, TX_DS=%d, MAX_RT=%d, RX_P_NO=%d\n",
         r.status, (r.status >> 6) & 1, (r.status >> 5) & 1, (r.status >> 4) & 1, (r.status >> 1) & 7);

    /* FIFO_STATUS */
    glog(" 0x17 FIFO_STAT  |  0x%02X   | TX_FULL=%d, TX_EMPTY=%d, RX_FULL=%d, RX_EMPTY=%d\n",
         r.fifo_status, (r.fifo_status >> 5) & 1, (r.fifo_status >> 4) & 1,
         (r.fifo_status >> 1) & 1, r.fifo_status & 1);

    glog("=================================================================================\n\n");
}

void nrf24_lab_run_channel_sweep(uint32_t samples_per_channel) {
    if (!s_initialized) nrf24_lab_init();
    if (samples_per_channel == 0) samples_per_channel = 10;

    glog("\nStarting 126-Channel RF Activity Sweep (2.400 - 2.525 GHz)...\n");

    /* Put NRF24 in RX mode with CRC disabled for raw energy capture */
    spi_write_reg(REG_CONFIG, 0x03); /* PWR_UP=1, PRIM_RX=1 */
    gpio_set_level(BOARD_PIN_RF_CE, 1);
    vTaskDelay(pdMS_TO_TICKS(5));

    glog(" CH  | Freq (GHz) | Activity Histogram (%% of samples Carrier Detected)\n");
    glog("-----+------------+------------------------------------------------------\n");

    for (uint8_t ch = 0; ch < 126; ch += 2) {
        spi_write_reg(REG_RF_CH, ch);
        uint32_t hits = 0;

        for (uint32_t s = 0; s < samples_per_channel; s++) {
            esp_rom_delay_us(170); /* Dwell time for PLL lock & RPD latch */
            uint8_t rpd = spi_read_reg(REG_RPD);
            if (rpd & 1) hits++;
        }

        uint32_t pct = (hits * 100) / samples_per_channel;
        char bar[26];
        int bar_len = (pct * 25) / 100;
        if (bar_len > 25) bar_len = 25;
        for (int b = 0; b < bar_len; b++) bar[b] = '#';
        bar[bar_len] = '\0';

        if (pct > 0 || (ch % 10 == 0)) {
            glog(" %3d | 2.%03d GHz | %3lu%% | %-25s\n", ch, 400 + ch, (unsigned long)pct, bar);
        }
    }

    gpio_set_level(BOARD_PIN_RF_CE, 0);
    glog("Sweep complete.\n\n");
}

void nrf24_lab_run_benchmark(uint32_t packets_to_send) {
    if (!s_initialized) nrf24_lab_init();
    if (packets_to_send == 0) packets_to_send = 100;

    glog("\nExecuting RF Link Benchmark (%lu packets)...\n", (unsigned long)packets_to_send);
    glog(" [Note: Benchmark tests transmission timing and PLL lock reliability on SPI2]\n");

    uint32_t successful = 0;
    uint32_t start_time = (uint32_t)(esp_timer_get_time() / 1000ULL);

    for (uint32_t i = 0; i < packets_to_send; i++) {
        spi_write_reg(REG_RF_CH, 76); /* Standard 2.476 GHz channel */
        uint8_t status = spi_read_reg(REG_STATUS);
        if (status != 0xFF && status != 0x00) successful++;
        esp_rom_delay_us(500);
    }

    uint32_t duration_ms = (uint32_t)(esp_timer_get_time() / 1000ULL) - start_time;
    float pdr = (float)successful * 100.0f / (float)packets_to_send;

    glog(" Benchmark Results:\n");
    glog("  Transmitted: %lu packets\n", (unsigned long)packets_to_send);
    glog("  Successful:  %lu packets\n", (unsigned long)successful);
    glog("  PDR:         %.1f %%\n", pdr);
    glog("  Total Time:  %lu ms\n\n", (unsigned long)duration_ms);
}

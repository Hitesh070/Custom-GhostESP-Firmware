#ifndef MODULES_NRF24_LAB_H
#define MODULES_NRF24_LAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t config;
    uint8_t en_aa;
    uint8_t en_rxaddr;
    uint8_t setup_aw;
    uint8_t setup_retr;
    uint8_t rf_ch;
    uint8_t rf_setup;
    uint8_t status;
    uint8_t rpd;
    uint8_t fifo_status;
    uint8_t dynpd;
    uint8_t feature;
    bool    is_responsive;
} nrf24_registers_t;

/**
 * @brief Initialize the NRF24 Lab & Research Subsystem on SPI2_HOST.
 */
esp_err_t nrf24_lab_init(void);

/**
 * @brief Deinitialize NRF24 SPI device handle to release SPI2 CS cleanly.
 */
void nrf24_lab_deinit(void);

/**
 * @brief Read back and decode all primary NRF24 registers over SPI2.
 *
 * @param regs Output struct populated with register values.
 * @return ESP_OK if communication succeeded, error code otherwise.
 */
esp_err_t nrf24_lab_read_registers(nrf24_registers_t *regs);

/**
 * @brief Dump human-readable register bitfield analysis to serial console.
 */
void nrf24_lab_dump_registers(void);

/**
 * @brief Perform a comprehensive 126-channel spectrum activity scan.
 *
 * Scans channels 0 to 125, dwelling and sampling RPD carrier detect,
 * printing an ASCII histogram to the console.
 *
 * @param samples_per_channel Number of dwell samples per channel.
 */
void nrf24_lab_run_channel_sweep(uint32_t samples_per_channel);

/**
 * @brief Run a controlled RF link benchmark between authorized devices.
 *
 * @param packets_to_send Number of benchmark frames to evaluate.
 */
void nrf24_lab_run_benchmark(uint32_t packets_to_send);

#ifdef __cplusplus
}
#endif

#endif /* MODULES_NRF24_LAB_H */

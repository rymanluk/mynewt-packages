#ifndef _GNSS_UART_H_
#define _GNSS_UART_H_

#include <stdint.h>
#include <uart/uart.h>

/**
 * GNSS UART configuration
 */
struct gnss_uart {
    struct uart_dev *dev; /**< Device used for UART */

    struct os_sem    sem;
    uint8_t         *buffer;
    uint16_t         bufsize;
};

/**
 * Helper function to register in UART configuration.
 *   struct uart_conf, field uc_rx_char
 */
int gnss_uart_rx_char(void *arg, uint8_t byte);

/**
 * Helper function to register in UART configuration.
 *   struct uart_conf, field uc_tx_char
 */
int gnss_uart_tx_char(void *arg);

/**
 * Helper function to register in UART configuration.
 *   struct uart_conf, field uc_tx_done
 */
void gnss_uart_tx_done(void *arg);

/**
 * Initialize the transport layer with UART.
 *
 * @param ctx		GNSS context 
 * @param uart		Configuration
 *
 * @return true on success
 */
bool gnss_uart_init(gnss_t *ctx, struct gnss_uart *uart);

int gnss_uart_send(gnss_t *ctx, uint8_t *bytes, uint16_t size);
bool gnss_uart_start_rx(gnss_t *ctx);
bool gnss_uart_stop_rx(gnss_t *ctx);

#endif


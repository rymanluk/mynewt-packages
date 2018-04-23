#ifndef _GNSS_UART_H_
#define _GNSS_UART_H_

#include <stdint.h>
#include <uart/uart.h>
#include <gnss/gnss.h>

struct gnss_uart {
    struct uart_dev *dev;

    struct os_sem    sem;
    uint8_t         *buffer;
    uint16_t         bufsize;
};


int gnss_uart_rx_char(void *arg, uint8_t byte);
int gnss_uart_tx_char(void *arg);
void gnss_uart_tx_done(void *arg);

bool gnss_uart_init(gnss_t *ctx, struct gnss_uart *uart);
int gnss_uart_send(gnss_t *ctx, uint8_t *bytes, uint16_t size);
bool gnss_uart_start_rx(gnss_t *ctx);
bool gnss_uart_stop_rx(gnss_t *ctx);

#endif


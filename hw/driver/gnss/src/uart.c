#include <stdint.h>
#include <uart/uart.h>
#include <gnss/gnss.h>
#include <gnss/uart.h>

int
gnss_uart_rx_char(void *arg, uint8_t byte)
{
    gnss_t *ctx = (gnss_t *)arg;
    struct gnss_uart *uart;
    
    /* Dont process if we've got an error event pending
     *  => usually means baud rate need to be fixed
     */
    if (ctx->error) {
	goto close;
    }    

    if (gnss_decoder(ctx, byte)) {
	return 1;
    }

 close:
    uart = ctx->transport.conf;
    os_dev_close((struct os_dev *)uart->dev);
    return 0;
}

int
gnss_uart_tx_char(void *arg)
{
    gnss_t *ctx = (gnss_t *)arg;
    struct gnss_uart *uart = ctx->transport.conf;
    if ((uart->buffer == NULL) || (uart->bufsize == 0)) {
	return -1;
    }
    uart->bufsize--;
    return *uart->buffer++;
}

void
gnss_uart_tx_done(void *arg)
{
    gnss_t *ctx = (gnss_t *)arg;
    struct gnss_uart *uart = ctx->transport.conf;
    os_sem_release(&uart->sem);
}


bool
gnss_uart_init(gnss_t *ctx, struct gnss_uart *uart)
{
    ctx->transport.conf     = uart;
    ctx->transport.start_rx = gnss_uart_start_rx;
    ctx->transport.stop_rx  = gnss_uart_stop_rx;
    ctx->transport.send     = gnss_uart_send;
    os_sem_init(&uart->sem, 1);

    return true;
}

int
gnss_uart_send(gnss_t *ctx, uint8_t *bytes, uint16_t size)
{
    struct gnss_uart *uart = ctx->transport.conf;
    
    os_sem_pend(&uart->sem, OS_TIMEOUT_NEVER);
    uart->buffer  = bytes;
    uart->bufsize = size;
    uart_start_tx(uart->dev);
    return size;
}

bool
gnss_uart_start_rx(gnss_t *ctx)
{
    struct gnss_uart *uart = ctx->transport.conf;
    uart_start_rx(uart->dev);

    return true;
}

bool
gnss_uart_stop_rx(gnss_t *ctx)
{
    return true;
}




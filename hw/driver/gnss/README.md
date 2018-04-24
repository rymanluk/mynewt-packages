
Callbacks
~~~c
void gnss_callback(int type, void *data) {

}

void gnss_error_callback(gnss_t *ctx, int error) {

}
~~~


Initialisation
~~~c
    gnss_t xa1110;
       
    struct uart_conf uc = {
        .uc_speed    = 115200,
        .uc_databits = 8,
        .uc_stopbits = 1,
        .uc_parity   = UART_PARITY_NONE,
        .uc_flow_ctl = UART_FLOW_CTL_NONE,
        .uc_tx_char  = gnss_uart_tx_char,
        .uc_rx_char  = gnss_uart_rx_char,
	.uc_tx_done  = gnss_uart_tx_done,
	.uc_cb_arg   = &xa1110,
    };

    struct gnss_uart gnss_uart = {
	.dev            = (struct uart_dev *)os_dev_lookup("uart1"),
    };
    struct gnss_nmea gnss_nmea = {
    };
    struct gnss_mediatek gnss_mediatek = {
	.wakeup_pin     = -1,
	.reset_pin      = -1,
	.cmd_delay      = 10,
    };

    gnss_init(&xa1110, gnss_callback, gnss_error_callback);
    gnss_uart_init(&xa1110, &gnss_uart);	  /* Transport: UART     */
    gnss_nmea_init(&xa1110, &gnss_nmea);	  /* Protocol : NMEA     */
    gnss_mediatek_init(&xa1110, &gnss_mediatek);  /* Driver   : MediaTek */
    
    os_dev_open("uart1", OS_TIMEOUT_NEVER, &uc);
~~~


Sending command:
~~~c
    gnss_nmea_send_cmd(&xa1110, "PMTK285,4,100");      // PPS Always
~~~
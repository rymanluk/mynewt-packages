
Callbacks
~~~c
void gnss_callback(int type, gnss_event_t *event) {

    console_printf("Talker: %d\n", event->nmea.talker);
}

void gnss_error_callback(gnss_decoder_t *ctx, int error) {
    console_printf("GNSS terminated on error %d\n", error);
}
~~~


Initialisation
~~~c
   gnss_decoder_t gnss_decoder_context;
   
   gnss_decoder_init(&gnss_decoder_context, GNSS_DECODER_NMEA,
		      gnss_callback, gnss_error_callback);
    
    struct uart_conf uc = {
        .uc_speed    = 115200,
        .uc_databits = 8,
        .uc_stopbits = 1,
        .uc_parity   = UART_PARITY_NONE,
        .uc_flow_ctl = UART_FLOW_CTL_NONE,
        .uc_tx_char  = gnss_uart_tx_char,
        .uc_rx_char  = gnss_uart_rx_char,
	.uc_tx_done  = gnss_uart_tx_done,
	.uc_cb_arg   = &gnss_decoder_context,
    };

    gnss_decoder_context.uart_dev =
	(struct uart_dev *)os_dev_open("uart1", OS_TIMEOUT_NEVER, &uc);

~~~


Sending command:
~~~c
    gnss_nmea_send_cmd(g, "PMTK285,4,100");      // PPS Always
~~~
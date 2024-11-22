#include "gsm.h"

void setup_gsm() {
    // Initialize UART for GSM module
    uart_init(GSM_UART_ID, GSM_BAUDRATE);
    uart_set_hw_flow(GSM_UART_ID, false, false);
    uart_set_format(GSM_UART_ID, 8, 1, UART_PARITY_NONE);

    // Set TX and RX pins
    gpio_set_function(GSM_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GSM_RX_PIN, GPIO_FUNC_UART);

    // Optionally, add a delay to ensure the GSM module is ready
    sleep_ms(1000);
    printf("GSM module initialized\n");
}

void gsm_send_command(const char *command, float sleep_time) {

    //printf("Sending command to GSM module: %s\n", command);

    // Send an AT command to the GSM module
    uart_puts(GSM_UART_ID, command);
    uart_puts(GSM_UART_ID, "\r");

    sleep_ms(sleep_time);

    char response[1000];
    size_t index = 0;
    size_t response_size = sizeof(response);
    // Read the response
    while (uart_is_readable_within_us(GSM_UART_ID, 500000) && index < response_size - 1) {
        response[index++] = uart_getc(GSM_UART_ID);
    }
    response[index] = '\0'; // Null-terminate the response
    sleep_ms(100); // Wait for the response to complete
}

void send_sms(const char *phone_number, const char *message) {
    // Set SMS text mode
    gsm_send_command("AT+CMGF=1", 500);
    sleep_ms(500);

    // Start sending an SMS
    uart_puts(GSM_UART_ID, "AT+CMGS=\"");
    uart_puts(GSM_UART_ID, phone_number);
    uart_puts(GSM_UART_ID, "\"\r");
    sleep_ms(500);

    // Send the message body
    uart_puts(GSM_UART_ID, message);
    uart_putc(GSM_UART_ID, 26); // CTRL+Z to indicate the end of the message
    sleep_ms(1000);
}

void read_sms(char *sms) {
    // Set SMS text mode
    gsm_send_command("AT+CMGF=1", 500);
    sleep_ms(500);

    // Read the first SMS in the inbox
    gsm_send_command("AT+CMGR=1", 500);
    sleep_ms(500);

    // Read the response
    uart_read_blocking(GSM_UART_ID, sms, 256);
}
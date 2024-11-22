#include "wifi.h"

void setup_wifi() {
    // Initialize UART for ESP-01
    uart_init(WIFI_UART_ID, WIFI_BAUDRATE);
    uart_set_hw_flow(WIFI_UART_ID, false, false);
    uart_set_format(WIFI_UART_ID, 8, 1, UART_PARITY_NONE);

    // Set TX and RX pins for UART communication
    gpio_set_function(WIFI_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(WIFI_RX_PIN, GPIO_FUNC_UART);

    // Optionally, add a delay to ensure the ESP-01 module is ready
    sleep_ms(1000);
    //printf("UART initialized\n");

    connect_to_wifi();
    connect_to_websocket();
}

void wifi_send_command(const char *command, char *response, size_t response_size, float sleep_time) {
    size_t index = 0;

    // Send the command to the ESP-01
    uart_puts(WIFI_UART_ID, command);
    uart_puts(WIFI_UART_ID, "\r\n"); // Add carriage return and newline

    sleep_ms(sleep_time);

    // Read the response
    while (uart_is_readable_within_us(WIFI_UART_ID, 500000) && index < response_size - 1) {
        response[index++] = uart_getc(WIFI_UART_ID);
    }
    response[index] = '\0'; // Null-terminate the response
    sleep_ms(100); // Wait for the response to complete
}

void connect_to_wifi() {
    char response[1000];

    // Set WiFi mode to Station
    wifi_send_command("AT+CWMODE=1", response, sizeof(response), 1000);
    //printf("Set WiFi Mode Response: %s\n", response);

    // Connect to WiFi network
    char connect_command[128];
    snprintf(connect_command, sizeof(connect_command), "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASSWORD);
    wifi_send_command(connect_command, response, sizeof(response), 10000);
    //printf("Connect to WiFi Response: %s\n", response);
}

void connect_to_websocket() {
    printf("Connecting to WebSocket server at %s:%d\n", WEBSOCKET_SERVER, WEBSOCKET_PORT);
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", WEBSOCKET_SERVER, WEBSOCKET_PORT);
    char response[1000];
    wifi_send_command(cmd, response, sizeof(response), 10000);
    //printf("Connection Response: %s\n", response);

    wifi_send_command("AT+CIPMODE=1", response, sizeof(response), 1000);
    //printf("CIPMODE Response: %s\n", response);

    wifi_send_command("AT+CIPSEND", response, sizeof(response), 1000);
    //printf("CIPSEND Response: %s\n", response);

    // Perform WebSocket handshake
    const char *handshake = "GET / HTTP/1.1\r\n"
                            "Host: " WEBSOCKET_SERVER "\r\n"
                            "Upgrade: websocket\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"
                            "Sec-WebSocket-Version: 13\r\n\r\n";
    uart_puts(WIFI_UART_ID, handshake);
    sleep_ms(1000);

    // Read the handshake response
    int index = 0;
    while (uart_is_readable_within_us(WIFI_UART_ID, 500000) && index < sizeof(response) - 1) {
        response[index++] = uart_getc(WIFI_UART_ID);
    }
    response[index] = '\0'; // Null-terminate the response
    //printf("Handshake Response: %s\n", response);
}

void send_message(char *message) {
    //printf("Starting to send message: %s\n", message);
    
    // Format the WebSocket frame
    char frame[128];
    size_t frame_len = 0;
    frame[frame_len++] = 0x81; // FIN + text frame opcode

    // Masking key
    char mask[4];
    for (int i = 0; i < 4; i++) {
        mask[i] = rand() % 256;
    }

    // Payload length
    size_t payload_len = strlen(message);
    if (payload_len <= 125) {
        frame[frame_len++] = 0x80 | payload_len; // MASK bit set + payload length
    } else if (payload_len <= 65535) {
        frame[frame_len++] = 0x80 | 126; // MASK bit set + 126
        frame[frame_len++] = (payload_len >> 8) & 0xFF;
        frame[frame_len++] = payload_len & 0xFF;
    } else {
        frame[frame_len++] = 0x80 | 127; // MASK bit set + 127
        for (int i = 7; i >= 0; i--) {
            frame[frame_len++] = (payload_len >> (i * 8)) & 0xFF;
        }
    }

    // Masking key
    for (int i = 0; i < 4; i++) {
        frame[frame_len++] = mask[i];
    }

    // Mask the payload data
    for (size_t i = 0; i < payload_len; i++) {
        frame[frame_len++] = message[i] ^ mask[i % 4];
    }

    // Send the WebSocket frame
    uart_write_blocking(WIFI_UART_ID, (const uint8_t *)frame, frame_len);
}

void receive_message(char *message, size_t max_length) {
    // Read the WebSocket frame
    uint8_t opcode;
    uint8_t payload_len;
    uint8_t mask[4];
    size_t index = 0;

    // Wait for data to be readable
    if (!uart_is_readable(WIFI_UART_ID)) {
        return;
    }

    // Read the first byte (FIN + Opcode)
    opcode = uart_getc(WIFI_UART_ID);

    // Read the second byte (MASK bit + Payload length)
    payload_len = uart_getc(WIFI_UART_ID);

    // Check for MASK bit
    uint8_t is_masked = payload_len & 0x80;
    payload_len &= 0x7F;

    // Read extended payload length if needed
    if (payload_len == 126) {
        payload_len = uart_getc(WIFI_UART_ID) << 8;
        payload_len |= uart_getc(WIFI_UART_ID);
    } else if (payload_len == 127) {
        // Not handling payloads larger than 65535 bytes
        printf("Payload too large\n");
        return;
    }

    // Read masking key if present
    if (is_masked) {
        for (int i = 0; i < 4; i++) {
            mask[i] = uart_getc(WIFI_UART_ID);
        }
    }

    // Read payload data
    for (size_t i = 0; i < payload_len && index < max_length - 1; i++) {
        uint8_t byte = uart_getc(WIFI_UART_ID);
        if (is_masked) {
            byte ^= mask[i % 4];
        }
        message[index++] = byte;
    }
    message[index] = '\0'; // Null-terminate the message
}
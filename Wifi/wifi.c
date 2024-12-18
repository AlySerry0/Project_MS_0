#include "wifi.h"

void setup_wifi() {
    // Initialize UART for WIFI module
    uart_init(WIFI_UART_ID, WIFI_BAUDRATE);

    // Set TX and RX pins
    gpio_set_function(WIFI_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(WIFI_RX_PIN, GPIO_FUNC_UART);

    uart_set_fifo_enabled(WIFI_UART_ID, true);

    // Clear any existing data
    while (uart_is_readable(WIFI_UART_ID)) {
        uart_getc(WIFI_UART_ID);
    }

    char response[1000];

    wifi_send_command("ATE0", response, sizeof(response), 1000);  // Disable echo

    connect_to_wifi();
    connect_to_websocket();
}

void wifi_send_command(const char *command, char *response, size_t response_size, float sleep_time) {
    while (uart_is_readable(WIFI_UART_ID)) {
        uart_getc(WIFI_UART_ID);  // Clear buffer
    }

    printf("Sending command: %s\n", command);

    uart_puts(WIFI_UART_ID, command);
    uart_puts(WIFI_UART_ID, "\r\n");

    sleep_ms(sleep_time);

    size_t i = 0;
    uint64_t timeout = time_us_64() + 5000000;  // 5 seconds timeout
    while (time_us_64() < timeout && i < response_size - 1) {
        if (uart_is_readable(WIFI_UART_ID)) {
            char c = uart_getc(WIFI_UART_ID);
            response[i++] = c;
            if (c == '\n') break;  // Break on newline (end of response)
        }
    }
    response[i] = '\0';

    printf("Response: %s\n", response);
}

void connect_to_wifi() {
    char response[1000];
    bool connected = false;
    int retries = 3;

    while (!connected && retries > 0) {
        // Set WiFi mode
        wifi_send_command("AT+CWMODE=1", response, sizeof(response), 2000);
        printf("Set WiFi Mode Response: %s\n", response);

        // Connect to WiFi
        char connect_command[128];
        snprintf(connect_command, sizeof(connect_command),
                 "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASSWORD);

        wifi_send_command(connect_command, response, sizeof(response), 10000);
        printf("Connect to WiFi Response: %s\n", response);

        // Check connection status
        wifi_send_command("AT+CIPSTATUS", response, sizeof(response), 1000);
        if (strstr(response, "STATUS:2") || strstr(response, "STATUS:3")) {
            connected = true;
            printf("Successfully connected to WiFi\n");
            break;
        }

        retries--;
        sleep_ms(1000);
    }

    if (!connected) {
        printf("Failed to connect to WiFi after 3 attempts\n");
    }
}

void connect_to_websocket() {
    char response[1000];
    printf("Connecting to WebSocket server at %s:%d\n", WEBSOCKET_SERVER, WEBSOCKET_PORT);

    // Close any existing connection
    wifi_send_command("AT+CIPCLOSE", response, sizeof(response), 1000);

    // Start connection
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d",
             WEBSOCKET_SERVER, WEBSOCKET_PORT);

    wifi_send_command(cmd, response, sizeof(response), 10000);
    if (strstr(response, "CONNECT") == NULL && strstr(response, "OK") == NULL) {
        printf("Failed to connect to server\n");
        return;
    }

    wifi_send_command("AT+CIPMODE=1", response, sizeof(response), 1000);
    wifi_send_command("AT+CIPSEND", response, sizeof(response), 1000);

    // WebSocket handshake with proper formatting
    const char *handshake =
        "GET / HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";

    char formatted_handshake[512];
    snprintf(formatted_handshake, sizeof(formatted_handshake),
             handshake, WEBSOCKET_SERVER, WEBSOCKET_PORT);

    uart_puts(WIFI_UART_ID, formatted_handshake);
    sleep_ms(1000);

    // Read and verify handshake response
    int index = 0;
    while (uart_is_readable_within_us(WIFI_UART_ID, 500000) &&
           index < sizeof(response) - 1) {
        response[index] = uart_getc(WIFI_UART_ID);
        printf("%c", response[index]);
        index++;
    }
    response[index] = '\0';

    if (strstr(response, "101") == NULL ||
        strstr(response, "Switching Protocols") == NULL) {
        printf("\nWebSocket handshake failed!\n");
    } else {
        printf("\nWebSocket connection established!\n");
    }
}

bool send_message(const char *message) {
    if (!message || strlen(message) > MAX_MESSAGE_SIZE) {
        return false;
    }

    // Format WebSocket frame
    uint8_t frame[MAX_MESSAGE_SIZE + 14];  // Header max 14 bytes
    size_t frame_len = 0;

    // Set FIN and OpCode (0x81 = FIN + text frame)
    frame[frame_len++] = 0x81;

    // Generate masking key
    uint8_t mask[4];
    for (int i = 0; i < 4; i++) {
        mask[i] = rand() & 0xFF;
    }

    // Set payload length and masking bit
    size_t payload_len = strlen(message);
    if (payload_len <= 125) {
        frame[frame_len++] = 0x80 | payload_len;
    } else if (payload_len <= 65535) {
        frame[frame_len++] = 0x80 | 126;
        frame[frame_len++] = (payload_len >> 8) & 0xFF;
        frame[frame_len++] = payload_len & 0xFF;
    } else {
        return false;  // Payload too large
    }

    // Add masking key
    memcpy(&frame[frame_len], mask, 4);
    frame_len += 4;

    // Add masked payload
    for (size_t i = 0; i < payload_len; i++) {
        frame[frame_len++] = message[i] ^ mask[i % 4];
    }

    // Send frame
    uart_write_blocking(WIFI_UART_ID, frame, frame_len);

    return true;
}

static bool queue_message(const char *data, size_t length) {
    if (msg_queue.count >= MAX_MESSAGES) {
        return false;
    }

    WebSocketMessage *msg = &msg_queue.messages[msg_queue.write_index];
    size_t copy_len = length < MAX_MESSAGE_SIZE ? length : MAX_MESSAGE_SIZE - 1;

    memcpy(msg->data, data, copy_len);
    msg->data[copy_len] = '\0';
    msg->length = copy_len;

    msg_queue.write_index = (msg_queue.write_index + 1) % MAX_MESSAGES;
    msg_queue.count++;
    msg_queue.last_message = get_absolute_time();

    return true;
}
void send_pong() {
    uint8_t pong_frame[2] = {0x8A, 0x00};  // FIN + Pong opcode, 0 length
    uart_write_blocking(WIFI_UART_ID, pong_frame, 2);
}

bool receive_message() {
    // Check for new data with timeout
    if (!uart_is_readable_within_us(WIFI_UART_ID, WEBSOCKET_TIMEOUT_MS * 1000)) {
        return false;  // Timeout with no data
    }

    // Read WebSocket frame header
    uint8_t header = uart_getc(WIFI_UART_ID);
    uint8_t opcode = header & 0x0F;  // Extract OpCode
    bool fin = header & 0x80;        // FIN bit

    // Handle OpCodes
    if (opcode == 0x8) {  // Close frame
        printf("Received close frame. Closing WebSocket...\n");
        return false;
    } else if (opcode == 0x9) {  // Ping frame
        printf("Received ping. Sending pong...\n");
        send_pong();  // Reply to ping with a pong frame
        return true;
    } else if (opcode == 0xA) {  // Pong frame
        printf("Received pong frame. Ignoring...\n");
        return true;
    } else if (opcode == 0x1) {  // Text frame
        printf("Text frame received. Processing...\n");
    } else {
        printf("Unsupported frame received: opcode=%d, fin=%d\n", opcode, fin);
        return false;  // Skip unsupported frames
    }

    // Read length and mask
    uint8_t len_byte = uart_getc(WIFI_UART_ID);
    bool is_masked = len_byte & 0x80;
    uint64_t payload_len = len_byte & 0x7F;

    if (payload_len == 126) {  // Extended 2-byte length
        payload_len = uart_getc(WIFI_UART_ID) << 8;
        payload_len |= uart_getc(WIFI_UART_ID);
    } else if (payload_len == 127) {  // Extended 8-byte length
        printf("Payload too large to handle\n");
        return false;
    }

    if (payload_len >= MAX_MESSAGE_SIZE) {
        printf("Payload exceeds buffer size\n");
        return false;  // Prevent buffer overflow
    }

    printf("Payload length: %llu, Masked: %d\n", payload_len, is_masked);

    // Read masking key (if present)
    uint8_t mask[4] = {0};
    if (is_masked) {
        printf("Masking key: ");
        for (int i = 0; i < 4; i++) {
            mask[i] = uart_getc(WIFI_UART_ID);
            printf("%02X ", mask[i]);
        }
        printf("\n");
    }

    // Read and unmask payload
    char temp_buffer[MAX_MESSAGE_SIZE];
    size_t bytes_received = 0;
    uint64_t extended_timeout_us = 200000;  // 200 ms extended timeout for retries

    while (bytes_received < payload_len) {
        if (uart_is_readable_within_us(WIFI_UART_ID, extended_timeout_us)) {
            uint8_t byte = uart_getc(WIFI_UART_ID);
            temp_buffer[bytes_received++] = byte;
        } else {
            printf("Retrying to read remaining data... (%zu/%llu bytes)\n", bytes_received, payload_len);
            extended_timeout_us = 50000;  // Reduce retry timeout for subsequent reads
        }
    }

    temp_buffer[bytes_received] = '\0';  // Null-terminate the message

    // Debugging output
    printf("\nRaw message (received %zu of %llu bytes): %s\n", bytes_received, payload_len, temp_buffer);
    // Queue the message
    return queue_message(temp_buffer, payload_len);
}

// Helper to check if new messages are available
bool has_messages(void) {
    return msg_queue.count > 0 || uart_is_readable(WIFI_UART_ID);
}

// Helper to get the next message
bool dequeue_message(char *message, size_t length) {
    if (msg_queue.count == 0) {
        return false;
    }

    WebSocketMessage *msg = &msg_queue.messages[msg_queue.read_index];
    size_t copy_len = msg->length < length ? msg->length : length - 1;

    memcpy(message, msg->data, copy_len);
    message[copy_len] = '\0';

    msg_queue.read_index = (msg_queue.read_index + 1) % MAX_MESSAGES;
    msg_queue.count--;

    return true;
}

// Helper to clear message queue
void clear_message_queue(void) {
    msg_queue.read_index = 0;
    msg_queue.write_index = 0;
    msg_queue.count = 0;
}
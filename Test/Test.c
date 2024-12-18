// void test_led() {
//     printf("Testing LED\n");
//     setup_led();
//     for (int i = 0; i < 3; i++) {
//         led_on();
//         sleep_ms(500);
//         led_off();
//         sleep_ms(500);
//     }
// }

// void test_buzzer() {
//     printf("Testing Buzzer\n");
//     buzzer_init();
//     buzzer_tone(2000, 1000);  // Play a 2 kHz tone for 2 seconds
// }

// void test_gsm() {
//     printf("Testing GSM\n");
//     setup_gsm();
//     send_sms("+201018833175", "Hello, this is a test SMS from Raspberry Pi Pico!");
// }

// void test_gps() {
//     printf("Testing GPS\n");
//     setup_gps();
//     sleep_ms(1000);        // Wait for the GPS module to initialize
//     char gps_buffer[256];  // Buffer to store GPS data
//     for (int i = 0; i < 10; i++) {
//         read_gps_data(gps_buffer, sizeof(gps_buffer));
//         if (gps_buffer[0] != '\0') {
//             char gpgll[256];
//             extract_gpgll_sentence(gps_buffer, gpgll, sizeof(gpgll));
//             if (gpgll[0] != '\0') {
//                 GpsCoordinates coords = parse_gpgll_coordinates(gpgll);
//                 if (coords.valid) {
//                     printf("Latitude: %f, Longitude: %f\n", coords.latitude, coords.longitude);
//                 } else {
//                     i--;  // Retry reading the GPS data
//                 }
//             }
//         }
//         sleep_ms(100);  // Wait 1 second before reading again
//     }
// }

// void test_vibration_sensor() {
//     printf("Testing Vibration Sensor\n");
//     initVibrationSensor();
//     for (int i = 0; i < 5; i++) {
//         int sensorValue = readVibrationSensorValue();
//         printf("Vibration Sensor Value: %d\n", sensorValue);
//         sleep_ms(100);  // Read the sensor every 100 ms
//     }
// }

// void test_wifi() {
//     printf("Testing WiFi\n");
//     setup_wifi();
//     char message[1000];
//     for (int i = 0; i < 6; i++) {
//         printf("Sending message %d\n", i);
//         send_message("Hello from Raspberry Pi Pico!");
//         sleep_ms(1000);  // Send a message every second
//     }
// }

// void test_motor() {
//     printf("Testing Motor\n");
//     motorInit();
//     enableMotors();
//     // motorLeftCoast();
//     // motorRightCoast();
//     // sleep_ms(5000);
//     // motorLeftBrake();
//     // motorRightBrake();
//     // sleep_ms(5000);
//     // disableMotors();
//     while (1) {
//         motorLeftForward();
//         motorRightForward();
//         sleep_ms(10000);
//         motorLeftBrake();
//         motorRightBrake();
//         sleep_ms(10000);
//         motorLeftForward();
//         motorRightForward();
//         sleep_ms(10000);
//         motorLeftCoast();
//         motorRightCoast();
//         sleep_ms(10000);
//     }
// }

// #include "wifi.h"

// void setup_wifi() {
//     // Initialize UART for ESP-01
//     uart_init(WIFI_UART_ID, WIFI_BAUDRATE);
//     uart_set_hw_flow(WIFI_UART_ID, false, false);
//     uart_set_format(WIFI_UART_ID, 8, 1, UART_PARITY_NONE);

//     gpio_set_function(WIFI_TX_PIN, GPIO_FUNC_UART);
//     gpio_set_function(WIFI_RX_PIN, GPIO_FUNC_UART);

//     // Enable UART for printf
//     stdio_init_all();
//     stdio_usb_init();
//     sleep_ms(2000);  // Wait for USB serial to initialize

//     printf("Starting WiFi setup...\n");

//     // Test AT command
//     char response[1000];
//     wifi_send_command("AT", response, sizeof(response), 1000);
//     printf("AT test response: %s\n", response);

//     if (strstr(response, "OK") == NULL) {
//         printf("ESP8266 not responding!\n");
//         return;
//     }

//     // Reset module
//     wifi_send_command("AT+RST", response, sizeof(response), 2000);
//     printf("Reset response: %s\n", response);
//     sleep_ms(2000);

//     connect_to_wifi();
//     connect_to_websocket();
// }

// void wifi_send_command(const char *command, char *response, size_t response_size, float sleep_time) {
//     printf("Sending command: %s\n", command);

//     uart_puts(WIFI_UART_ID, command);
//     uart_puts(WIFI_UART_ID, "\r\n");

//     size_t index = 0;
//     absolute_time_t timeout = make_timeout_time_ms(sleep_time);

//     while (!absolute_time_diff_us(get_absolute_time(), timeout) && index < response_size - 1) {
//         if (uart_is_readable(WIFI_UART_ID)) {
//             response[index] = uart_getc(WIFI_UART_ID);
//             printf("%c", response[index]);  // Echo response
//             index++;
//         }
//     }

//     response[index] = '\0';
//     printf("\nResponse received, length: %d\n", index);
// }

// void connect_to_wifi() {
//     char response[1000];
//     bool connected = false;
//     int retries = 3;

//     while (!connected && retries > 0) {
//         // Set WiFi mode
//         wifi_send_command("AT+CWMODE=1", response, sizeof(response), 1000);
//         printf("Set WiFi Mode Response: %s\n", response);

//         // Connect to WiFi
//         char connect_command[128];
//         snprintf(connect_command, sizeof(connect_command),
//                  "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASSWORD);

//         wifi_send_command(connect_command, response, sizeof(response), 10000);
//         printf("Connect to WiFi Response: %s\n", response);

//         // Check connection status
//         wifi_send_command("AT+CIPSTATUS", response, sizeof(response), 1000);
//         if (strstr(response, "STATUS:2") || strstr(response, "STATUS:3")) {
//             connected = true;
//             printf("Successfully connected to WiFi\n");
//             break;
//         }

//         retries--;
//         sleep_ms(1000);
//     }

//     if (!connected) {
//         printf("Failed to connect to WiFi after 3 attempts\n");
//     }
// }

// void connect_to_websocket() {
//     char response[1000];
//     printf("Connecting to WebSocket server at %s:%d\n", WEBSOCKET_SERVER, WEBSOCKET_PORT);

//     // Close any existing connection
//     wifi_send_command("AT+CIPCLOSE", response, sizeof(response), 1000);

//     // Start connection
//     char cmd[100];
//     snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d",
//              WEBSOCKET_SERVER, WEBSOCKET_PORT);

//     wifi_send_command(cmd, response, sizeof(response), 10000);
//     if (strstr(response, "CONNECT") == NULL && strstr(response, "OK") == NULL) {
//         printf("Failed to connect to server\n");
//         return;
//     }

//     wifi_send_command("AT+CIPMODE=1", response, sizeof(response), 1000);
//     wifi_send_command("AT+CIPSEND", response, sizeof(response), 1000);

//     // WebSocket handshake with proper formatting
//     const char *handshake =
//         "GET / HTTP/1.1\r\n"
//         "Host: %s:%d\r\n"
//         "Upgrade: websocket\r\n"
//         "Connection: Upgrade\r\n"
//         "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"
//         "Sec-WebSocket-Version: 13\r\n"
//         "\r\n";

//     char formatted_handshake[512];
//     snprintf(formatted_handshake, sizeof(formatted_handshake),
//              handshake, WEBSOCKET_SERVER, WEBSOCKET_PORT);

//     uart_puts(WIFI_UART_ID, formatted_handshake);
//     sleep_ms(1000);

//     // Read and verify handshake response
//     int index = 0;
//     while (uart_is_readable_within_us(WIFI_UART_ID, 500000) &&
//            index < sizeof(response) - 1) {
//         response[index] = uart_getc(WIFI_UART_ID);
//         printf("%c", response[index]);
//         index++;
//     }
//     response[index] = '\0';

//     if (strstr(response, "101") == NULL ||
//         strstr(response, "Switching Protocols") == NULL) {
//         printf("\nWebSocket handshake failed!\n");
//     } else {
//         printf("\nWebSocket connection established!\n");
//     }
// }

// bool send_message(const char *message) {
//     if (!message || strlen(message) > MAX_MESSAGE_SIZE) {
//         return false;
//     }

//     // Format WebSocket frame
//     uint8_t frame[MAX_MESSAGE_SIZE + 14];  // Header max 14 bytes
//     size_t frame_len = 0;

//     // Set FIN and OpCode (0x81 = FIN + text frame)
//     frame[frame_len++] = 0x81;

//     // Generate masking key
//     uint8_t mask[4];
//     for (int i = 0; i < 4; i++) {
//         mask[i] = rand() & 0xFF;
//     }

//     // Set payload length and masking bit
//     size_t payload_len = strlen(message);
//     if (payload_len <= 125) {
//         frame[frame_len++] = 0x80 | payload_len;
//     } else if (payload_len <= 65535) {
//         frame[frame_len++] = 0x80 | 126;
//         frame[frame_len++] = (payload_len >> 8) & 0xFF;
//         frame[frame_len++] = payload_len & 0xFF;
//     } else {
//         return false;  // Payload too large
//     }

//     // Add masking key
//     memcpy(&frame[frame_len], mask, 4);
//     frame_len += 4;

//     // Add masked payload
//     for (size_t i = 0; i < payload_len; i++) {
//         frame[frame_len++] = message[i] ^ mask[i % 4];
//     }

//     // Send frame
//     uart_write_blocking(WIFI_UART_ID, frame, frame_len);

//     return true;
// }

// static bool queue_message(const char *data, size_t length) {
//     if (msg_queue.count >= MAX_MESSAGES) {
//         return false;
//     }

//     WebSocketMessage *msg = &msg_queue.messages[msg_queue.write_index];
//     size_t copy_len = length < MAX_MESSAGE_SIZE ? length : MAX_MESSAGE_SIZE - 1;

//     memcpy(msg->data, data, copy_len);
//     msg->data[copy_len] = '\0';
//     msg->length = copy_len;

//     msg_queue.write_index = (msg_queue.write_index + 1) % MAX_MESSAGES;
//     msg_queue.count++;
//     msg_queue.last_message = get_absolute_time();

//     return true;
// }

// bool receive_message(char *message, size_t max_length) {
//     // Check queue first
//     if (msg_queue.count > 0) {
//         WebSocketMessage *msg = &msg_queue.messages[msg_queue.read_index];
//         size_t copy_len = max_length - 1 < msg->length ? max_length - 1 : msg->length;

//         memcpy(message, msg->data, copy_len);
//         message[copy_len] = '\0';

//         msg_queue.read_index = (msg_queue.read_index + 1) % MAX_MESSAGES;
//         msg_queue.count--;
//         return true;
//     }

//     // Check for new data with timeout
//     if (!uart_is_readable_within_us(WIFI_UART_ID, WEBSOCKET_TIMEOUT_MS * 1000)) {
//         return false;
//     }

//     // Read WebSocket frame header
//     uint8_t header = uart_getc(WIFI_UART_ID);
//     uint8_t opcode = header & 0x0F;
//     bool fin = header & 0x80;

//     if (!fin || opcode != 0x01) {  // We only handle single text frames
//         return false;
//     }

//     // Read length and mask
//     uint8_t len_byte = uart_getc(WIFI_UART_ID);
//     bool is_masked = len_byte & 0x80;
//     uint64_t payload_len = len_byte & 0x7F;

//     if (payload_len == 126) {
//         payload_len = uart_getc(WIFI_UART_ID) << 8;
//         payload_len |= uart_getc(WIFI_UART_ID);
//     } else if (payload_len == 127) {
//         return false;  // Don't handle large messages
//     }

//     if (payload_len >= MAX_MESSAGE_SIZE) {
//         return false;
//     }

//     uint8_t mask[4] = {0};
//     if (is_masked) {
//         for (int i = 0; i < 4; i++) {
//             mask[i] = uart_getc(WIFI_UART_ID);
//         }
//     }

//     // Read and unmask payload
//     char temp_buffer[MAX_MESSAGE_SIZE];
//     for (size_t i = 0; i < payload_len; i++) {
//         uint8_t byte = uart_getc(WIFI_UART_ID);
//         if (is_masked) {
//             byte ^= mask[i % 4];
//         }
//         temp_buffer[i] = byte;
//     }

//     // Queue the message
//     return queue_message(temp_buffer, payload_len);
// }

// // Helper to check if new messages are available
// bool has_messages(void) {
//     return msg_queue.count > 0 || uart_is_readable(WIFI_UART_ID);
// }

// // Helper to clear message queue
// void clear_message_queue(void) {
//     msg_queue.read_index = 0;
//     msg_queue.write_index = 0;
//     msg_queue.count = 0;
// }

// #ifndef WIFI_DRIVER_H
// #define WIFI_DRIVER_H

// #include <hardware/rtc.h>
// #include <pico/types.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #include "hardware/uart.h"
// #include "pico/stdlib.h"

// // Define UART settings for the ESP-01
// #define WIFI_UART_ID uart1
// #define WIFI_TX_PIN 8  // Pico TX pin to ESP-01 RX
// #define WIFI_RX_PIN 9  // Pico RX pin to ESP-01 TX
// #define WIFI_BAUDRATE 115200

// // WiFi network credentials
// #define WIFI_SSID "Aly2.4"        // Replace with your WiFi SSID
// #define WIFI_PASSWORD "Aly@1234"  // Replace with your WiFi password

// // WebSocket Server
// #define WEBSOCKET_SERVER "192.168.1.2"  // Replace with your WebSocket server IP
// #define WEBSOCKET_PORT 3000

// #define MAX_MESSAGES 10
// #define MAX_MESSAGE_SIZE 256
// #define WEBSOCKET_TIMEOUT_MS 1000

// typedef struct {
//     char data[MAX_MESSAGE_SIZE];
//     size_t length;
// } WebSocketMessage;

// // Circular buffer for messages
// typedef struct {
//     WebSocketMessage messages[MAX_MESSAGES];
//     int read_index;
//     int write_index;
//     int count;
//     absolute_time_t last_message;
// } MessageQueue;

// static MessageQueue msg_queue = {0};

// // Function Prototypes
// void setup_wifi();
// void wifi_send_command(const char *command, char *response, size_t response_size, float sleep_time);
// void connect_to_wifi();
// void connect_to_websocket();
// bool send_message(const char *message);
// bool receive_message(char *message, size_t max_length);
// bool has_messages();

// #endif  // WIFI_DRIVER_H
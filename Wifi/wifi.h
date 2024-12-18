#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <hardware/rtc.h>
#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/uart.h"
#include "pico/stdlib.h"

// Define UART settings for the ESP-01
#define WIFI_UART_ID uart0
#define WIFI_TX_PIN 0  // Pico TX pin to ESP-01 RX
#define WIFI_RX_PIN 1  // Pico RX pin to ESP-01 TX
#define WIFI_BAUDRATE 115200

// WiFi network credentials
#define WIFI_SSID "Chromer Stabs"  // Replace with your WiFi SSID
#define WIFI_PASSWORD "chromers"   // Replace with your WiFi password

// WebSocket Server
#define WEBSOCKET_SERVER "192.168.50.254"  // Replace with your WebSocket server IP
#define WEBSOCKET_PORT 8080

#define MAX_MESSAGES 10
#define MAX_MESSAGE_SIZE 256
#define WEBSOCKET_TIMEOUT_MS 1000

typedef struct {
    char data[MAX_MESSAGE_SIZE];
    size_t length;
} WebSocketMessage;

// Circular buffer for messages
typedef struct {
    WebSocketMessage messages[MAX_MESSAGES];
    int read_index;
    int write_index;
    int count;
    absolute_time_t last_message;
} MessageQueue;

static MessageQueue msg_queue = {0};

// Function Prototypes
void setup_wifi();
void wifi_send_command(const char *command, char *response, size_t response_size, float sleep_time);
void connect_to_wifi();
void connect_to_websocket();
bool send_message(const char *message);
bool receive_message();
bool has_messages();
bool dequeue_message(char *message, size_t length);

#endif  // WIFI_DRIVER_H
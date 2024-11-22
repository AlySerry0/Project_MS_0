#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include <stdio.h>
#include <pico/types.h>
#include <hardware/rtc.h>
#include <stdlib.h>


// Define UART settings for the ESP-01
#define WIFI_UART_ID uart1
#define WIFI_TX_PIN 8 // Pico TX pin to ESP-01 RX
#define WIFI_RX_PIN 9 // Pico RX pin to ESP-01 TX
#define WIFI_BAUDRATE 115200

// WiFi network credentials
#define WIFI_SSID "Aly2.4"       // Replace with your WiFi SSID
#define WIFI_PASSWORD "Aly@1234" // Replace with your WiFi password

// WebSocket Server
#define WEBSOCKET_SERVER "192.168.1.11" // Replace with your WebSocket server IP
#define WEBSOCKET_PORT 3000

// Function Prototypes
void setup_wifi();
void wifi_send_command(const char *command, char *response, size_t response_size, float sleep_time);
void connect_to_wifi();
void connect_to_websocket();
void send_message(char *message);
void receive_message(char *message, size_t max_length);
#endif // WIFI_DRIVER_H
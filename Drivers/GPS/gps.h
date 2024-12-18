#ifndef GPS_H
#define GPS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/uart.h"
#include "pico/stdlib.h"

// Define UART pins and settings for the GPS module
#define GPS_UART_ID uart1
#define GPS_TX_PIN 4  // Pico TX pin connected to GPS RX
#define GPS_RX_PIN 5  // Pico RX pin connected to GPS TX
#define GPS_BAUDRATE 9600

typedef struct {
    double latitude;
    double longitude;
    bool valid;
} GpsCoordinates;

// Function prototypes
void setup_gps(void);
void read_gps_data(char *buffer, size_t buffer_size);
char *extract_gpgll_sentence(const char *raw_data, char *buffer, size_t buffer_size);
GpsCoordinates parse_gpgll_coordinates(const char *gpgll);
GpsCoordinates get_current_coords(void);
#endif  // GPS_H
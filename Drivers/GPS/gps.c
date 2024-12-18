#include "gps.h"

void setup_gps() {
    // Initialize UART for GPS module
    uart_init(GPS_UART_ID, GPS_BAUDRATE);
    uart_set_hw_flow(GPS_UART_ID, false, false);
    uart_set_format(GPS_UART_ID, 8, 1, UART_PARITY_NONE);

    // Set TX and RX pins
    gpio_set_function(GPS_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPS_RX_PIN, GPIO_FUNC_UART);
}

void read_gps_data(char *buffer, size_t buffer_size) {
    size_t index = 0;
    uint32_t timeout = to_ms_since_boot(get_absolute_time()) + 500;  // 2-second timeout

    while (to_ms_since_boot(get_absolute_time()) < timeout && index < buffer_size - 1) {
        if (uart_is_readable(GPS_UART_ID)) {
            buffer[index++] = uart_getc(GPS_UART_ID);
        }
    }

    buffer[index] = '\0';  // Null-terminate the buffer
}

char *extract_gpgll_sentence(const char *raw_data, char *buffer, size_t buffer_size) {
    // Initialize output buffer
    if (buffer_size < 1) return NULL;
    buffer[0] = '\0';

    // Find GPGLL sentence
    const char *gpgll = strstr(raw_data, "$GPGLL");
    if (!gpgll) return NULL;

    // Copy until newline or end
    size_t i = 0;
    while (i < buffer_size - 1 && gpgll[i] != '\0' && gpgll[i] != '\n' && gpgll[i] != '\r') {
        buffer[i] = gpgll[i];
        i++;
    }
    buffer[i] = '\0';

    // Validate minimum length and checksum presence
    if (i < 10 || !strchr(buffer, '*')) {
        buffer[0] = '\0';
        return NULL;
    }
    return buffer;
}

GpsCoordinates parse_gpgll_coordinates(const char *gpgll) {
    GpsCoordinates coords = {0.0, 0.0, false};

    // Verify GPGLL format
    if (!gpgll || strncmp(gpgll, "$GPGLL", 6) != 0) {
        return coords;
    }

    char copy[100];
    strncpy(copy, gpgll, 99);
    copy[99] = '\0';

    char *token = strtok(copy, ",");
    char *fields[8] = {NULL};
    int field = 0;

    // Split into fields
    while (token && field < 8) {
        fields[field++] = token;
        token = strtok(NULL, ",");
    }

    // Need at least 5 fields for valid coordinates
    if (field < 5) {
        return coords;
    }

    // Parse latitude (DDMM.MMMM)
    double lat_deg = atof(fields[1]) / 100.0;
    lat_deg = (int)lat_deg + (lat_deg - (int)lat_deg) * 100.0 / 60.0;
    if (fields[2][0] == 'S') lat_deg = -lat_deg;

    // Parse longitude (DDDMM.MMMM)
    double lon_deg = atof(fields[3]) / 100.0;
    lon_deg = (int)lon_deg + (lon_deg - (int)lon_deg) * 100.0 / 60.0;
    if (fields[4][0] == 'W') lon_deg = -lon_deg;

    coords.latitude = lat_deg;
    coords.longitude = lon_deg;
    coords.valid = true;

    return coords;
}

GpsCoordinates get_current_coords() {
    char gps_buffer[256];
    read_gps_data(gps_buffer, sizeof(gps_buffer));
    if (gps_buffer[0] != '\0') {
        char gpgll[256];
        extract_gpgll_sentence(gps_buffer, gpgll, sizeof(gpgll));
        if (gpgll[0] != '\0') {
            GpsCoordinates coords = parse_gpgll_coordinates(gpgll);
            if (coords.valid) {
                return coords;
            } else {
                return (GpsCoordinates){0.0, 0.0, false};
            }
        }
    }
}
#include <stdio.h>
#include "Drivers/LED/led.h"
#include "Drivers/GSM/gsm.h"
#include "Drivers/GPS/gps.h"
#include "Drivers/Buzzer/Buzzer.h"
#include "Drivers/Vibration/Vibration.h"
#include "Wifi/wifi.h"

void test_led() {
    printf("Testing LED\n");
    setup_led();
    for (int i = 0; i < 3; i++) {
        led_on();
        sleep_ms(500);
        led_off();
        sleep_ms(500);
    }
}

void test_buzzer() {
    printf("Testing Buzzer\n");
    buzzer_init();
    buzzer_tone(2000, 1000); // Play a 2 kHz tone for 2 seconds
}

void test_gsm() {
    printf("Testing GSM\n");
    setup_gsm();
    send_sms("+201018833175", "Hello, this is a test SMS from Raspberry Pi Pico!");
}

void test_gps() {
    printf("Testing GPS\n");
    setup_gps();
    sleep_ms(1000); // Wait for the GPS module to initialize
    char gps_buffer[256]; // Buffer to store GPS data
    for(int i = 0; i < 10; i++) {
        read_gps_data(gps_buffer, sizeof(gps_buffer));
        if (gps_buffer[0] != '\0') {
            char gpgll[256];
            extract_gpgll_sentence(gps_buffer, gpgll, sizeof(gpgll));
            if (gpgll[0] != '\0') {
                GpsCoordinates coords = parse_gpgll_coordinates(gpgll);
                if (coords.valid) {
                    printf("Latitude: %f, Longitude: %f\n", coords.latitude, coords.longitude);
                } else {
                    i--; // Retry reading the GPS data
                }
            }
        }
        sleep_ms(100); // Wait 1 second before reading again
    }
}

void test_vibration_sensor() {
    printf("Testing Vibration Sensor\n");
    initVibrationSensor();
    for (int i = 0; i < 5; i++) {
        int sensorValue = readVibrationSensorValue();
        printf("Vibration Sensor Value: %d\n", sensorValue);
        sleep_ms(100); // Read the sensor every 100 ms
    }
}

void test_wifi() {
    printf("Testing WiFi\n");
    setup_wifi();
    char message[1000];
    for(int i = 0; i < 6; i++) {
        printf("Sending message %d\n", i);
        send_message("Hello from Raspberry Pi Pico!");
        sleep_ms(1000); // Send a message every second
    }
}

int main() {
    char gps_buffer[256]; // Buffer to store GPS data
    stdio_init_all(); // Initialize all of the present standard stdio types that are linked into the binary

    sleep_ms(10000); // Wait for the serial port to initialize

    // Test the LED
    test_led();

    // Test the Buzzer
    test_buzzer();

    // Test the Vibration Sensor
    test_vibration_sensor();

    // Test the GSM module
    test_gsm();

    // Test the GPS module
    test_gps();

    // Test the WiFi module
    test_wifi();

    while(1){
        sleep_ms(1000);
    }

    return 0;
}
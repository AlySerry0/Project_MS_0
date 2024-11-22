#include "led_driver.h"

int main() {
    char gps_buffer[256]; // Buffer to store GPS data
    stdio_init_all(); // Initialize all of the present standard stdio types that are linked into the binary

   
    setup_led(); // Initialize the LED
    setup_gsm(); // Initialize the GSM module
    setup_gps(); // Initialize the GPS module

    while (1) {
        led_on();         // Turn LED ON
        sleep_ms(1000);   // Wait for 1 second
        led_off();        // Turn LED OFF
        sleep_ms(1000);   // Wait for 1 second
    }

     while (1) {
        send_sms("+1234567890", "Hello, this is a test SMS from Raspberry Pi Pico!");
        sleep_ms(10000); // Send an SMS every 10 seconds
    }

    while (1) {
        // Read GPS data
        read_gps_data(gps_buffer, sizeof(gps_buffer));

        // Print the raw GPS data (NMEA sentences)
        printf("GPS Data: %s\n", gps_buffer);

        sleep_ms(1000); // Read data every second
    }

    return 0;
}

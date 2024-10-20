#include <stdio.h>
#include "pico/stdlib.h"
#include "led.h"

int main() {
    // Initialize stdio and LEDs
    stdio_init_all();
    led_init();

    while (true) {
        // LEDs off for the first 5 seconds
        sleep_ms(5000);

        // Red LED on for 1 second
        led_on(RED_LED_PIN);
        sleep_ms(1000);
        led_off(RED_LED_PIN);

        // Green LED on for 1 second
        led_on(GREEN_LED_PIN);
        sleep_ms(1000);
        led_off(GREEN_LED_PIN);

        // Blue LED on for 1 second
        led_on(BLUE_LED_PIN);
        sleep_ms(1000);
        led_off(BLUE_LED_PIN);

        // All LEDs on for 2 seconds
        led_on(RED_LED_PIN);
        led_on(GREEN_LED_PIN);
        led_on(BLUE_LED_PIN);
        sleep_ms(2000);
        led_off(RED_LED_PIN);
        led_off(GREEN_LED_PIN);
        led_off(BLUE_LED_PIN);
    }
}
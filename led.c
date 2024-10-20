#include "led.h"

// Initialize the GPIO pins for the LEDs
void led_init(void) {
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
    gpio_put(BLUE_LED_PIN, true); // Turn off the blue LED
}

// Turn on the specified LED
void led_on(uint led_pin) {
    if (led_pin == BLUE_LED_PIN) {
        gpio_put(led_pin, false); // Negative logic for blue LED
    } else {
        gpio_put(led_pin, true);
    }
}

// Turn off the specified LED
void led_off(uint led_pin) {
    if (led_pin == BLUE_LED_PIN) {
        gpio_put(led_pin, true); // Negative logic for blue LED
    } else {
        gpio_put(led_pin, false);
    }
}
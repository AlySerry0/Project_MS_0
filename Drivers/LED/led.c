#include "led.h"

void setup_led() {
    gpio_init(LED_PIN);           // Initialize the GPIO pin
    gpio_set_dir(LED_PIN, GPIO_OUT); // Set the pin as output
}

void led_on() {
    gpio_put(LED_PIN, 1); // Turn the LED ON
}

void led_off() {
    gpio_put(LED_PIN, 0); // Turn the LED OFF
}
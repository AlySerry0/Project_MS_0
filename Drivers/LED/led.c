#include "led.h"

void setup_led() {
    gpio_init(LED_PIN);               // Initialize the GPIO pin
    gpio_set_dir(LED_PIN, GPIO_OUT);  // Set the pin as output
}

void led_on() {
    gpio_put(LED_PIN, 1);  // Turn the LED ON
}

void led_off() {
    gpio_put(LED_PIN, 0);  // Turn the LED OFF
}

void led_alarm() {
    for (int i = 0; i < 10; i++) {
        led_on();
        sleep_ms(100);
        led_off();
        sleep_ms(100);
    }
}
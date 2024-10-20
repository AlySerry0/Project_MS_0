#ifndef LED_H
#define LED_H

#include "pico/stdlib.h"

// Define LED pins
#define RED_LED_PIN 2
#define GREEN_LED_PIN 3
#define BLUE_LED_PIN 4

// Function prototypes
void led_init(void);
void led_on(uint led_pin);
void led_off(uint led_pin);

#endif // LED_H
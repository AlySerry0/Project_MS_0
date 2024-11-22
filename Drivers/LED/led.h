#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "pico/stdlib.h"

// Define the GPIO pin connected to the LED
#define LED_PIN 16

// Function Prototypes
void setup_led(void);
void led_on(void);
void led_off(void);

#endif // LED_DRIVER_H
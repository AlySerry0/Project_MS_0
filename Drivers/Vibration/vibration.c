#include "vibration.h"

void initVibrationSensor() {
    // Initialize the GPIO for the sensor pin (ADC input)
    adc_init();                      // Initialize ADC hardware
    adc_gpio_init(VIBRATION_SENSOR_PIN_A);        // Initialize GPIO pin as ADC input
}

int readVibrationSensorValue() {
    adc_select_input(0);              // Select ADC0 (GPIO 26 by default)
    uint16_t sensorValue = adc_read(); // Read the 12-bit value (0-4095)
    return sensorValue;                         // Return the sensor value
}

#ifndef VIBRATION_H
#define VIBRATION_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

// Pin definitions
#define VIBRATION_SENSOR_PIN_A   26   // GPIO pin for the vibration sensor (ADC0)

//Function prototypes
void initVibrationSensor(void);
int readVibrationSensorValue(void);

#endif
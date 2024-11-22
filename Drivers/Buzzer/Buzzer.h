#include "pico/stdlib.h"
#include "C:/VSARM/sdk/pico/pico-sdk/src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "hardware/clocks.h"
#ifndef BUZZER_PIN 
#define BUZZER_PIN 15
#endif

void buzzer_init(void);
void buzzer_tone(uint16_t frequency, unsigned long duration );
void buzzer_no_tone();

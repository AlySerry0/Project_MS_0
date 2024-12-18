#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#ifndef BUZZER_PIN
#define BUZZER_PIN 15
#endif

void buzzer_init(void);
void buzzer_tone(uint16_t frequency, unsigned long duration);
void buzzer_no_tone();

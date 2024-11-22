#include "buzzer.h"

uint pwm_slice = 0;


void buzzer_init(void){
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(BUZZER_PIN);
}

void buzzer_tone(uint16_t frequency, unsigned long duration ){
    uint32_t clock_freq = clock_get_hz(clk_sys); 
    uint32_t wrap = clock_freq / frequency - 1;
    pwm_set_wrap(pwm_slice, wrap);
    pwm_set_gpio_level(BUZZER_PIN, wrap / 2); 
    pwm_set_enabled(pwm_slice, true);

    if (duration > 0) {
        sleep_ms(duration);
        buzzer_no_tone();
    }
}

void buzzer_no_tone(){
    pwm_set_enabled(pwm_slice, false);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_SIO);
    gpio_put(BUZZER_PIN, false);

}








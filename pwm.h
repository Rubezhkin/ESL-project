#ifndef PWM_H
#define PWM_H
#include <stdint.h>

void pwm_init(void);
void pwm_set_duty(uint16_t duty);
void pwm_ramp_start(void);
void pwm_ramp_stop(void);
void pwm_set_led(uint8_t led);

#endif
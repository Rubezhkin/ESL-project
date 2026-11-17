#include "pwm.h"
#include "nrfx_pwm.h"
#include "app_error.h"
#include "leds.h"
#include "app_timer.h"
#include <stdint.h>

static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

#define PWM_TOP_VALUE 16000

static nrf_pwm_values_individual_t  pwm_values[] =
{
	{
    0x8000 | PWM_TOP_VALUE,
    0x8000 | PWM_TOP_VALUE,
    0x8000 | PWM_TOP_VALUE,
    0x8000 | PWM_TOP_VALUE
	}
};

static nrf_pwm_sequence_t const pwm_sequence =
{
    .values.p_individual  = pwm_values,
    .length = NRF_PWM_VALUES_LENGTH(pwm_values),
    .repeats = 0,
    .end_delay = 0
};

APP_TIMER_DEF(pwm_timer);

#define PWM_STEP 400
#define PWM_INTERVAL APP_TIMER_TICKS(10)

static uint16_t current_duty = 0;
static int8_t direction = 1;

static uint8_t current_led = 0;

static const uint8_t led_cycles[] = {6, 5, 9, 5};
static uint8_t completed_cycles = 0;

static void pwm_timer_handler(void * p_context)
{
    (void)p_context;

    if (direction > 0)
    {
        if (current_duty >= PWM_TOP_VALUE)
        {
            direction = -1;
        }
        else
        {
            current_duty += PWM_STEP;
        }
    }
    else
    {
        if (current_duty <= 0)
        {
            direction = 1;
            completed_cycles++;

            if (completed_cycles >= led_cycles[current_led])
            {
                completed_cycles = 0;
                current_led++;

                if (current_led >= NUMBER_OF_LEDS)
                {
                    current_led = 0;
                }
            }
        }
        else
        {
            current_duty -= PWM_STEP;
        }
    }

    pwm_set_duty(current_duty);
}

void pwm_init(void)
{
    nrfx_pwm_config_t config =
{
    .output_pins =
	{
    	LED_1 | NRFX_PWM_PIN_INVERTED,
    	LED_2 | NRFX_PWM_PIN_INVERTED,
    	LED_3 | NRFX_PWM_PIN_INVERTED,
    	LED_4 | NRFX_PWM_PIN_INVERTED
	},
    .irq_priority = 6,
    .base_clock = NRF_PWM_CLK_16MHz,
    .count_mode = NRF_PWM_MODE_UP,
    .top_value = PWM_TOP_VALUE,
    .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
    .step_mode = NRF_PWM_STEP_AUTO
};

    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config, NULL));
	APP_ERROR_CHECK(
    app_timer_create(
        &pwm_timer,
        APP_TIMER_MODE_REPEATED,
        pwm_timer_handler
    )
);
    nrfx_pwm_simple_playback(
        &m_pwm0,
        &pwm_sequence,
        1,
        NRFX_PWM_FLAG_LOOP
    );
}

void pwm_set_duty(uint16_t duty)
{
    if (duty > PWM_TOP_VALUE)
    {
        duty = PWM_TOP_VALUE;
    }

    uint16_t off = 0x8000 | PWM_TOP_VALUE;

    pwm_values[0].channel_0 = off;
    pwm_values[0].channel_1 = off;
    pwm_values[0].channel_2 = off;
    pwm_values[0].channel_3 = off;

    uint16_t value = 0x8000 | (PWM_TOP_VALUE - duty);

    switch (current_led)
    {
        case 0:
            pwm_values[0].channel_0 = value;
            break;

        case 1:
            pwm_values[0].channel_1 = value;
            break;

        case 2:
            pwm_values[0].channel_2 = value;
            break;

        case 3:
            pwm_values[0].channel_3 = value;
            break;
    }
}

void pwm_ramp_start(void)
{
    app_timer_start(pwm_timer, PWM_INTERVAL, NULL);
}

void pwm_ramp_stop(void)
{
    app_timer_stop(pwm_timer);
}
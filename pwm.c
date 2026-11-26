#include "pwm.h"
#include "nrfx_pwm.h"
#include "app_error.h"
#include "leds.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "color.h"
#include "input_mode.h"
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

#define PWM_STEP 200
#define PWM_INTERVAL APP_TIMER_TICKS(10)

static uint16_t current_duty = 0;
static int8_t direction = 1;

static uint16_t current_hue;
static uint8_t current_saturation = 100;
static uint8_t current_brightness = 100;

static void pwm_timer_handler(void * p_context)
{
    (void)p_context;

    set_duty();

    pwm_set_duty(current_duty);
}

void set_duty(void)
{
    switch (current_mode)
    {
        case MODE_IDLE:
            current_duty = 0;
            break;

        case MODE_HUE:
            current_duty += PWM_STEP * direction;
            break;

        case MODE_SATURATION:
            current_duty += 2 * PWM_STEP * direction;
            break;

        case MODE_BRIGHTNESS:
            current_duty = PWM_TOP_VALUE;
            break;
    }

    if (current_duty >= PWM_TOP_VALUE)
    {
        current_duty = PWM_TOP_VALUE;
        direction = -1;
    }
    else if (current_duty == 0)
    {
        direction = 1;
    }
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

    current_hue = get_default_hue();
    update_rgb();

    nrfx_pwm_simple_playback(
        &m_pwm0,
        &pwm_sequence,
        1,
        NRFX_PWM_FLAG_LOOP
    );

    APP_ERROR_CHECK(
    app_timer_create(
        &pwm_timer,
        APP_TIMER_MODE_REPEATED,
        pwm_timer_handler
    )
    );

    app_timer_start(pwm_timer, PWM_INTERVAL, NULL);
}

void update_rgb(void)
{
    rgb_t rgb = hsv_to_rgb(current_hue, current_saturation, current_brightness);
    pwm_values[0].channel_0 = 0x8000 | PWM_TOP_VALUE;
    pwm_values[0].channel_1 = 0x8000 | (100-rgb.R)*PWM_TOP_VALUE/100;
    pwm_values[0].channel_2 = 0x8000 | (100-rgb.G)*PWM_TOP_VALUE/100;
    pwm_values[0].channel_3 = 0x8000 | (100-rgb.B)*PWM_TOP_VALUE/100;
}

void pwm_set_duty(uint16_t duty)
{
    if (duty > PWM_TOP_VALUE)
    {
        duty = PWM_TOP_VALUE;
    }

    uint16_t value = 0x8000 | (PWM_TOP_VALUE - duty);

    pwm_values[0].channel_0 = value;
}

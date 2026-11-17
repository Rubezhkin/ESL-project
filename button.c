
#include "button.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "app_error.h"
#include "app_timer.h"
#include "pwm.h"
#include <stdbool.h>
#include <stdint.h>

#define BUTTON_1 NRF_GPIO_PIN_MAP(1, 6)
#define IS_PRESSED 0

APP_TIMER_DEF(double_click_timer);

static volatile uint8_t click_count = 0;
static volatile bool sequence_running = false;

static void double_click_timeout(void * p_context)
{
    (void)p_context;
    click_count = 0;
}

static void button_handler(nrfx_gpiote_pin_t pin,
                           nrf_gpiote_polarity_t action)
{
    (void)action;

    if (nrf_gpio_pin_read(pin) != IS_PRESSED)
    {
        return;
    }

    click_count++;

    if (click_count == 1)
    {
        app_timer_start(
            double_click_timer,
            APP_TIMER_TICKS(400),
            NULL
        );
    }
    else if (click_count == 2)
    {
        app_timer_stop(double_click_timer);
        click_count = 0;

        sequence_running = !sequence_running;

        if (sequence_running)
        {
            pwm_ramp_start();
        }
        else
        {
            pwm_ramp_stop();
        }
    }
}

void button_init(void)
{
    ret_code_t err_code;

    err_code = app_timer_create(
        &double_click_timer,
        APP_TIMER_MODE_SINGLE_SHOT,
        double_click_timeout
    );
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_config_t config =
        NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);

    config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrfx_gpiote_in_init(
        BUTTON_1,
        &config,
        button_handler
    );
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}

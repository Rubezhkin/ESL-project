#include "button.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "app_error.h"
#include "app_timer.h"
#include "pwm.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "input_mode.h"
#include <stdbool.h>
#include <stdint.h>

#define BUTTON_1 NRF_GPIO_PIN_MAP(1, 6)
#define IS_PRESSED 0

APP_TIMER_DEF(double_click_timer);
APP_TIMER_DEF(debounce_timer);

static volatile uint8_t click_count = 0;
static volatile bool button_is_pressed = false;

static void double_click_timeout(void * p_context)
{
    (void)p_context;
    click_count = 0;
}

static void debounce_timeout(void * p_context)
{
    (void)p_context;

    if (nrf_gpio_pin_read(BUTTON_1) == IS_PRESSED)
    {
        ret_code_t err_code = app_timer_start(debounce_timer, APP_TIMER_TICKS(50), NULL);
        APP_ERROR_CHECK(err_code);
        return;
    }

    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}

static void button_handler(nrfx_gpiote_pin_t pin,
                           nrf_gpiote_polarity_t action)
{
    (void)action;

    if (nrf_gpio_pin_read(pin) != IS_PRESSED)
    {
        return;
    }

    nrfx_gpiote_in_event_disable(BUTTON_1);
    ret_code_t err_code = app_timer_start(debounce_timer, APP_TIMER_TICKS(50), NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Click");

    click_count++;

    if (click_count == 1)
    {
        app_timer_start(double_click_timer, APP_TIMER_TICKS(400), NULL);
    }
    else if (click_count == 2)
    {
        NRF_LOG_INFO("Double click");

        app_timer_stop(double_click_timer);
        click_count = 0;

        next_mode();
        NRF_LOG_INFO("Mode: %d", (unsigned int)current_mode);
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

    err_code = app_timer_create(
        &debounce_timer,
        APP_TIMER_MODE_SINGLE_SHOT,
        debounce_timeout
    );
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);

    config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrfx_gpiote_in_init(
        BUTTON_1,
        &config,
        button_handler
    );
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}

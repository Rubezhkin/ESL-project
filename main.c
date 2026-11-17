
#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "leds.h"
#include "button.h"
#include "pwm.h"

static void logs_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    logs_init();

    NRF_LOG_INFO("Application started");

    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
    }

    NRF_LOG_INFO("LFCLK started");

    APP_ERROR_CHECK(app_timer_init());

    NRF_LOG_INFO("App timer initialized");

    pwm_init();

    NRF_LOG_INFO("PWM initialized");

    button_init();

    NRF_LOG_INFO("Button initialized");

    while (true)
    {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();

        static uint32_t counter = 0;
        static uint32_t last_log = 0;

        if (counter - last_log >= 1000000)
        {
            last_log = counter;
            NRF_LOG_INFO("USB logging test");
        }

        counter++;
    }
}

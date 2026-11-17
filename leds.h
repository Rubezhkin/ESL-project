#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include "nrf_gpio.h"

#define NUMBER_OF_LEDS 4
#define LED_1 NRF_GPIO_PIN_MAP(0, 6)
#define LED_2 NRF_GPIO_PIN_MAP(0, 8)
#define LED_3 NRF_GPIO_PIN_MAP(1, 9)
#define LED_4 NRF_GPIO_PIN_MAP(0, 12)

#endif
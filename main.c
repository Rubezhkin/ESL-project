#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define LED_1 NRF_GPIO_PIN_MAP(0, 6)
#define LED_2 NRF_GPIO_PIN_MAP(0, 8)
#define LED_3 NRF_GPIO_PIN_MAP(1, 9)
#define LED_4 NRF_GPIO_PIN_MAP(0, 12)

#define BUTTON_1 NRF_GPIO_PIN_MAP(1, 6)

static uint8_t turn_on_led(uint8_t ind, const uint8_t id_digits[])
{
    if(ind < id_digits[0]){
    nrf_gpio_pin_write(LED_1,0);
    return 0;
    }
    ind -= id_digits[0];

    if(ind < id_digits[1]){
    nrf_gpio_pin_write(LED_2,0);
    return 1;
    }
    ind -= id_digits[1];

    if(ind < id_digits[2]){
    nrf_gpio_pin_write(LED_3,0);
    return 2;
    }
    ind -= id_digits[2];

    nrf_gpio_pin_write(LED_4,0);
    return 3;
}

static void turn_off_led(uint8_t ind){
    switch(ind){
        case 0:
        nrf_gpio_pin_write(LED_1,1);
        break;
        case 1:
        nrf_gpio_pin_write(LED_2,1);
        break;
        case 2:
        nrf_gpio_pin_write(LED_3,1);
        break;
        case 3:
        nrf_gpio_pin_write(LED_4,1);
        break;
        default:
        break;
    }
}

int main(void)
{
    
    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_pin_write(LED_1,1);
    nrf_gpio_cfg_output(LED_2);
    nrf_gpio_pin_write(LED_2,1);
    nrf_gpio_cfg_output(LED_3);
    nrf_gpio_pin_write(LED_3,1);
    nrf_gpio_cfg_output(LED_4);
    nrf_gpio_pin_write(LED_4,1);

    nrf_gpio_cfg_input(BUTTON_1,NRF_GPIO_PIN_PULLUP);

    const uint8_t id_digits[4] = {6,5,9,5};
    uint8_t ind = 0;
    uint8_t max_ind = 0;
    for(uint8_t i = 0; i < 4; i++){
        max_ind += id_digits[i];
    }

    uint8_t num_led;
    bool led_on = false;

    while (true)
    {
        if(nrf_gpio_pin_read(BUTTON_1)==0 && !led_on){
            num_led = turn_on_led(ind, id_digits);
            nrf_delay_ms(500);
            led_on = true;
        }
        else if(nrf_gpio_pin_read(BUTTON_1)==0 && led_on){
            turn_off_led(num_led);
            nrf_delay_ms(500);
            ind = (ind+1)%max_ind;
            led_on = false;
        }
        else{
            nrf_delay_ms(10);
        }
    }
}

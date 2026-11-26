#include "color.h"

#define device_id 6595

rgb_t hsv_to_rgb(uint16_t H, uint8_t S, uint8_t V)
{
    uint8_t H_i = (H / 60) % 6;

    uint8_t V_min = (100 - S) * V / 100;
    uint8_t Alpha = (V - V_min) * (H % 60) / 60;

    uint8_t V_sec = (H_i % 2 == 1)
                    ? V - Alpha
                    : V_min + Alpha;

    rgb_t rgb = {0, 0, 0};

    switch (H_i)
    {
        case 0:
            rgb = (rgb_t){V, V_sec, V_min};
            break;

        case 1:
            rgb = (rgb_t){V_sec, V, V_min};
            break;

        case 2:
            rgb = (rgb_t){V_min, V, V_sec};
            break;

        case 3:
            rgb = (rgb_t){V_min, V_sec, V};
            break;

        case 4:
            rgb = (rgb_t){V_sec, V_min, V};
            break;

        case 5:
            rgb = (rgb_t){V, V_min, V_sec};
            break;
    }

    return rgb;
}

uint16_t get_default_hue(void){
	return (device_id%100)*360/100;
}
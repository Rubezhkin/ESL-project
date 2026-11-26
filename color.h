#ifndef COLOR
#define COLOR
#include <stdint.h>
typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} rgb_t;
 	rgb_t hsv_to_rgb(uint16_t H, uint8_t S, uint8_t V);
	uint16_t get_default_hue(void);
#endif
#include "input_mode.h"

input_mode current_mode = MODE_IDLE;

void next_mode(void)
{
    switch (current_mode)
    {
        case MODE_IDLE:
            current_mode = MODE_HUE;
            break;

        case MODE_HUE:
            current_mode = MODE_SATURATION;
            break;

        case MODE_SATURATION:
            current_mode = MODE_BRIGHTNESS;
            break;

        case MODE_BRIGHTNESS:
            current_mode = MODE_IDLE;
            break;
    }
}
#ifndef INPUT_MODE
#define INPUT_MODE
typedef enum
{
    MODE_IDLE,
    MODE_HUE,
    MODE_SATURATION,
    MODE_BRIGHTNESS
} input_mode;

extern input_mode current_mode;
void next_mode(void);
#endif
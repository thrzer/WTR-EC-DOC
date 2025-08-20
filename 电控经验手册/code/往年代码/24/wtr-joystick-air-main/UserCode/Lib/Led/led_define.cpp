#include "led_define.hpp"

RgbLed KeyboardLed(&htim3, TIM_CHANNEL_4, &htim2, TIM_CHANNEL_1, &htim3, TIM_CHANNEL_1);

#pragma once

#include "gpio.h"
#include <vector>

namespace GpioLib
{
    typedef struct
    {
        GPIO_TypeDef *GPIOx;
        uint16_t GPIO_Pin;
    } Gpio_t;

    GPIO_PinState ReadPinAntiShake(Gpio_t &gpio, uint32_t sample_time_us);
    GPIO_PinState ReadPinAntiShake(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t sample_time_us);

    void ReadPinsAntiShake(std::vector<Gpio_t> &gpios, std::vector<GPIO_PinState> &result, uint32_t sample_time_us);

} // namespace GpioLib

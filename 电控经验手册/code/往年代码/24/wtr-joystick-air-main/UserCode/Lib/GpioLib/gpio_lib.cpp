#include "gpio_lib.hpp"
#include "HighPrecisionTime/high_precision_time.h"

GPIO_PinState GpioLib::ReadPinAntiShake(Gpio_t &gpio, uint32_t sample_time_us)
{
    return ReadPinAntiShake(gpio.GPIOx, gpio.GPIO_Pin, sample_time_us);
}

GPIO_PinState GpioLib::ReadPinAntiShake(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t sample_time_us)
{
    uint32_t pin_sum   = 0;
    uint32_t sum_count = 0;
    auto until_us      = HPT_GetUs() + sample_time_us;

    while (HPT_GetUs() <= until_us) {
        pin_sum += HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
        sum_count++;
    }

    return (pin_sum > sum_count / 2) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void GpioLib::ReadPinsAntiShake(std::vector<Gpio_t> &gpios, std::vector<GPIO_PinState> &result, uint32_t sample_time_us)
{
    result.resize(gpios.size());
    std::vector<uint32_t> pin_sum(gpios.size(), 0);
    uint32_t sum_count = 0;
    auto prev_us       = HPT_GetUs();

    while (HPT_GetUs() - prev_us <= sample_time_us) {
        for (size_t i = 0; i < gpios.size(); i++) {
            pin_sum.at(i) += HAL_GPIO_ReadPin(gpios.at(i).GPIOx, gpios.at(i).GPIO_Pin);
        }
        sum_count++;
    }

    for (size_t i = 0; i < gpios.size(); i++) {
        result.at(i) = (pin_sum.at(i) > sum_count / 2) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }
}

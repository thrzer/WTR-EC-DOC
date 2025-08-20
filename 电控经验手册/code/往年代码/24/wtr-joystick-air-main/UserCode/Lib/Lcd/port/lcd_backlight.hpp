#pragma once

#include "Led/led.hpp"

class LcdBacklight : Led
{
public:
    LcdBacklight(TIM_HandleTypeDef *htim, uint32_t channel)
        : Led(htim, channel){};

    void Init()
    {
        Led::Init();
    }

    /**
     * @brief 设置背光强度
     *
     * @param lightness 范围：[0,1]，0 为全黑，1为最亮
     */
    void SetBacklight(float lightness)
    {
        SetLightness(lightness);
    }
};

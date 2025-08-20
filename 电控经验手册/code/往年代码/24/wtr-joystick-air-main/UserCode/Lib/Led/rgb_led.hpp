#pragma once

#include "led.hpp"

class RgbLed
{
public:
    Led r_, g_, b_;

    RgbLed(TIM_HandleTypeDef *r_htim, uint32_t r_channel,
           TIM_HandleTypeDef *g_htim, uint32_t g_channel,
           TIM_HandleTypeDef *b_htim, uint32_t b_channel)
        : r_(r_htim, r_channel), g_(g_htim, g_channel), b_(b_htim, b_channel){};

    void Init()
    {
        r_.Init();
        g_.Init();
        b_.Init();
    }

    /**
     * @brief 设置灯的颜色，颜色范围 [0, 1]
     *
     * @param r 红
     * @param g 绿
     * @param b 蓝
     * @param light_factor 亮度系数，大于 1 会更亮，小于 1 会更暗
     */
    void SetColor(float r, float g, float b, float light_factor = 1)
    {
        r_.SetLightness(r * light_factor);
        g_.SetLightness(g * light_factor);
        b_.SetLightness(b * light_factor);
    }

    /**
     * @brief 灯的电源开关
     *
     * @param is_on 是否开启电源
     */
    void Power(bool is_on)
    {
        r_.Power(is_on);
        g_.Power(is_on);
        b_.Power(is_on);
    }
};

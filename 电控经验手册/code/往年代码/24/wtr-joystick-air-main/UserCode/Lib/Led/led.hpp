#pragma once

#include "tim.h"

class Led
{
private:
    TIM_HandleTypeDef *_htim;
    uint32_t _channel;

public:
    Led(TIM_HandleTypeDef *htim, uint32_t channel)
        : _htim(htim), _channel(channel){};

    void Init()
    {
        if (_htim != nullptr) {
            HAL_TIM_PWM_Start(_htim, _channel);
        }
    }

    void Deinit()
    {
        if (_htim != nullptr) {
            HAL_TIM_PWM_Stop(_htim, _channel);
        }
    }

    /**
     * @brief 设置灯的亮度
     *
     * @param lightness 范围：[0,1]，0 为全黑，1为最亮
     */
    void SetLightness(float lightness)
    {
        lightness = (lightness < 0) ? 0 : lightness;
        lightness = (lightness > 1) ? 1 : lightness;
        __HAL_TIM_SET_COMPARE(_htim, _channel, __HAL_TIM_GET_AUTORELOAD(_htim) * lightness);
    }

    /**
     * @brief 灯的电源开关
     *
     * @param is_on
     */
    void Power(bool is_on)
    {
        if (is_on) {
            HAL_TIM_PWM_Start(_htim, _channel);
        } else {
            HAL_TIM_PWM_Stop(_htim, _channel);
        }
    }
};

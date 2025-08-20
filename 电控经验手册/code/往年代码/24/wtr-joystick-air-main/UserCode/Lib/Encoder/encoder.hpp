#pragma once
#include <cstdint>
#include "gpio.h"
#include <array>

class Encoder
{
private:
    typedef struct
    {
        GPIO_TypeDef *port;
        uint16_t pin;
    } Gpio_t;

    typedef std::array<bool, 2> States_t;

    const std::array<Gpio_t, 2> channels;
    States_t prev_states;

    volatile int32_t count_        = 0; // 编码器计数值
    volatile uint32_t error_count_ = 0; // 编码器错误计数。当此值较大时，检查是否同时启用两相的上升下降沿中断、信号质量、编码器质量

    States_t ReadChannel() const
    {
        States_t result;
        for (size_t i = 0; i < result.size(); i++) {
            result.at(i) = HAL_GPIO_ReadPin(channels.at(i).port, channels.at(i).pin);
        }

        return result;
    }

public:
    Encoder(GPIO_TypeDef *A_GPIO_Port, uint16_t A_GPIO_Pin, GPIO_TypeDef *B_GPIO_Port, uint16_t B_GPIO_Pin)
        : channels{A_GPIO_Port, A_GPIO_Pin, B_GPIO_Port, B_GPIO_Pin} {};

    /**
     * @brief 初始化。请在调用该类的其他方法前初始化
     *
     */
    void Init()
    {
        prev_states = ReadChannel();
    }

    /**
     * @brief 编码器计数值
     *
     */
    std::remove_volatile<decltype(count_)>::type Count() const
    {
        return count_;
    }

    /**
     * @brief 编码器错误计数值
     *
     * @note 当此值较大时，检查是否同时启用两相的上升下降沿中断、信号质量、编码器质量
     */
    std::remove_volatile<decltype(error_count_)>::type ErrorCount() const
    {
        return error_count_;
    }

    /**
     * @brief 中断回调函数，请在 A, B 相的上升沿和下降沿调用此函数
     * @note 理论上以高频率轮询调用此函数也可以正常工作，不过不建议这样做
     */
    void ExtiCallback()
    {
        auto now_states = ReadChannel();

        bool isAChanged = prev_states.at(0) ^ now_states.at(0);
        bool isBChanged = prev_states.at(1) ^ now_states.at(1);

        if ((!isAChanged) && (!isBChanged)) {
            return;
        }

        if (isAChanged && isBChanged) {
            error_count_++;
            return;
        }

        // 判断方向
        if (prev_states.at(0) ^ prev_states.at(1) ^ isAChanged) {
            count_--;
        } else {
            count_++;
        }
        prev_states = now_states;
    }
};

#pragma once

#include "Adc/adc_class.hpp"

class Battery
{
private:
    Adc *adc_;
    size_t adc_index_;
    // 根据电阻分压，理论值为 11.5 / 1.5，但实际误差较大，实测需要再添加一个校正系数
    float voltage_ratio_ = 11.5 / 1.5 * 7.72 / 7.41;

public:
    Battery(Adc *adc, size_t index)
        : adc_(adc), adc_index_(index){};
    float GetVoltage()
    {
        return adc_->GetVoltage(adc_index_) * voltage_ratio_;
    }
};

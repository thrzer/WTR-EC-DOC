#pragma once

#include "adc_class.hpp"

extern Adc Adc1;
extern Adc Adc2;
extern Adc Adc3;

/**
 * @brief 获取内核温度
 * 
 * @return float 摄氏度
 */
float GetCoreTemperature();

#include "adc_class_define.hpp"

Adc Adc1(&hadc1);
Adc Adc2(&hadc2);
Adc Adc3(&hadc3);

float GetCoreTemperature()
{
    return Adc3.GetTemperature(2);
}

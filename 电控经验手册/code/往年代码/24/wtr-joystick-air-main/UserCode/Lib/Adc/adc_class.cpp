#include "Adc/adc_class.hpp"
#include "adc_class.hpp"

uint16_t Adc::CalcMaxRange() const
{
    // 计算 max_range_
    switch (hadc_->Init.Resolution) {
#ifdef ADC_RESOLUTION_6B
        case ADC_RESOLUTION_6B:
            return 0x3f;
            break;
#endif
#ifdef ADC_RESOLUTION_8B
        case ADC_RESOLUTION_8B:
            return 0xff;
            break;
#endif
#ifdef ADC_RESOLUTION_10B
        case ADC_RESOLUTION_10B:
            return 0x3ff;
            break;
#endif
#ifdef ADC_RESOLUTION_12B
        case ADC_RESOLUTION_12B:
            return 0xfff;
            break;
#endif
#ifdef ADC_RESOLUTION_14B
        case ADC_RESOLUTION_14B:
            return 0x3fff;
            break;
#endif
#ifdef ADC_RESOLUTION_16B
        case ADC_RESOLUTION_16B:
            return 0xffff;
            break;
#endif
#ifdef ADC_RESOLUTION_14B_OPT
        case ADC_RESOLUTION_14B_OPT:
            return 0x3fff;
            break;
#endif
#ifdef ADC_RESOLUTION_12B_OPT
        case ADC_RESOLUTION_12B_OPT:
            return 0xfff;
            break;
#endif

        default:
            return 0;
            break;
    }
}

void Adc::Init()
{
    if (is_inited_)
        return;

    is_inited_ = true;

    number_of_conversion_ = hadc_->Init.NbrOfConversion;
    adc_data_             = new std::remove_reference<decltype(*adc_data_)>::type[number_of_conversion_];
    max_range_            = CalcMaxRange();

    Calibrate();
}

std::vector<float> Adc::GetAllNormalizedData() const
{
    std::vector<float> result(number_of_conversion_);
    InvalidateDCache();
    for (size_t i = 0; i < number_of_conversion_; i++) {
        result.at(i) = (float)adc_data_[i] / max_range_;
    }

    return result;
}

std::vector<float> Adc::GetAllVoltage() const
{
    std::vector<float> result(number_of_conversion_);
    InvalidateDCache();
    for (size_t i = 0; i < number_of_conversion_; i++) {
        result.at(i) = (float)adc_data_[i] / max_range_ * vref_;
    }
    return result;
}

float Adc::GetTemperature(size_t temperature_sensor_index)
{
    /**
     * Calculation formula:
     *     Temperature = ((TS_ADC_DATA - TS_CAL1)
     *                     * (TS_CAL2_TEMP - TS_CAL1_TEMP))
     *                   / (TS_CAL2 - TS_CAL1) + TS_CAL1_TEMP
     *     with TS_ADC_DATA = temperature sensor raw data measured by ADC
     *          Avg_Slope = (TS_CAL2 - TS_CAL1)
     *                      / (TS_CAL2_TEMP - TS_CAL1_TEMP)
     *          TS_CAL1   = equivalent TS_ADC_DATA at temperature
     *                      TEMP_DEGC_CAL1 (calibrated in factory)
     *          TS_CAL2   = equivalent TS_ADC_DATA at temperature
     *                      TEMP_DEGC_CAL2 (calibrated in factory)
     */
    static const float FACTOR          = (float)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) / (*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR);
    static const float TEMPSENSOR_CAL1 = *TEMPSENSOR_CAL1_ADDR;
    static const float VOLTAGE_FACTOR  = vref_ * 1000.0f / TEMPSENSOR_CAL_VREFANALOG;

    return (__LL_ADC_CONVERT_DATA_RESOLUTION(GetData(temperature_sensor_index), hadc_->Init.Resolution, LL_ADC_RESOLUTION_16B) * VOLTAGE_FACTOR - TEMPSENSOR_CAL1) * FACTOR + TEMPSENSOR_CAL1_TEMP;
}

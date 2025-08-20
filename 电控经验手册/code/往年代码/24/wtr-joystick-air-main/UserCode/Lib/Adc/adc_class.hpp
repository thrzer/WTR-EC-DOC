#pragma once

#include "adc.h"
#include <vector>

/**
 * ADC 需要在 CubeMX 里先配置好，以下参数需要配置：（其他随意）
 * 单端
 * Continuous Conversion Mode: Enable
 * End Of Conversion Selection: End of sequence of conversion
 * Overrun behaviour: Overrun data overwritten
 * Conversion Data Management Mode: DMA Circular Mode（需要先在 DMA Settings 里添加 DMA 后才能选择该项）
 *
 * DMA Setting 配置：
 * Circular, Half Word
 * 推荐：Use Fifo, Threshold 根据 ADC 的 Number of Conversion 选择，使得 Fifo 中的大小刚好是一个转换序列的大小。
 * （DMA Fifo 总容量为 4 Words (即 32 Bytes)，因此 Half Full = 16 Bytes，以此类推。
 */

class Adc
{
private:
    ADC_HandleTypeDef *hadc_;
    const float vref_;
    uint16_t *adc_data_;
    uint8_t number_of_conversion_ = 0;
    bool is_inited_               = false;
    uint16_t max_range_;

    uint16_t CalcMaxRange() const;

    void InvalidateDCache() const
    {
        SCB_InvalidateDCache_by_Addr(adc_data_, sizeof(*adc_data_) * number_of_conversion_);
    }

public:
    /**
     *
     * @param hadc
     * @param vref 参考电压 (V)
     */
    Adc(ADC_HandleTypeDef *hadc, float vref = 3.3)
        : hadc_(hadc), vref_(vref){};

    /**
     * @brief 初始化 ADC
     * @note 该函数内部会调用一次 Calibrate() 来校准 ADC
     */
    void Init();

    /**
     * @brief 校准 ADC
     *
     */
    void Calibrate()
    {
        // 校准 ADC （不同 stm32 型号该函数可能不一样或不存在，注意修改）
        HAL_ADCEx_Calibration_Start(hadc_, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED); // For STM32H7
    }

    void StartDma()
    {
        // 需要先初始化才能调用该函数
        while (is_inited_ == false) {}
        HAL_ADC_Start_DMA(hadc_, (uint32_t *)adc_data_, number_of_conversion_);
    }

    void StopDma()
    {
        HAL_ADC_Stop_DMA(hadc_);
    }

    /**
     * @brief 获取 ADC 读数取值范围的最大值
     */
    uint16_t GetMaxRange() const
    {
        return max_range_;
    }

    /**
     * @brief 获取 ADC 原始数据
     *
     * @param index [0, NbrOfConversion - 1]
     */
    std::remove_reference<decltype(*adc_data_)>::type GetData(size_t index) const
    {
        if (index < number_of_conversion_) {
            InvalidateDCache();
            return adc_data_[index];
        } else {
            return 0;
        }
    }

    std::vector<std::remove_reference<decltype(*adc_data_)>::type> GetAllData() const
    {
        std::vector<std::remove_reference<decltype(*adc_data_)>::type> result(number_of_conversion_);
        InvalidateDCache();
        for (size_t i = 0; i < number_of_conversion_; i++) {
            result.at(i) = adc_data_[i];
        }
        return result;
    }

    /**
     * @brief 获取归一化数据
     *
     * @param index 范围：[0, NbrOfConversion - 1]
     * @return float 归一化数据，范围：[0, 1]
     */
    float GetNormalizedData(size_t index) const
    {
        return (float)GetData(index) / max_range_;
    }

    std::vector<float> GetAllNormalizedData() const;

    /**
     * @brief Get voltage (V)
     *
     * @param index [0, NbrOfConversion - 1]
     * @return float Voltage
     */
    float GetVoltage(size_t index) const
    {
        return (float)GetData(index) / max_range_ * vref_;
    }

    std::vector<float> GetAllVoltage() const;

    /**
     * @brief 获取参考电压 (V)
     *
     */
    float GetVref() const
    {
        return vref_;
    }

    /**
     * @brief 计算内部温度传感器的温度（ADC3 有一个 Temperature Sensor Channel，使用这个通道可以读取芯片温度）
     * @param temperature_sensor_index Temperature Sensor Channel 对应的 index
     *
     * @return 温度（摄氏度）
     */
    float GetTemperature(size_t temperature_sensor_index);

    ~Adc()
    {
        if (adc_data_ != nullptr) {
            delete[] adc_data_;
        }
    };
};

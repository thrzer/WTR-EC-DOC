#pragma once
#include "Adc/adc_class.hpp"

class Joystick
{
public:
    // 用于零点校正
    float x_max    = 1;
    float x_middle = 0.5;
    float x_min    = 0;

    float y_max    = 1;
    float y_middle = 0.5;
    float y_min    = 0;

    typedef struct
    {
        float x;
        float y;
    } Point_t;

    Joystick(Adc *adc, size_t index_x, size_t index_y, bool reverse_x = false, bool reverse_y = false)
        : adc_(adc), index_x_(index_x), index_y_(index_y), reverse_x_(reverse_x), reverse_y_(reverse_y){};

    void Init()
    {
        adc_->Init();
        adc_->StartDma();
    }

    Point_t Pos()
    {
        auto all_normalized_data = adc_->GetAllNormalizedData();
        Point_t result;

        if (reverse_x_ == false) {
            result.x = CalcCalibratedPos(all_normalized_data.at(index_x_), x_min, x_middle, x_max);
        } else {
            result.x = -CalcCalibratedPos(all_normalized_data.at(index_x_), x_min, x_middle, x_max);
        }

        if (reverse_y_ == false) {
            result.y = CalcCalibratedPos(all_normalized_data.at(index_y_), y_min, y_middle, y_max);
        } else {
            result.y = -CalcCalibratedPos(all_normalized_data.at(index_y_), y_min, y_middle, y_max);
        }

        return result;
    }

    Point_t RawPos()
    {
        auto all_normalized_data = adc_->GetAllNormalizedData();
        Point_t result;
        result.x = all_normalized_data.at(index_x_);
        result.y = all_normalized_data.at(index_y_);

        return result;
    }

private:
    Adc *adc_;
    size_t index_x_;
    size_t index_y_;
    bool reverse_x_;
    bool reverse_y_;

    /**
     * @brief 校正零点漂移和极值范围
     *
     * @param value 原始值
     * @param min 最小值
     * @param middle 中心值
     * @param max 最大值
     * @return float 校正后的位置，范围：[-1, 1]
     */
    float CalcCalibratedPos(float value, float min, float middle, float max)
    {
        float temp = value - middle;
        if (temp > 0) {
            temp = temp / (max - middle);
            return temp <= 1 ? temp : 1;
        } else {
            temp = temp / (middle - min);
            return temp >= -1 ? temp : -1;
        }
    }
};

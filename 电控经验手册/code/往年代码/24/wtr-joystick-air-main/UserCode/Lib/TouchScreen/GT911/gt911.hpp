#pragma once

#include "main.h"
#include "i2c.h"
#include <array>

namespace Gt911Regs
{
    const uint16_t RegStart        = 0x8047;
    const uint16_t ProductID_0     = 0x8140;
    const uint16_t CoordinateState = 0x814E;
    const uint16_t Points[5]       = {0x814F, 0x8157, 0x815F, 0x8167, 0x816F};
}

class GT911
{
private:
    const uint8_t device_id_;
    I2C_HandleTypeDef *const hi2c_;
    GPIO_TypeDef *const int_port_;
    const uint16_t int_pin_;
    GPIO_TypeDef *const rst_port_;
    const uint16_t rst_pin_;
    char product_id_[4];
    uint8_t coordinate_state_    = 0;
    uint8_t number_of_cordinate_ = 0;

    typedef struct TouchPoint {
        uint8_t track_id;
        uint16_t x;
        uint16_t y;
        uint16_t size;
    } TouchPoint_t;

    struct __packed ConfigData {
        uint8_t version;
        uint8_t x_output_max_low;
        uint8_t x_output_max_high;
        uint8_t y_output_max_low;
        uint8_t y_output_max_high;
        uint8_t touch_number;
        uint8_t module_switch1;
        uint8_t module_switch2;
        uint8_t shake_count;
    } config_data_;

    /**
     * @brief 写寄存器
     *
     * @param _usRegAddr
     * @param _pRegBuf
     * @param _ucLen
     * @return true 成功
     * @return false 失败
     */
    bool WriteReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen) const
    {
        if (HAL_I2C_Mem_Write(hi2c_, device_id_, _usRegAddr, I2C_MEMADD_SIZE_16BIT, _pRegBuf, _ucLen, 0xff) == HAL_OK)
            return true;
        else
            return false;
    }

    /**
     * @brief 读寄存器
     *
     * @param _usRegAddr
     * @param _pRegBuf
     * @param _ucLen
     * @return true 成功
     * @return false 失败
     */
    bool ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen) const
    {
        if (HAL_I2C_Mem_Read(hi2c_, device_id_, _usRegAddr, I2C_MEMADD_SIZE_16BIT, _pRegBuf, _ucLen, 0xff) == HAL_OK)
            return true;
        else
            return false;
    }

    void SetIntPinToInput() const
    {
        GPIO_InitTypeDef GPIO_InitStruct = {};
        GPIO_InitStruct.Pin              = int_pin_;
        GPIO_InitStruct.Mode             = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull             = GPIO_NOPULL;
        HAL_GPIO_Init(int_port_, &GPIO_InitStruct);
    }

    void SetIntPinToOutput() const
    {
        GPIO_InitTypeDef GPIO_InitStruct = {};
        GPIO_InitStruct.Pin              = int_pin_;
        GPIO_InitStruct.Mode             = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull             = GPIO_NOPULL;
        GPIO_InitStruct.Speed            = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(int_port_, &GPIO_InitStruct);
    }

    void SetRstPin(bool level) const
    {
        if (level) {
            HAL_GPIO_WritePin(rst_port_, rst_pin_, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(rst_port_, rst_pin_, GPIO_PIN_RESET);
        }
    }

    void SetIntPin(bool level) const
    {
        if (level) {
            HAL_GPIO_WritePin(int_port_, int_pin_, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(int_port_, int_pin_, GPIO_PIN_RESET);
        }
    }

    void Reset()
    {
        switch (device_id_) {
            case 0xBA:
                SetIntPin(0);
                SetIntPinToOutput();
                SetRstPin(0);
                HAL_Delay(5);
                SetRstPin(1);
                HAL_Delay(10);
                SetIntPinToInput();
                HAL_Delay(100);
                break;

            case 0x28:
                SetIntPin(0);
                SetIntPinToOutput();
                SetRstPin(0);
                HAL_Delay(5);
                SetIntPin(1);
                HAL_Delay(5);
                SetRstPin(1);
                HAL_Delay(10);
                SetIntPinToInput();
                HAL_Delay(100);
                break;

            default:
                break;
        }
    }

    bool ReadProductId()
    {
        return ReadReg(Gt911Regs::ProductID_0, (uint8_t *)product_id_, 4);
    }

    bool ReadConfig()
    {
        return ReadReg(Gt911Regs::RegStart, (uint8_t *)&config_data_, sizeof(config_data_));
    }

    bool GetCoordinateState()
    {
        return ReadReg(Gt911Regs::CoordinateState, &coordinate_state_, 1);
    }

    bool SetCoordinateState(uint8_t status)
    {
        return WriteReg(Gt911Regs::CoordinateState, &status, 1);
    }

public:
    std::array<TouchPoint_t, 5> touch_points;

    /**
     * @brief GT911 触摸屏
     *
     * @param hi2c i2c handle
     * @param int_port 中断引脚
     * @param int_pin 中断引脚
     * @param rst_port 重置引脚
     * @param rst_pin 重置引脚
     * @param device_id 设备id，可选值为 0xBA 或 0x28
     */
    GT911(
        I2C_HandleTypeDef *hi2c,
        GPIO_TypeDef *int_port, uint16_t int_pin,
        GPIO_TypeDef *rst_port, uint16_t rst_pin,
        uint8_t device_id = 0xBA)
        : device_id_(device_id), hi2c_(hi2c), int_port_(int_port), int_pin_(int_pin), rst_port_(rst_port), rst_pin_(rst_pin){};

    bool Init()
    {
        Reset();

        if (ReadProductId() != true) {
            return false;
        }

        if (ReadConfig() != true) {
            return false;
        }

        return true;
    }

    uint8_t ConfigVersion() const
    {
        return config_data_.version;
    }

    uint16_t MaxOutput_X() const
    {
        uint16_t result;
        result = (config_data_.x_output_max_high << 8) | (config_data_.x_output_max_low);
        return result;
    }

    uint16_t MaxOutput_Y() const
    {
        uint16_t result;
        result = (config_data_.y_output_max_high << 8) | (config_data_.y_output_max_low);
        return result;
    }

    uint8_t MaxTouchNum() const
    {
        return config_data_.touch_number;
    }

    uint8_t NumberOfTouchPoint() const
    {
        return number_of_cordinate_;
    }

    bool UpdateTouch()
    {
        if (GetCoordinateState() != true) {
            return false;
        }

        if ((coordinate_state_ & 0x80) != 0) {
            number_of_cordinate_ = coordinate_state_ & 0x0F;
            if (number_of_cordinate_ != 0) {
                if (number_of_cordinate_ > touch_points.size()) {
                    number_of_cordinate_ = touch_points.size();
                }

                for (size_t i = 0; i < number_of_cordinate_; i++) {
                    uint8_t buffer[7];
                    if (ReadReg(Gt911Regs::Points[i], buffer, 7) != true) { return false; }
                    touch_points.at(i).track_id = buffer[0];
                    touch_points.at(i).x        = (buffer[2] << 8) | buffer[1];
                    touch_points.at(i).y        = (buffer[4] << 8) | buffer[3];
                    touch_points.at(i).size     = (buffer[6] << 8) | buffer[5];
                }
            }

            // 读取完后需要手动刷新 IC 中的 state
            SetCoordinateState(0);
        }
        return true;
    }
};

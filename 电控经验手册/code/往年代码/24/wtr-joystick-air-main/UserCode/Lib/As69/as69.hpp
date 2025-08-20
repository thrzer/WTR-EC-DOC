#pragma once

#include "main.h"
#include "usart.h"

class As69
{
private:
    GPIO_TypeDef *const md1_gpio_port_;
    const uint16_t md1_gpio_pin_;
    const uint8_t CMD_READ_CONFIG[3] = {0xC1, 0XC1, 0XC1};
    const uint8_t CMD_SET_CONFIG[1]  = {0xC0};
    uint32_t baudrate_               = 0;

    bool SetBaudRate(uint32_t new_baud_rate);

public:
    struct __packed ConfigData {
        uint8_t byte0;
        uint8_t addrH;
        uint8_t addrL;
        uint8_t speed;
        uint8_t chan;
        uint8_t option;
    } config_data_;
    enum class Mode {
        Working, // 串口打开，无线打开，全双工传输
        Sleep    // 无线进入休眠状态，可以接收参数配置命令，进行参数配置
    };

    enum class Parity {
        None,
        Odd,
        Even
    };

    UART_HandleTypeDef *const huart_;

    As69(UART_HandleTypeDef *huart, GPIO_TypeDef *MD1_GPIO_Port, uint16_t MD1_GPIO_Pin)
        : md1_gpio_port_(MD1_GPIO_Port), md1_gpio_pin_(MD1_GPIO_Pin), huart_(huart){};

    bool Init();

    void SetMode(Mode mode);

    Mode GetMode();

    /**
     * @brief 从模块中读取配置，读取之后会存在私有变量中，之后可以调用 GetModuleXXX() 函数获得参数
     * @note 必须在 Mode::Sleep 时才能读取
     * @param timeout 读取的超时时间
     * @return true 读取成功
     * @return false 读取失败
     */
    bool ReadConfig(uint32_t timeout = 1000);

    bool WriteConfig(uint16_t addr, int32_t baud_rate, Parity parity, uint8_t wireless_channel, uint8_t send_power, uint8_t io_cfg);

    /**
     * @brief
     *
     * @return uint32_t 波特率
     */
    uint32_t GetModuleBaudRate() const;

    /**
     * @brief 获取奇偶校验配置
     *
     * @return Parity
     */
    Parity GetModuleParity() const;

    uint8_t GetModuleWirelessChannel() const
    {
        uint8_t data = (config_data_.chan & 0x7);
        return data;
    }

    uint16_t GetModuleAddress() const
    {
        return (config_data_.addrH << 8) | config_data_.addrL;
    }

    /**
     *
     * 0x0：20dBm（默认）
     * 0x1：12dBm
     * 0x2：10dBm
     * 0x3：5dBm
     *
     */
    uint8_t GetModuleSendPower() const
    {
        uint8_t data = (config_data_.option & 0x3);
        return data;
    }

    /**
     * 0：TXD、AUX 开路输出，RXD 开路输入
     * 1：TXD、AUX 推挽输出，RXD 上拉输入（默认）
     */
    bool GetModuleIoDrivingMode() const
    {
        return (config_data_.option & 0x40) != 0;
    }

    bool Read(uint8_t *pData, uint16_t Size, uint32_t Timeout)
    {
        return HAL_UART_Receive(huart_, pData, Size, Timeout) == HAL_OK;
    }

    bool Write(const uint8_t *pData, uint16_t Size, uint32_t Timeout)
    {
        return HAL_UART_Transmit(huart_, pData, Size, Timeout) == HAL_OK;
    }
};

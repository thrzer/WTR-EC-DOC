#include "as69.hpp"

bool As69::SetBaudRate(uint32_t new_baud_rate)
{
    if (new_baud_rate == 0) return false;
    huart_->Init.BaudRate = new_baud_rate;
    bool result           = (HAL_UART_Init(huart_) == HAL_OK);
    if (result == true) {
        HAL_Delay(200);
    }

    return result;
}

bool As69::Init()
{
    baudrate_ = huart_->Init.BaudRate;

    if (GetMode() != Mode::Sleep) {
        SetMode(Mode::Sleep);
    } else {
        SetBaudRate(9600);
    }

    if (ReadConfig() == false) return false;
    baudrate_ = GetModuleBaudRate();
    SetMode(Mode::Working);
    return true;
}

void As69::SetMode(Mode mode)
{
    switch (mode) {
        case Mode::Working:
            SetBaudRate(baudrate_);
            HAL_GPIO_WritePin(md1_gpio_port_, md1_gpio_pin_, GPIO_PIN_RESET);
            HAL_Delay(200);
            SetBaudRate(baudrate_);
            break;
        case Mode::Sleep:
            HAL_GPIO_WritePin(md1_gpio_port_, md1_gpio_pin_, GPIO_PIN_SET);
            HAL_Delay(200);
            SetBaudRate(9600);
            break;
        default:
            break;
    }
}

As69::Mode As69::GetMode()
{
    if (HAL_GPIO_ReadPin(md1_gpio_port_, md1_gpio_pin_) == GPIO_PIN_RESET) {
        return Mode::Working;
    } else {
        return Mode::Sleep;
    }
}

bool As69::ReadConfig(uint32_t timeout)
{
    if (GetMode() != Mode::Sleep) {
        return false;
    }

    Write(CMD_READ_CONFIG, sizeof(CMD_READ_CONFIG), HAL_MAX_DELAY);
    return Read((uint8_t *)&config_data_, sizeof(config_data_), timeout);
}

As69::ConfigData cfg_data;

bool As69::WriteConfig(uint16_t addr, int32_t baud_rate, Parity parity, uint8_t wireless_channel, uint8_t send_power, uint8_t io_cfg)
{
    if (GetMode() != Mode::Sleep) {
        return false;
    }

    uint8_t baud_rate_code;

    switch (baud_rate) {
        case 1200:
            baud_rate_code = 0;
            break;
        case 2400:
            baud_rate_code = 1;
            break;
        case 4800:
            baud_rate_code = 2;
            break;
        case 9600:
            baud_rate_code = 3;
            break;
        case 19200:
            baud_rate_code = 4;
            break;
        case 38400:
            baud_rate_code = 5;
            break;
        case 57600:
            baud_rate_code = 6;
            break;
        case 115200:
            baud_rate_code = 7;
            break;
        default:
            return false;
            break;
    }

    uint8_t parity_code;

    switch (parity) {
        case Parity::None:
            parity_code = 0;
            break;
        case Parity::Odd:
            parity_code = 1;
            break;
        case Parity::Even:
            parity_code = 2;
            break;
        default:
            return false;
            break;
    }

    cfg_data.byte0 = CMD_SET_CONFIG[0];
    cfg_data.addrH = addr >> 8;
    cfg_data.addrL = addr & 0xFF;
    cfg_data.chan  = wireless_channel & 0xF;

    cfg_data.speed  = (baud_rate_code << 3) | (parity_code << 6);
    cfg_data.option = (send_power & 0x3) | ((io_cfg & 1) << 6);
    Write((uint8_t *)&cfg_data, sizeof(cfg_data), HAL_MAX_DELAY);
    uint8_t response[2] = {};
    Read(response, sizeof(response), 1000);
    if (response[0] == 'O' && response[1] == 'K') {
        config_data_ = cfg_data;
        baudrate_    = baud_rate;
        Read(response, sizeof(response), 50); // 清除多余的消息
        return true;
    }
    for (size_t i = 0; i < 5; i++) {
        Read(response, sizeof(response), 50); // 清除多余的消息
    }

    return false;
}

uint32_t As69::GetModuleBaudRate() const
{
    uint8_t data = (config_data_.speed & 0x38) >> 3;
    switch (data) {
        case 0x0:
            return 1200;
            break;
        case 0x1:
            return 2400;
            break;
        case 0x2:
            return 4800;
            break;
        case 0x3:
            return 9600;
            break;
        case 0x4:
            return 19200;
            break;
        case 0x5:
            return 38400;
            break;
        case 0x6:
            return 57600;
            break;
        case 0x7:
            return 115200;
            break;
        default:
            return 0;
            break;
    }
}

As69::Parity As69::GetModuleParity() const
{
    uint8_t data = (config_data_.speed & 0xC0) >> 6;
    switch (data) {
        case 0x0:
            return Parity::None;
            break;
        case 0x1:
            return Parity::Odd;
            break;
        case 0x2:
            return Parity::Even;
            break;
        case 0x3:
            return Parity::None;
            break;
        default:
            return Parity::None;
            break;
    }
}

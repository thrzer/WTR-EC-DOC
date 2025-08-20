#include "laser_sdk.h"
#include "userconfig.h"
#include <stdio.h>

__IO int chassis_laser_x;
__IO int chassis_laser_y;

uint8_t laser_x_data[5];
uint8_t laser_y_data[5];

static uint8_t _count = 0;
static uint8_t _j     = 0;
__IO uint8_t laser_temp[1];

/**
 * @brief   激光解码函数
 */
uint8_t Laser_Decode(void)
{
    HAL_UART_Receive_IT(&LASER_UART_HANDLE, (uint8_t *)laser_temp, 1);
    switch (_count) {
        case 0:
            if (laser_temp[0] == 0xCC)
                _count++;
            else
                _count = 0;
            break;
        case 1:
            laser_x_data[_j] = laser_temp[0];
            _j++;
            if (_j >= 5) {
                _j = 0;
                _count++;
            }
            break;
        case 2:
            laser_y_data[_j] = laser_temp[0];
            _j++;
            if (_j >= 5) {
                _j = 0;
                _count++;
            }
            break;
        case 3:
            if (laser_temp[0] == 0xDD) {
                chassis_laser_x = (laser_x_data[0] - 48) * 1000 + (laser_x_data[2] - 48) * 100 + (laser_x_data[3] - 48) * 10 + (laser_x_data[4] - 48);
                chassis_laser_y = (laser_y_data[0] - 48) * 1000 + (laser_y_data[2] - 48) * 100 + (laser_y_data[3] - 48) * 10 + (laser_y_data[4] - 48);
            }
            _count = 0;
            break;
        default:
            _count = 0;
            break;
    }
    return 0;
}

/**
 * @brief   码盘初始化函数
 */
void Laser_Init(void)
{
    HAL_UART_Receive_IT(&LASER_UART_HANDLE, (uint8_t *)&laser_temp, 1);
}

/**
 * @brief   串口回调函数
 */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == LASER_UART) {
        Laser_Decode();
    }
}

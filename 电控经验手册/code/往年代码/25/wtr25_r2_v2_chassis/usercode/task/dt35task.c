#include "allusertask.h"
#include "Laser.h"
bool IsDt35Detect = false;
/**
 * @brief 激光雷达DT35线程(最后暂时用作查询法接收陀螺仪数据)
 * @author thrzer
 * @date 2025-7-26
 */
void Dt35Task(void *argument)
{
    osDelay(2000);
    // Laser_rev_Init();
    while (true)
    {
        static uint8_t ucTemp = 0;
        WitSerialDataIn(ucTemp);
        HAL_UART_Receive(&huart7, &ucTemp, 1,1);
        osDelay(9);
    }
}
#include "cmsis_os.h"
#include "rmctl.h"
#include "steering_chassis.h"
#include "Laser.h"
#include "message.h"
#include "gyro.h"

/**
 * @brief 所有回调函数
 * @author thrzer
 * @date 2025-4-23
 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == RMCTL_UART) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);//遥控器更新回调函数
    }
    //陀螺仪回调
    static uint8_t ucTemp = 0;
    if (huart->Instance == HUART_CURRENT.Instance) {
        WitSerialDataIn(ucTemp);
        HAL_UART_Receive_IT(huart, &ucTemp, 1);
    }
    // if (huart->Instance == MESSAGE_UART) {
    //     message_decode();
    //     message_receive(huart);
    // }
    // if (huart->Instance == LASER_DATA_UART) {
    //     Msg_Write(&Laser_rev_byte,1);
    //     HAL_UART_Receive_IT(huart,&Laser_rev_byte,1);
    // }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    swChassis_EXTI_Callback(&swChassis, GPIO_Pin);
}
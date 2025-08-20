#include "message.h"

uint8_t message_buffer[MESSAGE_LENGTH];//数据缓冲
float v_data[3];//速度数据

/**
 * @brief 上层机构数据接收初始化
 * @note 使用中断前必须先调用一次中断接收函数进入中断
 */
void message_init()
{
    HAL_UART_Receive_IT(&MESSAGE_UART_HANDLE,message_buffer,sizeof(uint8_t)*MESSAGE_LENGTH);
}

/**
 * @brief 上层机构数据接收
 */
void message_receive(UART_HandleTypeDef* huart)
{
    HAL_UART_Receive_IT(&MESSAGE_UART_HANDLE,message_buffer,sizeof(uint8_t)*MESSAGE_LENGTH);
    // HAL_UART_Receive(huart,message_buffer,sizeof(uint8_t)*MESSAGE_LENGTH,1000);
}

/**
 * @brief 上层机构数据解码
 */
int message_decode()
{
    if(message_buffer[0] == 0xff && message_buffer[MESSAGE_LENGTH-1] == 0xfe)//帧头帧尾
    {
        v_data[0] = message_buffer[1]*0.01f;
        v_data[1] = message_buffer[2]*0.01f;
        v_data[2] = message_buffer[3]*0.01f;
    }
    else
        return -1;
    return 0;
}
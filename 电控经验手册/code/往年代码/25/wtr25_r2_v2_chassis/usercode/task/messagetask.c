#include "allusertask.h"
#include "stdbool.h"
#include "message.h"

/**
 * @brief 上层机构通信
 * @author thrzer
 * @date 2025-7-30
 */
void MessageTask(void *argument)
{
    message_init();
    while (true)
    {
        // message_receive(&huart7);
        // message_decode();
        // uint8_t data = 'a';
        // HAL_UART_Transmit(&huart7,&data,sizeof(data),1000);
        osDelay(50);
    }   
}
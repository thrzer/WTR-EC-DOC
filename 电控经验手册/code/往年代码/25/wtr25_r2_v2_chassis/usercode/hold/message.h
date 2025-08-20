#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file message.h
 * @author thrzer
 * @brief 上层机构通信
 * @date 2025-7-30
 */

#include "usart.h"

#define MESSAGE_LENGTH 5
#define MESSAGE_UART UART7
#define MESSAGE_UART_HANDLE huart7

void message_init();
void message_receive(UART_HandleTypeDef* huart);
int message_decode();

extern float v_data[3];

#ifdef __cplusplus
}
#endif

#endif
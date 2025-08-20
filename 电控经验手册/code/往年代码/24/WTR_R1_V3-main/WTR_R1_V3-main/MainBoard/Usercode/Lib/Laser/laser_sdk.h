#ifndef __LASER_SDK_H
#define __LASER_SDK_H

#include "stm32h7xx.h"
#include "usart.h"
/************************ 用户定义 ***************************/
#define LASER_UART_HANDLE huart6 // 串口选项
#define LASER_UART        USART6  // 串口选项

extern __IO int chassis_laser_x;
extern __IO int chassis_laser_y;

extern __IO uint8_t laser_temp[1];

/************************ 用户函数 *************************/
void Laser_Init(void);
uint8_t Laser_Decode(void);

#endif
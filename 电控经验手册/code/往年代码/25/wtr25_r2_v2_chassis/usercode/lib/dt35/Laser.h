#ifndef _LASER_H
#define _LASER_H

#include "allusertask.h"
#include "circular_buffer.h"
#include "usart.h"

#define LASER_DATA_UART_HANDLE huart8 // 串口选项
#define LASER_DATA_UART        UART8 // 串口选项

#define FRAME_HEADER1 0xAA
#define FRAME_HEADER2 0xBB
#define FRAME_TAIL1   0xCC
#define FRAME_TAIL2   0xDD

extern uint8_t Laser_rev_byte;
extern uint8_t Laser_rev_buffer[24];
extern float Laser_x ;
extern float Laser_y ;
extern uint32_t Laser_rawdata[4];

extern void Laser_rev_Init();
extern void Laser_Data_Decode();
extern void Laser_Buffer_Decode();

#endif 
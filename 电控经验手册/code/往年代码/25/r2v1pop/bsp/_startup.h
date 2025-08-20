//
// Created by tony on 23-11-4.
//

#ifndef _STARTUP_H
#define _STARTUP_H


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "FreeRTOS.h"
#define PLATFORM_REDUCTION 2.0f
#define ARRAY_LEN 30 // String 数组长度
#define JOINT_DOUBLE_LEN 4 //关节数量
#define PLATFORM3508_ID 4
// #define PLATFORM_REDUCTION 2
#define PI               3.14159265358979f
/*
LED 定义
红色LED: CHASSIS_ONError_Handler
绿色LED1: CHASSIS_ON_Handler
绿色LED2: CHASSIS_ONCorrecting_Handler
绿色LED3: CHASSIS_ONReady_Handler
绿色LED4: 亮起为microros 正在初始化(初始化失败)
绿色LED5: 亮起为宇树电机启动失败
绿色LED6: 亮起为飞特舵机启动失败
绿色LED7: 亮起为云台复位失败

*/


//extern int8_t platformls_flag;

void StartUp();
#ifdef __cplusplus
}
#endif
#endif //_STARTUP_H

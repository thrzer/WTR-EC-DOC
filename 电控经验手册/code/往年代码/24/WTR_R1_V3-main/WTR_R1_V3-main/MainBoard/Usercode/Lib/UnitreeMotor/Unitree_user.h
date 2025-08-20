//
// Created by liam on 2023/10/4.
//

#ifndef UNITREE_M8010_UNITREE_USER_H
#define UNITREE_M8010_UNITREE_USER_H
#ifdef __cplusplus
extern "C" {
#endif
#include "usermain.h"
#include "motor_control.h"
#include "usart.h"
/**************************************变量类型定义*********************************************/
typedef struct {
    UART_HandleTypeDef *huart;
    MOTOR_send cmd;
    MOTOR_recv data;
    double offset;
} UnitreeMotor;

extern UnitreeMotor unitree_motor_right;
extern UnitreeMotor unitree_motor_left;

/**************************************用户函数定义*********************************************/
HAL_StatusTypeDef Unitree_init(UnitreeMotor *MotorInstance, UART_HandleTypeDef *usartx, int id);
HAL_StatusTypeDef Unitree_UART_tranANDrev(UnitreeMotor *MotorInstance, unsigned char motor_id,
                                          unsigned char mode, float T, float W,
                                          float Pos, float K_P, float K_W);
HAL_StatusTypeDef Unitree_Encoder_Autoclibrating(UnitreeMotor *MotorInstance);

void Unitree_User_Init(void);

#endif // UNITREE_M8010_UNITREE_USER_H

#ifndef UNITREE_M8010_UNITREE_USER_H_
#define UNITREE_M8010_UNITREE_USER_H_
/************************************端口及常量定义**********************************************/
#define UNITREE_REDUCTION_RATE 6.33
#define _PI                    3.14159265358979323846

#define RS485_DE_GPIO_Port     GPIOE
#define RS485_DE_Pin           GPIO_PIN_0

#define RS485_RE_GPIO_Port     GPIOE
#define RS485_RE_Pin           GPIO_PIN_1

#define SET_485_DE_UP()        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)
#define SET_485_DE_DOWN()      HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)

#define SET_485_RE_UP()        HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET)
#define SET_485_RE_DOWN()      HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET)

#define UART_UNITREE_HANDLER   huart3

#ifdef __cplusplus
}
#endif
#endif // UNITREE_M8010_UNITREE_USER_H_
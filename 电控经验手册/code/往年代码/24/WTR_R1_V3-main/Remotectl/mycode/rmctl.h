#ifndef RMCTL_H
#define RMCTL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* @file rmctl.h
 * @author thrzer
 * @brief wtr一代车遥控器，改自学长代码(23年r1)
 * @date 2024-11-5
 */

#include "ashining_as69.h"

#define USE_FREERTOS 1

typedef struct {
    bool btn_LeftCrossUp;
    bool btn_LeftCrossDown;
    bool btn_LeftCrossLeft;
    bool btn_LeftCrossRight;
    bool btn_LeftCrossMid;
    bool btn_RightCrossUp;
    bool btn_RightCrossDown;
    bool btn_RightCrossLeft;
    bool btn_RightCrossRight;
    bool btn_RightCrossMid;
    bool btn_Btn0;
    bool btn_Btn1;
    bool btn_Btn2;
    bool btn_Btn3;
    bool btn_Btn4;
    bool btn_Btn5;
    bool btn_KnobL;
    bool btn_KnobR;
    bool btn_JoystickL;
    bool btn_JoystickR;
    bool left_switch;
    bool right_switch;
    float right_y;
    float right_x;
    float left_y;
    float left_x;
    int usr_right_y;
    int usr_right_x;
    int usr_left_y;
    int usr_left_x;
    float right_knob;
    float left_knob;
    float usr_right_knob;
    float usr_left_knob;
} rmctl_msg_t;      //所有按键和摇杆

typedef struct{
    rmctl_msg_t rmctl_msg;
    UART_HandleTypeDef* huartx;
} rmctl_t;          //遥控器
#define RMCTL_UART USART2

void rmctl_Init(rmctl_t* this);
void rmctl_decode(rmctl_msg_t* this);

extern rmctl_t rmctl;

#ifdef __cplusplus
}
#endif

#endif
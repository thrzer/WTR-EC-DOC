#ifndef RMCTL_H
#define RMCTL_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file rmctl.h
 * @author thrzer
 * @brief mavlink遥控器，改自学长代码(23年r1)
 * @date 2024-11-5
 */
/**
 * 使用前，需要在h文件和c文件的初始化函数中修改串口号
 * 如果不能进入串口中断回调函数，说明两个as69未配对
 * as69的通道、波特率和地址需要一致，进入中断函数但收不到信息可能是地址不匹配
 * 遥控器上的as69被内部的主控板固定，只能通过上位机软件(asds)修改连接下位机的as69(需要用串口转TTL模块)
 */
#include "ashining_as69.h"

#define USE_FREERTOS 1

#define JOYSTICKMAX 850.0f //摇杆输入最大值(usr_left_x最大值)

#define RMCTL_UART USART6 // 遥控器串口

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

void rmctl_Init(rmctl_t* this);
void rmctl_decode(rmctl_msg_t* this);

extern rmctl_t rmctl;

#ifdef __cplusplus
}
#endif

#endif
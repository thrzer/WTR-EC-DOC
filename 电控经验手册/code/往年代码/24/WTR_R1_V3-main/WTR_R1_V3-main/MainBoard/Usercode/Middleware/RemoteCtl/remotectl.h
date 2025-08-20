#ifndef __REMOTECTL_H
#define __REMOTECTL_H
#ifdef __cplusplus
extern "C" {
#endif
//  C
#include "ashining_as69.h"
#include "cmsis_os2.h"

extern bool btn_LeftCrossUp;
extern bool btn_LeftCrossDown;
extern bool btn_LeftCrossLeft;
extern bool btn_LeftCrossRight;
extern bool btn_LeftCrossMid;
extern bool btn_RightCrossUp;
extern bool btn_RightCrossDown;
extern bool btn_RightCrossLeft;
extern bool btn_RightCrossRight;
extern bool btn_RightCrossMid;
extern bool btn_Btn0;
extern bool btn_Btn1;
extern bool btn_Btn2;
extern bool btn_Btn3;
extern bool btn_Btn4;
extern bool btn_Btn5;
extern bool btn_KnobL;
extern bool btn_KnobR;
extern bool btn_JoystickL;
extern bool btn_JoystickR;
extern bool left_switch;
extern bool right_switch;
extern float right_y;
extern float right_x;
extern float left_y;
extern float left_x;
extern int usr_right_y;
extern int usr_right_x;
extern int usr_left_y;
extern int usr_left_x;
extern float right_knob;
extern float usr_right_knob;
extern float left_knob;
extern float usr_left_knob;

/**
 * @brief   遥控器初始化
 */
void m_RemoteCtl_Init(void);
/**
 * @brief   遥控器接收线程初始化
 */
void m_RemoteCtl_Task_Start(void);

#ifdef __cplusplus
}
#endif
//  C++
#endif
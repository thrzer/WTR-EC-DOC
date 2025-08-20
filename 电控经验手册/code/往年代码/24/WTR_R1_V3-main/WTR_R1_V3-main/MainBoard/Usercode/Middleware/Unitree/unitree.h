#ifndef __UNITREE_H
#define __UNITREE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"
#include "Unitree_user.h"

extern float unitree_motor_left_pos;
extern float unitree_motor_left_kp;
extern float unitree_motor_left_kw;
extern float unitree_motor_right_pos;
extern float unitree_motor_right_kp;
extern float unitree_motor_right_kw;

extern float unitree_left_pos;
extern float unitree_right_pos;

void m_Unitree_Init(void);
void m_Unitree_UART_Message_TaskStart(void);
void m_Unitree_Ctl_Message_TaskStart(void);

#ifdef __cplusplus
}
#endif

#endif
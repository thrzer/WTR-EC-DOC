#ifndef __AUTO_BALL_TASK_H
#define __AUTO_BALL_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"

// 取球射球相关状态
enum Auto_Ball_State {
    Auto_Ball_Ready, // 默认状态
    Auto_Ball_Wait,  // 等待坐标状态
    Auto_Ball_Go,    // 取球状态
};
extern enum Auto_Ball_State auto_ball_state;

extern uint8_t up_flag;
extern uint8_t go_flag;

extern osThreadId_t auto_ball_TaskHandle;

void m_auto_ball_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif
#ifndef __HANDLE_BALL_TASK_H
#define __HANDLE_BALL_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"

// 取球射球相关状态
enum Handle_Ball_State {
    Handle_Ball_Ready, // 射球默认状态
    Handle_Ball_Pick,  // 取球状态
    Handle_Ball_Fire,  // 射球状态
};
extern enum Handle_Ball_State handle_ball_state;

extern osThreadId_t handle_ball_TaskHandle;

void m_handle_ball_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif
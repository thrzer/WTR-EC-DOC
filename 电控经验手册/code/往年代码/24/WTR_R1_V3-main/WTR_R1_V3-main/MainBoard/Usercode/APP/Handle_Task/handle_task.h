#ifndef __HANDLE_TASK_H
#define __HANDLE_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"
#include "handle_seed_task.h"
#include "handle_idle_task.h"
#include "handle_ball_task.h"

// 手动模式状态
enum Handle_State {
    HANDLE_IDLE_MODE = 0,
    HANDLE_SEED_MODE = 1,
    HANDLE_BALL_MODE = 2,
};
extern enum Handle_State handle_state;

extern osThreadId_t handle_TaskHandle;

void m_handle_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif
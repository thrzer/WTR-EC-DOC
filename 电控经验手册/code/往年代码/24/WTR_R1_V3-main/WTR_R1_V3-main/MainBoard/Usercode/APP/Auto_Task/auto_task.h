#ifndef __AUTO_TASK_H
#define __AUTO_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"
#include "auto_seed_task.h"
#include "auto_idle_task.h"
#include "auto_ball_task.h"

// 手动模式状态
enum Auto_State {
    AUTO_IDLE_MODE = 0,
    AUTO_SEED_MODE = 1,
    AUTO_BALL_MODE = 2,
};

extern enum Auto_State auto_state;

extern osThreadId_t auto_TaskHandle;

void m_auto_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif
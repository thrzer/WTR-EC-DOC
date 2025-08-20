#ifndef __HANDLE_SEED_TASK_H
#define __HANDLE_SEED_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"

enum Handle_Seed_State {
    Handle_Seed_Ready,
    Handle_Seed_Grip,
    Handle_Seed_Plant,
};
extern enum Handle_Seed_State handle_seed_state;

extern int deposit_point[4];

extern osThreadId_t handle_seed_TaskHandle;

void m_handle_seed_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif
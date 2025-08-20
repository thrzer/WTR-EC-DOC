#ifndef __AUTO_SEED_TASK_H
#define __AUTO_SEED_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"

enum Auto_Seed_State {
    Auto_Seed_Ready,
    Auto_Seed_Go,
};
extern enum Auto_Seed_State auto_seed_state;

extern osThreadId_t auto_seed_TaskHandle;

void m_auto_seed_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif
#ifndef __USERMAIN_TASK_H
#define __USERMAIN_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C
#include "usermain.h"

// 方向状态，初始化指定后不可更改
enum General_State {
    LEFT_MODE  = 0,
    RIGHT_MODE = 1
};
extern enum General_State general_state;

// 运行状态
enum Run_State{
    AUTO_MODE = 0,
    HANDLE_MODE = 1,
};
extern enum Run_State run_state;

extern int seed_count;

// 接口函数

int motor_wait_to_finish(float feedback, float setpoint);
void m_main_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++
#endif
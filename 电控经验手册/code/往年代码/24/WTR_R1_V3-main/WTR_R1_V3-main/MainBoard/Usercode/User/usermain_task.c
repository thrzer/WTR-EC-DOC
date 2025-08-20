#include "usermain_task.h"

enum General_State general_state; // 总方向状态定义，初始化结束后不可改变
enum Run_State run_state;
int seed_count;

/*****************************************************************************
 *  线程定义
 */
osThreadId_t main_TaskHandle;
const osThreadAttr_t main_Task_attributes = {
    .name       = "main_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_main_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   总线程初始化
 */
void m_main_Task_Start(void)
{
    seed_count      = 0;
    run_state       = HANDLE_MODE;
    main_TaskHandle = osThreadNew(m_main_Task, NULL, &main_Task_attributes);
}

/*****************************************************************************
 * 辅助函数
 */
int motor_wait_to_finish(float feedback, float setpoint)
{
    if (((feedback - setpoint) < -5.0f) || ((feedback - setpoint) > 5.0f)) {
        return 0;
    } else {
        return 1;
    }
}

/*****************************************************************************
 * 线程函数定义
 */
/**
 * @brief   总线程
 */
void m_main_Task(void *argument)
{
    m_auto_Task_Start();
    m_handle_Task_Start();
    for (;;) {
        // 自动/手动状态转换
        switch (run_state) {
            case AUTO_MODE:
                osThreadSuspend(handle_TaskHandle);
                osThreadResume(auto_TaskHandle);
                JoystickSwitchTitle(ID_RUN, run_title, &mav_run_title);
                JoystickSwitchMsg(ID_RUN, run_auto_msg, &mav_run_msg);
                // 自动优先级较低，任何情况下如果有切换至手动指令将强行切换至手动空闲状态
                if (left_switch == 0) {
                    osThreadSuspend(auto_seed_TaskHandle);
                    osThreadSuspend(auto_ball_TaskHandle);
                    run_state = HANDLE_MODE;
                }
                break;
            case HANDLE_MODE:
                osThreadSuspend(auto_TaskHandle);
                osThreadResume(handle_TaskHandle);
                JoystickSwitchTitle(ID_RUN, run_title, &mav_run_title);
                JoystickSwitchMsg(ID_RUN, run_handle_msg, &mav_run_msg);
                break;
            default:
                break;
        }
        // 指示方向状态
        switch (general_state) {
            case LEFT_MODE:
                JoystickSwitchTitle(ID_DIRECT, direct_title, &mav_dir_title);
                JoystickSwitchMsg(ID_DIRECT, direct_left_msg, &mav_dir_msg);
                break;
            case RIGHT_MODE:
                JoystickSwitchTitle(ID_DIRECT, direct_title, &mav_dir_title);
                JoystickSwitchMsg(ID_DIRECT, direct_right_msg, &mav_dir_msg);
                break;
            default:
                break;
        }
        osDelay(1);
    }
}
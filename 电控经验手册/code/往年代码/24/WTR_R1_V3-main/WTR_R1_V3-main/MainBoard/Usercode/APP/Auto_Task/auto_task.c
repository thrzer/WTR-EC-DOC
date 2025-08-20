#include "auto_task.h"

enum Auto_State auto_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_TaskHandle;
const osThreadAttr_t auto_Task_attributes = {
    .name       = "auto_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_auto_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   总线程初始化
 */
void m_auto_Task_Start(void)
{
    auto_state      = AUTO_IDLE_MODE; // 初始化为空闲状态
    auto_TaskHandle = osThreadNew(m_auto_Task, NULL, &auto_Task_attributes);
}

/*****************************************************************************
 * 线程函数定义
 */
/**
 * @brief   手动线程
 */
void m_auto_Task(void *argument)
{
    up_flag = 0;
    go_flag = 0;
    m_auto_seed_Task_Start();
    m_auto_ball_Task_Start();
    for (;;) {
        switch (auto_state) {
            case AUTO_IDLE_MODE:
                // 默认复位状态
                Reset_Action();
                // 挂起线程
                osThreadSuspend(auto_seed_TaskHandle);
                osThreadSuspend(auto_ball_TaskHandle);
                // 1. 转换到自动球状态
                if (usr_right_y > 500.0f) {
                    osThreadSuspend(auto_seed_TaskHandle);
                    osThreadResume(auto_ball_TaskHandle);
                    auto_state = AUTO_BALL_MODE;
                }
                // 2. 转换到自动苗状态
                if (usr_right_y < -500.0f) {
                    osThreadSuspend(auto_ball_TaskHandle);
                    osThreadResume(auto_seed_TaskHandle);
                    auto_state = AUTO_SEED_MODE;
                }
                break;
            case AUTO_SEED_MODE:
                // 转换至空闲状态
                if (btn_Btn5 == 1 && auto_seed_state == Auto_Seed_Ready) {
                    auto_state = AUTO_IDLE_MODE;
                }
                break;
            case AUTO_BALL_MODE:
                // 转换至空闲状态
                if (btn_Btn5 == 1 && auto_ball_state == Auto_Ball_Ready) {
                    auto_state = AUTO_IDLE_MODE;
                }
                break;
            default:
                break;
        }
        switch (auto_state) {
            case AUTO_IDLE_MODE:
                // 发送状态指令
                JoystickDelete(ID_AUTO_SEED, &mav_joystick_del);
                JoystickDelete(ID_AUTO_BALL, &mav_joystick_del);
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_idle_msg, &mav_mode_msg);
                break;
            case AUTO_SEED_MODE:
                // 发送状态指令
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_seed_msg, &mav_mode_msg);
                break;
            case AUTO_BALL_MODE:
                // 发送状态指令
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_ball_msg, &mav_mode_msg);
                break;
            default:
                break;
        }

        osDelay(1);
    }
}
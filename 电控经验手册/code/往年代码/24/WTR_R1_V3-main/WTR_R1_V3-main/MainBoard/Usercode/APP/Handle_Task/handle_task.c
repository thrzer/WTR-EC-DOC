#include "handle_task.h"

enum Handle_State handle_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t handle_TaskHandle;
const osThreadAttr_t handle_Task_attributes = {
    .name       = "handle_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_handle_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   总线程初始化
 */
void m_handle_Task_Start(void)
{
    handle_state      = HANDLE_IDLE_MODE; // 初始化为空闲状态
    handle_TaskHandle = osThreadNew(m_handle_Task, NULL, &handle_Task_attributes);
}

/*****************************************************************************
 * 线程函数定义
 */
/**
 * @brief   手动线程
 */
void m_handle_Task(void *argument)
{
    /**
     * 左侧开关Switch指示自动/手动状态，在手动线程中，仅有空闲状态方能切换到自动状态
     * 右侧摇杆纵向值指示苗/球状态
     */
    m_handle_ball_Task_Start();
    m_handle_seed_Task_Start();
    for (;;) {
        // 手动线程管理
        switch (handle_state) {
            case HANDLE_IDLE_MODE: // 空闲状态
                // 挂起线程
                osThreadSuspend(handle_seed_TaskHandle);
                osThreadSuspend(handle_ball_TaskHandle);
                // 发送状态指令
                JoystickDelete(ID_HANDLE_SEED, &mav_joystick_del);
                JoystickDelete(ID_HANDLE_BALL, &mav_joystick_del);
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_idle_msg, &mav_mode_msg);
                // 复位状态
                Reset_Action();
                // 等待转换指令
                // 1. 转换到自动状态
                // 自动状态入口状态总为空闲状态
                if (left_switch == 1) {
                    auto_state = AUTO_IDLE_MODE;
                    run_state  = AUTO_MODE;
                }
                // 2. 转换到手动球状态
                if (usr_right_y > 500.0f) {
                    osThreadSuspend(handle_seed_TaskHandle);
                    osThreadResume(handle_ball_TaskHandle);
                    handle_state = HANDLE_BALL_MODE;
                }
                // 3. 转换到手动苗状态
                if (usr_right_y < -500.0f) {
                    osThreadSuspend(handle_ball_TaskHandle);
                    osThreadResume(handle_seed_TaskHandle);
                    handle_state = HANDLE_SEED_MODE;
                }
                break;
            case HANDLE_SEED_MODE:
                // 发送状态指令
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_seed_msg, &mav_mode_msg);
                if (btn_Btn5 == 1 && handle_seed_state == Handle_Seed_Ready) {
                    handle_state = HANDLE_IDLE_MODE;
                }
                break;
            case HANDLE_BALL_MODE:
                // 发送状态指令
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_ball_msg, &mav_mode_msg);
                // 1. 转换到空闲状态
                if (btn_Btn5 == 1 && handle_ball_state == Handle_Ball_Ready) {
                    handle_state = HANDLE_IDLE_MODE;
                }
                break;
            default:
                break;
        }
        static int i = 0;
        i++;
        if (i == 1000) {
            i = 0;
            HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
        }
        osDelay(1);
    }
}
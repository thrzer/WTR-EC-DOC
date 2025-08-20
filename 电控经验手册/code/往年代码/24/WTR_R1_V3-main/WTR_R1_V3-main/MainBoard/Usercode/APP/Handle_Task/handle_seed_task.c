#include "handle_seed_task.h"

enum Handle_Seed_State handle_seed_state;

int deposit_point[4] = {
    960,
    960,
    770,
    590,
}; // 挡板标记点

/*****************************************************************************
 *  线程定义
 */
osThreadId_t handle_seed_TaskHandle;
const osThreadAttr_t handle_seed_Task_attributes = {
    .name       = "handle_seed_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_handle_seed_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   手动取放苗线程初始化
 */
void m_handle_seed_Task_Start(void)
{
    handle_seed_state      = Handle_Seed_Ready;
    handle_seed_TaskHandle = osThreadNew(m_handle_seed_Task, NULL, &handle_seed_Task_attributes);
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放苗线程
 */
void m_handle_seed_Task(void *argument)
{
    down_flag = 0;
    for (;;) {
        switch (handle_seed_state) {
            case Handle_Seed_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_SEED, handle_seed_title, &mav_handle_seed_title);
                JoystickSwitchMsg(ID_HANDLE_SEED, handle_seed_ready_msg, &mav_handle_seed_msg);
                // 复位状态
                Reset_Action();
                // 状态转换
                if (btn_KnobR == 1 && usr_right_x < -500.0f) {
                    handle_seed_state = Handle_Seed_Grip;
                }
                if (btn_KnobR == 1 && usr_right_x > 500.0f) {
                    handle_seed_state = Handle_Seed_Plant;
                }
                break;
            case Handle_Seed_Grip:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_SEED, handle_seed_title, &mav_handle_seed_title);
                JoystickSwitchMsg(ID_HANDLE_SEED, handle_seed_grip_msg, &mav_handle_seed_msg);
                // 取苗动作执行
                Seed_Grip_Action();
                // 状态转换
                handle_seed_state = Handle_Seed_Ready;
                break;
            case Handle_Seed_Plant:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_SEED, handle_seed_title, &mav_handle_seed_title);
                JoystickSwitchMsg(ID_HANDLE_SEED, handle_seed_plant_msg, &mav_handle_seed_msg);
                // 动作执行
                Seed_Plant_Action();
                // 状态转换
                handle_seed_state = Handle_Seed_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}
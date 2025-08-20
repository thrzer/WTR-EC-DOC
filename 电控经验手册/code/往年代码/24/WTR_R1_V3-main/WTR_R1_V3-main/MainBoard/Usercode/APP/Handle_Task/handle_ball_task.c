#include "handle_ball_task.h"

enum Handle_Ball_State handle_ball_state = Handle_Ball_Ready;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t handle_ball_TaskHandle;
const osThreadAttr_t handle_ball_Task_attributes = {
    .name       = "handle_ball_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_handle_ball_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   手动取放球线程初始化
 */
void m_handle_ball_Task_Start(void)
{
    handle_ball_state      = Handle_Ball_Ready;
    handle_ball_TaskHandle = osThreadNew(m_handle_ball_Task, NULL, &handle_ball_Task_attributes);
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放球线程
 */
void m_handle_ball_Task(void *argument)
{
    for (;;) {
        switch (handle_ball_state) {
            case Handle_Ball_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_ready_msg, &mav_handle_ball_msg);
                // 复位状态
                Reset_Action();
                // 状态机转换
                if (btn_KnobR == 1) {
                    handle_ball_state = Handle_Ball_Pick;
                }
                break;
            case Handle_Ball_Pick:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_pick_msg, &mav_handle_ball_msg);
                // 动作指令
                Ball_Pick_Action();
                // 状态转换
                handle_ball_state = Handle_Ball_Fire;
                break;
            case Handle_Ball_Fire:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_fire_msg, &mav_handle_ball_msg);
                // 动作指令
                Ball_Fire_Action();
                // 状态转换
                handle_ball_state = Handle_Ball_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}
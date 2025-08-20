#include "unitree.h"

// Unitree 状态相关值
float unitree_motor_left_pos;
float unitree_motor_left_kp;
float unitree_motor_left_kw;
float unitree_motor_right_pos;
float unitree_motor_right_kp;
float unitree_motor_right_kw;

// Unitree 用户值
float unitree_left_pos;
float unitree_right_pos;

/************************************************************
 * 线程定义
 */
osThreadId_t unitree_uart_message_TaskHandle;
const osThreadAttr_t unitree_uart_message_Task_attributes = {
    .name       = "unitree_uart_message_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_Unitree_UART_Message_Task(void *argument);

osThreadId_t unitree_left_ctl_TaskHandle;
const osThreadAttr_t unitree_left_ctl_Task_attributes = {
    .name       = "unitree_left_ctl_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_Unitree_left_ctl_Task(void *argument);

osThreadId_t unitree_right_ctl_TaskHandle;
const osThreadAttr_t unitree_right_ctl_Task_attributes = {
    .name       = "unitree_right_ctl_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_Unitree_right_ctl_Task(void *argument);

/***********************************************************
 * 接口函数
 */
/**
 * @brief   Unitree电机初始化
 */
void m_Unitree_Init(void)
{
    Unitree_User_Init();
}

/**
 * @brief   Unitree消息发送线程创建
 */
void m_Unitree_UART_Message_TaskStart(void)
{
    unitree_uart_message_TaskHandle = osThreadNew(m_Unitree_UART_Message_Task, NULL, &unitree_uart_message_Task_attributes);
}

/**
 * @brief   Unitree控制线程创建
 */
void m_Unitree_Ctl_Message_TaskStart(void)
{
    unitree_left_ctl_TaskHandle  = osThreadNew(m_Unitree_left_ctl_Task, NULL, &unitree_left_ctl_Task_attributes);
    unitree_right_ctl_TaskHandle = osThreadNew(m_Unitree_right_ctl_Task, NULL, &unitree_right_ctl_Task_attributes);
}

/**
 * @brief   Unitree电机平滑运动处理函数（反丢步 + 平滑）
 * @param   id          电机ID
 * @param   move_angle  旋转目标角度
 * @param   KP          力矩系数
 * @param   KW          阻尼系数
 */
void unitree_move_smooth(int id, float move_angle, float KP, float KW)
{
    int dir; // 方向定义
    int step;
    float delta_angle; // 相距角度，正数为逆时针，负数为顺时针
    float abs_angle;   // 绝对值化后的角度
    float start_point; // 起始点
    // 1. 计算距离
    switch (id) {
        case 0:
            // 记录起始点
            start_point = unitree_motor_right_pos;
            break;
        case 1:
            start_point = unitree_motor_left_pos;
            break;
        default:
            start_point = 0;
            break;
    }
    delta_angle = move_angle - start_point;
    // 2. 角度方向处理
    if (delta_angle < 0.0f) // 顺时针
    {
        abs_angle = -delta_angle;
        dir       = 0;
    } else // 逆时针
    {
        abs_angle = delta_angle;
        dir       = 1;
    }
    // 3. 计算步数
    step = (int)(abs_angle / MOVE_STEPLENGTH) + 1;
    // 4. 根据ID进行移动
    switch (id) {
        case 0: // 右侧电机
            // 设定参数值
            unitree_motor_right_kp = KP;
            unitree_motor_right_kw = KW;
            // 前n步进行依次步进
            for (int i = 0; i < (step - 1); i++) {
                if (dir == 0) {
                    unitree_motor_right_pos = unitree_motor_right_pos - MOVE_STEPLENGTH;
                } else if (dir == 1) {
                    unitree_motor_right_pos = unitree_motor_right_pos + MOVE_STEPLENGTH;
                }
                osDelay(1);
            }
            // 最后一步到达目标点
            unitree_motor_right_pos = move_angle;
            break;
        case 1:
            // 设定参数值
            unitree_motor_left_kp = KP;
            unitree_motor_left_kw = KW;
            // 前n步进行依次步进
            for (int i = 0; i < (step - 1); i++) {
                if (dir == 0) {
                    unitree_motor_left_pos = unitree_motor_left_pos - MOVE_STEPLENGTH;
                } else if (dir == 1) {
                    unitree_motor_left_pos = unitree_motor_left_pos + MOVE_STEPLENGTH;
                }
                osDelay(1);
            }
            // 最后一步到达目标点
            unitree_motor_left_pos = move_angle;
            break;
        default:
            break;
    }
}

/************************************************************
 * 线程函数
 */
/**
 * @brief   底盘Unitree电机UART消息发送线程
 */
void m_Unitree_UART_Message_Task(void *argument)
{
    for (;;) {
        Unitree_UART_tranANDrev(&unitree_motor_right, 0, 1, 0, 0, unitree_motor_right_pos, unitree_motor_right_kp, unitree_motor_right_kw);
        osDelay(2);
        Unitree_UART_tranANDrev(&unitree_motor_left, 1, 1, 0, 0, unitree_motor_left_pos, unitree_motor_left_kp, unitree_motor_left_kw);
        osDelay(2);
    }
}

/**
 * @brief   底盘Unitree电机控制线程
 */
void m_Unitree_left_ctl_Task(void *argument)
{
    for (;;) {
        unitree_move_smooth(1, unitree_motor_left.offset + unitree_left_pos, 0.35, 0.1);
        osDelay(1);
    }
}

/**
 * @brief   底盘Unitree电机控制线程
 */
void m_Unitree_right_ctl_Task(void *argument)
{
    for (;;) {
        unitree_move_smooth(0, unitree_motor_right.offset + unitree_right_pos, 0.35, 0.1);
        osDelay(1);
    }
}
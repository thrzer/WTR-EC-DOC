#include "odom.h"

float chassis_x_point = 0; // 底盘纵向坐标
float chassis_y_point = 0; // 底盘横向坐标

osThreadId_t chassis_odom_TaskHandle;
const osThreadAttr_t chassis_odom_Task_attributes = {
    .name       = "chassis_odom_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityHigh,
};

/**
 * @brief   码盘初始化
 */
void m_Chassis_Odom_Init(void)
{
    OPS_Init();
}

/**
 * @brief   码盘线程开启
 */
void m_Chassis_Odom_TaskStart(void)
{
    chassis_odom_TaskHandle = osThreadNew(m_Chassis_Odom_Task, NULL, &chassis_odom_Task_attributes);
}

/**
 * @brief   码盘线程
 */
void m_Chassis_Odom_Task(void *argument)
{
    for (;;) {
        chassis_x_point = -OPS_Data.pos_y;
        chassis_y_point = OPS_Data.pos_x;
        osDelay(1);
    }
}
#include "laser.h"

osThreadId_t chassis_laser_TaskHandle;
const osThreadAttr_t chassis_laser_Task_attributes = {
    .name       = "chassis_laser_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityHigh,
};

/**
 * @brief   激光初始化
 */
void m_Chassis_Laser_Init(void)
{
    Laser_Init();
}

/**
 * @brief   激光线程开启
 */
void m_Chassis_Laser_TaskStart(void)
{
    chassis_laser_TaskHandle = osThreadNew(m_Chassis_Laser_Task, NULL, &chassis_laser_Task_attributes);
}

/**
 * @brief   激光线程
 */
void m_Chassis_Laser_Task(void *argument)
{
    for (;;) {
        osDelay(1);
    }
}
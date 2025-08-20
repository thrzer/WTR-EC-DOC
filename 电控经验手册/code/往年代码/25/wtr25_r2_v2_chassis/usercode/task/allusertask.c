#include "allusertask.h"

/* 状态机 */
osThreadId_t hsmtaskHandle;
const osThreadAttr_t hsmtask_attributes = {
  .name = "hsmtask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* 底盘 */
osThreadId_t chassistaskHandle;
const osThreadAttr_t chassistask_attributes = {
  .name = "chassistask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* 遥控器 */
osThreadId_t rmctltaskHandle;
const osThreadAttr_t rmctltask_attributes = {
  .name = "rmctltask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* 上层机构通信 */
osThreadId_t messagetaskHandle;
const osThreadAttr_t messagetask_attributes = {
  .name = "messagetask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* DT35 */
osThreadId_t dt35taskHandle;
const osThreadAttr_t dt35task_attributes = {
  .name = "dt35task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* 陀螺仪 */
osThreadId_t chassis_gyro_TaskHandle;
const osThreadAttr_t chassis_gyro_Task_attributes = {
    .name       = "chassis_gyro_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
// /* 测试计时器 */
// osThreadId_t timecnttaskHandle;
// const osThreadAttr_t timecnttask_attributes = {
//   .name = "timecnttask",
//   .stack_size = 128 * 4,
//   .priority = (osPriority_t) osPriorityHigh,
// };

/**
 * @brief 自定义线程初始化函数，在freertos.c的defaulttask调用，将相应新建函数注释掉即可删除任务
 * @date 2024-11-23
 */
void User_FREERTOS_Init(void)
{
  hsmtaskHandle = osThreadNew(HsmTask, NULL, &hsmtask_attributes);
  chassistaskHandle = osThreadNew(ChassisTask, NULL, &chassistask_attributes);
  rmctltaskHandle = osThreadNew(RmctlTask, NULL, &rmctltask_attributes);
  chassis_gyro_TaskHandle = osThreadNew(m_Chassis_Gyro_Task, NULL, &chassis_gyro_Task_attributes);
  // messagetaskHandle = osThreadNew(MessageTask, NULL, &messagetask_attributes);
  dt35taskHandle = osThreadNew(Dt35Task, NULL, &dt35task_attributes);
  // timecnttaskHandle = osThreadNew(TimecntTask, NULL, &timecnttask_attributes);
}
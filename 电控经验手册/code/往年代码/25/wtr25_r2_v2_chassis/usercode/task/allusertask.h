#ifndef ALLUSERTASK_H
#define ALLUSERTASK_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file allusertask.h
 * @author thrzer
 * @brief wtr二代车freertos任务定义，在freertos.c中include
 * @date 2024-5-16
 */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "gyro.h"

void User_FREERTOS_Init(void);
void HsmTask(void *argument);
void ChassisTask(void *argument);
void RmctlTask(void *argument);
void MessageTask(void *argument);
void Dt35Task(void *argument);

#ifdef __cplusplus
}
#endif

#endif
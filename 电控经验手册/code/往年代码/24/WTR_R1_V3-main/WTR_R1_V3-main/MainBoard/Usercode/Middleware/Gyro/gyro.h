#ifndef __GYRO_H
#define __GYRO_H

#include "usermain.h"

extern uint8_t chassis_gyro_state;
extern float chassis_offset;
extern float chassis_yaw;

void m_Chassis_Gyro_Init(void);
void m_Chassis_Gyro_TaskStart(void);
void m_Chassis_Gyro_Task(void *argument);

#endif
#ifndef __LASER_H
#define __LASER_H

#include "usermain.h"
#include "laser_sdk.h"

void m_Chassis_Laser_Init(void);
void m_Chassis_Laser_TaskStart(void);
void m_Chassis_Laser_Task(void *argument);

#endif
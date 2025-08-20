#ifndef __ODOM_H
#define __ODOM_H

#include "usermain.h"

extern float chassis_x_point; // 底盘纵向坐标
extern float chassis_y_point; // 底盘横向坐标

void m_Chassis_Odom_Init(void);
void m_Chassis_Odom_TaskStart(void);
void m_Chassis_Odom_Task(void *argument);

#endif
#ifndef __CHASSIS_H
#define __CHASSIS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "usermain.h"
#include "Caculate.h"
#include "math.h"
#include "wtr_can.h"

/************************类型定义*****************************/
typedef struct C_PID {
    __IO float SetPoint;    /* PID目标值 */
    __IO float ActualValue; /* PID输出值 */
    __IO float SumError;    /* PID误差 */
    __IO float Proportion;  /* P */
    __IO float Integral;    /* I */
    __IO float Derivative;  /* D */
    __IO float Error;       /* Error[1] */
    __IO float LastError;   /* Error[-1] */
    __IO float PrevError;   /* Error[-2] */
} C_PID;

extern C_PID chassis_yaw_pid; // 底盘偏航角PID控制器
extern C_PID chassis_y_pid;   // 底盘横向PID控制器
extern C_PID chassis_x_pid;   // 底盘纵向PID控制器

/****************************************************************************
 * 接口定义
 */

extern int arm_angle;            // 机械臂角度
extern int friction_speed_up;    // 上摩擦轮速度
extern int friction_speed_down;  // 下摩擦轮速度
extern float v_1, v_2, v_3, v_4; // 底盘速度
extern float motor_r_gripseed;   // 右侧取苗电机位置
extern float motor_l_gripseed;   // 左侧取苗电机位置
extern float motor_r_plantseed;  // 右侧放苗电机位置
extern float motor_l_plantseed;  // 左侧放苗电机位置

/*************************************************************
 * 用户函数
 */

void m_Chassis_CAN_Message_TaskStart(void);
void m_Chassis_Ctl_TaskStart(void);
void m_Chassis_Init(void);

void chassis_pid_init(C_PID *upid, float KP, float KI, float KD);
float chassis_yaw_pid_calc(C_PID *upid, float Feedback_value);
float chassis_pos_pid_calc(C_PID *upid, float Feedback_value);

void Inverse_kinematic_equation(float vx, float vy, float wc, float *_v_1, float *_v_2, float *_v_3, float *_v_4);

#ifdef __cplusplus
}
#endif
#endif
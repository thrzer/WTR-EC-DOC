// Last update: 2024-12-29 
#ifndef PID_AUTOSETTING_H
#define PID_AUTOSETTING_H
// #include "pid.h"
#include "main.h"
/**
 * 使用Lambda法自动设置PID参数
 * 原理详见《PID参数整定与复杂控制》冯少辉
 * Created by tony on 24-11-16.
 */
typedef enum _PID_AutoSetting_State
{
PID_AUTOSETTING_PROCESS=0,
PID_AUTOSETTING_DONE=1
}PID_AutoSetting_State_t;
typedef enum _PID_AutoSetting_Type
{
TYPE_SELF_BALANCING=0,//自衡对象，比如使用pwm控制的直流电机速度环
TYPE_INTEGRAL =1//积分对象，比如DJI电机的速度环，或者通过速度控制的位置环
}PID_AutoSetting_Type_t;
typedef struct
{
PID_AutoSetting_Type_t type;
double lambda;
double test_output;
double test_time_ms;
double init_fdbk;

double delta;
double dt_ms;
double *pv;
double tau;
double K;
double T;
uint64_t pv_num;
uint64_t pv_index;
}PID_AutoSetting_Buffers_t;


void PID_AutoSetting_Init(
    PID_AutoSetting_Buffers_t *pas,
    PID_AutoSetting_Type_t type,
    double lambda,
    double init_fdbk,
    double test_time_ms,
    double dt_ms,
    double delta,
    double test_output);
PID_AutoSetting_State_t PID_AutoSetting_Update(PID_AutoSetting_Buffers_t *pas,double fdbk);
void PID_AutoSetting_Set_Param(PID_AutoSetting_Buffers_t *pas,double *Kp,double *Ki,double *Kd);
#endif //PID_AUTOSETTING_H
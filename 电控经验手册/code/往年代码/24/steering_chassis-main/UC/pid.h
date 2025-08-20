#ifndef _PID_H__
#define _PID_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "main.h"
#define PID_SETTING_INTEGRAL_LIMIT_ENABLE 0x01 // 启用积分限幅
#define PID_SETTING_OUTPUT_LIMIT_ENABLE 0x02 // 启用输出限幅
#define PID_SETTING_DEADBAND_ENABLE 0x04 // 启用死区控制
typedef struct _PID_TypeDef{
    double kp;
    double ki;
    double kd;
    uint32_t setting_flags; 
    double integral_max;  // 积分项最大值(绝对值)
    double output_min;  // 输出最小值
    double output_max;  // 输出最大值
    double deadband;    // 死区(绝对值)
    

    double setpoint;    // 设定值
    double integral;    // 积分项
    double prev_error;  // 上一次误差
    double prev_measured; // 上一次测量值，用于微分计算
    double output;      // 控制输出
    uint8_t initialed; // PID控制器是否初始化
} PID_TypeDef;
void PID_Init(PID_TypeDef *pid, double Kp, double Ki, double Kd);
void PID_Init_NoParams(PID_TypeDef *pid);
void PID_Init_Classic(PID_TypeDef *pid, double Kc, double Ti, double Td,double Dt);
void PID_Reset(PID_TypeDef *pid, double Kp, double Ki, double Kd);
void PID_Reset_Classic(PID_TypeDef *pid, double Kc, double Ti, double Td,double Dt);
int PID_Set_integral_limit(PID_TypeDef *pid, double integral_max);
int PID_Set_output_limit(PID_TypeDef *pid, double output_min, double output_max);
int PID_Set_deadband(PID_TypeDef *pid, double deadband);
int PID_update(PID_TypeDef *pid, double measured_value);
#ifdef __cplusplus
}
#endif

#endif
#include "pid.h"
#include<stdlib.h>
#define ABS(x)		((x>0)? x: -x)
void PID_Reset(PID_TypeDef *pid, double Kp, double Ki, double Kd)
{
    pid->kp = Kp;
    pid->ki = Ki;
    pid->kd = Kd;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->prev_measured = 0;
    pid->output = 0;
}
void PID_Reset_Classic(PID_TypeDef *pid, double Kc, double Ti, double Td, double Dt)
{
    double kp = Kc ;
    double ki = Kc * Dt / Ti;
    double kd = Kc * Td / Dt;
    PID_Reset(pid, kp, ki, kd);
}
void PID_Init(PID_TypeDef *pid, double Kp, double Ki, double Kd)
{
    PID_Reset(pid, Kp, Ki, Kd);
    pid->setting_flags = 0;
    pid->initialed = 1;
    
}
void PID_Init_Classic(PID_TypeDef *pid, double Kc, double Ti, double Td, double Dt)
{
    PID_Reset_Classic(pid, Kc, Ti, Td, Dt);
    pid->setting_flags = 0;
    pid->initialed = 1;
}
void PID_Init_NoParams(PID_TypeDef *pid)
{
    pid->setting_flags = 0;
    pid->initialed = 1;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->prev_measured = 0;
    pid->output = 0;
}


int PID_Set_integral_limit(PID_TypeDef *pid, double integral_max)
{
    if (!pid->initialed||integral_max<0)
    {
        return -1;
    }
    pid->setting_flags |= PID_SETTING_INTEGRAL_LIMIT_ENABLE;
    pid->integral_max = integral_max;
    return 0;
}

int PID_Set_output_limit(PID_TypeDef *pid, double output_min, double output_max)
{
    if (!pid->initialed||output_max<output_min)
    {
        return -1;
    }
    pid->setting_flags |= PID_SETTING_OUTPUT_LIMIT_ENABLE;
    pid->output_min = output_min;
    pid->output_max = output_max;
    return 0;
}

int PID_Set_deadband(PID_TypeDef *pid, double deadband)
{
    if (!pid->initialed||deadband<0)
    {
        return -1;
    }
    pid->setting_flags |= PID_SETTING_DEADBAND_ENABLE;
    pid->deadband = deadband;
    return 0;
}

int PID_update(PID_TypeDef *pid, double feedback)
{
    if (!pid->initialed||pid->kp*pid->ki==0)
    {
        return -1;
    }
    double error = pid->setpoint - feedback;
    if(pid->setting_flags & PID_SETTING_DEADBAND_ENABLE)
    {
        if (ABS(error) < pid->deadband)
        {
            error = 0;
        }
    }    
    pid->integral += error;
    if(pid->setting_flags & PID_SETTING_INTEGRAL_LIMIT_ENABLE)
    {
        if (pid->integral > pid->integral_max)
        {
            pid->integral = pid->integral_max;
        }
        else if (pid->integral < -pid->integral_max)
        {
            pid->integral = -pid->integral_max;
        }
    }
    
    pid->output = pid->kp * error + pid->ki * pid->integral + pid->kd * (error - pid->prev_error);
    if(pid->setting_flags & PID_SETTING_OUTPUT_LIMIT_ENABLE)
    {
        if (pid->output > pid->output_max)
        {
            pid->output = pid->output_max;
        }
        else if (pid->output < pid->output_min)
        {
            pid->output = pid->output_min;
        }
    }
    return 0;
}

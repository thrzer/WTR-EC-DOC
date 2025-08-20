#include "_startup.h"

#include "DJI.h"
#include "pid.h"
#include "wtr_can.h"
#include "pid_param_setting.h"
// #include <sensor_msgs/msg/joint_state.h>

// double joint_angle[JOINT_DOUBLE_LEN] = {0};
// double joint_fd[JOINT_DOUBLE_LEN] = {0};

    PID_TypeDef speedpid[4];
    PID_TypeDef pospid[4];

void StartUp()
{

    CANFilterInit(&hcan1);
    CAN2FilterInit(&hcan2);
    for (int i = 0; i < 4; i++)
        hDJI[i].motorType = M2006;
    DJI_Init();
    for (int i = 0; i < 4; i++) {
        PID_Init(&speedpid[i],200,10,0);
        PID_Set_output_limit(&speedpid[i],-10000,10000);
        PID_Set_integral_limit(&speedpid[i],500);
        PID_Init(&pospid[i],2.06,0.0105,0);
        PID_Set_integral_limit(&pospid[i],100);
        PID_Set_output_limit(&pospid[i],-550,550);
        PID_Set_deadband(&pospid[i], 1);

    }



}

void pid_auto_set()
{
    PID_AutoSetting_Buffers_t pas;
}





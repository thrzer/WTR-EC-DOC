#include "_startup.h"

#include "DJI.h"
#include "wtr_can.h"

// double joint_angle[JOINT_DOUBLE_LEN] = {0};
double joint_fd[JOINT_DOUBLE_LEN] = {0};


void StartUp()
{

    CANFilterInit(&hcan1);
    // CAN2FilterInit(&hcan2);
    for (int i = 0; i < 4; i++)
        hDJI[i].motorType = M3508;
    DJI_Init();
    for (int i = 0; i < 4; i++) {
        hDJI[i].posPID.KI=1.5;
        hDJI[i].posPID.KD=0;
        hDJI[i].speedPID.outputMax=8000;
        hDJI[i].speedPID.KI=0.02;
        hDJI[i].speedPID.KD=0.1;
        hDJI[i].speedPID.KP=0.8;
    }    
}





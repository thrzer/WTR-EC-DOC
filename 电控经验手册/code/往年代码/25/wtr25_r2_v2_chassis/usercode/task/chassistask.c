#include "allusertask.h"
#include "HSM.h"
#include "rmctl.h"
#include "steering_chassis.h"
#include "omniwheel_chassis.h"
void testfn(void);

extern bool IsDt35Detect;

/**
 * @brief 舵轮底盘线程
 * @author thrzer
 * @date 2025-4-23
 */
void ChassisTask(void *argument)
{    
    wtrV2.chassis_mechanism = HSM_cSTATE_INIT;
    owChassisInit(&chassis);
    osDelay(2000);
    wtrV2.chassis_mechanism = HSM_cSTATE_CORRECTING;
    while(true)
    {
        // testfn();
        // if(1)//如果上位机没有主动控制
            owChassis_rmctlinput(&chassis);
            // owChassis_set_targetVelocity(&chassis,2,0,0);
        owChassis_executor(&chassis);
        osDelay(5);
    }
}

void testfn(void)
{ 
    static int cnt = 0;
    cnt++;
    static float a = 50;
    for(int i = 0;i<4;i++)
    {
    // chassis.wheel[i].motor->posPID.KP = 120.0f;
    // chassis.wheel[i].motor->posPID.KI = 1.0f;
    // chassis.wheel[i].motor->posPID.KD = 0.1f;
     positionServo(100+a,chassis.wheel[i].motor);
    }
    if(cnt >= 600)
    {
        cnt = 0;
        a = -a;
    }
    
    // swChassis.wheel[0].hdji->f_current = 0;
    // swChassis.wheel[1].hdji->f_current = 0;
    // swChassis.wheel[2].hdji->f_current = 0;
    // if(cnt <= 100000)
    // CanTransmit_DJI_1234(swChassis.rhcanx, 
    //                     swChassis.wheel[0].hdji->f_current,
    //                     swChassis.wheel[1].hdji->f_current,
    //                     swChassis.wheel[2].hdji->f_current,0);
    // else
    // CanTransmit_DJI_1234(swChassis.rhcanx,0,0,0,0);
    // CanTransmit_DJI_1234(swChassis.rhcanx,0,
    // swChassis.wheel[1].hdji->speedPID.output + signf(swChassis.wheel[1].hdji->speedPID.output)*swChassis.wheel[1].hdji->f_current,
    // 0,0);
    CanTransmit_DJI_1234(chassis.hcanx,0,
    chassis.wheel[1].motor->speedPID.output,
    0,
    0);
    // CanTransmit_DJI_1234(swChassis.rhcanx,
    // swChassis.wheel[0].hdji->speedPID.output + signf(swChassis.wheel[0].hdji->speedPID.output)*swChassis.wheel[0].hdji->f_current,
    // swChassis.wheel[1].hdji->speedPID.output + signf(swChassis.wheel[1].hdji->speedPID.output)*swChassis.wheel[1].hdji->f_current,
    // swChassis.wheel[2].hdji->speedPID.output + signf(swChassis.wheel[2].hdji->speedPID.output)*swChassis.wheel[2].hdji->f_current,
    // 0);
}
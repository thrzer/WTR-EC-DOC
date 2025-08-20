#include "allusertask.h"
#include "rmctl.h"
#include "steering_chassis.h"

int flag = 0;

/**
 * @brief 测试用计时器
 * @author thrzer
 * @date 2025-7-15
 */
void TimecntTask(void *argument)
{
    if(flag == 1)
    {
        swChassis_setVelocity(&swChassis,0.6,0,0);
        osDelay(1500);
        swChassis_setVelocity(&swChassis,0,0,0);
    }
}
////
//// Created by tony on 23-10-3.
////


// 底盘控制线程
#include "steering_wheel_chassis.h"
// #include "cmsis_os.h"
#include "_startup.h"
//#include "stm32f427xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "Event_Define.h"
// #include "stm32f4xx_hal_uart.h"
// #include "usart.h"
#include <stdint.h>

// #include "wtr_vesc.h"
// #include "wtr_can.h"
// #include "Caculate.h"
// #include "DJI.h"
swChassis_t mychassis;
float target[3]={0,0,0};
void StartChassisTask(void *argument)
{
    HSM_CHASSIS_Init(&mychassis, "chassis");
    HSM_CHASSIS_Run(&mychassis, HSM_CHASSIS_START, NULL);    
    for (;;) {
        swChassis_set_targetVelocity(&mychassis, target[0], target[1],target[2]);
        // swChassis_set_targetVelocity(&mychassis, 0, 0,1);
        HSM_CHASSIS_Run(&mychassis, Next_Event, NULL);

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    swChassis_EXTI_Callback(&mychassis, GPIO_Pin);
}


#include "Event_Define.h"
#include "HStateMachine.h"
#include "portmacro.h"
#include "steering_wheel_chassis.h"
#include "stm32f427xx.h"
#include "_startup.h"
#include <stdint.h>

HSM_STATE CHASSIS_ON;
HSM_STATE CHASSIS_OFF;
HSM_STATE CHASSIS_ONReady;
HSM_STATE CHASSIS_ONError;
HSM_STATE CHASSIS_ONCorrecting;
HSM_STATE CHASSIS_ONReadyRunning;
HSM_STATE CHASSIS_ONReadySTOP;

//swChassis_t mychassis;
VESC_t hVESC[4];
HSM_EVENT Next_Event;

// int8_t Detect_Power(HSM *This);

HSM_EVENT CHASSIS_OFF_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {

    }
    else if(event == HSM_EXIT)
    {
    }
    else if(event ==HSM_CHASSIS_START)
    {
        HSM_Tran(This, &CHASSIS_ON, 0, NULL);
        Next_Event = HSM_CHASSIS_INIT;
        return 0;
    }
    // else if(event == HSM_CHASSIS_POWEROFF)
    // {
    //     if(Detect_Power(This) == 1)
    //         Next_Event = HSM_CHASSIS_START;
    //     return 0;
    // }
    return event;
}

HSM_EVENT CHASSIS_ON_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
    }
    else if(event == HSM_EXIT)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
    }
    else if(event == HSM_CHASSIS_INIT)
    {
        HSM_Tran(This, &CHASSIS_ONCorrecting, 0, NULL); 
        return 0;
    }
    else if(event == HSM_CHASSIS_ERROR)
    {
        HSM_Tran(This, &CHASSIS_ONError, 0, NULL); 
        return 0;
    }
    else if(event == HSM_CHASSIS_POWEROFF)
    {
        HSM_Tran(This, &CHASSIS_OFF, 0, NULL);
        return 0;
    }
    return event;
}

HSM_EVENT CHASSIS_ONError_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {
        HAL_GPIO_WritePin(LDR_GPIO_Port,LDR_Pin, GPIO_PIN_RESET);

    }
    else if(event == HSM_EXIT)
    {
    }
    else if(event == HSM_CHASSIS_ERROR)
    {
        // if(Detect_Power(This) == -1)
        // {
        //     HSM_Tran(This, &CHASSIS_OFF, 0, NULL);
        //     Next_Event = HSM_CHASSIS_POWEROFF;
        //     return 0;
        // }
        HSM_Tran(This, &CHASSIS_OFF, 0, NULL);
        HAL_GPIO_TogglePin(LDR_GPIO_Port, LDR_Pin);
        vTaskDelay(100/portTICK_RATE_MS);
        return 0;
    }
    return event;
}


HSM_EVENT CHASSIS_ONCorrecting_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
        Next_Event = HSM_CHASSIS_CORRECTING;
        if(swChassis_startCorrect(&mychassis) != HAL_OK)
        {
            HSM_Tran(This, &CHASSIS_ONError, 0, NULL); 
            Next_Event = HSM_CHASSIS_ERROR;
        }
    }
    else if(event == HSM_EXIT)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);
    }
    else if (event == HSM_CHASSIS_CORRECTING) 
    {
        swChassis_executor(&mychassis);
        if(swChassis_CheckCorrect(&mychassis) == HAL_OK)
        {
            Next_Event = HSM_CHASSIS_CORRECTED;
        }
        return 0;
    }
    else if(event == HSM_CHASSIS_CORRECTED)
    {
        HSM_Tran(This, &CHASSIS_ONReady, 0, NULL); 
        Next_Event = HSM_CHASSIS_READY;
        return 0;
    }
    return event;
}


HSM_EVENT CHASSIS_ONReady_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);
        swChassis_set_still(&mychassis);
        swChassis_executor(&mychassis);
    }
    else if(event == HSM_EXIT)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);
    }
    else if(event == HSM_CHASSIS_READY)
    {
        // vPortEnterCritical();
        // if(Detect_Power(This) == -1)
        // {
        //     HSM_Tran(This, &CHASSIS_OFF, 0, NULL);
        //     Next_Event = HSM_CHASSIS_POWEROFF;
        //     return 0;
        // }
        // vPortExitCritical();

        swChassis_set_still(&mychassis);
        swChassis_executor(&mychassis);
        if(swChassis_check_velocity(&mychassis) == HAL_BUSY)
        {
            HSM_Tran(This, &CHASSIS_ONReadyRunning, 0, NULL);
            Next_Event = HSM_CHASSIS_RUNNING;
        }
        return 0;
    }
    else if(event == HSM_CHASSIS_ERROR)
    {
        HSM_Tran(This, &CHASSIS_ONError, 0, NULL); 
        return 0;
    }
    return event;
}

HSM_EVENT CHASSIS_ONReadyRunning_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {
    }
    else if(event == HSM_EXIT)
    {
    }
    else if(event == HSM_CHASSIS_RUNNING)
    {
        // if(Detect_Power(This) == -1)
        // {
        //     HSM_Tran(This, &CHASSIS_OFF, 0, NULL);
        //     Next_Event = HSM_CHASSIS_POWEROFF;
        //     return 0;
        // }

        calculate_target_velocity(&mychassis);
        swChassis_set_Running(&mychassis);
        if(swChassis_check_AimorRun(&mychassis) == 0)
        {
            Next_Event = HSM_CHASSIS_AMING;
        }
        swChassis_executor(&mychassis);
        
        if(swChassis_check_velocity(&mychassis) == HAL_OK)
        {
            HSM_Tran(This, &CHASSIS_ONReady, 0, NULL);
            Next_Event = HSM_CHASSIS_READY;
        }
        return 0;
    }
    else if(event == HSM_CHASSIS_AMING)
    {
        // if(Detect_Power(This) == -1)
        // {
        //     HSM_Tran(This, &CHASSIS_OFF, 0, NULL);
        //     Next_Event = HSM_CHASSIS_POWEROFF;
        //     return 0;
        // }

        calculate_target_velocity(&mychassis);
        if(swChassis_check_AimorRun(&mychassis) == 0)
        {
            Next_Event = HSM_CHASSIS_RUNNING;
            swChassis_set_Running(&mychassis);
        }
        swChassis_executor(&mychassis);
        return 0;
    }
    return event;
}

HSM_EVENT CHASSIS_ONReadyStop_Handler(HSM *This, HSM_EVENT event, void *param)
{
    if(event == HSM_ENTRY)
    {
    }
    else if(event == HSM_EXIT)
    {
    }
    else if(event == HSM_CHASSIS_STOP)
    {
        swChassis_set_still(&mychassis);
        swChassis_executor(&mychassis);
        if(swChassis_check_velocity(&mychassis) == HAL_OK)
        {
            HSM_Tran(This, &CHASSIS_ONReady, 0, NULL);
            Next_Event = HSM_CHASSIS_READY;
        }
        return 0;
    }
    return event;
}


void HSM_CHASSIS_Init(swChassis_t *This, char *name)
{
    HSM_STATE_Create(&CHASSIS_ON, "CHASSIS_ON", CHASSIS_ON_Handler, NULL);
    HSM_STATE_Create(&CHASSIS_OFF, "CHASSIS_OFF", CHASSIS_OFF_Handler, NULL);
    HSM_STATE_Create(&CHASSIS_ONReady, "CHASSIS_ONReady", CHASSIS_ONReady_Handler, &CHASSIS_ON);
    HSM_STATE_Create(&CHASSIS_ONError, "CHASSIS_ONError", CHASSIS_ONError_Handler, &CHASSIS_ON);
    HSM_STATE_Create(&CHASSIS_ONCorrecting, "CHASSIS_ONCorrecting", CHASSIS_ONCorrecting_Handler, &CHASSIS_ON);
    HSM_STATE_Create(&CHASSIS_ONReadyRunning, "CHASSIS_ONReadyRunning", CHASSIS_ONReadyRunning_Handler, &CHASSIS_ONReady);
    //HSM_STATE_Create(&CHASSIS_ONReadySTOP, "CHASSIS_ONReadyStop", CHASSIS_ONReadyStop_Handler, &CHASSIS_ONReady);
    HSM_Create((HSM *)This, "chassis_main", &CHASSIS_OFF);
    swChassis_init(&mychassis);
}

void HSM_CHASSIS_Run(swChassis_t *This, HSM_EVENT event, void *param)
{
    HSM_Run((HSM *)This, event, param);
}

/**
 * @brief 上下电检测函数
 * 
 * @return int8_t 
 *  1     上电
 * -1     下电
 */
int8_t Detect_Power(HSM *This)
{
    static uint8_t count_ = 0;
    static int8_t state = 0;
    if((HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_0) == 1) && (This->curState == &CHASSIS_OFF))
    {
        count_ ++;
        if(count_ >= 5)
        {
            count_ = 0;
            state = 1;
            return 1;
        }
    }
    else if (HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_0) == 0) 
    {
        count_ ++;
        if(count_ >= 5)
        {
            count_ = 0;
            state = -1;
            return -1;
        }
    }
    return state;
}



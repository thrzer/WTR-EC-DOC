#include "servo.h"

/**
 * @brief 舵机初始化
 */
void m_Servo_Init(void)
{
    // 左侧舵机
    HAL_TIM_PWM_Start(&Left_Plant_Seed_Timer_Handle, Left_Plant_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Left_Front_Seed_Timer_Handle, Left_Front_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Left_Buffer_Seed_Timer_Handle, Left_Buffer_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Left_Deposit_Seed_Timer_Handle, Left_Deposit_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Left_Left_Seed_Timer_Handle, Left_Left_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Left_Right_Seed_Timer_Handle, Left_Right_Seed_Timer_Channel);
    // 右侧舵机
    HAL_TIM_PWM_Start(&Right_Plant_Seed_Timer_Handle, Right_Plant_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Right_Front_Seed_Timer_Handle, Right_Front_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Right_Buffer_Seed_Timer_Handle, Right_Buffer_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Right_Deposit_Seed_Timer_Handle, Right_Deposit_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Right_Left_Seed_Timer_Handle, Right_Left_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Right_Right_Seed_Timer_Handle, Right_Right_Seed_Timer_Channel);
    // 球舵机
    HAL_TIM_PWM_Start(&Ball_Pull_Timer_Handle, Ball_Pull_Timer_Channel);
    HAL_TIM_PWM_Start(&Ball_Middle_Timer_Handle, Ball_Middle_Timer_Channel);
    HAL_TIM_PWM_Start(&Ball_Left_Timer_Handle, Ball_Left_Timer_Channel);
    HAL_TIM_PWM_Start(&Ball_Right_Timer_Handle, Ball_Right_Timer_Channel);

    Seed_Grip();
    Seed_Deposit_Open();
    Seed_Plant_Close();
    
}
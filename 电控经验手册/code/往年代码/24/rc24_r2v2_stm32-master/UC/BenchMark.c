#include "main.h"
#include "FreeRTOSConfig.h"
#include "stm32f427xx.h"

volatile uint32_t FreeRTOSRunTimeTicks = 0UL;

//初始化TIM2为FreeRTOS的时间提供基础时基
void ConfigureTimeForRunTimeStats(void)
{
	FreeRTOSRunTimeTicks = 0UL;	
}

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM11) {
//         FreeRTOSRunTimeTicks++;
//     }
// }
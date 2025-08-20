#include "main.h"
#include "Lcd/lcd_st7796.hpp"
#include "lvgl/lvgl.h"
#include "adc.h"
#include "Encoder/encoder_define.hpp"
#include "Mavlink/wtr_mavlink.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 接收通道X的消息
    wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
}

// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
// {
//     TxIntCount++;
//     // wtrMavlink_UARTTxCpltCallback(huart, MAVLINK_COMM_0);
//     // if (huart->Instance == hMAVLink[MAVLINK_COMM_0].huart->Instance) {
//     //     // extern SemaphoreHandle_t MavUartTxSem;
//     //     BaseType_t xHigherPriorityTaskWoken = pdFALSE;

//     //     xSemaphoreGiveFromISR(MavUartTxSem, &xHigherPriorityTaskWoken);
//     //     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     // }
// }

// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
// {
//     extern uint32_t Adc3CpltCount;
//     extern uint32_t Adc1CpltCount;
//     if (hadc->Instance == hadc1.Instance) {
//         Adc1CpltCount++;
//     } else if (hadc->Instance == hadc3.Instance) {
//         Adc3CpltCount++;
//     }
// }

// void vApplicationTickHook(void)
// {
//     lv_tick_inc(1000 / configTICK_RATE_HZ);
// }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case KnobL_A_Pin:
            KnobEncoderL.ExtiCallback();
            break;
        case KnobL_B_Pin:
            KnobEncoderL.ExtiCallback();
            break;
        case KnobR_A_Pin:
            KnobEncoderR.ExtiCallback();
            break;
        case KnobR_B_Pin:
            KnobEncoderR.ExtiCallback();
            break;

        default:
            break;
    }
}

#ifdef __cplusplus
}
#endif

void LcdFmc_DmaXferCpltCallback(MDMA_HandleTypeDef *_hmdma)
{
    extern LcdSt7796 LCD;

    if (_hmdma->Instance == LCD.GetMdma()->Instance) {
        LCD.DmaXferCpltCallback();
    }
}
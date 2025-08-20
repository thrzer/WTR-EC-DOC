/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KnobL_A_Pin GPIO_PIN_3
#define KnobL_A_GPIO_Port GPIOE
#define KnobL_A_EXTI_IRQn EXTI3_IRQn
#define KnobL_B_Pin GPIO_PIN_5
#define KnobL_B_GPIO_Port GPIOE
#define KnobL_B_EXTI_IRQn EXTI9_5_IRQn
#define KnobR_A_Pin GPIO_PIN_6
#define KnobR_A_GPIO_Port GPIOE
#define KnobR_A_EXTI_IRQn EXTI9_5_IRQn
#define KnobL_Btn_Pin GPIO_PIN_13
#define KnobL_Btn_GPIO_Port GPIOC
#define KnobR_B_Pin GPIO_PIN_0
#define KnobR_B_GPIO_Port GPIOC
#define KnobR_B_EXTI_IRQn EXTI0_IRQn
#define JoystickL_Btn_Pin GPIO_PIN_1
#define JoystickL_Btn_GPIO_Port GPIOC
#define KnobR_Btn_Pin GPIO_PIN_2
#define KnobR_Btn_GPIO_Port GPIOC
#define BatVoltage_Pin GPIO_PIN_3
#define BatVoltage_GPIO_Port GPIOC
#define Kb_UartTx_Pin GPIO_PIN_0
#define Kb_UartTx_GPIO_Port GPIOA
#define Kb_UartRx_Pin GPIO_PIN_1
#define Kb_UartRx_GPIO_Port GPIOA
#define Wifi_Tx_Pin GPIO_PIN_2
#define Wifi_Tx_GPIO_Port GPIOA
#define Wifi_Rx_Pin GPIO_PIN_3
#define Wifi_Rx_GPIO_Port GPIOA
#define Wifi_Rst_Pin GPIO_PIN_4
#define Wifi_Rst_GPIO_Port GPIOA
#define Kb_LedG_Pin GPIO_PIN_5
#define Kb_LedG_GPIO_Port GPIOA
#define JoystickL_X_Pin GPIO_PIN_6
#define JoystickL_X_GPIO_Port GPIOA
#define JoystickL_Y_Pin GPIO_PIN_7
#define JoystickL_Y_GPIO_Port GPIOA
#define JoystickR_X_Pin GPIO_PIN_4
#define JoystickR_X_GPIO_Port GPIOC
#define JoystickR_Y_Pin GPIO_PIN_5
#define JoystickR_Y_GPIO_Port GPIOC
#define JoystickR_Btn_Pin GPIO_PIN_0
#define JoystickR_Btn_GPIO_Port GPIOB
#define Kb_LedR_Pin GPIO_PIN_1
#define Kb_LedR_GPIO_Port GPIOB
#define MPU9250_ECl_Pin GPIO_PIN_10
#define MPU9250_ECl_GPIO_Port GPIOB
#define MPU9250_EDa_Pin GPIO_PIN_11
#define MPU9250_EDa_GPIO_Port GPIOB
#define MPU9250_Int_Pin GPIO_PIN_12
#define MPU9250_Int_GPIO_Port GPIOB
#define AS69_MD1_Pin GPIO_PIN_13
#define AS69_MD1_GPIO_Port GPIOB
#define AS69_Tx_Pin GPIO_PIN_14
#define AS69_Tx_GPIO_Port GPIOB
#define AS69_Rx_Pin GPIO_PIN_15
#define AS69_Rx_GPIO_Port GPIOB
#define Lcd_Bl_Pin GPIO_PIN_6
#define Lcd_Bl_GPIO_Port GPIOC
#define Kb_MR1_Pin GPIO_PIN_7
#define Kb_MR1_GPIO_Port GPIOC
#define Kb_MC4_Pin GPIO_PIN_8
#define Kb_MC4_GPIO_Port GPIOA
#define Kb_MR2_Pin GPIO_PIN_9
#define Kb_MR2_GPIO_Port GPIOA
#define Kb_SwL_Pin GPIO_PIN_10
#define Kb_SwL_GPIO_Port GPIOA
#define Kb_MR3_Pin GPIO_PIN_15
#define Kb_MR3_GPIO_Port GPIOA
#define Kb_SwR_Pin GPIO_PIN_3
#define Kb_SwR_GPIO_Port GPIOD
#define Kb_MR4_Pin GPIO_PIN_6
#define Kb_MR4_GPIO_Port GPIOD
#define Kb_MC1_Pin GPIO_PIN_3
#define Kb_MC1_GPIO_Port GPIOB
#define Kb_LedB_Pin GPIO_PIN_4
#define Kb_LedB_GPIO_Port GPIOB
#define Kb_MC2_Pin GPIO_PIN_5
#define Kb_MC2_GPIO_Port GPIOB
#define Tp_SDa_Pin GPIO_PIN_7
#define Tp_SDa_GPIO_Port GPIOB
#define Tp_SCl_Pin GPIO_PIN_8
#define Tp_SCl_GPIO_Port GPIOB
#define Tp_Int_Pin GPIO_PIN_9
#define Tp_Int_GPIO_Port GPIOB
#define Tp_Int_EXTI_IRQn EXTI9_5_IRQn
#define Tp_Rst_Pin GPIO_PIN_0
#define Tp_Rst_GPIO_Port GPIOE
#define Kb_MC3_Pin GPIO_PIN_1
#define Kb_MC3_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

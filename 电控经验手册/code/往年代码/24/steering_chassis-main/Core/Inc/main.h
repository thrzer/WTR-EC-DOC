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
#include "stm32f4xx_hal.h"

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
#define LED8_Pin GPIO_PIN_8
#define LED8_GPIO_Port GPIOG
#define LED7_Pin GPIO_PIN_7
#define LED7_GPIO_Port GPIOG
#define LED6_Pin GPIO_PIN_6
#define LED6_GPIO_Port GPIOG
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOG
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOG
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOG
#define PLATFORM_LS_1_Pin GPIO_PIN_10
#define PLATFORM_LS_1_GPIO_Port GPIOH
#define PLATFORM_LS_1_EXTI_IRQn EXTI15_10_IRQn
#define SWHEEL_3_LS_Pin GPIO_PIN_15
#define SWHEEL_3_LS_GPIO_Port GPIOD
#define SWHEEL_3_LS_EXTI_IRQn EXTI15_10_IRQn
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOG
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOG
#define SWHEEL_2_LS_Pin GPIO_PIN_14
#define SWHEEL_2_LS_GPIO_Port GPIOD
#define SWHEEL_2_LS_EXTI_IRQn EXTI15_10_IRQn
#define SWHEEL_1_LS_Pin GPIO_PIN_13
#define SWHEEL_1_LS_GPIO_Port GPIOD
#define SWHEEL_1_LS_EXTI_IRQn EXTI15_10_IRQn
#define SWHEEL_0_LS_Pin GPIO_PIN_12
#define SWHEEL_0_LS_GPIO_Port GPIOD
#define SWHEEL_0_LS_EXTI_IRQn EXTI15_10_IRQn
#define LDR_Pin GPIO_PIN_11
#define LDR_GPIO_Port GPIOE
#define LDG_Pin GPIO_PIN_14
#define LDG_GPIO_Port GPIOF

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

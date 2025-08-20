/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DJI.h"
#include "Caculate.h"
#include "wtr_can.h"
#include "_startup.h"
#include "pop.h"
#include "rmctl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for rmctltask */
osThreadId_t rmctltaskHandle;
const osThreadAttr_t rmctltask_attributes = {
  .name = "rmctltask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void RmctlTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of rmctltask */
  rmctltaskHandle = osThreadNew(RmctlTask, NULL, &rmctltask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    PopInit(&pop);
    PopCorrect(&pop);
    int ledcnt = 0;
    float testdis = 0;
    for (;;) {
      PopCharge(&pop,testdis);
      PopExecutor(&pop);  
      if (rmctl.rmctl_msg.btn_Btn1 == true)//蓄力
      {
        testdis += 4.0f;
        if (testdis >= 16500.0f)
        {
          testdis = 16500.0f;
        }
      }
      else if (rmctl.rmctl_msg.btn_Btn2 == true)//反蓄力
      {
        testdis -= 4.0f;
        if (testdis <= 0.0f)
        {
          testdis = 0.0f;
        }
      }
      if (rmctl.rmctl_msg.btn_Btn0 == true)
      {
        PopRelease(&pop);
        testdis = 0.0f;
      }
      //led
      ledcnt++;
      if (ledcnt >= 100)
      {
        ledcnt = 0;
        HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_2);
      }
      osDelay(5);
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_RmctlTask */
/**
* @brief Function implementing the rmctltask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RmctlTask */
void RmctlTask(void *argument)
{
  /* USER CODE BEGIN RmctlTask */
  /* Infinite loop */
  // __HAL_UART_ENABLE_IT(&huart6,UART_IT_RXNE);
  rmctl_Init(&rmctl);
  rmctl_decode(&rmctl.rmctl_msg);
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END RmctlTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


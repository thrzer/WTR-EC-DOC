/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "event_groups.h"

#include "_chassis_thread.h"
#include "_micro_ros_thread.h"
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

xTaskHandle upControlTaskHandle;
xTaskHandle microrosTaskHandle;
xTaskHandle chassisTaskHandle;

SemaphoreHandle_t sync_mutex;
SemaphoreHandle_t data_mutex;
/* USER CODE END Variables */
/* Definitions for nullTask */
osThreadId_t nullTaskHandle;
const osThreadAttr_t nullTask_attributes = {
  .name = "nullTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartNullTask(void *argument);

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
  
    //sync_mutex = xSemaphoreCreateBinary();
    data_mutex = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of nullTask */
  nullTaskHandle = osThreadNew(StartNullTask, NULL, &nullTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

if(xTaskCreate(StartMicrorosTask,"micro ros",3000,NULL,1,&microrosTaskHandle)!=pdPASS)
{
    while(1);
}
if(xTaskCreate(StartChassisTask,"chassis task",512,NULL,5,&chassisTaskHandle)!=pdPASS)
{
    while (1); 
}


  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */

  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartNullTask */
/**
  * @brief  Function implementing the nullTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartNullTask */
void StartNullTask(void *argument)
{
  /* USER CODE BEGIN StartNullTask */
  /* Infinite loop */
  vTaskDelete(NULL);
  /* USER CODE END StartNullTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


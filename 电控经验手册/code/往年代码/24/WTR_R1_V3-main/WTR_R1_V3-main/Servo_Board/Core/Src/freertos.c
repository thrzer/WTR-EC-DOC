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
#include "tim.h"
#include "usart.h"
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
uint8_t board_rev_buffer[7];
uint8_t laserx_rev_buffer[11];
uint8_t lasery_rev_buffer[11];

uint8_t laser_y_buffer[5];
uint8_t laser_x_buffer[5];

uint8_t x_flag = 0;
uint8_t y_flag = 0;

char laser_msg[5]          = "iFACM";
uint8_t laser_send_msg[12] = {0};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t laserTaskHandle;
const osThreadAttr_t laserTask_attributes = {
    .name       = "laserTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void LaserTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
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
    osDelay(3000);
    HAL_UART_Transmit(&huart6, (uint8_t *)laser_msg, 5, 0xFF);
    HAL_UART_Transmit(&huart7, (uint8_t *)laser_msg, 5, 0xFF);

    HAL_UART_Receive_DMA(&huart7, laserx_rev_buffer, 11);
    __HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart6, lasery_rev_buffer, 11);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

    laserTaskHandle = osThreadNew(LaserTask, NULL, &laserTask_attributes);
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void LaserTask(void *argument)
{
    for (;;) {
        laser_send_msg[0]  = 0xCC;
        laser_send_msg[11] = 0xDD;
        if (x_flag == 1) {
            laser_send_msg[1] = laser_x_buffer[0];
            laser_send_msg[2] = laser_x_buffer[1];
            laser_send_msg[3] = laser_x_buffer[2];
            laser_send_msg[4] = laser_x_buffer[3];
            laser_send_msg[5] = laser_x_buffer[4];
            x_flag            = 0;
        }
        if (y_flag == 1) {
            laser_send_msg[6]  = laser_y_buffer[0];
            laser_send_msg[7]  = laser_y_buffer[1];
            laser_send_msg[8]  = laser_y_buffer[2];
            laser_send_msg[9]  = laser_y_buffer[3];
            laser_send_msg[10] = laser_y_buffer[4];
            y_flag             = 0;
        }
        HAL_UART_Transmit(&huart3, laser_send_msg, 12, 0xFF);
        osDelay(1);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART7) {
        if (laserx_rev_buffer[0] == 0x44 && laserx_rev_buffer[1] == 0x3D && laserx_rev_buffer[7] == 0x6D && laserx_rev_buffer[8] == 0x0D && laserx_rev_buffer[9] == 0x0A) {
            laser_x_buffer[0] = laserx_rev_buffer[2];
            laser_x_buffer[1] = laserx_rev_buffer[3];
            laser_x_buffer[2] = laserx_rev_buffer[4];
            laser_x_buffer[3] = laserx_rev_buffer[5];
            laser_x_buffer[4] = laserx_rev_buffer[6];
            x_flag            = 1;
        } else {
            for (int i = 0; i < 11; i++) {
                laserx_rev_buffer[i] = 0;
            }
        }
    } else if (huart->Instance == USART6) {
        if (lasery_rev_buffer[0] == 0x44 && lasery_rev_buffer[1] == 0x3D && lasery_rev_buffer[7] == 0x6D && lasery_rev_buffer[8] == 0x0D && lasery_rev_buffer[9] == 0x0A) {
            laser_y_buffer[0] = lasery_rev_buffer[2];
            laser_y_buffer[1] = lasery_rev_buffer[3];
            laser_y_buffer[2] = lasery_rev_buffer[4];
            laser_y_buffer[3] = lasery_rev_buffer[5];
            laser_y_buffer[4] = lasery_rev_buffer[6];
            y_flag            = 1;
        } else {
            for (int i = 0; i < 11; i++) {
                lasery_rev_buffer[i] = 0;
            }
        }
    }
}
/* USER CODE END Application */

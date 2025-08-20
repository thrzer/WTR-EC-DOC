//
// Created by tony on 23-11-18.
//

#include "_mavlink_thread.h"
#include "main.h"
// #include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"
#include "wtr_mavlink.h"
#include <stdint.h>
// #include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "_chassis_thread.h"

mavlink_speed_t mv_cmd;
// mavlink_up_control_t event ;
uint8_t ch[1];
void upControl_event_pub(const mavlink_message_t * msg);

void StartMavlinkTask(void *argument)
{

    // 绑定串口和通道
    wtrMavlink_BindChannel(&huart6, MAVLINK_COMM_0);
    // 开启通道0的接收中断
    wtrMavlink_StartReceiveIT(MAVLINK_COMM_0);
    HAL_UART_Receive_IT(&huart6, (uint8_t *)&ch, 1);
    // event.cmd=0;
    //vTaskDelay(3000/ portTICK_RATE_MS);
    for(;;)
    {


        mavlink_heartbeat_t heartbeat;
        heartbeat.status = 0;
        heartbeat.time = xTaskGetTickCount();
        mavlink_msg_heartbeat_send_struct(MAVLINK_COMM_0, &heartbeat);


        // if(event.cmd)
        //     xEventGroupSetBits(UP_Control_Event_Handle, event.cmd);
        // //HAL_GPIO_TogglePin(LDG_GPIO_Port, LDG_Pin);
        // uint32_t w = uxTaskGetStackHighWaterMark(NULL);
        // printf("%d",w);
        vTaskDelay(1000/ portTICK_RATE_MS);
           }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 接收通道0的消息
    wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
}

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    switch (msg->msgid) {
//        case MAVLINK_MSG_ID_POSE:
//            // 将接收到的消息解码
//            mavlink_msg_pose_decode(msg, &actual_pose);
//            break;
        case MAVLINK_MSG_ID_SPEED:
            // 将接收到的消息解码
            //xSemaphoreTakeFromISR(data_mutex, 0);HAL_GPIO_TogglePin(LDG_GPIO_Port, LDG_Pin);
            mavlink_msg_speed_decode(msg, &mv_cmd);
            target[0] =mv_cmd.vx;
            target[1] =mv_cmd.vy;
            target[2] =mv_cmd.vw;
            //xSemaphoreGiveFromISR(data_mutex, 0);
            break;
            // ......
        // case MAVLINK_MSG_ID_UP_CONTROL:
        //     //upControl_event_pub(msg);
        //     HAL_GPIO_TogglePin(LDG_GPIO_Port, LDG_Pin);
        //     mavlink_msg_up_control_decode(msg, &event);
        //     break;
        default:
            break;
    }
}


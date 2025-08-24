#ifndef ENCODER_H
#define ENCODER_H

/**
 * @file encoder.h
 * @author Man
 * @brief 维特智能JY-ME02-CAN角编码使用代码
 * @date 2025-3-22
 */
/**
 * 请使用CAN转USB模块连接角编码器，并用上位机进行包括但不限于波特率、初始角度、回传速率等参数的配置
 * 产品说明书：https://wit-motion.yuque.com/wumwnr/docs/qcgu36
 * 计算数据结构体EncoderCalculateData中的数据均是实际数值的100倍
 * 记得在所有任务执行之前写上ENCODER_CANFilterInit(&hcan2);HAL_CAN_Start(&hcan2);HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
 */
#include "stm32f4xx_hal.h"
#include "can.h"
#include <stdio.h>

// 定义接收数据结构
typedef struct {
    float angle;       // 角度
    float angularSpeed; // 角速度
    int16_t revolutions; // 转数
    float temperature; // 温度
} EncoderData;

//定义计算数据结构体
typedef struct {
    int32_t angle;       // 角度
    int32_t angularSpeed; // 角速度
}EncoderCalculateData;

extern EncoderData data;
extern EncoderCalculateData caldata;

// 函数声明
HAL_StatusTypeDef ENCODER_CANFilterInit(CAN_HandleTypeDef* hcan);
HAL_StatusTypeDef SendConfigCommand(CAN_HandleTypeDef* hcan, uint8_t* command, uint8_t length);
void Encoder_Setup(CAN_HandleTypeDef* hcan);
void ParseCANData(CAN_RxHeaderTypeDef *RxHeader, uint8_t *RxData, EncoderData *data,EncoderCalculateData *caldata);
HAL_StatusTypeDef Encoder_Init(void);

extern CAN_HandleTypeDef hcan2;
#endif // ENCODER_H
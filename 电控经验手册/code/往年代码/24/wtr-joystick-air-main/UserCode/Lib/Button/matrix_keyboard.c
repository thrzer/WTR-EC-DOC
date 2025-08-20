/**
 * @file matrix_keyboard.cpp
 * @author X. Y.
 * @brief 矩阵键盘读取程序。所有引脚都要配置成上拉开漏输出
 * @version 0.1
 * @date 2023-05-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "matrix_keyboard.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "main.h"

/**
 * @brief 读取所有列
 *
 * @return uint8_t 低4位有效，0 表示按下
 */
static uint8_t ReadColumn()
{
    uint8_t result = 0;
    result |= ((int)HAL_GPIO_ReadPin(Kb_MC1_GPIO_Port, Kb_MC1_Pin)) << 0;
    result |= ((int)HAL_GPIO_ReadPin(Kb_MC2_GPIO_Port, Kb_MC2_Pin)) << 1;
    result |= ((int)HAL_GPIO_ReadPin(Kb_MC3_GPIO_Port, Kb_MC3_Pin)) << 2;
    result |= ((int)HAL_GPIO_ReadPin(Kb_MC4_GPIO_Port, Kb_MC4_Pin)) << 3;
    return result;
}

/**
 * @brief 设置第 index - 1 行低电平，其他行高电平（如果超出范围，则所有行都为高电平）
 *
 */
static void SetRow(int index)
{
    HAL_GPIO_WritePin(Kb_MR1_GPIO_Port, Kb_MR1_Pin, (index == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Kb_MR2_GPIO_Port, Kb_MR2_Pin, (index == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Kb_MR3_GPIO_Port, Kb_MR3_Pin, (index == 2) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(Kb_MR4_GPIO_Port, Kb_MR4_Pin, (index == 3) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief 扫描所有按键
 *
 * @return uint16_t 16个按键的状态。1号按键为最低位，16号按键为最高位。每一位 1 表示按下。
 * @note 由于矩阵键盘的限制，当三个及以上键同时按下时，可能有键位冲突，此时扫描结果可能是错误的
 */
uint16_t MatrixKeyboard_Scan()
{
    uint16_t result = 0;

    for (size_t i = 0; i < 4; i++) {
        SetRow(i);
        HPT_DelayUs(50); // 延时等待 gpio 稳定
        result |= ReadColumn() << (i * 4);
    }

    SetRow(-1); // 所有行设为高电平，大概能省一点点电

    return ~result;
}

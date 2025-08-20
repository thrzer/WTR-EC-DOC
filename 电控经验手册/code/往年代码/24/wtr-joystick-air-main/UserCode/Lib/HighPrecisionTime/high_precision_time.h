/**
 * @file micro_second.h
 * @author X. Y.
 * @brief 高精度时间库
 * @version 0.4
 * @date 2023-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief 获取当前微秒值
 *
 * @return uint32_t 微秒值
 */
uint32_t HPT_GetUs();

/**
 * @brief 获取当前纳秒值
 * @note 在 8 位 或 16 位 MCU 上不建议在中断中使用该函数（32 位 MCU 不受影响）
 * @return uint64_t 纳秒值
 */
uint64_t HPT_GetNs();

/**
 * @brief 高精度延时
 * @note 比 vTaskDelay 和 HAL_Delay 精度高
 * @param ms 要延时的毫秒数
 */
void HPT_DelayMs(uint32_t ms);

/**
 * @brief 高精度延时
 * @note 在 FreeRTOS 下，如果在线程中调用该函数且延时大于1000微秒，会自动调用 vTaskDelay（即会让线程进入阻塞状态(Blocked state)）
 *       如果小于 1000 微秒，则不会让线程阻塞（这意味着优先级比该线程低的所有线程在延时中都不会运行）
 *       在线程中调用时，如果对延时精度要求很高，建议把该线程的优先级提高到最高等级
 * @param us 要延时的微秒数
 */
void HPT_DelayUs(uint32_t us);

#ifdef __cplusplus
}
#endif

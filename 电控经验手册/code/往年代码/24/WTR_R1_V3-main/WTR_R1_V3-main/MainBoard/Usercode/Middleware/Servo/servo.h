#ifndef __SERVO_H
#define __SERVO_H
#ifdef __cplusplus
extern "C" {
#endif
#include "usermain.h"
#include "tim.h"

/*************************************************************
 * 舵机定义
 */
// 左侧放苗板
#define Left_Plant_Seed_Timer_Handle  htim4
#define Left_Plant_Seed_Timer_Channel TIM_CHANNEL_4
// 左侧前限位板
#define Left_Front_Seed_Timer_Handle  htim4
#define Left_Front_Seed_Timer_Channel TIM_CHANNEL_3
// 左侧推苗板
#define Left_Buffer_Seed_Timer_Handle  htim4
#define Left_Buffer_Seed_Timer_Channel TIM_CHANNEL_2
// 左侧轨道限位板
#define Left_Deposit_Seed_Timer_Handle  htim4
#define Left_Deposit_Seed_Timer_Channel TIM_CHANNEL_1
// 左侧左夹爪
#define Left_Left_Seed_Timer_Handle  htim3
#define Left_Left_Seed_Timer_Channel TIM_CHANNEL_4
// 左侧右夹爪
#define Left_Right_Seed_Timer_Handle  htim3
#define Left_Right_Seed_Timer_Channel TIM_CHANNEL_3

// 右侧放苗板
#define Right_Plant_Seed_Timer_Handle  htim5
#define Right_Plant_Seed_Timer_Channel TIM_CHANNEL_1
// 右侧前限位板
#define Right_Front_Seed_Timer_Handle  htim5
#define Right_Front_Seed_Timer_Channel TIM_CHANNEL_2
// 右侧推苗板
#define Right_Buffer_Seed_Timer_Handle  htim5
#define Right_Buffer_Seed_Timer_Channel TIM_CHANNEL_3
// 右侧轨道限位板
#define Right_Deposit_Seed_Timer_Handle  htim5
#define Right_Deposit_Seed_Timer_Channel TIM_CHANNEL_4
// 右侧左夹爪
#define Right_Left_Seed_Timer_Handle  htim8
#define Right_Left_Seed_Timer_Channel TIM_CHANNEL_3
// 右侧右夹爪
#define Right_Right_Seed_Timer_Handle  htim8
#define Right_Right_Seed_Timer_Channel TIM_CHANNEL_4

// 球推球板
#define Ball_Pull_Timer_Handle  htim15
#define Ball_Pull_Timer_Channel TIM_CHANNEL_1
// 球中夹爪
#define Ball_Middle_Timer_Handle  htim2
#define Ball_Middle_Timer_Channel TIM_CHANNEL_4
// 球左夹爪
#define Ball_Left_Timer_Handle  htim2
#define Ball_Left_Timer_Channel TIM_CHANNEL_3
// 球右夹爪
#define Ball_Right_Timer_Handle  htim2
#define Ball_Right_Timer_Channel TIM_CHANNEL_1

/*************************************************************
 * 舵机动作定义
 */

// 球动作
#define Ball_Servo_In()                                                                  \
    do {                                                                                 \
        __HAL_TIM_SET_COMPARE(&Ball_Left_Timer_Handle, Ball_Left_Timer_Channel, 750);    \
        __HAL_TIM_SET_COMPARE(&Ball_Right_Timer_Handle, Ball_Right_Timer_Channel, 2250); \
    } while (0) // 左右夹爪动作-----向内收缩

#define Ball_Servo_Out()                                                                 \
    do {                                                                                 \
        __HAL_TIM_SET_COMPARE(&Ball_Left_Timer_Handle, Ball_Left_Timer_Channel, 1750);   \
        __HAL_TIM_SET_COMPARE(&Ball_Right_Timer_Handle, Ball_Right_Timer_Channel, 1250); \
    } while (0) // 左右夹爪动作-----向外伸张

#define Ball_Servo_Grip()                                                                 \
    do {                                                                                  \
        __HAL_TIM_SET_COMPARE(&Ball_Middle_Timer_Handle, Ball_Middle_Timer_Channel, 700); \
    } while (0) // 夹爪抓取动作

#define Ball_Servo_Reset()                                                                 \
    do {                                                                                   \
        __HAL_TIM_SET_COMPARE(&Ball_Middle_Timer_Handle, Ball_Middle_Timer_Channel, 2000); \
    } while (0) // 夹爪放松动作

#define Ball_Servo_Ready()                                                             \
    do {                                                                               \
        __HAL_TIM_SET_COMPARE(&Ball_Pull_Timer_Handle, Ball_Pull_Timer_Channel, 2100); \
    } while (0) // 射球准备动作

#define Ball_Servo_Fire()                                                              \
    do {                                                                               \
        __HAL_TIM_SET_COMPARE(&Ball_Pull_Timer_Handle, Ball_Pull_Timer_Channel, 1400); \
    } while (0) // 射球动作

// 苗动作
#define Seed_Grip()                                                                                  \
    do {                                                                                             \
        __HAL_TIM_SET_COMPARE(&Left_Left_Seed_Timer_Handle, Left_Left_Seed_Timer_Channel, 1500);     \
        __HAL_TIM_SET_COMPARE(&Left_Right_Seed_Timer_Handle, Left_Right_Seed_Timer_Channel, 1500);   \
        __HAL_TIM_SET_COMPARE(&Right_Left_Seed_Timer_Handle, Right_Left_Seed_Timer_Channel, 1550);   \
        __HAL_TIM_SET_COMPARE(&Right_Right_Seed_Timer_Handle, Right_Right_Seed_Timer_Channel, 1450); \
    } while (0) // 夹爪夹取苗动作

#define Seed_Deposit()                                                                               \
    do {                                                                                             \
        __HAL_TIM_SET_COMPARE(&Left_Left_Seed_Timer_Handle, Left_Left_Seed_Timer_Channel, 1800);     \
        __HAL_TIM_SET_COMPARE(&Left_Right_Seed_Timer_Handle, Left_Right_Seed_Timer_Channel, 1200);   \
        __HAL_TIM_SET_COMPARE(&Right_Left_Seed_Timer_Handle, Right_Left_Seed_Timer_Channel, 1200);   \
        __HAL_TIM_SET_COMPARE(&Right_Right_Seed_Timer_Handle, Right_Right_Seed_Timer_Channel, 1700); \
    } while (0) // 夹爪放置苗动作

#define Seed_Deposit_Buffer_Open()                                                                    \
    do {                                                                                              \
        __HAL_TIM_SET_COMPARE(&Left_Buffer_Seed_Timer_Handle, Left_Buffer_Seed_Timer_Channel, 2500);  \
        __HAL_TIM_SET_COMPARE(&Right_Buffer_Seed_Timer_Handle, Right_Buffer_Seed_Timer_Channel, 900); \
    } while (0) // 推苗板打开,左2500,右侧900

#define Seed_Deposit_Buffer_Close()                                                                    \
    do {                                                                                               \
        __HAL_TIM_SET_COMPARE(&Left_Buffer_Seed_Timer_Handle, Left_Buffer_Seed_Timer_Channel, 2000);   \
        __HAL_TIM_SET_COMPARE(&Right_Buffer_Seed_Timer_Handle, Right_Buffer_Seed_Timer_Channel, 1250); \
    } while (0) // 推苗板放下

#define Seed_Plant_Open()                                                                            \
    do {                                                                                             \
        __HAL_TIM_SET_COMPARE(&Left_Plant_Seed_Timer_Handle, Left_Plant_Seed_Timer_Channel, 2000);   \
        __HAL_TIM_SET_COMPARE(&Right_Plant_Seed_Timer_Handle, Right_Plant_Seed_Timer_Channel, 1000); \
    } while (0) // 放苗板打开

#define Seed_Plant_Close()                                                                           \
    do {                                                                                             \
        __HAL_TIM_SET_COMPARE(&Left_Plant_Seed_Timer_Handle, Left_Plant_Seed_Timer_Channel, 1500);   \
        __HAL_TIM_SET_COMPARE(&Right_Plant_Seed_Timer_Handle, Right_Plant_Seed_Timer_Channel, 1500); \
    } while (0) // 放苗板关闭

#define Seed_Deposit_Close()                                                                             \
    do {                                                                                                 \
        __HAL_TIM_SET_COMPARE(&Left_Deposit_Seed_Timer_Handle, Left_Deposit_Seed_Timer_Channel, 1600);   \
        __HAL_TIM_SET_COMPARE(&Right_Deposit_Seed_Timer_Handle, Right_Deposit_Seed_Timer_Channel, 1600); \
    } while (0) // 轨道限位板关闭

#define Seed_Deposit_Open()                                                                              \
    do {                                                                                                 \
        __HAL_TIM_SET_COMPARE(&Left_Deposit_Seed_Timer_Handle, Left_Deposit_Seed_Timer_Channel, 2400);    \
        __HAL_TIM_SET_COMPARE(&Right_Deposit_Seed_Timer_Handle, Right_Deposit_Seed_Timer_Channel, 600); \
    } while (0) // 轨道限位板打开

#define Seed_Front_Close()                                                                          \
    do {                                                                                            \
        __HAL_TIM_SET_COMPARE(&Left_Front_Seed_Timer_Handle, Left_Front_Seed_Timer_Channel, 2000);  \
        __HAL_TIM_SET_COMPARE(&Right_Front_Seed_Timer_Handle, Right_Front_Seed_Timer_Channel, 1100); \
    } while (0) // 前侧限位板关闭

#define Seed_Front_Open()                                                                            \
    do {                                                                                             \
        __HAL_TIM_SET_COMPARE(&Left_Front_Seed_Timer_Handle, Left_Front_Seed_Timer_Channel, 600);    \
        __HAL_TIM_SET_COMPARE(&Right_Front_Seed_Timer_Handle, Right_Front_Seed_Timer_Channel, 2400); \
    } while (0) // 前侧限位板打开

/*************************************************************
 * 用户函数
 */

void m_Servo_Init(void);

#ifdef __cplusplus
}
#endif
#endif
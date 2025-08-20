#ifndef POP_H
#define POP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pop.h
 * @author thrzer
 * @brief wtr一代车弹射机构
 * @date 2024-11-6
 */

#include "DJI.h"

//电磁铁开关引脚
#define POP_GPIOX GPIOF
#define POP_GPIO_PIN GPIO_PIN_0

#define RESET_SPEED1 -1000.0f     //第1段复位速度rpm,快速回到复位点附近
#define RESET_SPEED2 400.0f      //第2段复位速度rpm,回弹速度
#define RESET_SPEED3 -200.0f     //第3段复位速度rpm,慢速精准复位

#define STALL_CURRENT -900.0f    //堵转电流标志
#define BUFFER_POSITION 50.0f    //缓冲距离
#define PRECHARGE_POSITION 0.0f  //预蓄力位置

typedef struct
{
    DJI_t *motor;   //蓄力电机
    float position; //蓄力位置(电机角度)
    float reset_position;   //复位点位置
} pop_t;            //弹射机构

void PopInit(pop_t* this);
void PopCorrect(pop_t *this);
void PopRelease(pop_t* this);
void PopCharge(pop_t* this,float input_positon);
void PopExecutor(pop_t* this);

extern pop_t pop;

#ifdef __cplusplus
}
#endif
#endif //POP_H

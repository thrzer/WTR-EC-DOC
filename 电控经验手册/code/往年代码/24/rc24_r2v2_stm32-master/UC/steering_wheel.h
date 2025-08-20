//
// Created by tony on 23-10-24.
//

#ifndef STEERING_WHEEL_H
#define STEERING_WHEEL_H
#ifdef __cplusplus
extern "C" {
#endif
// #define USE_M2006_AS_ROTATION_MOTOR
#define USE_DEFAULT_MOTOR_PARAM
#ifdef USE_DEFAULT_MOTOR_PARAM
#include "wtr_vesc.h"
extern VESC_t hVESC[4];
#endif

#define STEERING_WHEEL_DIAMETER           0.1f // 轮子直径，单位m
#include "main.h"
#include "HStateMachine.h"

typedef enum {
    CORRECTING,
    RUNNING,
    AIMMING,
    STOP,
    ERROR_
} Swheel_state_e; // 舵轮状态机
typedef void (*r_getSpeed_func)(void *this);
typedef void (*r_getPos_func)(void *this);
typedef void (*r_speedServo_func)(void *this, const float speed);
typedef void (*r_posServo_func)(void *this, const float pos);
typedef void (*r_reset_func)(void *this);
typedef void (*r_stop_func)(void *this);
typedef void (*m_speedServo_func)(void *this, const float speed);
typedef void (*m_stop_func)(void *this);

typedef struct {
    uint8_t id;
    GPIO_TypeDef *_LS_GPIOx;    // 限位开关GPIO
    uint16_t _LS_GPIO_Pin;      // 限位开关引脚
    float rotation_reduction;// 转向齿轮减速比（大比小）
    float correcting_speed; // 校准速度，单位rad/s
    r_getSpeed_func RgetSpeed;
    r_getPos_func RgetPos;
    r_speedServo_func RspeedServo;
    r_posServo_func RposServo;
    r_reset_func Rreset;
    r_stop_func Rstop;
    m_speedServo_func MspeedServo;
    m_stop_func Mstop;

    HSM parent;
    uint8_t _light_switch_flag; // 开关标志位
    float rotation_speed; // 转向速度，单位rad/s
    float target_direction;
    float target_main_speed;
    float direction; // 方向，弧度制，0为正前方，逆时针为正，范围-2pi到2pi
    float rotation_pos; // 弧度制，0为正前方，逆时针为正
    int16_t correcting_stage;
    Swheel_state_e state;
} Swheel_t;



void Swheel_init(
    Swheel_t *this,    
    uint8_t id,
    GPIO_TypeDef *_LS_GPIOx,
    uint16_t _LS_GPIO_Pin, 
    float rotation_reduction,
    float correcting_speed,
    r_getSpeed_func RgetSpeed,
    r_getPos_func RgetPos,
    r_speedServo_func RspeedServo,
    r_posServo_func RposServo,
    r_reset_func Rreset,
    r_stop_func Rstop,
    m_speedServo_func MspeedServo,
    m_stop_func Mstop);
void Swheel_startCorrect(Swheel_t *this);
void steeringWheel_executor(Swheel_t *this);
void Swheel_EXTI_Callback(Swheel_t *this, uint16_t GPIO_Pin);
void steeringWheel_rMotor_reset(Swheel_t *this);
//    void Swheel_mMotor_getSpeed(Swheel_t *this);
//    void Swheel_mMotor_speedServo(Swheel_t *this, const float speed);
//    void Swheel_mMotor_getPos(Swheel_t *this);

#ifdef USE_DEFAULT_MOTOR_PARAM
void mDJI_RgetSpeed(void *this);
void mDJI_RgetPos(void *this);
void mDJI_RspeedServo(void *this, const float speed);
void mDJI_RposServo(void *this, const float pos);
void mDJI_Rreset(void *this);
void vesc_MspeedServo(void *this, const float speed);
void vesc_Mstop(void *this);
#endif

#ifdef __cplusplus
}
#endif
#endif // STEERING_WHEEL_H

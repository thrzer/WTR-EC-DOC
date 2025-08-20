#ifndef HSM_H
#define HSM_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file HSM.h
 * @author thrzer
 * @brief wtr二代车底盘部分状态机
 * @date 2024-4-23
 */

typedef enum
{
    HSM_STATE_ON,       //上电
    HSM_STATE_RUNNING,  //正在运行
    HSM_STATE_OFF       //急停
} HSM_State_e;          //整车状态

typedef enum
{
    HSM_rSTATE_DISCONNECTED, //未连接
    HSM_rSTATE_CONNECTED,    //已连接
} HSM_RemotectlState_e;      //遥控器状态

typedef enum
{
    HSM_cSTATE_ON,          //上电
    HSM_cSTATE_INIT,        //初始化中
    HSM_cSTATE_CORRECTING,  //校正中
    HSM_cSTATE_STOP,        //静止态
    HSM_cSTATE_AIMMING,     //定向态(仅转向轮运动)
    HSM_cSTATE_RUNNING,     //运行态
    HSM_cSTATE_LOCK,        //锁定态
} HSM_ChassisState_e;       //底盘状态

typedef struct
{
    HSM_State_e state;
    HSM_RemotectlState_e remotectl;
    HSM_ChassisState_e chassis_mechanism;
    char** error_msg;   //错误信息
} HSM_t;  //二代车底盘部分

void HSM_start(HSM_t* this);

extern HSM_t wtrV2;
extern char HSM_error_msg[5][20];

#ifdef __cplusplus
}
#endif

#endif
//
// Created by Liam
// 2023/12/01

#ifndef HSTATEMACHINE_H
#define HSTATEMACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

#define HSM_MAX_DEPTH  5

//----State definitions----
#define HSM_NULL   0
#define HSM_START  1
#define HSM_INIT   ((HSM_EVENT)(-3))
#define HSM_ENTRY  ((HSM_EVENT)(-2))
#define HSM_EXIT   ((HSM_EVENT)(-1))


typedef uint16_t HSM_EVENT ;
typedef struct HSM_T HSM;
typedef struct HSM_STATE_T HSM_STATE;
typedef HSM_EVENT (* HSM_FN)(HSM *This, HSM_EVENT event, void *param);

typedef struct HSM_STATE_T
{
    struct HSM_START_T *parent;          // parent state
    HSM_FN handler;             // associated event handler for state
    const char *name;           // name of state
    uint8_t level;              // depth level of the state
};

struct HSM_T
{
    HSM_STATE *curState;        // Current HSM State
};


void HSM_STATE_Create(HSM_STATE *This, const char *name, HSM_FN handler, HSM_STATE *parent);
void HSM_Create(HSM *This, const char *name, HSM_STATE *initState);
HSM_STATE *HSM_GetState(HSM *This);
uint8_t HSM_IsInState(HSM *This, HSM_STATE *state);
void HSM_Run(HSM *This, HSM_EVENT event, void *param);
void HSM_Tran(HSM *This, HSM_STATE *nextState, void *param, void (*method)(HSM *This, void *param));


typedef enum{R2_INIT,R2_READY,R2_RUNNING,R2_ERROR}R2_State_e;  //R2状态机 
typedef enum{CRUNNING,CFAIL,CERROR}Communication_State_e;  //通信状态机

#ifdef __cplusplus
}
#endif
#endif
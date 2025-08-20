#ifndef EVENT_DEFINE_H
#define EVENT_DEFINE_H
#include "HStateMachine.h"
#include "steering_wheel_chassis.h"
#include "sys/_intsup.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f4xx.h"

#define HSM_CHASSIS_START        (HSM_START)
#define HSM_CHASSIS_INIT         (HSM_START+1)
#define HSM_CHASSIS_CORRECTED    (HSM_START+2)   
#define HSM_CHASSIS_CORRECTING   (HSM_START+3)
#define HSM_CHASSIS_READY        (HSM_START+4)
#define HSM_CHASSIS_AMING        (HSM_START+5)
#define HSM_CHASSIS_RUNNING      (HSM_START+6)
#define HSM_CHASSIS_ERROR        (HSM_START+7)
#define HSM_CHASSIS_POWEROFF     (HSM_START+8)
#define HSM_CHASSIS_STOP         (HSM_START+9)

HSM_EVENT CHASSIS_ON_Handler(HSM *This, HSM_EVENT event, void *param);
HSM_EVENT CHASSIS_OFF_Handler(HSM *This, HSM_EVENT event, void *param);
HSM_EVENT CHASSIS_ONReady_Handler(HSM *This, HSM_EVENT event, void *param);
HSM_EVENT CHASSIS_ONError_Handler(HSM *This, HSM_EVENT event, void *param);
HSM_EVENT CHASSIS_ONCorrecting_Handler(HSM *This, HSM_EVENT event, void *param);
HSM_EVENT CHASSIS_ONReadyRunning_Handler(HSM *This, HSM_EVENT event, void *param);

void HSM_CHASSIS_Init(swChassis_t *This, char *name);
void HSM_CHASSIS_Run(swChassis_t *This, HSM_EVENT event, void *param);

extern swChassis_t mychassis;
extern HSM_EVENT Next_Event;

#ifdef __cplusplus
}
#endif
#endif
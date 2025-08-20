#ifndef __USERMAIN_H
#define __USERMAIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdbool.h"
#include "cmsis_os2.h"

#include "remotectl.h"
#include "HWT101CT_sdk.h"
#include "ops_sdk.h"
#include "laser_sdk.h"
#include "laser.h"
#include "gyro.h"
#include "chassis.h"
#include "unitree.h"
#include "servo.h"
#include "odom.h"

#include "auto_task.h"
#include "handle_task.h"
#include "action.h"

#include "userconfig.h"
#include "userdebug.h"
#include "usermain_task.h"

extern uint8_t rst_flag;

#ifdef __cplusplus
}
#endif
#endif
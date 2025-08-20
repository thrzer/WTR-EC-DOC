#pragma once
#include "Mavlink/wtr_mavlink.h"

void InitMavlinkThread();

void StartMavlinkThread();

void StopMavlinkThread();

#ifdef __cplusplus
extern "C" {
#endif

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg);

#ifdef __cplusplus
}
#endif

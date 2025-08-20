#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void StartLvglThread();
void LvglLock();
void LvglUnlock();

#ifdef __cplusplus
}
#endif

#include "allusertask.h"
#include "HSM.h"
#include "rmctl.h"

/**
 * @brief 遥控器线程
 * @author thrzer
 * @date 2024-11-23
 */
void RmctlTask(void *argument)
{
  wtrV2.remotectl = HSM_rSTATE_DISCONNECTED;
  rmctl_Init(&rmctl);
  wtrV2.remotectl = HSM_rSTATE_CONNECTED;
  rmctl_decode(&rmctl.rmctl_msg);
}

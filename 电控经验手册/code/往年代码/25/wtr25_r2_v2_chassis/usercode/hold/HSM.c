#include "HSM.h"

HSM_t wtrV2;
char HSM_error_msg[5][20] = {};

/**
 * @brief 状态机启动函数
 * @param this
 */
void HSM_start(HSM_t* this)
{
    this->state = HSM_STATE_ON;
    this->error_msg = (char**)HSM_error_msg;
    if (1)//开机按键
    {
        this->state = HSM_STATE_RUNNING;
    }
    if (0)//紧急关机
    {
        this->state = HSM_STATE_OFF;
        this->error_msg[0] = "error!";
    }
}
#include "allusertask.h"
#include "HSM.h"
#include "rmctl.h"

extern unsigned int speedmode;//速度挡位
extern unsigned int lockmode;//锁定挡位

/**
 * @brief 状态机线程，优先级最高
 * @author thrzer
 * @date 2024-11-23
 */
void HsmTask(void *argument)
{
    int led1cnt = 0, led2cnt = 0, led3cnt = 0;
    /*
    PG1闪烁代表底盘运行正常
    PG2闪烁频率低代表低速模式，频率高代表高速模式
    PG3闪烁代表底盘未锁定，常亮代表底盘被锁定
    */
    m_Chassis_Gyro_Init();
    for (;;) {
        HSM_start(&wtrV2);
        led1cnt++;led2cnt++;led3cnt++;
        const static unsigned int N1 = 100;
        static unsigned int N2 = 100;
        N2 = speedmode == 0 ? 100 : 20;
        const static unsigned int N3 = 100;
        if (led1cnt >= N1)
        {
          led1cnt = 0;
          if (wtrV2.state != HSM_STATE_OFF)
            HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_1);
        }
        if (led2cnt >= N2)
        {
          led2cnt = 0;
          if (wtrV2.state != HSM_STATE_OFF)
            HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_2);
        }
        if (led3cnt >= N3)
        {
          led3cnt = 0;
          if (wtrV2.state != HSM_STATE_OFF)
          {
            if(lockmode == 0)
              HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_3);
            else
              HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3,RESET);
          }
        }

        osDelay(5);
    }
}
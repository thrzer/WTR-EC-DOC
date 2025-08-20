#pragma once
#include "flexible_button.h"

enum Keys {
    Btn_LeftCrossUp     = 2,
    Btn_LeftCrossDown   = 1,
    Btn_LeftCrossLeft   = 13,
    Btn_LeftCrossRight  = 5,
    Btn_LeftCrossMid    = 9,
    Btn_RightCrossUp    = 4,
    Btn_RightCrossDown  = 3,
    Btn_RightCrossLeft  = 7,
    Btn_RightCrossRight = 15,
    Btn_RightCrossMid   = 11,
    Btn_Btn0            = 6,
    Btn_Btn1            = 10,
    Btn_Btn2            = 14,
    Btn_Btn3            = 8,
    Btn_Btn4            = 12,
    Btn_Btn5            = 16,
    Btn_KnobL           = 17,
    Btn_KnobR           = 18,
    Btn_JoystickL       = 19,
    Btn_JoystickR       = 20,
    Switch_L            = 21,
    Switch_R            = 22
};

#ifdef __cplusplus
extern "C" {
#endif

void Buttons_Init();
void Buttons_Scan();
/**
 * @brief 读取按键或开关
 *
 * @param keys 按键或开关，如 Btn_LeftCrossUp, Switch_L
 * @return uint8_t 按键按下为 1，开关向上拨为 1
 */
uint8_t Buttons_Read(uint8_t keys);

#ifdef __cplusplus
}
#endif

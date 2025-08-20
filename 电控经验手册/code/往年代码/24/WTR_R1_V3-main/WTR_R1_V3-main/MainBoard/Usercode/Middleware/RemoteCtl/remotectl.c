#include "remotectl.h"

/*****************************************************************************
 *  线程定义
 */
osThreadId_t remotectl_TaskHandle;
const osThreadAttr_t remotectl_Task_attributes = {
    .name       = "remotectl_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_Remotectl_Task(void *argument);

/*****************************************************************************
 * 接口变量定义
 */
bool btn_LeftCrossUp;
bool btn_LeftCrossDown;
bool btn_LeftCrossLeft;
bool btn_LeftCrossRight;
bool btn_LeftCrossMid;
bool btn_RightCrossUp;
bool btn_RightCrossDown;
bool btn_RightCrossLeft;
bool btn_RightCrossRight;
bool btn_RightCrossMid;
bool btn_Btn0;
bool btn_Btn1;
bool btn_Btn2;
bool btn_Btn3;
bool btn_Btn4;
bool btn_Btn5;
bool btn_KnobL;
bool btn_KnobR;
bool btn_JoystickL;
bool btn_JoystickR;
bool left_switch;
bool right_switch;
float right_y;
float right_x;
float left_y;
float left_x;
int usr_right_y;
int usr_right_x;
int usr_left_y;
int usr_left_x;
float right_knob;
float left_knob;
float usr_right_knob;
float usr_left_knob;

/*****************************************************************************
 *  接口函数定义
 */
/**
 * @brief   遥控器初始化
 */
void m_RemoteCtl_Init(void)
{
    AS69_RemoteControl_Init();
    wtrMavlink_StartReceiveIT(MAVLINK_COMM_0); // 以mavlink接收遥控器
}

/**
 * @brief   遥控器接收线程初始化
 */
void m_RemoteCtl_Task_Start(void)
{
    remotectl_TaskHandle = osThreadNew(m_Remotectl_Task, NULL, &remotectl_Task_attributes);
}

/*****************************************************************************
 *  线程函数定义
 */
/**
 * @brief   遥控器接收线程初始化
 */
void m_Remotectl_Task(void *argument)
{
    static float left_knob_offset;
    for (int i = 0; i < 10; i++) {
        left_knob = ReadJoystickKnobsLeft(&msg_joystick_air);
        osDelay(5);
    }
    left_knob_offset = left_knob;
    for (;;) {
        btn_LeftCrossUp     = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossUp);
        btn_LeftCrossDown   = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossDown);
        btn_LeftCrossLeft   = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossLeft);
        btn_LeftCrossRight  = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossRight);
        btn_LeftCrossMid    = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossMid);
        btn_RightCrossUp    = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossUp);
        btn_RightCrossDown  = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossDown);
        btn_RightCrossLeft  = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossLeft);
        btn_RightCrossRight = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossRight);
        btn_RightCrossMid   = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossMid);
        btn_Btn0            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn0);
        btn_Btn1            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn1);
        btn_Btn2            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn2);
        btn_Btn3            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn3);
        btn_Btn4            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn4);
        btn_Btn5            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn5);
        btn_JoystickL       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickL);
        btn_JoystickR       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickR);
        btn_KnobL           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobL);
        btn_KnobR           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobR);
        left_switch         = ReadJoystickSwitchs(&msg_joystick_air, Left_switch);
        right_switch        = ReadJoystickSwitchs(&msg_joystick_air, Right_switch);
        right_x             = ReadJoystickRight_x(&msg_joystick_air);
        right_y             = ReadJoystickRight_y(&msg_joystick_air);
        left_x              = ReadJoystickLeft_x(&msg_joystick_air);
        left_y              = ReadJoystickLeft_y(&msg_joystick_air);
        left_knob           = ReadJoystickKnobsLeft(&msg_joystick_air);
        if (right_x < 0.15f && right_x > -0.15f) {
            usr_right_x = 0;
        } else if (right_x > 0.15f) {
            usr_right_x = (int)((right_x - 0.15) * 1000);
        } else {
            usr_right_x = (int)((right_x + 0.15) * 1000);
        }
        if (right_y < 0.15f && right_y > -0.15f) {
            usr_right_y = 0;
        } else if (right_y > 0.15f) {
            usr_right_y = (int)((right_y - 0.15) * 1000);
        } else {
            usr_right_y = (int)((right_y + 0.15) * 1000);
        }
        if (left_x < 0.15f && left_x > -0.15f) {
            usr_left_x = 0;
        } else if (left_x > 0.15f) {
            usr_left_x = (int)((left_x - 0.15) * 1000);
        } else {
            usr_left_x = (int)((left_x + 0.15) * 1000);
        }
        if (left_y < 0.15f && left_y > -0.15f) {
            usr_left_y = 0;
        } else if (left_y > 0.15f) {
            usr_left_y = (int)((left_y - 0.15) * 1000);
        } else {
            usr_left_y = (int)((left_y + 0.15) * 1000);
        }
        usr_left_knob = left_knob - left_knob_offset;
        osDelay(1);
    }
}
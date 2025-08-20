#include "rmctl.h"
#include "cmsis_os.h"

rmctl_t rmctl;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == RMCTL_UART) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
    }
}

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    switch (msg->msgid) {
        case MAVLINK_MSG_ID_JOYSTICK_AIR:
            mavlink_msg_joystick_air_decode(msg, &msg_joystick_air);
            break;
        default:
            break;
    }
}

/**
 * @brief 遥控器初始化，匹配串口
 * @param this
 */
void rmctl_Init(rmctl_t* this)
{
    this->huartx = &huart6;
    AS69_RemoteControl_Init();
}

/**
 * @brief 遥控器信息解码，会自动持续更新数据
 * @param this
 */
void rmctl_decode(rmctl_msg_t* this)
{
    static float left_knob_offset;
    for (int i = 0; i < 10; i++) {
        this->left_knob = ReadJoystickKnobsLeft(&msg_joystick_air);
        osDelay(5);
    }
    left_knob_offset = this->left_knob;
    for (;;) {
        this->btn_LeftCrossUp     = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossUp);
        this->btn_LeftCrossDown   = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossDown);
        this->btn_LeftCrossLeft   = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossLeft);
        this->btn_LeftCrossRight  = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossRight);
        this->btn_LeftCrossMid    = ReadJoystickButtons(&msg_joystick_air, Btn_LeftCrossMid);
        this->btn_RightCrossUp    = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossUp);
        this->btn_RightCrossDown  = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossDown);
        this->btn_RightCrossLeft  = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossLeft);
        this->btn_RightCrossRight = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossRight);
        this->btn_RightCrossMid   = ReadJoystickButtons(&msg_joystick_air, Btn_RightCrossMid);
        this->btn_Btn0            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn0);
        this->btn_Btn1            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn1);
        this->btn_Btn2            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn2);
        this->btn_Btn3            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn3);
        this->btn_Btn4            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn4);
        this->btn_Btn5            = ReadJoystickButtons(&msg_joystick_air, Btn_Btn5);
        this->btn_JoystickL       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickL);
        this->btn_JoystickR       = ReadJoystickButtons(&msg_joystick_air, Btn_JoystickR);
        this->btn_KnobL           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobL);
        this->btn_KnobR           = ReadJoystickButtons(&msg_joystick_air, Btn_KnobR);
        this->left_switch         = ReadJoystickSwitchs(&msg_joystick_air, Left_switch);
        this->right_switch        = ReadJoystickSwitchs(&msg_joystick_air, Right_switch);
        this->right_x             = ReadJoystickRight_x(&msg_joystick_air);
        this->right_y             = ReadJoystickRight_y(&msg_joystick_air);
        this->left_x              = ReadJoystickLeft_x(&msg_joystick_air);
        this->left_y              = ReadJoystickLeft_y(&msg_joystick_air);
        this->left_knob           = ReadJoystickKnobsLeft(&msg_joystick_air);
        if (this->right_x < 0.15f && this->right_x > -0.15f) {
            this->usr_right_x = 0;
        } else if (this->right_x > 0.15f) {
            this->usr_right_x = (int)((this->right_x - 0.15) * 1000);
        } else {
            this->usr_right_x = (int)((this->right_x + 0.15) * 1000);
        }
        if (this->right_y < 0.15f && this->right_y > -0.15f) {
            this->usr_right_y = 0;
        } else if (this->right_y > 0.15f) {
            this->usr_right_y = (int)((this->right_y - 0.15) * 1000);
        } else {
            this->usr_right_y = (int)((this->right_y + 0.15) * 1000);
        }
        if (this->left_x < 0.15f && this->left_x > -0.15f) {
            this->usr_left_x = 0;
        } else if (this->left_x > 0.15f) {
            this->usr_left_x = (int)((this->left_x - 0.15) * 1000);
        } else {
            this->usr_left_x = (int)((this->left_x + 0.15) * 1000);
        }
        if (this->left_y < 0.15f && this->left_y > -0.15f) {
            this->usr_left_y = 0;
        } else if (this->left_y > 0.15f) {
            this->usr_left_y = (int)((this->left_y - 0.15) * 1000);
        } else {
            this->usr_left_y = (int)((this->left_y + 0.15) * 1000);
        }
        this->usr_left_knob = this->left_knob - left_knob_offset;
        osDelay(1);
    }
}
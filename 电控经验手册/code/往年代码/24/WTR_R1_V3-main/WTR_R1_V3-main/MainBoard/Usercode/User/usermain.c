/********************************************************************
 * @brief   WTR2024 R1 机械臂版本工程
 * @note
 */
#include "usermain.h"

char debug_title[20] = "Debug";
char debug_msg[20];
mavlink_joystick_air_dashboard_set_title_t mav_debug_title;
mavlink_joystick_air_dashboard_set_msg_t mav_debug_msg;
uint8_t rst_flag = 0;

void StartDefaultTask(void *argument)
{
    // Hardware Init
    rst_flag = 0;
    m_RemoteCtl_Init(); // 遥控器初始化
    // m_Chassis_Laser_Init(); // 激光初始化
    m_Chassis_Init(); // DJI电机初始化
    m_Servo_Init();   // 舵机初始化
    osDelay(1000);
    m_Unitree_Init();      // Unitree电机初始化
    m_Chassis_Gyro_Init(); // 陀螺仪初始化
    // m_Chassis_Odom_Init(); // 码盘初始化

    // Task start
    // m_Chassis_Odom_TaskStart(); // 码盘坐标转换线程
    m_Chassis_Gyro_TaskStart(); // 陀螺仪线程
    while (chassis_gyro_state != 1) {
        osDelay(1);
    }
    m_RemoteCtl_Task_Start(); // 遥控器线程
    // m_Chassis_Laser_TaskStart(); // 激光线程

    // Left and Right choose
    do {
        // 发送等待消息
        JoystickSwitchTitle(ID_DIRECT_CHOOSE, direct_choose_title, &mav_dir_choose_title);
        JoystickSwitchMsg(ID_DIRECT_CHOOSE, direct_choose_msg, &mav_dir_choose_msg);
        JoystickSwitchTitle(ID_RST, rst_choose_title, &mav_rst_choose_title);
        if (btn_Btn4 == 1) {
            rst_flag = 1;
        }
        // JoystickSwitchTitle(ID_UP, up_choose_title, &mav_up_choose_title);
        // 等待选择
        if (btn_KnobR == 1 && usr_right_x > 500.0f) {
            general_state = RIGHT_MODE;                          // 指定为右侧状态
            JoystickDelete(ID_DIRECT_CHOOSE, &mav_joystick_del); // 取消等待指令
            break;
        } else if (btn_KnobR == 1 && usr_right_x < -500.0f) {
            general_state = LEFT_MODE;                           // 指定为左侧状态
            JoystickDelete(ID_DIRECT_CHOOSE, &mav_joystick_del); // 取消等待指令
            break;
        }
        if (rst_flag == 0) {
            JoystickSwitchMsg(ID_RST, norst_choose_msg, &mav_dir_choose_msg);
        } else if (rst_flag == 1) {
            JoystickSwitchMsg(ID_RST, rst_choose_msg, &mav_dir_choose_msg);
        }
        osDelay(1);
    } while (1);

    m_Chassis_CAN_Message_TaskStart();  // CAN通信线程
    m_Chassis_Ctl_TaskStart();          // 底盘控制线程
    m_Unitree_Ctl_Message_TaskStart();  // Unitree电机控制线程
    m_Unitree_UART_Message_TaskStart(); // Unitree电机通信线程

    osDelay(100);
    // 初始化位置
    v_1       = 0;
    v_2       = 0;
    v_3       = 0;
    v_4       = 0;
    arm_angle = -140;
    if(rst_flag == 0)
    {
        unitree_right_pos = -PI / 2 - 0.2;
        unitree_left_pos  = PI / 2 + 0.2;
        motor_l_gripseed  = -1005;
        motor_r_gripseed  = -1005;
        while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 5.0f) ||
               ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -5.0f) ||
               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 5.0f) ||
               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -5.0f)) {
            osDelay(1);
        }
    }

    chassis_pid_init(&chassis_yaw_pid, 1.0, 0.0, 0.15);
    chassis_yaw_pid.SetPoint = chassis_offset;

    while (btn_KnobR != 1) {
        osDelay(1);
    }

    // main task entry point
    m_main_Task_Start();
    // main run
    static int i = 0;
    for (;;) {
        // Run State
        i++;
        if (i == 1000) {
            i = 0;
            HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
        }
        sprintf(debug_msg, "yaw:%d,seed:%d", (int)(chassis_yaw - chassis_offset), seed_count);
        JoystickSwitchTitle(10, debug_title, &mav_debug_title);
        JoystickSwitchMsg(10, debug_msg, &mav_dir_choose_msg);
        osDelay(1);
    }
}

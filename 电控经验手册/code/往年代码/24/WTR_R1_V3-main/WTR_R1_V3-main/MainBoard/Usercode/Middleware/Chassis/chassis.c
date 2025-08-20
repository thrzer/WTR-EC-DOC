#include "chassis.h"
/***************************************************************************
 * 方向定义：
 * (1) 底盘方向定义
 *         ^x
 *         |
 *     y   |
 *     <---.
 * (2)码盘方向定义
 *         ^-y
 *         |
 *     x   |
 *     <---.
 */

/****************************************************************************
 * 线程定义
 */

osThreadId_t can_message_TaskHandle;
const osThreadAttr_t can_message_Task_attributes = {
    .name       = "can_message_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityHigh1,
};
void m_CAN_Message_Task(void *argument);

osThreadId_t chassis_ctl_TaskHandle;
const osThreadAttr_t chassis_ctl_Task_attributes = {
    .name       = "chassis_ctl_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityHigh1,
};
void m_Chassis_Ctl_Task(void *argument);
/****************************************************************************
 * 接口定义
 */

int arm_angle           = 0; // 机械臂角度
int friction_speed_up   = 0; // 上摩擦轮速度
int friction_speed_down = 0; // 下摩擦轮速度
float v_1, v_2, v_3, v_4;    // 底盘速度
float motor_r_gripseed;      // 右侧取苗电机位置
float motor_l_gripseed;      // 左侧取苗电机位置
float motor_r_plantseed;     // 右侧放苗电机位置
float motor_l_plantseed;     // 左侧放苗电机位置

/****************************************************************************
 * 底盘运动控制定义
 */
C_PID chassis_yaw_pid; // 底盘偏航角PID控制器
C_PID chassis_y_pid;   // 底盘横向PID控制器
C_PID chassis_x_pid;   // 底盘纵向PID控制器

/****************************************************************************
 * 电机初始化 CAN消息发送相关函数
 */

/**
 * @brief   底盘初始化
 */
void m_Chassis_Init(void)
{
    fdcan1.FDCAN_Rx_Filter_Init();
    fdcan2.FDCAN_Rx_Filter_Init();
    fdcan1.FDCAN_Start();
    fdcan2.FDCAN_Start();
    fdcan1.FDCAN_Interrupt_Init();
    fdcan2.FDCAN_Interrupt_Init();
    hDJI[0][0].motorType = M3508;
    hDJI[1][0].motorType = M3508;
    hDJI[2][0].motorType = M3508;
    hDJI[3][0].motorType = M3508;
    hDJI[6][0].motorType = M3508;
    hDJI[7][0].motorType = M3508;
    hDJI[0][1].motorType = M2006; // 右侧取苗爪升降电机
    hDJI[1][1].motorType = M2006; // 右侧存储机构运动电机
    hDJI[2][1].motorType = M2006; // 左侧取苗爪升降电机
    hDJI[3][1].motorType = M2006; // 左侧存储机构运动电机
    hDJI[4][1].motorType = M3508;
    DJI_Init();
    // 机械臂电机修正
    hDJI[4][1].reductionRate    = 72;
    hDJI[4][1].posPID.outputMax = 4000;
    // 底盘偏航角控制
    // chassis_pid_init(&chassis_yaw_pid, 1.0, 0.0, 0.15);
    // 底盘PID坐标控制
    // chassis_pid_init(&chassis_x_pid, 0.1, 0.0, 0.001);
    // chassis_pid_init(&chassis_y_pid, 0.1, 0.0, 0.001);
}
/**
 * @brief   底盘电机CAN消息发送线程创建
 */
void m_Chassis_CAN_Message_TaskStart(void)
{
    can_message_TaskHandle = osThreadNew(m_CAN_Message_Task, NULL, &can_message_Task_attributes);
}

/**
 * @brief   底盘控制启动线程
 */
void m_Chassis_Ctl_TaskStart(void)
{
    chassis_ctl_TaskHandle = osThreadNew(m_Chassis_Ctl_Task, NULL, &chassis_ctl_Task_attributes);
}

/****************************************************************************
 * 线程函数
 */

/**
 * @brief   底盘电机CAN消息发送线程
 */
void m_CAN_Message_Task(void *argument)
{
    for (;;) {
        speedServo(v_1, &hDJI[0][0]);
        speedServo(v_2, &hDJI[1][0]);
        speedServo(v_3, &hDJI[2][0]);
        speedServo(v_4, &hDJI[3][0]);
        positionServo(arm_angle, &hDJI[4][1]);
        speedServo(-friction_speed_up, &hDJI[6][0]);
        speedServo(friction_speed_down, &hDJI[7][0]);
        positionServo(motor_r_gripseed, &hDJI[0][1]);
        positionServo(motor_l_gripseed, &hDJI[1][1]);
        positionServo(motor_r_plantseed, &hDJI[2][1]);
        positionServo(motor_l_plantseed, &hDJI[3][1]);
        CanTransmit_DJI_1234(fdcan1, hDJI[0][0].speedPID.output, hDJI[1][0].speedPID.output, hDJI[2][0].speedPID.output, hDJI[3][0].speedPID.output);
        CanTransmit_DJI_5678(fdcan1, 0, 0, hDJI[6][0].speedPID.output, hDJI[7][0].speedPID.output);
        CanTransmit_DJI_1234(fdcan2, hDJI[0][1].speedPID.output, hDJI[1][1].speedPID.output, hDJI[2][1].speedPID.output, hDJI[3][1].speedPID.output);
        CanTransmit_DJI_5678(fdcan2, hDJI[4][1].speedPID.output, 0, 0, 0);
        osDelay(1);
    }
}

/**
 * @brief   底盘电机控制线程
 */
void m_Chassis_Ctl_Task(void *argument)
{
    static float mvx;
    static float mvy;
    static float _mvx;
    static float _mvy;
    static float mwc;
    for (;;) {
        if (run_state == HANDLE_MODE) {
            if (right_switch == 0) {
                if (usr_left_knob > -5.0f && usr_left_knob < 5.0f) {
                    chassis_yaw_pid.SetPoint = chassis_offset;
                } else {
                    if (usr_left_knob > 5.0f) {
                        chassis_yaw_pid.SetPoint = chassis_offset - (usr_left_knob - 5);
                    } else if (usr_left_knob < -5.0f) {
                        chassis_yaw_pid.SetPoint = chassis_offset - (usr_left_knob + 5);
                    }
                    if (chassis_yaw_pid.SetPoint > (chassis_offset + 170.0f)) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 170.0f;
                    } else if (chassis_yaw_pid.SetPoint < (chassis_offset - 170.0f)) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 170.0f;
                    }
                }
            } else if (right_switch == 1) {
                if (general_state == LEFT_MODE) {
                    chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                } else if (general_state == RIGHT_MODE) {
                    chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                }
            }
            // 手动模式下为遥控控制底盘
            if (down_flag == 0) {
                _mvx = (float)(usr_left_y * 300) / 4000.0;
                _mvy = -(float)(usr_left_x * 300) / 4000.0;
            } else if (down_flag == 1) {
                if (general_state == LEFT_MODE) {
                    _mvy = 15;
                    _mvx = 0;
                } else if (general_state == RIGHT_MODE) {
                    _mvy = -15;
                    _mvx = 0;
                }
            }
            mvx = _mvx * cos(((chassis_yaw - chassis_offset) * PI) / 180) + _mvy * sin(((chassis_yaw - chassis_offset) * PI) / 180);
            mvy = -_mvx * sin(((chassis_yaw - chassis_offset) * PI) / 180) + _mvy * cos(((chassis_yaw - chassis_offset) * PI) / 180);
            mwc = chassis_yaw_pid_calc(&chassis_yaw_pid, chassis_yaw);
        } else if (run_state == AUTO_MODE) {
            if (up_flag == 0) {
                _mvx = chassis_pos_pid_calc(&chassis_x_pid, chassis_x_point);
                _mvy = chassis_pos_pid_calc(&chassis_y_pid, chassis_y_point);
                mwc  = chassis_yaw_pid_calc(&chassis_yaw_pid, chassis_yaw);
            } else if (up_flag == 1) {
                _mvx = 60;
                _mvy = 0;
                mwc  = chassis_yaw_pid_calc(&chassis_yaw_pid, chassis_yaw);
            } else if (up_flag == 2) {
                _mvx = 0;
                _mvy = 0;
                mwc  = chassis_yaw_pid_calc(&chassis_yaw_pid, chassis_yaw);
            } else if (up_flag == 3) {
                _mvx = (float)(usr_left_y * 200) / 4000.0;
                _mvy = -(float)(usr_left_x * 200) / 4000.0;
                mwc  = chassis_yaw_pid_calc(&chassis_yaw_pid, chassis_yaw);
            }
            mvx = _mvx * cos(((chassis_yaw - chassis_offset) * PI) / 180) + _mvy * sin(((chassis_yaw - chassis_offset) * PI) / 180);
            mvy = -_mvx * sin(((chassis_yaw - chassis_offset) * PI) / 180) + _mvy * cos(((chassis_yaw - chassis_offset) * PI) / 180);
        } else {
            chassis_x_pid.SetPoint = chassis_x_point;
            chassis_y_pid.SetPoint = chassis_y_point;
            if (usr_left_knob > -5.0f && usr_left_knob < 5.0f) {
                chassis_yaw_pid.SetPoint = chassis_offset;
            } else {
                chassis_yaw_pid.SetPoint = chassis_offset - usr_left_knob;
            }
            // 手动模式下为遥控控制底盘
            _mvx = (float)(usr_left_y * 200) / 4000.0;
            _mvy = -(float)(usr_left_x * 200) / 4000.0;
            mvx  = _mvx * cos(((chassis_yaw - chassis_offset) * PI) / 180) + _mvy * sin(((chassis_yaw - chassis_offset) * PI) / 180);
            mvy  = -_mvx * sin(((chassis_yaw - chassis_offset) * PI) / 180) + _mvy * cos(((chassis_yaw - chassis_offset) * PI) / 180);
            mwc  = chassis_yaw_pid_calc(&chassis_yaw_pid, chassis_yaw);
        }
        Inverse_kinematic_equation(mvx, mvy, mwc, &v_1, &v_2, &v_3, &v_4);
        osDelay(1);
    }
}

/**********************************底盘运动学解算相关函数*************************************/

/**
 * @brief   逆运动学方程，输入速度求解电机转速
 * @param   vx          底盘前向速度，单位m/s
 * @param   vy          底盘左横向速度，单位m/s
 * @param   wc          转向角速度，使用弧度，单位rad/s
 */
void Inverse_kinematic_equation(float vx, float vy, float wc, float *_v_1, float *_v_2, float *_v_3, float *_v_4)
{
    float v1, v2, v3, v4;
    v1 = (float)((sqrt(2) * vx / 2 - sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);
    v2 = (float)((-sqrt(2) * vx / 2 - sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);
    v3 = (float)((-sqrt(2) * vx / 2 + sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);
    v4 = (float)((sqrt(2) * vx / 2 + sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);

    *_v_1 = (float)(v1 * 60.0) / (2 * PI);
    *_v_2 = (float)(v2 * 60.0) / (2 * PI);
    *_v_3 = (float)(v3 * 60.0) / (2 * PI);
    *_v_4 = (float)(v4 * 60.0) / (2 * PI);
}

/**************************************PID控制相关函数******************************************** */
/**
 * @brief       pid初始化
 * @param       KP  比例常数
 * @param       KI  积分常数
 * @param       KD  微分常数
 * @retval      无
 */
void chassis_pid_init(C_PID *upid, float KP, float KI, float KD)
{
    upid->SetPoint    = 0;   /* 设定目标值 */
    upid->ActualValue = 0.0; /* 期望输出值 */
    upid->SumError    = 0.0; /* 积分值 */
    upid->Error       = 0.0; /* Error[1] */
    upid->LastError   = 0.0; /* Error[-1] */
    upid->PrevError   = 0.0; /* Error[-2] */
    upid->Proportion  = KP;  /* 比例常数 Proportional Const */
    upid->Integral    = KI;  /* 积分常数 Integral Const */
    upid->Derivative  = KD;  /* 微分常数 Derivative Const */
}

/**
 * @brief       底盘偏航角pid闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
float chassis_yaw_pid_calc(C_PID *upid, float Feedback_value)
{
    upid->Error = (float)(upid->SetPoint - Feedback_value); /* 计算偏差 */

    upid->SumError += upid->Error;
    upid->ActualValue = (upid->Proportion * upid->Error)                        /* 比例环节 */
                        + (upid->Integral * upid->SumError)                     /* 积分环节 */
                        + (upid->Derivative * (upid->Error - upid->LastError)); /* 微分环节 */
    upid->LastError = upid->Error;

    return (upid->ActualValue); /* 返回计算后输出的数值 */
}

/**
 * @brief       底盘坐标pid闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
float chassis_pos_pid_calc(C_PID *upid, float Feedback_value)
{
    upid->Error = (float)(upid->SetPoint - Feedback_value); /* 计算偏差 */

    upid->SumError += upid->Error;
    upid->ActualValue = (upid->Proportion * upid->Error)                        /* 比例环节 */
                        + (upid->Integral * upid->SumError)                     /* 积分环节 */
                        + (upid->Derivative * (upid->Error - upid->LastError)); /* 微分环节 */
    upid->LastError = upid->Error;

    return (upid->ActualValue); /* 返回计算后输出的数值 */
}
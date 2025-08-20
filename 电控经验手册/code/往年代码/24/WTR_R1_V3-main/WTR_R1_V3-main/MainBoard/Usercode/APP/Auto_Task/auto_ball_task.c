/****************
 * 自动球点编号
 *      1 2 3 4 5 6
 *      7 8 9 10 11 12
 *  7 需要特殊处理
 *
 * 1 2 3 6
 * 7 8 9 12
 *
 * 可以使用激光校正
 */
#include "auto_ball_task.h"

enum Auto_Ball_State auto_ball_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_ball_TaskHandle;
const osThreadAttr_t auto_ball_Task_attributes = {
    .name       = "auto_ball_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_auto_ball_Task(void *argument);

uint8_t up_flag = 0; // 上坡运动状态标志位
uint8_t go_flag = 0; // 上坡状态标志位

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   自动取放球线程初始化
 */
void m_auto_ball_Task_Start(void)
{
    auto_ball_state      = Auto_Ball_Ready;
    auto_ball_TaskHandle = osThreadNew(m_auto_ball_Task, NULL, &auto_ball_Task_attributes);
}
/*****************************************************************************
 * 辅助函数
 */
/**
 * @brief   校正坐标函数
 */
static void update_coordinate(void)
{
    osDelay(1000);
    up_flag = 2;
    // 更新底盘X坐标，X=-Y
    OPS_Update_Y((int)(-330 + chassis_laser_x));
    osDelay(20);
    // 更新底盘Y坐标，Y=X
    if (general_state == LEFT_MODE) {
        OPS_Update_X((int)(1390 - chassis_laser_y));
    } else if (general_state == RIGHT_MODE) {
        OPS_Update_X((int)(chassis_laser_y - 1390));
    }
    osDelay(1000);
    up_flag = 0;
}

/**
 * @brief   校正坐标函数
 */
static void update_coordinate_for_6_and_12(void)
{
    osDelay(1000);
    up_flag = 2;
    // 更新底盘X坐标，X=-Y
    OPS_Update_Y((int)(-1420 + chassis_laser_x));
    osDelay(20);
    // 更新底盘Y坐标，Y=X
    if (general_state == LEFT_MODE) {
        OPS_Update_X((int)(1390 - chassis_laser_y));
    } else if (general_state == RIGHT_MODE) {
        OPS_Update_X((int)(chassis_laser_y - 1390));
    }
    osDelay(1000);
    up_flag = 0;
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放球线程
 */
void m_auto_ball_Task(void *argument)
{
    static int last_choose;
    static int ball_error_x;
    static int ball_error_y;
    for (;;) {
        switch (auto_ball_state) {
            case Auto_Ball_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_BALL, auto_ball_title, &mav_auto_ball_title);
                JoystickSwitchMsg(ID_AUTO_BALL, auto_ball_ready_msg, &mav_auto_ball_msg);
                osDelay(1);
                // 复位状态
                Reset_Action();
                // 状态机转换
                if (btn_KnobR == 1) {
                    auto_ball_state = Auto_Ball_Go;
                }
                break;
            case Auto_Ball_Go:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_BALL, auto_ball_title, &mav_auto_ball_title);
                JoystickSwitchMsg(ID_AUTO_BALL, auto_ball_go_msg, &mav_auto_ball_msg);
                osDelay(1);
                // 自动射球动作
                // if (go_flag == 0) {
                //     if (right_switch == 0) {
                //         // 1. 到达坡前航路点
                //         chassis_x_pid.SetPoint = 1450;
                //         if (general_state == LEFT_MODE) {
                //             chassis_y_pid.SetPoint = 600;
                //         } else if (general_state == RIGHT_MODE) {
                //             chassis_y_pid.SetPoint = -600;
                //         }
                //         chassis_yaw_pid.SetPoint = chassis_offset;
                //         while ((chassis_x_point - chassis_x_pid.SetPoint < -20.0f) ||
                //                (chassis_x_point - chassis_x_pid.SetPoint > 20.0f) ||
                //                (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                //                (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                //             osDelay(1);
                //         }
                //         // 2. 上坡
                //         up_flag = 1;
                //         osDelay(2500);
                //         up_flag = 2;
                //         // 3. 校正
                //         osDelay(1200);
                //         OPS_Update_X(0);
                //         osDelay(20);
                //         OPS_Update_Y(0);
                //         osDelay(1200);
                //         ball_error_x = chassis_laser_x - 330;
                //         ball_error_y = chassis_laser_y - 1390;
                //         if (general_state == LEFT_MODE) {
                //             chassis_x_pid.SetPoint   = chassis_x_point + ball_error_x;
                //             chassis_y_pid.SetPoint   = chassis_y_point + ball_error_y;
                //             chassis_yaw_pid.SetPoint = chassis_offset;
                //         } else if (general_state == RIGHT_MODE) {
                //             chassis_x_pid.SetPoint   = chassis_x_point + ball_error_x;
                //             chassis_y_pid.SetPoint   = chassis_y_point - ball_error_y;
                //             chassis_yaw_pid.SetPoint = chassis_offset;
                //         }
                //         up_flag = 0;
                //         while ((chassis_x_point - chassis_x_pid.SetPoint < -1.0f) ||
                //                (chassis_x_point - chassis_x_pid.SetPoint > 1.0f) ||
                //                (chassis_y_point - chassis_y_pid.SetPoint < -1.0f) ||
                //                (chassis_y_point - chassis_y_pid.SetPoint > 1.0f)) {
                //             osDelay(1);
                //         }
                //         up_flag = 2;
                //         OPS_Update_X(0);
                //         osDelay(20);
                //         OPS_Update_Y(0);
                //         chassis_x_pid.SetPoint = 0;
                //         chassis_y_pid.SetPoint = 0;
                //         up_flag                = 0;
                //         osDelay(100);
                //         go_flag     = 1;
                //         last_choose = 0;
                //     } else if (right_switch == 1) {
                //         // 校正
                //         up_flag = 2;
                //         osDelay(1200);
                //         OPS_Update_X(0);
                //         osDelay(20);
                //         OPS_Update_Y(0);
                //         osDelay(1200);
                //         ball_error_x = chassis_laser_x - 330;
                //         ball_error_y = chassis_laser_y - 1390;
                //         if (general_state == LEFT_MODE) {
                //             chassis_x_pid.SetPoint   = chassis_x_point + ball_error_x;
                //             chassis_y_pid.SetPoint   = chassis_y_point + ball_error_y;
                //             chassis_yaw_pid.SetPoint = chassis_offset;
                //         } else if (general_state == RIGHT_MODE) {
                //             chassis_x_pid.SetPoint   = chassis_x_point + ball_error_x;
                //             chassis_y_pid.SetPoint   = chassis_y_point - ball_error_y;
                //             chassis_yaw_pid.SetPoint = chassis_offset;
                //         }
                //         up_flag = 0;
                //         while ((chassis_x_point - chassis_x_pid.SetPoint < -1.0f) ||
                //                (chassis_x_point - chassis_x_pid.SetPoint > 1.0f) ||
                //                (chassis_y_point - chassis_y_pid.SetPoint < -1.0f) ||
                //                (chassis_y_point - chassis_y_pid.SetPoint > 1.0f)) {
                //             osDelay(1);
                //         }
                //         up_flag = 2;
                //         OPS_Update_X(0);
                //         osDelay(20);
                //         OPS_Update_Y(0);
                //         chassis_x_pid.SetPoint = 0;
                //         chassis_y_pid.SetPoint = 0;
                //         up_flag                = 0;
                //         osDelay(100);
                //         go_flag     = 1;
                //         last_choose = 0;
                //     }
                // }
                while (btn_LeftCrossRight == 0 &&
                       btn_LeftCrossUp == 0 &&
                       btn_Btn0 == 0 &&
                       btn_Btn3 == 0 &&
                       btn_Btn5 == 0 &&
                       btn_RightCrossLeft == 0 &&
                       btn_LeftCrossDown == 0 &&
                       btn_Btn2 == 0 &&
                       btn_Btn1 == 0 &&
                       btn_RightCrossUp == 0 &&
                       btn_Btn4 == 0 &&
                       btn_RightCrossDown == 0) {
                    osDelay(1);
                }
                if (btn_LeftCrossRight == 1) // 1号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 4 || last_choose == 5) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    } else if (last_choose == 6) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[1];
                            chassis_y_pid.SetPoint   = left_ball_point_y[1];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[1];
                            chassis_y_pid.SetPoint   = right_ball_point_y[1];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 到达航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[0];
                        chassis_y_pid.SetPoint   = left_ball_point_y[0];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[0];
                        chassis_y_pid.SetPoint   = right_ball_point_y[0];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 2. 倒退
                    chassis_x_pid.SetPoint -= 180;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 180;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    update_coordinate();
                    last_choose = 1;
                } else if (btn_LeftCrossUp == 1) // 2号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 5 || last_choose == 6) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 到达航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[1];
                        chassis_y_pid.SetPoint   = left_ball_point_y[1];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[1];
                        chassis_y_pid.SetPoint   = right_ball_point_y[1];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 2. 倒退
                    chassis_x_pid.SetPoint -= 180;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 180;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    update_coordinate();
                    last_choose = 2;
                } else if (btn_Btn0 == 1) // 3号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 6) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 进入航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[2];
                        chassis_y_pid.SetPoint   = left_ball_point_y[2];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[2];
                        chassis_y_pid.SetPoint   = right_ball_point_y[2];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 180;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 180;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    update_coordinate();
                    last_choose = 3;
                } else if (btn_Btn3 == 1) // 4号点位
                {
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 1) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[3];
                        chassis_y_pid.SetPoint   = left_ball_point_y[3];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[3];
                        chassis_y_pid.SetPoint   = right_ball_point_y[3];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 2. 倒退
                    chassis_x_pid.SetPoint -= 180;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 180;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 4;
                } else if (btn_Btn5 == 1) // 5号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 1 || last_choose == 2) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 进入航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[4];
                        chassis_y_pid.SetPoint   = left_ball_point_y[4];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[4];
                        chassis_y_pid.SetPoint   = right_ball_point_y[4];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 2. 倒退
                    chassis_x_pid.SetPoint -= 180;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 180;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 5;
                } else if (btn_RightCrossLeft == 1) // 6号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 1) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[4];
                            chassis_y_pid.SetPoint   = left_ball_point_y[4];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[4];
                            chassis_y_pid.SetPoint   = right_ball_point_y[4];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 2 || last_choose == 3) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 检查航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[5];
                        chassis_y_pid.SetPoint   = left_ball_point_y[5];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[5];
                        chassis_y_pid.SetPoint   = right_ball_point_y[5];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 180;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 180;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    update_coordinate_for_6_and_12();
                    last_choose = 6;
                } else if (btn_LeftCrossDown == 1) // 7号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 6 || last_choose == 5) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[1];
                            chassis_y_pid.SetPoint   = left_ball_point_y[1];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[1];
                            chassis_y_pid.SetPoint   = right_ball_point_y[1];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 进入航路点
                    chassis_x_pid.SetPoint = 0;
                    chassis_y_pid.SetPoint = 0;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[6];
                        chassis_y_pid.SetPoint   = left_ball_point_y[6];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[6];
                        chassis_y_pid.SetPoint   = right_ball_point_y[6];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 旋转
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag = 0;
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[6];
                        chassis_y_pid.SetPoint   = left_ball_point_y[6];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[6];
                        chassis_y_pid.SetPoint   = right_ball_point_y[6];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    chassis_x_pid.SetPoint = 0;
                    chassis_y_pid.SetPoint = 0;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    update_coordinate();
                    last_choose = 0;
                } else if (btn_Btn2 == 1) // 8号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 5 || last_choose == 6) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 进入航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[1];
                        chassis_y_pid.SetPoint   = left_ball_point_y[1];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[1];
                        chassis_y_pid.SetPoint   = right_ball_point_y[1];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 700;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 700;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 2;
                    update_coordinate();
                } else if (btn_Btn1 == 1) // 9号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 6) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 到达航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[2];
                        chassis_y_pid.SetPoint   = left_ball_point_y[2];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[2];
                        chassis_y_pid.SetPoint   = right_ball_point_y[2];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 700;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 700;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 3;
                    update_coordinate();
                } else if (btn_RightCrossUp == 1) // 10号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 1) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 到达航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[3];
                        chassis_y_pid.SetPoint   = left_ball_point_y[3];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[3];
                        chassis_y_pid.SetPoint   = right_ball_point_y[3];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 700;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 700;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 4;
                } else if (btn_Btn4 == 1) // 11号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 1 || last_choose == 2) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[4];
                        chassis_y_pid.SetPoint   = left_ball_point_y[4];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[4];
                        chassis_y_pid.SetPoint   = right_ball_point_y[4];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 700;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 700;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 5;
                } else if (btn_RightCrossDown == 1) // 12号点位
                {
                    // 0. 航路点检查
                    if (last_choose == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 1) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[0];
                            chassis_y_pid.SetPoint   = left_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[0];
                            chassis_y_pid.SetPoint   = right_ball_point_y[0];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[2];
                            chassis_y_pid.SetPoint   = left_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[2];
                            chassis_y_pid.SetPoint   = right_ball_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[4];
                            chassis_y_pid.SetPoint   = left_ball_point_y[4];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[4];
                            chassis_y_pid.SetPoint   = right_ball_point_y[4];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    if (last_choose == 2 || last_choose == 3) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_ball_point_x[3];
                            chassis_y_pid.SetPoint   = left_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_ball_point_x[3];
                            chassis_y_pid.SetPoint   = right_ball_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                            osDelay(1);
                        }
                    }
                    // 1. 进入航路点
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[5];
                        chassis_y_pid.SetPoint   = left_ball_point_y[5];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[5];
                        chassis_y_pid.SetPoint   = right_ball_point_y[5];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 700;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 3. 取球
                    Ball_Pick_Action();
                    // 4. 手动校正(总是进入此状态)
                    up_flag = 3;
                    while (btn_KnobR != 1) {
                        osDelay(1);
                    }
                    Ball_Fire_Action();
                    // 5. 射球完毕后退出航路点并记录当前数据
                    up_flag                  = 0;
                    chassis_yaw_pid.SetPoint = 0;
                    chassis_x_pid.SetPoint += 700;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    last_choose = 5;
                    update_coordinate_for_6_and_12();
                }
                // 状态转换
                auto_ball_state = Auto_Ball_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}
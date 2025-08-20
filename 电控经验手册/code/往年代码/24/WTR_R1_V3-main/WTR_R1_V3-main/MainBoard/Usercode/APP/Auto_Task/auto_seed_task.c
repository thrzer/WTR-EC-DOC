#include "auto_seed_task.h"

enum Auto_Seed_State auto_seed_state;

/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_seed_TaskHandle;
const osThreadAttr_t auto_seed_Task_attributes = {
    .name       = "auto_seed_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_auto_seed_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   手动取放苗线程初始化
 */
void m_auto_seed_Task_Start(void)
{
    auto_seed_state      = Auto_Seed_Ready;
    auto_seed_TaskHandle = osThreadNew(m_auto_seed_Task, NULL, &auto_seed_Task_attributes);
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放苗线程
 */
void m_auto_seed_Task(void *argument)
{
    static float now_grip_x  = 100;
    static float last_grip_x = 0;
    static float now_grip_y  = 100;
    static float last_grip_y = 0;
    static int plant_x_error;
    static int plant_y_error;
    static int j;
    for (;;) {
        switch (auto_seed_state) {
            case Auto_Seed_Ready:
                // 复位状态
                Reset_Action();
                // 状态转换
                if (btn_KnobR == 1) {
                    auto_seed_state = Auto_Seed_Go;
                }
                break;
            case Auto_Seed_Go:
                // 第一段：寻找第一航路点
                // 如果此时车点在苗架线以内，先运行到苗架线以外，反之则不用运行至第一航路点(250,250)
                if (chassis_x_point < 350.0f) {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = 350.0f;
                        chassis_y_pid.SetPoint   = -350.0f;
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = 350.0f;
                        chassis_y_pid.SetPoint   = 350.0f;
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    osDelay(50);
                    // 运行至第一航路点
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -30.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 30.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -30.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 30.0f)) {
                        osDelay(1);
                    }
                }
                // 运行至第一取苗点
                if (general_state == LEFT_MODE) {
                    chassis_x_pid.SetPoint   = left_grip_seed_point_x[0];
                    chassis_y_pid.SetPoint   = left_grip_seed_point_y[0];
                    chassis_yaw_pid.SetPoint = chassis_offset;
                } else if (general_state == RIGHT_MODE) {
                    chassis_x_pid.SetPoint   = right_grip_seed_point_x[0];
                    chassis_y_pid.SetPoint   = right_grip_seed_point_y[0];
                    chassis_yaw_pid.SetPoint = chassis_offset;
                }
                // 第二段：连续取苗
                for (int i = 0; i < 6; i++) {
                    // 1. 运行至航路点
                    while ((now_grip_x - last_grip_x > 0.1f) ||
                           (now_grip_x - last_grip_x < -0.1f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -8.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 8.0f)) {
                        j++;
                        if (j == 10) {
                            last_grip_x = now_grip_x;
                            now_grip_x  = chassis_x_point;
                            last_grip_y = now_grip_y;
                            now_grip_y  = chassis_y_point;
                            j           = 0;
                        }
                        if ((now_grip_y - last_grip_y < 1.0f) && (now_grip_y - last_grip_y > -1.0f)) {
                            chassis_y_pid.Integral   = 0.00002;
                            chassis_y_pid.Derivative = 0.0004;
                        } else {
                            chassis_y_pid.Integral   = 0.0000;
                            chassis_y_pid.Derivative = 0.0000;
                        }
                        osDelay(1);
                    }
                    chassis_y_pid.Derivative = 0;
                    chassis_y_pid.Integral   = 0;
                    now_grip_x               = 100;
                    last_grip_x              = 0;
                    j                        = 0;
                    // 2. 开始取苗
                    /******************************************
                     * 准备取苗动作
                     *      1. 宇树电机取苗准备位置
                     *      2. 夹爪放松
                     *      3. 取苗门打开
                     *      4. 电机降低
                     */
                    unitree_right_pos = PI / 2 + 0.9;
                    unitree_left_pos  = -PI / 2 - 0.9;
                    Seed_Deposit();
                    osDelay(700);
                    motor_l_gripseed = -5;
                    motor_r_gripseed = -5;
                    while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
                           ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
                        osDelay(1);
                    }

                    /*******************************************
                     * 取苗动作
                     */
                    // 第一段动作
                    // 宇树电机运动至超过苗轴线
                    unitree_right_pos = PI / 2 - 0.25;
                    unitree_left_pos  = -PI / 2 + 0.25;
                    // 夹紧，开轨道门
                    osDelay(700);
                    Seed_Grip();
                    Seed_Deposit_Open();
                    osDelay(700);
                    // 将苗夹起
                    motor_l_gripseed = -715;
                    motor_r_gripseed = -715;
                    while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
                           ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
                        osDelay(1);
                    }
                    // 第二段动作
                    // 前4个苗
                    if (i == 0 || i == 1 || i == 2 || i == 3) {
                        unitree_right_pos = -PI / 2 - 0.2;
                        unitree_left_pos  = PI / 2 + 0.2;
                        osDelay(500);
                        // 此时允许运动至下一点
                        if (i == 0) {
                            if (general_state == LEFT_MODE) {
                                chassis_x_pid.SetPoint   = left_grip_seed_point_x[1];
                                chassis_y_pid.SetPoint   = left_grip_seed_point_y[1];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            } else if (general_state == RIGHT_MODE) {
                                chassis_x_pid.SetPoint   = right_grip_seed_point_x[1];
                                chassis_y_pid.SetPoint   = right_grip_seed_point_y[1];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            }
                        } else if (i == 1) {
                            if (general_state == LEFT_MODE) {
                                chassis_x_pid.SetPoint   = left_grip_seed_point_x[2];
                                chassis_y_pid.SetPoint   = left_grip_seed_point_y[2];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            } else if (general_state == RIGHT_MODE) {
                                chassis_x_pid.SetPoint   = right_grip_seed_point_x[2];
                                chassis_y_pid.SetPoint   = right_grip_seed_point_y[2];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            }
                        } else if (i == 2) {
                            if (general_state == LEFT_MODE) {
                                chassis_x_pid.SetPoint   = left_grip_seed_point_x[3];
                                chassis_y_pid.SetPoint   = left_grip_seed_point_y[3];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            } else if (general_state == RIGHT_MODE) {
                                chassis_x_pid.SetPoint   = right_grip_seed_point_x[3];
                                chassis_y_pid.SetPoint   = right_grip_seed_point_y[3];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            }
                        } else if (i == 3) {
                            if (general_state == LEFT_MODE) {
                                chassis_x_pid.SetPoint   = left_grip_seed_point_x[4];
                                chassis_y_pid.SetPoint   = left_grip_seed_point_y[4];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            } else if (general_state == RIGHT_MODE) {
                                chassis_x_pid.SetPoint   = right_grip_seed_point_x[4];
                                chassis_y_pid.SetPoint   = right_grip_seed_point_y[4];
                                chassis_yaw_pid.SetPoint = chassis_offset;
                            }
                        }
                        // 继续
                        osDelay(500);
                        Seed_Deposit();
                        Seed_Deposit_Close();
                        osDelay(500);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        osDelay(100);
                        motor_l_gripseed = -1005;
                        motor_r_gripseed = -1005;
                        osDelay(200);
                        Seed_Deposit_Buffer_Close();
                        osDelay(500);
                        if (i == 0) {
                            motor_l_plantseed = 980;
                            motor_r_plantseed = -980;
                        } else if (i == 1) {
                            motor_l_plantseed = 770;
                            motor_r_plantseed = -770;
                        } else if (i == 2) {
                            motor_l_plantseed = 560;
                            motor_r_plantseed = -560;
                        } else if (i == 3) {
                            motor_l_plantseed = 385;
                            motor_r_plantseed = -385;
                        }
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            if (i == 0) {
                                if (hDJI[3][1].AxisData.AxisAngle_inDegree > 600 || hDJI[2][1].AxisData.AxisAngle_inDegree < -600) {
                                    Seed_Deposit_Buffer_Open();
                                }
                            }
                            if (i == 1) {
                                if (hDJI[3][1].AxisData.AxisAngle_inDegree > 500 || hDJI[2][1].AxisData.AxisAngle_inDegree < -500) {
                                    Seed_Deposit_Buffer_Open();
                                }
                            }
                            if (i == 2) {
                                if (hDJI[3][1].AxisData.AxisAngle_inDegree > 300 || hDJI[2][1].AxisData.AxisAngle_inDegree < -300) {
                                    Seed_Deposit_Buffer_Open();
                                }
                            }
                            if (i == 3) {
                                Seed_Deposit_Buffer_Open();
                            }
                            osDelay(1);
                        }
                        osDelay(300);
                        motor_l_plantseed = 2;
                        motor_r_plantseed = -2;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        Seed_Deposit_Buffer_Close();
                        // Seed_Deposit_Buffer_Open();
                    }
                    // 第五个苗，不推
                    else if (i == 4) {
                        unitree_right_pos = -PI / 2 - 0.2;
                        unitree_left_pos  = PI / 2 + 0.2;
                        osDelay(500);
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_grip_seed_point_x[5];
                            chassis_y_pid.SetPoint   = left_grip_seed_point_y[5];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_grip_seed_point_x[5];
                            chassis_y_pid.SetPoint   = right_grip_seed_point_y[5];
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        osDelay(500);
                        Seed_Deposit();
                        Seed_Deposit_Close();
                        osDelay(500);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        motor_l_gripseed  = -1005;
                        motor_r_gripseed  = -1005;
                        osDelay(200);
                        Seed_Deposit_Buffer_Open();
                        osDelay(500);
                        motor_l_plantseed = 230;
                        motor_r_plantseed = -230;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    }
                    // 最后一个苗，不进门
                    else if (i == 5) {
                        // 前往第二航路点
                        if (general_state == LEFT_MODE) {
                            chassis_y_pid.Integral   = 0.0;
                            chassis_y_pid.Derivative = 0.0;
                            chassis_x_pid.SetPoint   = 400.0f;
                            chassis_y_pid.SetPoint   = -3500.0f;
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_y_pid.Integral   = 0.0;
                            chassis_y_pid.Derivative = 0.0;
                            chassis_x_pid.SetPoint   = 400.0f;
                            chassis_y_pid.SetPoint   = 3500.0f;
                            chassis_yaw_pid.SetPoint = chassis_offset;
                        }
                        Seed_Deposit_Close();
                        osDelay(200);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        Seed_Grip();
                    }
                    osDelay(1);
                }
                // 3. 等待到达第二航路点
                while ((chassis_x_point - chassis_x_pid.SetPoint < -30.0f) ||
                       (chassis_x_point - chassis_x_pid.SetPoint > 30.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint < -30.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint > 30.0f)) {
                    osDelay(1);
                }
                // 4. 旋转
                if (general_state == LEFT_MODE) {
                    chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                } else if (general_state == RIGHT_MODE) {
                    chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                }
                while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                       (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                    osDelay(1);
                }
                // 中间状态，等待
                while (btn_LeftCrossMid != 1) {
                    osDelay(1);
                }
                if (general_state == LEFT_MODE) {
                    chassis_x_pid.SetPoint   = left_plant_seed_point_x[0];
                    chassis_y_pid.SetPoint   = left_plant_seed_point_y[0];
                    chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                } else if (general_state == RIGHT_MODE) {
                    chassis_x_pid.SetPoint   = right_plant_seed_point_x[0];
                    chassis_y_pid.SetPoint   = right_plant_seed_point_y[0];
                    chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                }
                osDelay(200);
                // 3. 开始放苗
                for (int i = 0; i < 6; i++) {
                    // 等待到达航路点
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    osDelay(1000);
                    // 校正
                    if (general_state == LEFT_MODE) {
                        plant_x_error = chassis_laser_y - left_plant_seed_point_ay[i];
                        plant_y_error = chassis_laser_x - left_plant_seed_point_ax[i];
                        if (plant_x_error < 400 && plant_x_error > -400 && plant_y_error < 400 && plant_y_error > -400) {
                            chassis_x_pid.SetPoint = left_plant_seed_point_x[i] + plant_x_error;
                            chassis_y_pid.SetPoint = left_plant_seed_point_y[i] - plant_y_error;
                        }
                    } else if (general_state == RIGHT_MODE) {
                        plant_x_error = chassis_laser_y - right_plant_seed_point_ay[i];
                        plant_y_error = chassis_laser_x - right_plant_seed_point_ax[i];
                        if (plant_x_error < 400 && plant_x_error > -400 && plant_y_error < 400 && plant_y_error > -400) {
                            chassis_x_pid.SetPoint = right_plant_seed_point_x[i] + plant_x_error;
                            chassis_y_pid.SetPoint = right_plant_seed_point_y[i] + plant_y_error;
                        }
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 1.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -1.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Seed_Deposit_Buffer_Open();
                    // 放苗
                    /***********************
                     * 放苗动作
                     *
                     */
                    // 第一段
                    Seed_Plant_Open();
                    osDelay(400);
                    Seed_Front_Open();
                    osDelay(500);
                    // 退出苗区
                    if (general_state == LEFT_MODE) {
                        chassis_y_pid.SetPoint += 120;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_y_pid.SetPoint -= 120;
                    }
                    osDelay(250);
                    Seed_Front_Close();
                    // 脱离接触后关闭放苗门
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    Seed_Plant_Close();
                    // 前往下一航路点
                    if (i == 0) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_plant_seed_point_x[1];
                            chassis_y_pid.SetPoint   = left_plant_seed_point_y[1];
                            chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_plant_seed_point_x[1];
                            chassis_y_pid.SetPoint   = right_plant_seed_point_y[1];
                            chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                        }
                    } else if (i == 1) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_plant_seed_point_x[2];
                            chassis_y_pid.SetPoint   = left_plant_seed_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_plant_seed_point_x[2];
                            chassis_y_pid.SetPoint   = right_plant_seed_point_y[2];
                            chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                        }
                    } else if (i == 2) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_plant_seed_point_x[3];
                            chassis_y_pid.SetPoint   = left_plant_seed_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_plant_seed_point_x[3];
                            chassis_y_pid.SetPoint   = right_plant_seed_point_y[3];
                            chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                        }
                    } else if (i == 3) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_plant_seed_point_x[4];
                            chassis_y_pid.SetPoint   = left_plant_seed_point_y[4];
                            chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_plant_seed_point_x[4];
                            chassis_y_pid.SetPoint   = right_plant_seed_point_y[4];
                            chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                        }
                    } else if (i == 4) {
                        if (general_state == LEFT_MODE) {
                            chassis_x_pid.SetPoint   = left_plant_seed_point_x[5];
                            chassis_y_pid.SetPoint   = left_plant_seed_point_y[5];
                            chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                        } else if (general_state == RIGHT_MODE) {
                            chassis_x_pid.SetPoint   = right_plant_seed_point_x[5];
                            chassis_y_pid.SetPoint   = right_plant_seed_point_y[5];
                            chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                        }
                    }
                    if (i == 0) {
                        motor_l_plantseed = 385;
                        motor_r_plantseed = -385;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        // 到达航路点后解除限位
                        while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                               (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                               (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                            osDelay(1);
                        }
                        motor_l_plantseed = 2;
                        motor_r_plantseed = -2;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        // 运送爪上的苗
                        Seed_Deposit_Open();
                        motor_l_gripseed = -745;
                        motor_r_gripseed = -745;
                        while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
                               ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
                               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
                               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
                            osDelay(1);
                        }
                        osDelay(500);
                        unitree_right_pos = -PI / 2 - 0.2;
                        unitree_left_pos  = PI / 2 + 0.2;
                        osDelay(700);
                        Seed_Deposit();
                        osDelay(200);
                        Seed_Deposit_Close();
                        osDelay(200);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        osDelay(200);
                        motor_l_plantseed = 200;
                        motor_r_plantseed = -200;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    } else if (i == 1) {
                        motor_l_plantseed = 385;
                        motor_r_plantseed = -385;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    } else if (i == 2) {
                        motor_l_plantseed = 580;
                        motor_r_plantseed = -580;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    } else if (i == 3) {
                        motor_l_plantseed = 810;
                        motor_r_plantseed = -810;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    } else if (i == 4) {
                        motor_l_plantseed = 990;
                        motor_r_plantseed = -990;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    } else if (i == 5) {
                        motor_l_plantseed = 2;
                        motor_r_plantseed = -2;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                    }
                }
                // 等待到达航路点
                while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                       (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                    osDelay(1);
                }
                if (general_state == LEFT_MODE) {
                    chassis_x_pid.SetPoint   = left_plant_seed_point_x[5];
                    chassis_y_pid.SetPoint   = left_plant_seed_point_y[5] + 500.0f;
                    chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                } else if (general_state == RIGHT_MODE) {
                    chassis_x_pid.SetPoint   = right_plant_seed_point_x[5];
                    chassis_y_pid.SetPoint   = right_plant_seed_point_y[5] - 500.0f;
                    chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                }
                while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                       (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                    osDelay(1);
                }
                chassis_yaw_pid.SetPoint = chassis_offset;
                while ((chassis_yaw - chassis_yaw_pid.SetPoint < -3.0f) ||
                       (chassis_yaw - chassis_yaw_pid.SetPoint > 3.0f)) {
                    osDelay(1);
                }
                // 状态转换
                auto_seed_state = Auto_Seed_Ready;
                break;
            default:
                break;
        }
        switch (auto_seed_state) {
            case Auto_Seed_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_ready_msg, &mav_auto_seed_msg);
                break;
            case Auto_Seed_Go:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_go_msg, &mav_auto_seed_msg);
                break;
            default:
                break;
        }
        osDelay(1);
    }
}
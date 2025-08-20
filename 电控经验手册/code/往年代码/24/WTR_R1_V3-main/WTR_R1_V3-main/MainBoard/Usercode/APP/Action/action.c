#include "action.h"

uint8_t down_flag = 0;

/************************************
 * @brief   总复位状态
 */
void Reset_Action(void)
{
    // 1. 球复位状态
    /***************************************
     * 默认动作
     *      1. 左右夹爪内收
     *      2. 中夹爪夹取状态
     *      3. 供球板打开
     *      4. 摩擦轮停止
     *      5. 机械臂内收
     */
    Ball_Servo_Grip();
    Ball_Servo_In();
    Ball_Servo_Ready();
    arm_angle           = -140;
    friction_speed_up   = 0;
    friction_speed_down = 0;
    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 5.0f) ||
           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -5.0f)) {
        osDelay(1);
    }
    // 2. 苗复位状态
    /*******************************************
     * 默认动作
     *      1. 宇树电机内收；
     *      2. 夹取状态；
     *      3. 取苗门关闭；
     *      4. 放苗门关闭；
     *      6. 电机处于高位;
     *      7. 前侧限位板关闭
     */
    if (rst_flag == 0) {
        // unitree_right_pos = -PI / 4;
        // unitree_left_pos  = PI / 4;
        unitree_right_pos = 0;
        unitree_left_pos  = 0;
        Seed_Grip();
        Seed_Deposit_Close();
        Seed_Plant_Close();
        Seed_Deposit_Buffer_Open();
        Seed_Front_Close();
        // motor_l_gripseed = -715;
        // motor_r_gripseed = -715;
        // while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 5.0f) ||
        //        ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -5.0f) ||
        //        ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 5.0f) ||
        //        ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -5.0f)) {
        //     osDelay(1);
        // }
    }
}

/************************************
 * @brief   取苗动作
 */
void Seed_Grip_Action(void)
{
    // 取苗动作
    /******************************************
     * 准备取苗动作
     *      1. 宇树电机取苗准备位置
     *      2. 夹爪放松
     *      3. 取苗门打开
     *      4. 电机降低
     */
    if (general_state == LEFT_MODE) {
        unitree_right_pos = PI / 2 + 0.9;
    } else if (general_state == RIGHT_MODE) {
        unitree_left_pos = -PI / 2 - 0.9;
    }
    Seed_Deposit();
    osDelay(700);
    if (general_state == LEFT_MODE) {
        motor_l_gripseed = -5;
    } else if (general_state == RIGHT_MODE) {
        motor_r_gripseed = -5;
    }
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
    while (btn_KnobR != 1) {
        osDelay(1);
    }
    // 宇树电机运动至超过苗轴线
    if (general_state == LEFT_MODE) {
        unitree_right_pos = PI / 2 - 0.25;
    } else if (general_state == RIGHT_MODE) {
        unitree_left_pos = -PI / 2 + 0.25;
    }
    // 夹紧，开轨道门
    osDelay(700);
    Seed_Grip();
    // Seed_Deposit_Open();
    osDelay(700);
    // 将苗夹起
    if (general_state == LEFT_MODE) {
        motor_l_gripseed = -715;
    } else if (general_state == RIGHT_MODE) {
        motor_r_gripseed = -715;
    }
    while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
           ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
        osDelay(1);
    }
    // // 第二段动作
    // // 前4个苗
    // if (seed_count == 0 || seed_count == 1 || seed_count == 2 || seed_count == 3) {
    //     motor_l_plantseed = 2;
    //     motor_r_plantseed = -2;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     Seed_Deposit_Buffer_Close();
    // unitree_right_pos = -PI / 2 - 0.2;
    // unitree_left_pos  = PI / 2 + 0.2;
    //     osDelay(1000);
    //     Seed_Deposit();
    //     Seed_Deposit_Close();
    //     osDelay(500);
    //     unitree_right_pos = PI / 4;
    //     unitree_left_pos  = -PI / 4;
    //     osDelay(100);
    //     motor_l_gripseed = -1005;
    //     motor_r_gripseed = -1005;
    //     osDelay(200);
    //     Seed_Deposit_Buffer_Close();
    //     osDelay(500);
    //     if (seed_count == 0) {
    //         motor_l_plantseed = 960;
    //         motor_r_plantseed = -960;
    //     } else if (seed_count == 1) {
    //         motor_l_plantseed = 770;
    //         motor_r_plantseed = -770;
    //     } else if (seed_count == 2) {
    //         motor_l_plantseed = 560;
    //         motor_r_plantseed = -560;
    //     } else if (seed_count == 3) {
    //         motor_l_plantseed = 385;
    //         motor_r_plantseed = -385;
    //     }
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         if (seed_count == 0) {
    //             if (hDJI[3][1].AxisData.AxisAngle_inDegree > 600 || hDJI[2][1].AxisData.AxisAngle_inDegree < -600) {
    //                 Seed_Deposit_Buffer_Open();
    //             }
    //         }
    //         if (seed_count == 1) {
    //             if (hDJI[3][1].AxisData.AxisAngle_inDegree > 500 || hDJI[2][1].AxisData.AxisAngle_inDegree < -500) {
    //                 Seed_Deposit_Buffer_Open();
    //             }
    //         }
    //         if (seed_count == 2) {
    //             if (hDJI[3][1].AxisData.AxisAngle_inDegree > 300 || hDJI[2][1].AxisData.AxisAngle_inDegree < -300) {
    //                 Seed_Deposit_Buffer_Open();
    //             }
    //         }
    //         if (seed_count == 3) {
    //             Seed_Deposit_Buffer_Open();
    //         }
    //         osDelay(1);
    //     }
    //     seed_count++;
    // }
    // // 第五个苗，不推
    // else if (seed_count == 4) {
    //     motor_l_plantseed = 2;
    //     motor_r_plantseed = -2;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     unitree_right_pos = -PI / 2 - 0.2;
    //     unitree_left_pos  = PI / 2 + 0.2;
    //     osDelay(1000);
    //     Seed_Deposit();
    //     Seed_Deposit_Close();
    //     osDelay(500);
    //     unitree_right_pos = PI / 4;
    //     unitree_left_pos  = -PI / 4;
    //     motor_l_gripseed  = -1005;
    //     motor_r_gripseed  = -1005;
    //     osDelay(200);
    //     Seed_Deposit_Buffer_Open();
    //     osDelay(500);
    //     motor_l_plantseed = 230;
    //     motor_r_plantseed = -230;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     seed_count++;
    // } else if (seed_count == 5) {
    //     Seed_Deposit_Close();
    //     osDelay(200);
    //     Seed_Grip();
    //     seed_count = 6;
    // } else if (seed_count == 6) {
    //     seed_count = 6;
    // }
    // osDelay(1);
    osDelay(1);
}

/************************************
 * @brief   放苗动作
 */
void Seed_Plant_Action(void)
{
    // /***********************
    //  * 放苗动作
    //  *
    //  */
    // // 第一段
    // Seed_Plant_Open();
    // osDelay(400);
    // Seed_Front_Open();
    // osDelay(500);
    // // 退出苗区
    // down_flag = 1;
    // osDelay(100);
    // Seed_Front_Close();
    // osDelay(400);
    // down_flag = 0;
    // osDelay(100);
    // Seed_Plant_Close();
    // osDelay(100);
    // // 推苗
    // if (seed_count == 6) {
    //     motor_l_plantseed = 385;
    //     motor_r_plantseed = -385;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     // 推苗校正
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         }
    //         osDelay(50);
    //     }
    //     motor_l_plantseed = 2;
    //     motor_r_plantseed = -2;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     // 运送爪上的苗
    //     Seed_Deposit_Open();
    //     motor_l_gripseed = -745;
    //     motor_r_gripseed = -745;
    //     while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
    //            ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
    //            ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
    //            ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     osDelay(500);
    //     unitree_right_pos = -PI / 2 - 0.2;
    //     unitree_left_pos  = PI / 2 + 0.2;
    //     osDelay(700);
    //     Seed_Deposit();
    //     osDelay(200);
    //     Seed_Deposit_Close();
    //     osDelay(200);
    //     unitree_right_pos = PI / 4;
    //     unitree_left_pos  = -PI / 4;
    //     osDelay(200);
    //     motor_l_plantseed = 200;
    //     motor_r_plantseed = -200;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     seed_count--;
    // } else if (seed_count == 5) {
    //     motor_l_plantseed = 385;
    //     motor_r_plantseed = -385;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     // 推苗校正
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         }
    //         osDelay(50);
    //     }
    //     seed_count--;
    // } else if (seed_count == 4) {
    //     motor_l_plantseed = 600;
    //     motor_r_plantseed = -600;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         }
    //         osDelay(50);
    //     }
    //     seed_count--;
    // } else if (seed_count == 3) {
    //     motor_l_plantseed = 810;
    //     motor_r_plantseed = -810;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         }
    //         osDelay(50);
    //     }
    //     seed_count--;
    // } else if (seed_count == 2) {
    //     motor_l_plantseed = 960;
    //     motor_r_plantseed = -960;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         }
    //         osDelay(50);
    //     }
    //     seed_count--;
    // } else if (seed_count == 1) {
    //     motor_l_plantseed = 2;
    //     motor_r_plantseed = -2;
    //     while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
    //            ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
    //            ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
    //         osDelay(1);
    //     }
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         }
    //         osDelay(50);
    //     }
    //     seed_count--;
    // } else if (seed_count == 0) {
    //     while (btn_KnobR != 1) {
    //         if (btn_LeftCrossUp == 1) {
    //             motor_l_plantseed += 20;
    //         } else if (btn_LeftCrossDown == 1) {
    //             motor_l_plantseed -= 20;
    //         } else if (btn_RightCrossUp == 1) {
    //             motor_r_plantseed -= 20;
    //         } else if (btn_RightCrossDown == 1) {
    //             motor_r_plantseed += 20;
    //         }
    //         osDelay(50);
    //     }
    //     seed_count = 0;
    // }
    if (general_state == RIGHT_MODE) {
        unitree_left_pos = 0;
    } else if (general_state == LEFT_MODE) {
        unitree_right_pos = 0;
    }
    if (general_state == LEFT_MODE) {
        motor_l_gripseed = -25;
    } else if (general_state == RIGHT_MODE) {
        motor_r_gripseed = -25;
    }
    while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
           ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
        osDelay(1);
    }
    while (btn_KnobR != 1) {
        osDelay(1);
    }
    Seed_Deposit();
    osDelay(1500);
    while (btn_KnobR != 1) {
        osDelay(1);
    }
    Seed_Grip();
    osDelay(1);
}

/**
 * @brief   取球动作
 */
void Ball_Pick_Action(void)
{
    Ball_Servo_Reset();
    osDelay(5);
    arm_angle = -10;
    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
        osDelay(1);
    }
    Ball_Servo_Out();
    osDelay(5);
}

/**
 * @brief   射球动作
 */
void Ball_Fire_Action(void)
{
    friction_speed_down = -600;
    friction_speed_up   = 4000;
    osDelay(2000);
    Ball_Servo_Grip();
    osDelay(500);
    arm_angle = -140;
    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
        osDelay(1);
    }
    Ball_Servo_In();
    osDelay(1000);
    Ball_Servo_Reset();
    while (btn_KnobR != 1) {
        osDelay(1);
    }
    osDelay(500);
    Ball_Servo_Fire();
    osDelay(1000);
    Ball_Servo_Ready();
    osDelay(500);
}
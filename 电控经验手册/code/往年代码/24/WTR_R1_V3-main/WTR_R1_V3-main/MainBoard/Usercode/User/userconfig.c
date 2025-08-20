#include "userconfig.h"

/*****************************************************************************
 * 消息定义
 */

mavlink_joystick_air_dashboard_set_title_t mav_dir_title;
mavlink_joystick_air_dashboard_set_msg_t mav_dir_msg;
char direct_title[20]     = "Direct";
char direct_left_msg[20]  = "LeftMode";
char direct_right_msg[20] = "RightMode";

mavlink_joystick_air_dashboard_set_title_t mav_run_title;
mavlink_joystick_air_dashboard_set_msg_t mav_run_msg;
char run_title[20]      = "Run";
char run_auto_msg[20]   = "AutoMode";
char run_handle_msg[20] = "HandleMode";

mavlink_joystick_air_dashboard_set_title_t mav_rst_choose_title;
mavlink_joystick_air_dashboard_set_msg_t mav_rst_choose_msg;
char rst_choose_title[20]  = "RstChoose";
char rst_choose_msg[20] = "NoStart";
char norst_choose_msg[20] = "Start";

mavlink_joystick_air_dashboard_set_title_t mav_dir_choose_title;
mavlink_joystick_air_dashboard_set_msg_t mav_dir_choose_msg;
char direct_choose_title[20] = "DirectChoose";
char direct_choose_msg[20]   = "Waiting";

mavlink_joystick_air_dashboard_set_title_t mav_mode_title;
mavlink_joystick_air_dashboard_set_msg_t mav_mode_msg;
char mode_title[20]    = "Mode";
char mode_idle_msg[20] = "IDLEMode";
char mode_seed_msg[20] = "SeedMode";
char mode_ball_msg[20] = "BallMode";

mavlink_joystick_air_dashboard_set_title_t mav_handle_seed_title;
mavlink_joystick_air_dashboard_set_msg_t mav_handle_seed_msg;
char handle_seed_title[20]     = "SeedWork";
char handle_seed_ready_msg[20] = "Ready";
char handle_seed_grip_msg[20]  = "Grip";
char handle_seed_plant_msg[20] = "Plant";

mavlink_joystick_air_dashboard_set_title_t mav_handle_ball_title;
mavlink_joystick_air_dashboard_set_msg_t mav_handle_ball_msg;
char handle_ball_title[20]     = "BallWork";
char handle_ball_ready_msg[20] = "Ready";
char handle_ball_pick_msg[20]  = "Pick";
char handle_ball_fire_msg[20]  = "Fire";

mavlink_joystick_air_dashboard_set_title_t mav_auto_seed_title;
mavlink_joystick_air_dashboard_set_msg_t mav_auto_seed_msg;
char auto_seed_title[20]     = "SeedWork";
char auto_seed_ready_msg[20] = "Ready";
char auto_seed_go_msg[20]    = "Go";

mavlink_joystick_air_dashboard_set_title_t mav_auto_ball_title;
mavlink_joystick_air_dashboard_set_msg_t mav_auto_ball_msg;
char auto_ball_title[20]     = "BallWork";
char auto_ball_ready_msg[20] = "Ready";
char auto_ball_go_msg[20]    = "Going";

/*****************************************************************************
 * 点位定义
 */
float left_grip_seed_point_x[6]   = {130, 130, 130, 130, 130, 130};               // 左侧取苗纵向坐标
float left_grip_seed_point_y[6]   = {-1230, -1480, -2230, -2480, -3230, -3480};   // 左侧取苗横向坐标
float left_plant_seed_point_x[6]  = {1942, 1942, 1942, 1942, 1942, 1942};         // 左侧放苗纵向坐标
float left_plant_seed_point_y[6]  = {-3530, -3030, -2530, -2030, -1530, -1030};   // 左侧放苗横向坐标
float left_plant_seed_point_ax[6] = {984, 1484, 1984, 2484, 2984, 3484};          // 左侧放苗激光标定点(车前)
float left_plant_seed_point_ay[6] = {1162, 1162, 1162, 1162, 1162, 1162};         // 左侧放苗激光标定点(车侧)
float left_ball_point_x[7]        = {0, 0, 0, 0, 0, 0, -1120};                    // 左侧球纵向坐标
float left_ball_point_y[7]        = {-655, -1155, -1655, -2155, -2655, -3155, 0}; // 左侧球横向坐标

float right_grip_seed_point_x[6]   = {130, 130, 130, 130, 130, 130};       // 右侧取苗纵向坐标
float right_grip_seed_point_y[6]   = {1230, 1480, 2230, 2480, 3230, 3480}; // 右侧取苗横向坐标
float right_plant_seed_point_x[6]  = {1942, 1942, 1942, 1942, 1942, 1942}; // 右侧放苗纵向坐标
float right_plant_seed_point_y[6]  = {3530, 3030, 2530, 2030, 1530, 1030}; // 右侧放苗横向坐标
float right_plant_seed_point_ax[6] = {984, 1484, 1984, 2484, 2984, 3484};  // 右侧放苗激光标定点(车前)
float right_plant_seed_point_ay[6] = {1162, 1162, 1162, 1162, 1162, 1162};
float right_ball_point_x[7]        = {0, 0, 0, 0, 0, 0, -1120};              // 右侧球纵向坐标
float right_ball_point_y[7]        = {655, 1155, 1655, 2155, 2655, 3155, 0}; // 右侧球横向坐标
#include "omniwheel_chassis.h"

owChassis_t chassis;

void owChassis_calculateVelocity(owChassis_t* this);

/**
 * @brief 全向轮底盘初始化
 * @param this
 */
HAL_StatusTypeDef owChassisInit(owChassis_t* this)
{
    this->hcanx = &hcan1;
    CANFilterInit(&hcan1);
    // CAN2FilterInit(&hcan2);
    for (int i = 0; i < 4; i++)
    {
        this->wheel[i].motor->motorType = M3508;
        this->wheel[i].targetspeed = 0;
        this->wheel[i].motor = &hDJI[i];
    }
    this->wheel[0].motor->speedPID.KP = 13.0f;
    this->wheel[0].motor->speedPID.KI = 0.15f;
    this->wheel[0].motor->speedPID.KD = 4.0f;
    this->wheel[1].motor->speedPID.KP = 13.0f;
    this->wheel[1].motor->speedPID.KI = 0.15f;
    this->wheel[1].motor->speedPID.KD = 4.0f;
    this->wheel[2].motor->speedPID.KP = 13.0f;
    this->wheel[2].motor->speedPID.KI = 0.15f;
    this->wheel[2].motor->speedPID.KD = 4.0;
    this->wheel[3].motor->speedPID.KP = 13.0f;
    this->wheel[3].motor->speedPID.KI = 0.15f;
    this->wheel[3].motor->speedPID.KD = 4.0f;
    for (int i = 0; i < 4; i++)
    {
        this->wheel[i].motor->encoder_resolution = 8192.0f;
		this->wheel[i].motor->speedPID.outputMax = 10000;

        this->wheel[i].motor->speedPID.KP = 20.0f;
        this->wheel[i].motor->speedPID.KI = 0.82f;
        this->wheel[i].motor->speedPID.KD = 1.5f;
		// 位置环PID
		this->wheel[i].motor->posPID.KP = 80.0f;
        this->wheel[i].motor->posPID.KI = 0.0f;
        this->wheel[i].motor->posPID.KD = 0.5f;
		this->wheel[i].motor->posPID.outputMax = 5000;
		// this->wheel[i].motor->posPID.outputMin = 1500;
		this->wheel[i].motor->f_current = 0;
        this->wheel[i].motor->reductionRate = 3591.0f / 187.0f; // 2006减速比为36 3508减速比约为19
    }
    this->target_velocity.vmax = 30.0f;
    this->target_velocity.vwmax = 35.0f;
    return HAL_OK;
}

/**
 * @brief 全向轮底盘设置目标速度
 * @param this
 * @param vx 左右速度m/s
 * @param vy 前后速度m/s
 * @param vw 角速度rad/s
 */
void owChassis_set_targetVelocity(owChassis_t* this, float vx, float vy, float vw)
{
    this->target_velocity.vx = vx;
    this->target_velocity.vy = vy;
    this->target_velocity.vw = vw;
    owChassis_calculateVelocity(this);  //速度解算
}

float targetangle = 0;//锁定时的目标角度
/**
 * @brief 全向轮底盘锁定角度
 * @param this
 */
float owChassis_lock_w_calculate(owChassis_t* this)
{
    float nowangle = chassis_yaw;
    while(nowangle - targetangle >= 180.0f)//陀螺仪只返回0-360，需要找最佳运动方向
        nowangle -= 360.0f;
    while(nowangle - targetangle <= -180.0f)
        nowangle += 360.0f;
    static const float KP = 5.3f;
    static const float KI = 0.0f;
    static const float KD = 3.0f;
    float error = 0;
    static float last_error = 0;
    static float last_last_error = 0;
    static float output = 0;
    error = targetangle - nowangle;
    output += KI * error + 
              KP * (error - last_error) + 
              KD * (error - 2 * last_error + last_last_error);
    last_last_error = last_error;
    last_error = error;
    return output;
}

/**
 * @brief 全向轮底盘遥控器控制
 * @param this
 */
void owChassis_rmctlinput(owChassis_t* this)
{
    //将摇杆输入坐标缩放到半径为v的圆中
    float v = sqrt((float)(rmctl.rmctl_msg.usr_left_x*rmctl.rmctl_msg.usr_left_x + rmctl.rmctl_msg.usr_left_y*rmctl.rmctl_msg.usr_left_y))
                / JOYSTICKMAX * this->target_velocity.vmax;
    float angle = (float)atan2((double)rmctl.rmctl_msg.usr_left_y,(double)rmctl.rmctl_msg.usr_left_x);
    //打滑修正
    // float nowangle = chassis_yaw;
    // while(nowangle - targetangle >= 180.0f)//陀螺仪只返回0-360，需要找最佳运动方向
    //     nowangle -= 360.0f;
    // while(nowangle - targetangle <= -180.0f)
    //     nowangle += 360.0f;
    // float angle_error = targetangle - nowangle;//修正角
    // // if(fabs(angle_error) >= 15.0f)
    // //     angle_error = 0.0f;
    // float angle_offset = v >= 0.0f * this->target_velocity.vmax ? angle - 1.0f * angle_error : angle ;

    this->target_velocity.vw = rmctl.rmctl_msg.usr_right_x / JOYSTICKMAX * this->target_velocity.vwmax;//使摇杆方向与旋转方向一致
    if(fabs(this->target_velocity.vw) < 0.01)
        this->target_velocity.vw = 0;
    if(v >= this->target_velocity.vmax)
        v = this->target_velocity.vmax;
    else if(v > this->target_velocity.vmax * 0.01f)
    {
        this->target_velocity.vx = v * cosf(angle);
        this->target_velocity.vy = v * sinf(angle);
    }
    else
    {
        this->target_velocity.vx = 0.0f;
        this->target_velocity.vy = 0.0f;
    }
    //角度锁定
    static bool isinit = false;
    if(isinit == false)
    {
        targetangle = chassis_yaw;
        isinit = true;
    }
    if(fabsf(this->target_velocity.vw) > this->target_velocity.vwmax*0.01f)
    {
        targetangle = chassis_yaw;
    }
    else
    {
        this->target_velocity.vw = -owChassis_lock_w_calculate(this);//自动校正方向
    }
    //速度解算
    owChassis_calculateVelocity(this);
}


/**
 * @brief (内部函数)四轮全向轮底盘速度解算
 * @param this
 */
void owChassis_calculateVelocity(owChassis_t* this)
{
    this->wheel[0].targetspeed = (this->target_velocity.vy / COS_ANGLE
                                 + this->target_velocity.vx / SIN_ANGLE
                                 + this->target_velocity.vw * CENTRE2WHEEL_LENGTH)
                                 /WHEEL_RADIUS*60/TWOPI;
    this->wheel[1].targetspeed = (-this->target_velocity.vy / COS_ANGLE
                                 + this->target_velocity.vx / SIN_ANGLE
                                 + this->target_velocity.vw * CENTRE2WHEEL_LENGTH)
                                 /WHEEL_RADIUS*60/TWOPI;
    this->wheel[2].targetspeed = (-this->target_velocity.vy / COS_ANGLE
                                 - this->target_velocity.vx / SIN_ANGLE
                                 + this->target_velocity.vw * CENTRE2WHEEL_LENGTH)
                                 /WHEEL_RADIUS*60/TWOPI;
    this->wheel[3].targetspeed = (this->target_velocity.vy / COS_ANGLE
                                 - this->target_velocity.vx / SIN_ANGLE
                                 + this->target_velocity.vw * CENTRE2WHEEL_LENGTH)
                                 /WHEEL_RADIUS*60/TWOPI;
}

/**
 * @brief 全向轮底盘差速PID补偿，确保起步同步(效果不好)
 * @param this
 * @param id 电机编号
 */
float owChassis_DSPID(owChassis_t* this,int id)
{
    static const float KP = 0.1f;
    static const float KI = 0.0f;
    static const float KD = 0.5f;
    float error[4] = {0,0,0,0};
    static float last_error[4] = {0,0,0,0};
    static float last_last_error[4] = {0,0,0,0};
    static float dsoutput[4] = {0,0,0,0};
    float target = (this->wheel[0].motor->FdbData.rpm + 
                    this->wheel[1].motor->FdbData.rpm + 
                    this->wheel[2].motor->FdbData.rpm + 
                    this->wheel[3].motor->FdbData.rpm) / 4;
    error[id] = target - this->wheel[id].motor->FdbData.rpm;
    dsoutput[id] += KI * error[id] + 
                    KP * (error[id] - last_error[id]) + 
                    KD * (error[id] - 2 * last_error[id] + last_last_error[id]);
    last_last_error[id] = last_error[id];
    last_error[id] = error[id];
    return dsoutput[id];
}

/**
 * @brief 全向轮底盘执行器函数，调用PID并发送，建议周期为5ms
 * @param this
 */
void owChassis_executor(owChassis_t* this)
{
    //PID
    for (int i = 0; i < 4; i++)
    {
        speedServo(this->wheel[i].targetspeed,this->wheel[i].motor);
    }
    //CAN发送
    CanTransmit_DJI_1234(this->hcanx, this->wheel[0].motor->speedPID.output,
                                      this->wheel[1].motor->speedPID.output,
                                      this->wheel[2].motor->speedPID.output,
                                      this->wheel[3].motor->speedPID.output);
}
// void owChassis_executor(owChassis_t* this)
// { 
//     static uint32_t last_time = 0;
//     static float position[4] = {0,0,0,0};
//     uint32_t now_time = HAL_GetTick();
//     float dt = (now_time - last_time) / 1000.0f;
//     for (int i = 0; i < 4; i++)
//     {
//         float dx = this->wheel[i].targetspeed / this->wheel[i].motor->reductionRate * dt;
//         position[i] += dx;
//         positionServo(position[i],this->wheel[i].motor);
//     }
//     //CAN发送
//     CanTransmit_DJI_1234(this->hcanx, this->wheel[0].motor->speedPID.output,
//                                     this->wheel[1].motor->speedPID.output,
//                                     this->wheel[2].motor->speedPID.output,
//                                     this->wheel[3].motor->speedPID.output);
//     last_time = now_time;
// }

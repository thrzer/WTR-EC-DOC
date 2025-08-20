//
// Created by tony on 23-10-24.
//

#include "steering_wheel.h"
#include "arm_math.h"
#include "steering_wheel_chassis.h"
#include <stdint.h>

uint8_t Swheel_aim(Swheel_t *this);

#ifdef USE_DEFAULT_MOTOR_PARAM
#include "DJI.h"
#include "Caculate.h"
#include "wtr_vesc.h"

//默认电机api (转向电机为M2006)
/**
 * @brief 内部函数，转向速度获取
 * @return 轮子转向速度，单位rpm
 */
void mDJI_RgetSpeed(void *this)
{
    ((Swheel_t*)this)->rotation_speed = hDJI[((Swheel_t*)this)->id].FdbData.rpm/hDJI[((Swheel_t*)this)->id].reductionRate/((Swheel_t*)this)->rotation_reduction;
}
/**
 * @brief 内部函数，转向位置获取
 * @return 转过的总弧度，单位rad
 * */
void mDJI_RgetPos(void *this)
{
    ((Swheel_t*)this)->rotation_pos= hDJI[((Swheel_t*)this)->id].Calculate.RotorAngle_all/hDJI[((Swheel_t*)this)->id].reductionRate*PI/180.f/((Swheel_t*)this)->rotation_reduction;
    // = fmod(range,2*M_PI);
}
/**
 * @brief 内部函数，转向速度设置
 * @param speed 轮子转向速度，单位rpm
 */
void mDJI_RspeedServo(void *this, const float speed)
{
    speedServo(speed*hDJI[((Swheel_t*)this)->id].reductionRate*((Swheel_t*)this)->rotation_reduction,&hDJI[((Swheel_t*)this)->id]);
}
/**
 * @brief 内部函数，转向位置设置
 * @param pos 轮子转过的总弧度，单位rad
 */
void mDJI_RposServo(void *this, const float pos)
{
    positionServo(pos*180.f/PI*((Swheel_t*)this)->rotation_reduction,&hDJI[((Swheel_t*)this)->id]);
}
/**
 * @brief 转向电机复位
 */
void mDJI_Rreset(void *this)
{
    hDJI[((Swheel_t*)this)->id].Calculate.RotorAngle_all = 0;
    hDJI[((Swheel_t*)this)->id].Calculate.RotorAngle_0_360_Log[0] = 0;
    hDJI[((Swheel_t*)this)->id].Calculate.RotorAngle_0_360_Log[1] = 0;
    hDJI[((Swheel_t*)this)->id].Calculate.RotorRound = 0;
    hDJI[((Swheel_t*)this)->id].Calculate.RotorAngle_0_360_OffSet = hDJI[((Swheel_t*)this)->id].FdbData.RotorAngle_0_360;
}
void vesc_MspeedServo(void *this, const float speed)
{
    float erpm = 840*speed/STEERING_WHEEL_DIAMETER/PI;//n(rpm)=60f=60v/(pi*D) 840=14*60
    VESC_CAN_SET_ERPM(&hVESC[((Swheel_t*)this)->id],erpm);
}
void vesc_Mstop(void *this)
{
    VESC_CAN_SET_BRAKE_CURRENT(&hVESC[((Swheel_t*)this)->id],-20);//-20:刹车电流
}
#endif



/**
 * @brief 舵轮初始化
 * @param id
 * @param LS_GPIOx 光电开关GPIO
 * @param LS_GPIO_Pin 光电开关引脚
 * @return
 */
void Swheel_init(
    Swheel_t *this,    
    uint8_t id,
    GPIO_TypeDef *_LS_GPIOx,    // 限位开关GPIO
    uint16_t _LS_GPIO_Pin,      // 限位开关引脚
    float rotation_reduction,// 转向齿轮减速比（大比小）
    float correcting_speed,
    r_getSpeed_func RgetSpeed,
    r_getPos_func RgetPos,
    r_speedServo_func RspeedServo,
    r_posServo_func RposServo,
    r_reset_func Rreset,
    r_stop_func Rstop,
    m_speedServo_func MspeedServo,
    m_stop_func Mstop)
{
    this->id = id;
    this->_LS_GPIOx = _LS_GPIOx;
    this->_LS_GPIO_Pin = _LS_GPIO_Pin;
    this->rotation_reduction = rotation_reduction;
    this->correcting_speed = correcting_speed;
    this->RgetSpeed = RgetSpeed;
    this->RgetPos = RgetPos;
    this->RspeedServo = RspeedServo;
    this->RposServo = RposServo;
    this->Rreset = Rreset;
    this->Rstop = Rstop;
    this->MspeedServo = MspeedServo;
    this->Mstop = Mstop;

    this->_light_switch_flag = 0;
    this->direction = 0;
    this->target_main_speed =0;
    this->rotation_speed = 0;
    this->rotation_pos = 0;
    this->correcting_stage=0;
    this->state = STOP;
}
/**
 * @brief 转向轮启动校准
 * @param this
 */
void Swheel_startCorrect(Swheel_t *this)
{
    this->state = CORRECTING;
}
/**
 * @brief 转向轮限位开关外部中断回调函数
 * @param this
 * @param GPIO_Pin
 */
void Swheel_EXTI_Callback(Swheel_t *this, uint16_t GPIO_Pin)
{
    if(GPIO_Pin==this->_LS_GPIO_Pin)
        this->_light_switch_flag= 1;
}
/**
 * @brief 转向轮校准
 * @param this
 * @description
 * 1.转向轮转动到限位开关处 (stage=0)
 * 2.转向轮反向转动到限位开关处 (stage = 1~1000,如果限位开关触发则stage=-1,若一直不触发直到stage=1000则stage=0)
 * 3.转向轮停止,等待（stage = -1->-10）
 * 4.电机复位（stage = -10）
 */
static inline uint8_t _Swheel_correcting(Swheel_t *this)
{

    if(this->correcting_stage==0)
    {
        if(this->_light_switch_flag)
        {
            this->RspeedServo(this, -1.0*this->correcting_speed/5);
            this->_light_switch_flag = 0;
            this->correcting_stage=1;
        }
        else
        {
            this->RspeedServo(this, this->correcting_speed);
        }
        return 0;
    }
    else if(this->correcting_stage>0&&this->correcting_stage<1000)
    {
        if(this->_light_switch_flag)
        {
            this->RspeedServo(this, 0);
            this->_light_switch_flag = 0;
            this->correcting_stage=-1;
            return 0;
        }
        else
        {   this->correcting_stage++;
            this->RspeedServo(this, -1.0*this->correcting_speed/5);
            return 0;
        }
    }
    else if(this->correcting_stage>=1000){this->correcting_stage = 0;return 0;}//重头再来
    else if(this->correcting_stage<0&&this->correcting_stage>-100)
    {
        this->correcting_stage--;
        this->RspeedServo(this, 0);
        return 0;
    }//延时
    else if(this->correcting_stage<=-100)
    {
        this->Rreset(this);
        this->correcting_stage=0;
        return 1;
    }
}

/**
 * @brief 舵轮执行器函数
 * @param this
 * @description
 * 1.如果状态为STOP，则电机停止
 * 2.如果状态为CORRECTING，则执行校准
 * 3.如果状态为RUNNING，则执行转向
 */
void steeringWheel_executor(Swheel_t *this)
{
    this->RgetPos(this);
    this->direction=fmod(this->rotation_pos,2*PI);//+this->direction_offset;
    if(this->direction>PI)
        this->direction-=2*PI;
    
    this->RgetSpeed(this);
    if(this->state==CORRECTING)
    {
        if(_Swheel_correcting(this))
            this->state = STOP;
        return;
    }
    if(this->state==STOP)
    {
        this->RspeedServo(this, 0);
        this->Mstop(this);
        //_Swheel_mMotor_speedServo(this, 0);
        return;
    }
    if(this->state==RUNNING)
    {
        if(Swheel_aim(this))
        {
            // this->state=AIMMING;
            return;
        }
        this->MspeedServo(this, this->target_main_speed);
        return;
    }
    if(this->state==AIMMING) 
    {
        if(!Swheel_aim(this))
        {
            this->state=STOP;
            return;
        }
        return;
    }

}


uint8_t Swheel_aim(Swheel_t *this)
{
    float tgt_d[2];
    float d[2];
    float d_err;
    //int8_t is_invert;
    d[0] = this->direction;
    d[1] = d>=0 ? d[0]-PI:d[1]+PI;
    tgt_d[0] = this->target_direction;
    tgt_d[1] = tgt_d[0]>0 ? tgt_d[0]-2*PI:tgt_d[0]+2*PI;
    for(int8_t i=0;i<=1;i++)
    {
            d_err = tgt_d[i]-d[0];
            if(fabsf(d_err)<PI/2)
                break;
            d_err = tgt_d[i]-d[1];
            if(fabsf(d_err)<PI/2)
            {
                this->target_main_speed*=-1;
                break;
            }   
    }
    

    // if (fabsf(tgt_d1-d)<=PI/2) {
    //     d_err = tgt_d1-d;
    //     //this->target_main_speed*=1;
    // }
    // else if (fabsf(tgt_d2-d)<=PI/2) {
    //     d_err = tgt_d1-d;
    // }
    // else {
    //     d_err = this->target_direction-invert_d;        
    //     this->target_main_speed*=-1;
    // }
    this->RposServo(this, d_err+this->rotation_pos);
    if(fabsf(d_err)<0.05)
        return 0;
    else
        return 1;

}
//
// Created by tony on 23-10-27.
//

#include "steering_wheel_chassis.h"
#include "_startup.h"
#include "main.h"
#include "arm_math.h"
#include "steering_wheel.h"
// #include "stm32f4xx_hal_def.h"
#include "FreeRTOS.h"
#include <stdint.h>
#ifdef USE_DEFAULT_MOTOR_PARAM
#include "DJI.h"
HAL_StatusTypeDef swChassis_rM_setFcurrent(swChassis_t *this);
#endif
#define norm(a,b) sqrt((a)*(a)+(b)*(b)) //计算模长
/*四舵轮底盘配置__start*/
#ifdef FOUR_WHEELS_CHASSIS
const double swChassis_wheel_x[4]={CHASSIS_WHEEL_LENGTH/2,CHASSIS_WHEEL_LENGTH/2,-CHASSIS_WHEEL_LENGTH/2,-CHASSIS_WHEEL_LENGTH/2};
const double swChassis_wheel_y[4]={CHASSIS_WHEEL_DISTANCE/2,-CHASSIS_WHEEL_DISTANCE/2,CHASSIS_WHEEL_DISTANCE/2,-CHASSIS_WHEEL_DISTANCE/2};
/**
 * @brief 转向轮初始化(2006+vesc配置)
 * @param this
 * @param id 转向轮id
 * @param GPIOx 光电门口GPIOx
 * @param GPIO_Pin  光电门口GPIO_Pin
 */
void vesc_2006_swheel_init(Swheel_t *this, uint8_t id, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
        Swheel_init(
        this,
        id,
        GPIOx,
        GPIO_Pin,
        2.f,
        30*powf(-1,id),
        mDJI_RgetSpeed,
        mDJI_RgetPos,
        mDJI_RspeedServo,
        mDJI_RposServo,
        mDJI_Rreset,
        NULL,
        vesc_MspeedServo,
        vesc_Mstop
        );
}
#endif
/*四舵轮底盘配置__end*/
void calculate_target_velocity(swChassis_t *this);
void chassis_state_machine(swChassis_t *this);

/**
 * @brief 底盘初始化
 */
void swChassis_init(swChassis_t *this)
{
    #ifdef USE_DEFAULT_MOTOR_PARAM
    //DJI电机初始化
    for (int i=1;i<5;i++)
    {
        hVESC[i-1].hcann= &hcan2;
        hVESC[i-1].controller_id= 100+i;
    }
    #endif //USE_DEFAULT_MOTOR_PARAM


    #ifdef FOUR_WHEELS_CHASSIS
    vesc_2006_swheel_init(&this->wheels[0],0,SWHEEL_0_LS_GPIO_Port,SWHEEL_0_LS_Pin);
    vesc_2006_swheel_init(&this->wheels[1],1,SWHEEL_1_LS_GPIO_Port,SWHEEL_1_LS_Pin);
    vesc_2006_swheel_init(&this->wheels[2],2,SWHEEL_2_LS_GPIO_Port,SWHEEL_2_LS_Pin);
    vesc_2006_swheel_init(&this->wheels[3],3,SWHEEL_3_LS_GPIO_Port,SWHEEL_3_LS_Pin);

    this->swheel_num = 4;
    #endif //FOUR_WHEELS_CHASSIS

    this->state = CHASSIS_STOP;
    this->target_v.vx = 0;
    this->target_v.vy = 0;
    this->target_v.vw = 0;
    this->current_v.vx = 0;
    this->current_v.vy = 0;
    this->current_v.vw = 0;

}
/**
 * @brief 底盘开始校正
 */
HAL_StatusTypeDef swChassis_startCorrect(swChassis_t *this)
{
    //this->state= CHASSIS_CORRECTING;
    #ifdef USE_DEFAULT_MOTOR_PARAM
    // HAL_StatusTypeDef correct_state;
    //测出M2006电机的静态摩擦力，设置前馈电流
    // correct_state = swChassis_rM_setFcurrent(this);
    // if(correct_state!=HAL_OK)
    //     return HAL_ERROR;
    #endif
    for (uint8_t i = 0; i <this->swheel_num ; i++)
        this->wheels[i].state=CORRECTING;
    return HAL_OK;
}
/**
 * @brief 检查底盘是否校准完成
 * 
 * @param this 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef swChassis_CheckCorrect(swChassis_t *this)
{
    for (uint8_t i = 0; i < this->swheel_num ; i++)
    {
        if(this->wheels[i].state!=STOP)
            return HAL_BUSY;
    }
    return HAL_OK;
    
}
/**
 * @brief 设置底盘目标速度
 * @param vx x方向速度
 * @param vy y方向速度
 * @param vw 角速度
 */
void swChassis_set_targetVelocity(swChassis_t *this, const double vx, const double vy, const double vw)
{
    this->target_v.vx = vx;
    this->target_v.vy = vy;
    this->target_v.vw = vw;
}

/**
 * @brief 保持底盘伺服静止
 * 
 * @param this 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef swChassis_set_still(swChassis_t *this)
{
    for (uint8_t i = 0;i<this->swheel_num;i++)
        this->wheels[i].state = STOP;
    return HAL_OK;
}

/**
 * @brief 检查底盘是否有速度指令
 * 
 * @param this 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef swChassis_check_velocity(swChassis_t *this)
{
    if(fabs(this->target_v.vx) <0.01 &&fabs(this->target_v.vy) < 0.01&& fabs(this->target_v.vw) < 0.01)
        return HAL_OK;
    else
        return HAL_BUSY;
}

/**
 * @brief 检查底盘是否有转向指令，有则返回0，无则返回1
 * 
 * @param this 
 * @return uint8_t 
 */
uint8_t swChassis_check_AimorRun(swChassis_t *this)
{
    for (uint8_t i = 0; i <this->swheel_num ; i++)
        {
            if(this->wheels[i].state==AIMMING)
            {
                return 0;
            }
        }
    return 1;
}

HAL_StatusTypeDef swChassis_set_Running(swChassis_t *this)
{
    for (uint8_t i = 0; i <this->swheel_num ; i++)
        this->wheels[i].state = RUNNING;
    return HAL_OK;
}


/**
 * @brief 底盘执行器函数
 * @param this
 */
void swChassis_executor(swChassis_t *this)
{
    //chassis_state_machine(this);
    
    // double dji_output_withfc[4];
    for(int i=0;i<this->swheel_num;i++)
    {
        steeringWheel_executor(&this->wheels[i]);
        // if(hDJI[i].speedPID.output>100)
        //     dji_output_withfc[i]=hDJI[i].speedPID.output+hDJI[i].f_current;
        // else if(hDJI[i].speedPID.output<-100)
        //     dji_output_withfc[i]=hDJI[i].speedPID.output-hDJI[i].f_current;
        // else
        //     dji_output_withfc[i]=hDJI[i].speedPID.output;
    }
    // CanTransmit_DJI_1234(&hcan1,dji_output_withfc[0],dji_output_withfc[1],dji_output_withfc[2],dji_output_withfc[3]);
        // CanTransmit_DJI_1234(&hcan1,hDJI[0].speedPID.output,hDJI[1].speedPID.output,hDJI[2].speedPID.output,hDJI[3].speedPID.output);
    CanTransmit_DJI_1234(&hcan1,speedpid[0].output,speedpid[1].output,speedpid[2].output,speedpid[3].output);

}
/**
 * @brief 底盘速度解算
 * @param this
 */
#ifdef FOUR_WHEELS_CHASSIS
// void calculate_target_velocity(swChassis_t *this)
// {
//     double vx=this->target_v.vx;
//     double vy=this->target_v.vy;
//     double vw=this->target_v.vw;
//     if(vx==0)
//         vx=0.0001f;
//     double v = norm(vx,vy);
//     double slip_angle = atan2(vy,vx);
//     if(vw!=0)
//     {
//         double radius = fabsf(v/vw); //曲率半径
//         double theta;
//         if(vw>0)
//             theta = slip_angle+PI/2;
//         else
//             theta = slip_angle-PI/2;
//         double Ix = radius*cos(theta);
//         double Iy = radius*sin(theta);
//         for (int i = 0; i < this->swheel_num; i++) {
//             double td;
//             if(vw>0)
//                 td = atan2(Iy-swChassis_wheel_y[i],Ix-swChassis_wheel_x[i])-PI/2;
                
//             else
//                 td = atan2(Iy-swChassis_wheel_y[i],Ix-swChassis_wheel_x[i])+PI/2;
//             //td:-2pi~2pi,把 target_direction 限制到-pi~pi
//             if(td<-2*PI)
//                 this->wheels[i].target_direction = td +2*PI;
//             else if (td>2*PI)
//                 this->wheels[i].target_direction = td -2*PI;
//             else
//                 this->wheels[i].target_direction = td;
//             //如果下一个周期可以
//             this->wheels[i].target_main_speed = fabsf(vw) * norm(Ix-swChassis_wheel_x[i],Iy-swChassis_wheel_y[i]);
//             //this->wheels[i].target_main_speed = vw * norm(0.15,0.15);
//         }

//     } else 
//     {
//         for (int i = 0; i < this->swheel_num; i++) {
//             this->wheels[i].target_direction = slip_angle;
//             this->wheels[i].target_main_speed = v;
//         }
//     }
// }
void calculate_target_velocity(swChassis_t *this)
{
    for(int i=0;i<this->swheel_num;i++)
    {
        double vi[2];//轮子在车刚体坐标系下的速度矢量
        vi[0] = this->target_v.vx - this->target_v.vw*swChassis_wheel_y[i];
        vi[1] = this->target_v.vy + this->target_v.vw*swChassis_wheel_x[i];
        this->wheels[i].target_vxy[0]=cos(this->wheels[i].current_pos)*vi[0]+sin(this->wheels[i].current_pos)*vi[1];
        this->wheels[i].target_vxy[1]=-sin(this->wheels[i].current_pos)*vi[0]+cos(this->wheels[i].current_pos)*vi[1];
        // this->wheels[i].target_main_speed = norm(vi[0],vi[1]);
        // if(vi[0]!=0)
        //     this->wheels[i].target_direction = atan2(vi[1],vi[0]);
        // else if(vi[1]>0)
        //     this->wheels[i].target_direction = PI/2;
        // else
        //     this->wheels[i].target_direction = -PI/2;
    }
}
#endif
/**
 * @brief 底盘外部中断回调函数
 * @param this
 * @param GPIO_Pin
 */
void swChassis_EXTI_Callback(swChassis_t *this, uint16_t GPIO_Pin)
{
    for (int i = 0; i < this->swheel_num; i++)
    {
        Swheel_EXTI_Callback(&this->wheels[i], GPIO_Pin);
    }
}


#ifdef USE_DEFAULT_MOTOR_PARAM
/**
 * @brief 测出M2006电机的静态摩擦力，设置前馈电流
 * @param this
 */
// HAL_StatusTypeDef swChassis_rM_setFcurrent(swChassis_t *this)
// {
//     uint8_t flag=0x00;
//     uint16_t time_count[4] = {0};     //记录电流如果为零的次数过多，则认为电机故障

//     while (flag!=0x0f)
//     {
//         for (uint8_t i = 0; i < this->swheel_num ; i++)
//         {
//             if(hDJI[i].FdbData.rpm>0.1)
//             {
//                 flag = flag|0x01<<i;
//                 continue;
//             }
//             else if (hDJI[i].FdbData.rpm == 0.0 && !(flag&(0x01<<i))) {
//                 time_count[i] ++;
//             }
//             hDJI[i].f_current++;
//         }
//         if (time_count[0] > 3000 || time_count[1] > 3000 || time_count[2] > 3000 || time_count[3] > 3000) {
//             return HAL_ERROR;
//         }
//         CanTransmit_DJI_1234(&hcan1,hDJI[0].f_current,hDJI[1].f_current,hDJI[2].f_current,hDJI[3].f_current);
//         Delay_ms(2);
//     }
//     CanTransmit_DJI_1234(&hcan1,0,0,0,0);
//     return HAL_OK;
// }

#endif //USE_DEFAULT_MOTOR_PARAM

/**
 * @brief 底盘状态机(2023/12/06弃用)
 * @param this
 * @description
 * 1.如果底盘处于校准状态，检查是否校准完成，如果完成则进入准备状态
 * 2.如果底盘处于非校准状态，检查是否有速度指令，如果无速度指令则进入停止状态
 * 3.如果有速度指令，则计算目标速度，如果目标速度方向与当前方向偏差太大，则进入转向状态，否则进入运行状态
 *
void chassis_state_machine(swChassis_t *this)
{
    if(this->state==CHASSIS_CORRECTING)
    {
        for (uint8_t i = 0; i <this->swheel_num ; i++)
        {
            if(this->wheels[i].state!=STOP)
                return;
        }
        this->state = CHASSIS_READY;
        
    }
    else
    {
        if (this->target_v.vx==0&&this->target_v.vy==0&&this->target_v.vw==0)
        {
            for (uint8_t i = 0;i<this->swheel_num;i++)
                this->wheels[i].state = STOP;
            this->state = CHASSIS_STOP;
        }
        else {
        calculate_target_velocity(this);
        for (uint8_t i = 0; i <this->swheel_num ; i++)
        {
            if(this->wheels[i].state==AIMMING)
            {
                this->state=CHASSIS_AIMMING;
                return;
            }
        }
        this->state = CHASSIS_RUNNING;
        for (uint8_t i = 0; i <this->swheel_num ; i++)
            this->wheels[i].state = RUNNING;
        }

        
        
        // this->state = CHASSIS_RUNNING;
        // for (uint8_t i = 0; i <this->swheel_num ; i++)
        // {
        //     if(fabsf(this->wheels[i].target_direction-this->wheels[i].direction)>0.5f)
        //     {
        //         this->state = CHASSIS_AIMMING;
        //         break;
        //     }
        // }
        // if (this->state==CHASSIS_AIMMING)
        // {
        //     for (uint8_t i = 0; i <this->swheel_num ; i++)
        //         this->wheels[i].state = AIMMING;
        // }
        // else
        // {
        //     for (uint8_t i = 0; i <this->swheel_num ; i++)
        //         this->wheels[i].state = RUNNING;
        // }
    }
}
*/
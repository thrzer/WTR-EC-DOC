#ifndef STEERING_CHASSIS_H
#define STEERING_CHASSIS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "math.h"
#include "can.h"
#include "cmsis_os.h"
#include "DJI.h"
#include "Caculate.h"
#include "wtr_can.h"
#include "wtr_vesc.h"
#include "rmctl.h"
#include "HSM.h"

#define USE_3WHEELS //如果使用三轮，必须使用wheel 0-2，r2最终设计为三轮(非等边)
#define DISTENCE_WHEEL2CENTRE 0.22f //四轮舵轮轮子到底盘中心距离，r2设计呈正方形
#define DIAMETER 0.1f //行进轮直径
#define PI 3.14159265359f

/**
 * @file steering_chassis.h
 * @author thrzer
 * @brief 舵轮底盘(2006电机 + C610电调 + 光电门 、 5010电机 + AS5048A磁编码器 + VESC电调)
 * @date 2025-4-23
 * @note
 * 1. 建议使用CAN1、CAN2，使用时注意按照原理图(大疆A板或硬件自己设计的板)配置CAN的引脚
 * 2. vesctool使用最新版即可(附在soft目录下)，使用USB线连接后，架空(不能碰地)设置电机FOC参数：
 *    智能类型\200g\34_02\电池数6\电机齿67,轮齿110(这个是等效值)\电机极数14\无温度传感器
 *    电机自动运行一段时间后，会获得参数，特别要注意编码器是否为encoder，若为sensorless说明编码器接线有问题(线序错或受干扰)
 *    motor：编码器选AS5047(如果发现电机不能低速运动，可能是minerpm默认值太大); app：id任意\CAN波特率1M
 *    可以直接在右侧按钮下载已有的配置对比、使用方向键进行电机运动测试、查看实时数据
 *    这个教程可以参考：https://blog.csdn.net/2403_89422757/article/details/148547706
 * 3. 5010电机三条电源线任意连接，连接电调至CAN2和编码器时注意线序
 *    AS5048A按原理图连接，注意线序，最好套上屏蔽网(25年r2找硬件定制了分线板，编码器改焊了线序)
 *    光电门引出的三条线：红->5V, 黑/深褐->GND, 黄/白->信号线。注意探头不要用会一定程度透光的浅色打印件
 *    PS：刚开始接手学长调过的底盘时，CAN线是把3pin焊成一坨插到4pin口中，很乱
 * 4. makerx和makerbase电调可以不连编码器进行开环控制，也可以用闭环控制，但需要依照手册连编码器线
 *    如果低速运行时会剧烈抖动，说明可能在开环跑，非要开环跑的话需要使用前馈控制
 * 6. 每个轮子其实都设计了一个降温用的小风扇(电机转久了确实热)，5V和GND通电即用，不过25正赛没用上
 */

typedef struct {
    float vx;
    float vy;
    float vw;
    float vmax;//最大行进速度m/s，即遥控器摇杆推到最底时的行进速度
    float vwmax;//最大转向速度rad/s，即遥控器摇杆推到最底时的转向速度
} swChassis_velocity_t;//底盘整体速度

typedef struct {
    float v;//模长
    float angle;//辐角，极坐标轴朝正前，顺时针为正(双齿轮导致反向)
} swChassis_wheelInputVector_t;//极坐标下速度向量

typedef struct {
    GPIO_TypeDef* GPIOX;//对应引脚
    uint16_t GPIO_Pin;
    bool state;//是否触发
} swChassis_photogate_t;//光电门

typedef struct
{
    DJI_t *hdji;//2006
    VESC_t *hvesc;//5010及vesc电调
    swChassis_photogate_t photogate;//光电门
    float deviation_angle;//初始偏离角，校正时测定
    swChassis_wheelInputVector_t velocity_vector;//单个舵轮的(标准化)输入速度向量
    swChassis_wheelInputVector_t real_input_vector;//实际输入向量(未计入偏移角)
} swChassis_wheel_t;//单个舵轮

typedef struct
{
    CAN_HandleTypeDef* mhcanx;//行进电机的CAN(C610)
    CAN_HandleTypeDef* rhcanx;//转向电机的CAN(vesc)
    swChassis_wheel_t wheel[4];//四轮：右后0 左后1 左前2 右前3；三轮：正前0 左后1 右后2
    swChassis_velocity_t target_velocity;
} swChassis_t;

extern swChassis_t swChassis;

void swChassis_init(swChassis_t* this);
void swChassis_correct(swChassis_t* this);
void swChassis_rmctlInput(swChassis_t* this);
void swChassis_setVelocity(swChassis_t* this, float vx, float vy, float vw);
void swChassis_exexutor(swChassis_t* this);
void swChassis_EXTI_Callback(swChassis_t *this, uint16_t GPIO_Pin);

float inline signf(float num)
{
    return num > 0 ? 1 : num < 0 ? -1 : 0;
}

#ifdef __cplusplus
}
#endif

#endif
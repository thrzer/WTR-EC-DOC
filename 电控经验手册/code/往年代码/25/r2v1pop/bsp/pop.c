#include "pop.h"
#include "Caculate.h"
#include "cmsis_os.h"
#include "wtr_can.h"

float AngleCalculate(float target_position);
void SpeedReset(pop_t* this,float reset_speed);

pop_t pop;

/**
 * @brief 弹射机构初始化
 * @param this
 */
void PopInit(pop_t* this)
{
  //CAN滤波器初始化，可能在别的函数中已执行过
  CANFilterInit(&hcan1);
  //电机初始化
  this->motor = &hDJI[0];

  this->motor->motorType = M3508;
  // 速度环PID
  this->motor->speedPID.KP = 0.8;
  this->motor->speedPID.KI = 0.02;
  this->motor->speedPID.KD = 0.1;
  this->motor->speedPID.outputMax = 8000;
  // 位置环PID
  this->motor->posPID.KP = 80.0f;
  this->motor->posPID.KI=1.5f;
  this->motor->posPID.KD=0.0f;
  this->motor->posPID.outputMax = 5000;
  // hDJI[i].posPID.outputMin = 1500;
  this->motor->f_current = 0;
  this->motor->reductionRate = 3591.0f / 187.0f;
  this->motor->encoder_resolution = 8192.0f;

  HAL_GPIO_WritePin(POP_GPIOX, POP_GPIO_PIN, GPIO_PIN_RESET); //  电磁铁断电
  osDelay(500);
}

/**
 * @brief 弹射校正，检测电机堵转
 * @param this
 */
void PopCorrect(pop_t *this)
{
  osDelay(2000); //避免上电时冲激电流
  HAL_GPIO_WritePin(POP_GPIOX, POP_GPIO_PIN, GPIO_PIN_RESET); //电磁铁断电
  float reset_begin_position = this->motor->AxisData.AxisAngle_inDegree;

  SpeedReset(this,RESET_SPEED1);  //先快速复位
  int timecnt = 0;
  float reset_end_position[3];
  for (int i = 0;i < 3;++i) //复位3次平均值
  {
    timecnt = 0;
    while(timecnt < 80) //电机回弹一段时间(0.4s)
    {
      timecnt++;
      speedServo(RESET_SPEED2, this->motor);
      CanTransmit_DJI_1234(&hcan1, this->motor->speedPID.output, 0,0,0);//这句可能要换位置
      osDelay(5);
    }
    SpeedReset(this,RESET_SPEED3); //慢速复位
    reset_end_position[i] = this->motor->AxisData.AxisAngle_inDegree;
  }
  this->reset_position = (reset_end_position[0] + reset_end_position[1] + reset_end_position[2])/3
                         - reset_begin_position + BUFFER_POSITION;
  //实际位置输入：this->position + this->reset_position

  HAL_GPIO_WritePin(POP_GPIOX, POP_GPIO_PIN, GPIO_PIN_SET); // 电磁铁通电
  osDelay(500);
  this->motor->speedPID.integral = 0;
  this->motor->posPID.integral = 0;
  this->position = 0;
}

/**
 * @brief (内部函数)电机角度计算，函数为matlab拟合结果
 * @param this
 * @param target_position 目标位置
 * @return 电机位置(角度)
 */
float AngleCalculate(float target_distence)
{
    //0 to 500 touch,7500max,-9500min
    return target_distence;
}

/**
 * @brief 弹射释放
 * @param this
 */
void PopRelease(pop_t* this)
{
  // 电磁铁先断电，后通电
  HAL_GPIO_WritePin(POP_GPIOX, POP_GPIO_PIN, GPIO_PIN_RESET);
  osDelay(300);
  HAL_GPIO_WritePin(POP_GPIOX, POP_GPIO_PIN, GPIO_PIN_SET);
  // 回到起始点，重新吸附弹簧
  PopCharge(this,0);
}

/**
 * @brief 弹射蓄力，蓄力距离为0时复位
 * @param this
 * @param target_distence 目标距离
 */
void PopCharge(pop_t* this,float target_distence)
{
  // 电磁铁始终通电
  this->position = AngleCalculate(target_distence);
  HAL_GPIO_WritePin(POP_GPIOX, POP_GPIO_PIN, GPIO_PIN_SET); // 电磁铁通电
}

/**
 * @brief 弹射机构电机执行器，建议执行周期5ms
 * @param this
 */
void PopExecutor(pop_t* this)
{
  positionServo(this->position + this->reset_position, this->motor);
  CanTransmit_DJI_1234(&hcan1, this->motor->speedPID.output, 0,0,0);//这句可能要换位置
}

/**
 * @brief (内部函数)速度伺服复位
 * @param this
 * @param reset_speed 复位速度
 * @return 电机位置(角度)
 */
void SpeedReset(pop_t* this,float reset_speed)
{
  int overcnt = 0,breakflag = 1;
  while(breakflag)//检测电流
  {
    if(this->motor->FdbData.current < STALL_CURRENT)//避免摩擦毛刺
    {
      overcnt++;
      if(overcnt >= 3)
      {
        breakflag = 0;
      }
    }
    else
    {
      overcnt = 0;
    }
    speedServo(reset_speed, this->motor);
    CanTransmit_DJI_1234(&hcan1, this->motor->speedPID.output, 0,0,0);//这句可能要换位置
    osDelay(5);
  }
}

//按键控制，弃用
// static int key_sta,key_cnt,turn_cnt = 0;
// switch(key_sta)
// {
//   case 0:	//no key
//     if( 0 == HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) )
//     {
//       key_sta = 1;
//     }
//     break;
//   case 1: //key down wait release
//     if( 0 == HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) )
//     {
//       key_sta = 2;
//       key_cnt++;
//     }
//     else
//     {
//       key_sta = 0;
//     }
//     break;
//   case 2:
//     if( 0 != HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) )
//     {
//       key_sta = 0;
//     }
//     break;
// }
// if(key_cnt>=1)//xiaodou
// {
//   key_cnt = 0;
//   if (turn_cnt == 0)
//   {
//     PopCharge(&pop,angle);
//     turn_cnt = 1;
//   }
//   else if (turn_cnt == 1)
//   {
//     PopRelease(&pop);
//     turn_cnt = 0;
//   }
// }
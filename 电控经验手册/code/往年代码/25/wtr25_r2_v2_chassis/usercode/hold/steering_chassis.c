#include "steering_chassis.h"

swChassis_t swChassis;

swChassis_wheelInputVector_t VelocityVectorAddition(swChassis_wheelInputVector_t velocity_vector1, swChassis_wheelInputVector_t velocity_vector2);
void VectorSimplify(swChassis_wheelInputVector_t* vector);
bool swChassis_checkDirection(swChassis_t* this);
void swChassis_calculate(swChassis_t* this);
void swChassis_lock(swChassis_t* this);
void swChassis_outputVelocityCalculate(swChassis_t* this);
float swChassis_feedforwardCalculate(int wheel_id,float erpm,bool COMMOND);

/**
 * @brief 舵轮底盘初始化
 * @param this
 */
void swChassis_init(swChassis_t* this)
{
    CANFilterInit(&hcan1);
    // CAN2FilterInit(&hcan2);
    // this->mhcanx = &hcan2;
    this->rhcanx = &hcan1;

    swChassis_wheelInputVector_t empty_vector = {0.0f, 0.0f};

    for (int i = 0; i < 4; i++)
    {
        //2006初始化
        this->wheel[i].hdji = &hDJI[i];
        this->wheel[i].hdji->motorType = M2006;
		// PID
		this->wheel[i].hdji->speedPID.KP = 12.0f;
		this->wheel[i].hdji->speedPID.KI = 0.2f;
		this->wheel[i].hdji->speedPID.KD = 5.0f;
		this->wheel[i].hdji->speedPID.outputMax = 8000;
        this->wheel[i].hdji->posPID.KP = 120.0f;
        this->wheel[i].hdji->posPID.KI = 1.0f;
        this->wheel[i].hdji->posPID.KD = 5.0f;

		this->wheel[i].hdji->posPID.outputMax = 8000;
		this->wheel[i].hdji->f_current = 0;
        this->wheel[i].hdji->reductionRate = 36.0f*2.0; // 2006减速比为36,2.0为外加齿轮减速比 3508减速比约为3591.0f/187.0f = 19
        this->wheel[i].hdji->encoder_resolution = 8192.0f;

        this->wheel[i].deviation_angle = 0.0f;
        //VESC初始化
        this->wheel[i].hvesc = &hVESC[i];
        this->wheel[i].hvesc->hcann = this->mhcanx;
        this->wheel[i].hvesc->controller_id = 101+i;//与在上位机软件vesctool设置对应(图方便刚好设置成这样)

        this->wheel[i].real_input_vector = empty_vector;
        this->wheel[i].velocity_vector = empty_vector;

        this->wheel[i].photogate.state = false;
        this->wheel[i].photogate.GPIOX = GPIOA;
    }
    this->wheel[0].deviation_angle = -90.0f;//光电门装配位置不是正前方就需要补上这部分的角度，最好还是都装到正前
    this->wheel[1].deviation_angle = -90.0f;
    this->wheel[2].deviation_angle = -90.0f;
    this->wheel[3].deviation_angle = 0.0f;
    this->wheel[0].photogate.GPIO_Pin = GPIO_PIN_0;
    this->wheel[1].photogate.GPIO_Pin = GPIO_PIN_1;
    this->wheel[2].photogate.GPIO_Pin = GPIO_PIN_2;
    this->wheel[3].photogate.GPIO_Pin = GPIO_PIN_3;

    this->target_velocity.vx = 0;
    this->target_velocity.vy = 0;
    this->target_velocity.vw = 0;
    this->target_velocity.vmax = 0.75f;//最大速度
    this->target_velocity.vwmax = 0.6f;//最大转向速度
}

bool istriggered[4] = {0,0,0,0};//是否触发过
float deviation_angle_buff[4] = {0,0,0,0};//复位角度缓存
/**
 * @brief 舵轮底盘校正
 * @param this
 * @note 注意上电时光电门是否已经触发
 */
void swChassis_correct(swChassis_t* this)
{
    osDelay(3000);
    for (int i = 0; i < 4; i++)
    {
        if(this->wheel[i].photogate.state == true)//如果刚好在光电门内，就进行短时间的速度伺服移出
        {
            for (int k = 0; k < 4; k++)
            {
                this->wheel[k].hdji->speedPID.output = 0;//确保不会误触发其他电机
            }
            for(int j = 0; j < 150; j++)//150*5ms
            {
                speedServo(500,this->wheel[i].hdji);
                CanTransmit_DJI_1234(this->rhcanx, this->wheel[0].hdji->speedPID.output,
                    this->wheel[1].hdji->speedPID.output,
                    this->wheel[2].hdji->speedPID.output,
                    this->wheel[3].hdji->speedPID.output);
                osDelay(5);
            }
        }
    }
    //设定速度
    float v1 = 600;
    float v2 = 0;
    //清除标志
    for (int i = 0; i < 4; i++)
        istriggered[i] = false;
    //逆时针触发，如果需要提高精度可以再进行一次顺时针触发
    while(1)
    {
        for (int i = 0; i < 4; i++)
        {
            speedServo(v1,this->wheel[i].hdji);
            if(istriggered[i] == true)
                this->wheel[i].hdji->speedPID.output = 0;//确保不会误触发其他电机
        }
        CanTransmit_DJI_1234(this->rhcanx, this->wheel[0].hdji->speedPID.output,
                            this->wheel[1].hdji->speedPID.output,
                            this->wheel[2].hdji->speedPID.output,
                            this->wheel[3].hdji->speedPID.output);
        osDelay(5);
        if(istriggered[0] && istriggered[1] && istriggered[2])// && istriggered[3])
            break;
    }
    //使用+=是因为光电门的安装本身可能与正前方存在差值，这个差值会提前测好，在初始化时给定
    //误差正比于速度平方，据此加权计算
    for (int i = 0; i < 4; i++)
        this->wheel[i].deviation_angle += (v1*v1/(v1*v1+v2*v2))*deviation_angle_buff[i];
    // //重新清除标志
    // for (int i = 0; i < 4; i++)
    //     istriggered[i] = false;
    // //顺时针触发
    // while(0)//弃用
    // {
    //     for (int i = 0; i < 4; i++)
    //     {
    //         speedServo(v2,this->wheel[i].hdji);
    //         if(istriggered[i] == true)
    //             this->wheel[i].hdji->speedPID.output = 0;//确保不会误触发其他电机
    //     }
    //     CanTransmit_DJI_1234(this->rhcanx, this->wheel[0].hdji->speedPID.output,
    //                         this->wheel[1].hdji->speedPID.output,
    //                         this->wheel[2].hdji->speedPID.output,
    //                         this->wheel[3].hdji->speedPID.output);
    //     osDelay(5);
    //     if(istriggered[0] && istriggered[1] && istriggered[2] && istriggered[3])
    //         break;
    // }
    // for (int i = 0; i < 4; i++)
    //     this->wheel[i].deviation_angle += (v2*v2/(v1*v1+v2*v2))*deviation_angle_buff[i];
}

/**
 * @brief 舵轮底盘外部中断回调函数
 * @param this
 * @param GPIO_Pin
 */
void swChassis_EXTI_Callback(swChassis_t *this, uint16_t GPIO_Pin)
{
    for (int i = 0; i < 4; i++)
    {
        if(GPIO_Pin == this->wheel[i].photogate.GPIO_Pin)
        {
            this->wheel[i].photogate.state = true;
            if(istriggered[i] == false)
            {
                deviation_angle_buff[i] = this->wheel[i].hdji->AxisData.AxisAngle_inDegree;
                istriggered[i] = true;//保证只会触发一次
            }
        }
        else
            this->wheel[i].photogate.state = false;
    }
}

unsigned int speedmode = 0;//速度挡位
unsigned int lockmode = 0;//锁定挡位
/**
 * @brief 输入遥控器给出的速度
 * @param this
 */
void swChassis_rmctlInput(swChassis_t* this)
{
    static bool flag1 = false, flag2 = false;
    if(rmctl.rmctl_msg.btn_RightCrossMid == true && flag1 == false)//上升沿
    {
        flag1 = true;
        speedmode++;
        if(speedmode >= 2)
            speedmode = 0;
        if(speedmode == 0)
        {
            this->target_velocity.vmax = 0.4;
            this->target_velocity.vwmax = 0.5;
        }
        else if(speedmode == 1)
        {
            this->target_velocity.vmax = 1.1;
            this->target_velocity.vwmax = 0.7;
        }
    }
    else if(rmctl.rmctl_msg.btn_RightCrossMid == false && flag1 == true)//下降沿
    {
        flag1 = false;
    }
    if(rmctl.rmctl_msg.btn_RightCrossLeft == true && flag2 == false)//上升沿
    {
        flag2 = true;
        lockmode++;
        if(lockmode >= 2)
            lockmode = 0;
        if(lockmode == 0)
        {
            wtrV2.chassis_mechanism = HSM_cSTATE_STOP;
        }
        else if(lockmode == 1)
        {
            wtrV2.chassis_mechanism = HSM_cSTATE_LOCK;
        }
    }
    else if(rmctl.rmctl_msg.btn_RightCrossLeft == false && flag2 == true)//下降沿
    {
        flag2 = false;
    }
    //将摇杆输入坐标缩放到半径为v的圆中
    float v = sqrt((float)(rmctl.rmctl_msg.usr_left_x*rmctl.rmctl_msg.usr_left_x + rmctl.rmctl_msg.usr_left_y*rmctl.rmctl_msg.usr_left_y))
                / JOYSTICKMAX * this->target_velocity.vmax;
    this->target_velocity.vw = -rmctl.rmctl_msg.usr_right_x / JOYSTICKMAX * this->target_velocity.vwmax;//使摇杆方向与旋转方向一致
    if(fabs(this->target_velocity.vw) < 0.01)
        this->target_velocity.vw = 0;
    if(v >= this->target_velocity.vmax)
        v = this->target_velocity.vmax;
    if(v > 0.01)
    {
        this->target_velocity.vx = v * (float)rmctl.rmctl_msg.usr_left_x
                                /sqrt((float)(rmctl.rmctl_msg.usr_left_x*rmctl.rmctl_msg.usr_left_x + rmctl.rmctl_msg.usr_left_y*rmctl.rmctl_msg.usr_left_y));
        this->target_velocity.vy = v * (float)rmctl.rmctl_msg.usr_left_y
                                /sqrt((float)(rmctl.rmctl_msg.usr_left_x*rmctl.rmctl_msg.usr_left_x + rmctl.rmctl_msg.usr_left_y*rmctl.rmctl_msg.usr_left_y));
    }
    else
    {
        this->target_velocity.vx = 0.0f;
        this->target_velocity.vy = 0.0f;
    }
}

/**
 * @brief 上位机输入速度
 * @param this
 */
void swChassis_MCInput(swChassis_t* this)
{
    ;
}

/**
 * @brief 直接输入速度
 * @param this
 */
void swChassis_setVelocity(swChassis_t* this, float vx, float vy, float vw)
{
    this->target_velocity.vx = vx;
    this->target_velocity.vy = vy;
    this->target_velocity.vw = vw;
}

/**
 * @brief (内部函数)开环方案前馈计算，提供一个指数衰减脉冲
 * @param wheel_id 轮子编号
 * @param erpm 目标速度
 * @param COMMOND 命令输入，0表示停止状态，1表示前进状态
 */
float swChassis_feedforwardCalculate(int wheel_id,float erpm,bool COMMOND)
{
    const float A = 60.0f * expf(-fabsf(erpm)/1000.0f) + 0.2f;//脉冲幅度(非线性)
    //考虑erpm主要在500-5000，用这个函数控制低速时前馈大，高速时前馈小，且保持在0.2-2.7之间
    static const float T = 0.30f;//脉冲持续时间s
    static const float T2 = 0.006f;//脉冲持续时间s
    static const float P = 0.3f;//衰减倍率
    static const float t = 0.005f;//执行器周期，默认5ms
    static long int cnt[4] = {0};//防溢出
    static float last_erpm = 0;
    // assert(wheel_id <= sizeof(cnt)/sizeof(long int));
    float ret = 0;
    if(COMMOND == 0 || last_erpm*erpm <= 0.0f)//符号改变时也需要重新进行前馈
    {
        cnt[wheel_id] = 0;
        ret = 0;
    }
    else if(COMMOND == 1)
    {
        cnt[wheel_id]++;
        if(cnt[wheel_id] <= (long int)((T2)/t))
        {
            ret = A*erpm*(cnt[wheel_id]/(long int)((T2)/t));
        }
        else if(cnt[wheel_id] <= (long int)((T+T2)/t))
        {
            // int cnt0 = cnt[wheel_id]*2 <= (long int)(T/t) ? cnt[wheel_id]*2 : (long int)(T/t);
            ret = A*erpm*(float)exp(-cnt[wheel_id]*P*t);
            //如果速度跳变时响应较慢，还需要考虑差分前馈
        }
        else
        {
            ret = 0;
        }
    }
    last_erpm = erpm;
    return ret; 
}

/**
 * @brief 舵轮底盘执行器，执行周期5ms
 * @param this
 * @note 无速度输入时伺服静止，有速度输入时先使转向电机到达给定角度附近，随后正常运动
 */
void swChassis_exexutor(swChassis_t* this)
{
    static const int N = 3;
    swChassis_calculate(this);
    if(wtrV2.chassis_mechanism == HSM_cSTATE_LOCK)
    {
        swChassis_lock(this);
        for(int i = 0; i < N; i++)
        {
            VESC_CAN_SET_BRAKE_CURRENT(this->wheel[i].hvesc,-100);//vesc静止，静止电流-
            positionServo(this->wheel[i].real_input_vector.angle + this->wheel[i].deviation_angle,this->wheel[i].hdji);
        }
    }
    else if(fabsf(this->target_velocity.vx)+fabsf(this->target_velocity.vy)+fabsf(this->target_velocity.vw) <= 0.01f)
    {
        wtrV2.chassis_mechanism = HSM_cSTATE_STOP;
        for(int i = 0; i < N; i++)
        {
            swChassis_feedforwardCalculate(i,0,0);//清零计数器
            VESC_CAN_SET_BRAKE_CURRENT(this->wheel[i].hvesc,-100);//vesc静止，静止电流-20
            positionServo(this->wheel[i].real_input_vector.angle + this->wheel[i].deviation_angle,this->wheel[i].hdji);
        }
    }
    else
    {
        if(swChassis_checkDirection(this) == false)//角度检查
        {
            wtrV2.chassis_mechanism = HSM_cSTATE_AIMMING;
            for(int i = 0; i < N; i++)
            {
                //可以考虑给一个起步速度加快响应
                VESC_CAN_SET_ERPM(this->wheel[i].hvesc,0);
                positionServo(this->wheel[i].real_input_vector.angle + this->wheel[i].deviation_angle,this->wheel[i].hdji);
            }
        }
        else
        {
            wtrV2.chassis_mechanism = HSM_cSTATE_RUNNING;
            for(int i = 0; i < N; i++)
            {
                float erpm = 14.0f*60.0f*this->wheel[i].real_input_vector.v/DIAMETER/PI*(float)(1.0+19.0/67.0+3.0*24.0/67.0);
                //转速 = 电机极数 * 转轴转速
                //减速比 = 1 + (Ns / Nr) + (n * Np / Nr)，其中Ns为太阳轮齿数，Nr为环形齿轮齿数，Np为行星轮齿数，n为行星轮个数
                erpm += swChassis_feedforwardCalculate(i,erpm,1);//使用前馈
                VESC_CAN_SET_ERPM(this->wheel[i].hvesc,erpm);
                positionServo(this->wheel[i].real_input_vector.angle + this->wheel[i].deviation_angle,this->wheel[i].hdji);
            }
        }
    }
    CanTransmit_DJI_1234(this->rhcanx, this->wheel[0].hdji->speedPID.output,
                        this->wheel[1].hdji->speedPID.output,
                        this->wheel[2].hdji->speedPID.output,
                        0);
}

/**
 * @brief (内部函数)舵轮底盘检测方向是否到达给定角度附近
 * @param this
 */
bool swChassis_checkDirection(swChassis_t* this)
{
    bool result = true;
    for(int i = 0; i < 3; i++)
    {
        if(fabsf(this->wheel[i].real_input_vector.angle + this->wheel[i].deviation_angle - this->wheel[i].hdji->AxisData.AxisAngle_inDegree) > 30.0f)
        {
            result = false;
        }
    }
    return result;
}

/**
 * @brief (内部函数)舵轮底盘速度解算
 * @param this
 * @note 方法是叠加平移和自旋的向量，24年舵轮底盘的解算方法是计算曲率圆心，两者结果相同
 */
void swChassis_calculate(swChassis_t* this)
{
    //无输入时只设置速度为0，方向保持不变
    if(fabs(this->target_velocity.vx)+fabs(this->target_velocity.vy)+fabs(this->target_velocity.vw) <= 0.01f)
    {
        for(int i = 0; i < 4; i++)
        {
            this->wheel[i].velocity_vector.v = 0;
        }
    }
    else
    {
        //计算左右平移的速度向量，极坐标轴朝正前
        swChassis_wheelInputVector_t velocity_vector_x;
        velocity_vector_x.v = this->target_velocity.vx;
        velocity_vector_x.angle = 90.0f;
        //计算前后平移的速度向量
        swChassis_wheelInputVector_t velocity_vector_y;
        velocity_vector_y.v = this->target_velocity.vy;
        velocity_vector_y.angle = 0.0f;
        //计算自旋的速度向量
        #ifdef USE_4WHEELS//四轮：右后0 左后1 左前2 右前3
        swChassis_wheelInputVector_t velocity_vector_w[4];
        for(int i = 0; i < 4; i++)
        {
            velocity_vector_w[i].v = this->target_velocity.vw * DISTENCE_WHEEL2CENTRE;
            velocity_vector_w[i].angle = 45.0f + 90.0f*i;
        }
        #endif
        #ifdef USE_3WHEELS//三轮：正前0 左后1 右后2
        swChassis_wheelInputVector_t velocity_vector_w[4];
        velocity_vector_w[0].v = this->target_velocity.vw * (DISTENCE_WHEEL2CENTRE/(float)sqrt(2.0));
        velocity_vector_w[0].angle = -90.0f;
        velocity_vector_w[1].v = this->target_velocity.vw * DISTENCE_WHEEL2CENTRE;
        velocity_vector_w[1].angle = 135.0f;
        velocity_vector_w[2].v = this->target_velocity.vw * DISTENCE_WHEEL2CENTRE;
        velocity_vector_w[2].angle = 45.0f;
        velocity_vector_w[3].v = 0;
        velocity_vector_w[3].angle = 0;
        #endif
        //叠加向量
        for(int i = 0; i < 4; i++)
        {
            this->wheel[i].velocity_vector = VelocityVectorAddition(VelocityVectorAddition(velocity_vector_x,velocity_vector_y),velocity_vector_w[i]);
        }
    }
    swChassis_outputVelocityCalculate(this);
}

/**
 * @brief (内部函数)舵轮底盘锁定，供测试使用
 * @param this
 * @note 其实就是所有轮子指向中心并给静止电流
 */
void swChassis_lock(swChassis_t* this)
{
    swChassis_wheelInputVector_t velocity_vector[4];
    velocity_vector[0].v = 0;
    velocity_vector[0].angle = 0.0f;
    velocity_vector[1].v = 0;
    velocity_vector[1].angle = 45.0f;
    velocity_vector[2].v = 0;
    velocity_vector[2].angle = -45.0f;
    velocity_vector[3].v = 0;
    velocity_vector[3].angle = 0;
    //叠加向量
    for(int i = 0; i < 3; i++)
    {
        this->wheel[i].velocity_vector = velocity_vector[i];
    }
    swChassis_outputVelocityCalculate(this);
}

/**
 * @brief (内部函数)舵轮底盘输出速度计算(逆标准化)
 * @param this
 * @note 通过合适的计算，每次更新舵轮速度时，转向电机的角度变化方向一定存在不超过90°的情况，以此可以提高响应速度
 */
void swChassis_outputVelocityCalculate(swChassis_t* this)
{
    for(int i = 0; i < 4; i++)
    {
        //计算实际速度向量的角度
        swChassis_wheelInputVector_t last_vector;
        last_vector.v = this->wheel[i].velocity_vector.v;//这个值其实用不到
        last_vector.angle = this->wheel[i].hdji->AxisData.AxisAngle_inDegree - this->wheel[i].deviation_angle;
        //这样写避免对输入向量进行更改
        swChassis_wheelInputVector_t input_vector = this->wheel[i].velocity_vector;
        //判断合适的方向
        float min_angle = 91.0f;//最小变化角，必须大于90°
        for(int j = 0;;j++)
        {
            if(fabs(last_vector.angle - (input_vector.angle + 360.0f*j)) < min_angle)//同向1
            {
                this->wheel[i].real_input_vector.v = input_vector.v;
                this->wheel[i].real_input_vector.angle = input_vector.angle + 360.0f*j;
                break;
            }
            else if(fabs(last_vector.angle - (input_vector.angle - 360.0f*j)) < min_angle)//同向2
            {
                this->wheel[i].real_input_vector.v = input_vector.v;
                this->wheel[i].real_input_vector.angle = input_vector.angle - 360.0f*j;
                break;
            }
            else if(fabs(last_vector.angle - (input_vector.angle + 360.0f*j - 180.0f)) < min_angle)//反向1
            {
                this->wheel[i].real_input_vector.v = -input_vector.v;
                this->wheel[i].real_input_vector.angle = input_vector.angle + 360.0f*j - 180.0f;
                break;
            }
            else if(fabs(last_vector.angle - (input_vector.angle - 360.0f*j + 180.0f)) < min_angle)//反向2
            {
                this->wheel[i].real_input_vector.v = -input_vector.v;
                this->wheel[i].real_input_vector.angle = input_vector.angle - 360.0f*j + 180.0f;
                break;
            }
        }
    }
}

/**
 * @brief (内部函数)舵轮速度向量加法，输出标准化的向量
 * @param velocity_vector 任意两个速度向量
 */
swChassis_wheelInputVector_t VelocityVectorAddition(swChassis_wheelInputVector_t velocity_vector1, swChassis_wheelInputVector_t velocity_vector2)
{
    swChassis_wheelInputVector_t velocity_vector_result;
    //先将两向量的模长化为正值，辐角限定在-180°~+180°之间
    VectorSimplify(&velocity_vector1);
    VectorSimplify(&velocity_vector2);
    float x = velocity_vector1.v*(float)cos(velocity_vector1.angle*PI/180.0f) + velocity_vector2.v*(float)cos(velocity_vector2.angle*PI/180.0f);
    float y = velocity_vector1.v*(float)sin(velocity_vector1.angle*PI/180.0f) + velocity_vector2.v*(float)sin(velocity_vector2.angle*PI/180.0f);
    velocity_vector_result.v = (float)sqrt(x*x + y*y);
    //虽然由于浮点数计算不会出现x=0，但此处仍做处理
    if(x==0)
        x = 0.0001f;
    if(x>=0 && y>=0)//注意atan2函数输出为double类型
        velocity_vector_result.angle = (float)atan2(y, x)*180.0f/PI;
    else if(x<0 && y>=0)
        velocity_vector_result.angle = 180.0f - (float)atan2(y, -x)*180.0f/PI;
    else if(x<0 && y<0)
        velocity_vector_result.angle = -180.0f + (float)atan2(-y, -x)*180.0f/PI;
    else
        velocity_vector_result.angle = -(float)atan2(-y, x)*180.0f/PI;
    return velocity_vector_result;
}

/**
 * @brief (内部函数)速度向量标准化，即模长为正，辐角在-180°~+180°之间
 * @param vector 任意速度向量
 */
void VectorSimplify(swChassis_wheelInputVector_t* vector)
{
    if(vector->v < 0)
    {
        vector->v = -vector->v;
        vector->angle += 180.0f;
    }
    while(vector->angle > 180.0f)
        vector->angle -= 360.0f;
    while(vector->angle < -180.0f)
        vector->angle += 360.0f;
}
#include "window_filter.h"
#include "rclcpp/rclcpp.hpp"
#include <chrono>

extern KalmanFilter2D_t kf2dx,kf2dy;
extern int count;
/**
 * @brief 卡尔曼滤波器重置
 * @note 调整参数只涉及QRPX，Q越大越灵敏，R越大越稳定，P和X是初值，用于减少起步误差
 */
void KalmanFilter2D_reset(KalmanFilter2D_t* kf, float dt)
{
    // 状态转移矩阵 [1 dt; 0 1]（匀速模型）
    kf->F[0][0] = 1.0f; kf->F[0][1] = dt;
    kf->F[1][0] = 0.0f; kf->F[1][1] = 1.0f;
    // 观测矩阵 H = [1 0]（只能观测位置）
    kf->H[0] = 1.0f; kf->H[1] = 0.0f;
    // 过程噪声协方差（假设位置和速度噪声独立）
    kf->Q[0][0] = 0.1f; kf->Q[0][1] = 0.0f;
    kf->Q[1][0] = 0.00f;  kf->Q[1][1] = 0.425f;
    // 观测噪声
    kf->R = 0.59f;
    // 初始状态和协方差
    kf->X[0] = 0.0f; kf->X[1] = 0.0f;  // [位置=0, 速度=0]
    kf->P[0][0] = 0.0001f; kf->P[0][1] = 0.0001f;
    kf->P[1][0] = 0.0f; kf->P[1][1] = 19.0f;
}

/**
 * @brief 卡尔曼滤波器计算
 * @param measurement 测量值
 * @return 最优估计值
 */
float KalmanFilter2D_calculate(KalmanFilter2D_t* kf, float measurement)
{
    // 状态预测: X = F * X
    float x_pred = kf->F[0][0] * kf->X[0] + kf->F[0][1] * kf->X[1];
    float v_pred = kf->F[1][0] * kf->X[0] + kf->F[1][1] * kf->X[1];
    kf->X[0] = x_pred;
    kf->X[1] = v_pred;
    // 协方差预测: P = F * P * F^T + Q
    float P00 = kf->F[0][0] * kf->P[0][0] + kf->F[0][1] * kf->P[1][0];
    float P01 = kf->F[0][0] * kf->P[0][1] + kf->F[0][1] * kf->P[1][1];
    float P10 = kf->F[1][0] * kf->P[0][0] + kf->F[1][1] * kf->P[1][0];
    float P11 = kf->F[1][0] * kf->P[0][1] + kf->F[1][1] * kf->P[1][1];
    kf->P[0][0] = P00 * kf->F[0][0] + P01 * kf->F[0][1] + kf->Q[0][0];
    kf->P[0][1] = P00 * kf->F[1][0] + P01 * kf->F[1][1] + kf->Q[0][1];
    kf->P[1][0] = P10 * kf->F[0][0] + P11 * kf->F[0][1] + kf->Q[1][0];
    kf->P[1][1] = P10 * kf->F[1][0] + P11 * kf->F[1][1] + kf->Q[1][1];
    // 计算卡尔曼增益: K = P * H^T / (H * P * H^T + R)
    float S = kf->H[0] * kf->P[0][0] * kf->H[0] +
        kf->H[0] * kf->P[0][1] * kf->H[1] +
        kf->H[1] * kf->P[1][0] * kf->H[0] +
        kf->H[1] * kf->P[1][1] * kf->H[1] +
        kf->R;
    kf->K[0] = (kf->P[0][0] * kf->H[0] + kf->P[0][1] * kf->H[1]) / S;
    kf->K[1] = (kf->P[1][0] * kf->H[0] + kf->P[1][1] * kf->H[1]) / S;
    // 状态更新: X = X + K * (measurement - H * X)
    float y = measurement - (kf->H[0] * kf->X[0] + kf->H[1] * kf->X[1]);
    kf->X[0] += kf->K[0] * y;
    kf->X[1] += kf->K[1] * y;
    // 协方差更新: P = (I - K * H) * P
    float P00_new = kf->P[0][0] - kf->K[0] * kf->H[0] * kf->P[0][0] - kf->K[0] * kf->H[1] * kf->P[1][0];
    float P01_new = kf->P[0][1] - kf->K[0] * kf->H[0] * kf->P[0][1] - kf->K[0] * kf->H[1] * kf->P[1][1];
    float P10_new = kf->P[1][0] - kf->K[1] * kf->H[0] * kf->P[0][0] - kf->K[1] * kf->H[1] * kf->P[1][0];
    float P11_new = kf->P[1][1] - kf->K[1] * kf->H[0] * kf->P[0][1] - kf->K[1] * kf->H[1] * kf->P[1][1];
    kf->P[0][0] = P00_new; kf->P[0][1] = P01_new;
    kf->P[1][0] = P10_new; kf->P[1][1] = P11_new;
    return kf->X[0]; // 返回估计的位置
}

float windowfilter_x(float data,const int n)
{
    static volatile float databuffer[100]={data};

    for(int i=0;i<n-1;i++)
    {
        databuffer[i]=databuffer[i+1];
    }
    databuffer[n-1]=data;
    float ret=0;
    for(int i=0;i<n;i++)
    {
        ret+=databuffer[i];
    }
    return ret/(n*1.0f);
}

float windowfilter_y(float data,const int n)
{
    static volatile float databuffer[100]={data};
    for(int i=0;i<n-1;i++)
    {
        databuffer[i]=databuffer[i+1];
    }
    databuffer[n-1]=data;
    float ret=0;
    for(int i=0;i<n;i++)
    {
        ret+=databuffer[i];
    }
    return ret/(n*1.0f);
}

float windowfilter_testx(float data,const int n,bool* flag_x)
{
    static volatile float databuffer[120]={data};

    while(*flag_x)
    {
        for(int i=0;i<n;i++)
        {
            databuffer[i]=data;
        }
        *flag_x=false;
    }

    for(int i=0;i<n-1;i++)
    {
        databuffer[i]=databuffer[i+1];
    }
    databuffer[n-1]=data;
    float ret=0;
    for(int i=0;i<n;i++)
    {
        ret+=databuffer[i];
    }
    return ret/(n*1.0f);
}

float windowfilter_testy(float data,const int n,bool* flag_y)
{
    static volatile float databuffer[120]={data};

    while(*flag_y)
    {
        for(int i=0;i<n;i++)
        {
            databuffer[i]=data;
        }
        *flag_y=false;
        // count++;
        // std::cout<<"count is "<<count<<std::endl;
    }    

    for(int i=0;i<n-1;i++)
    {
        databuffer[i]=databuffer[i+1];
    }
    databuffer[n-1]=data;
    float ret=0;
    for(int i=0;i<n;i++)
    {
        ret+=databuffer[i];
    }
    return ret/(n*1.0f);
}


#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file kalman_filter.h
 * @author thrzer
 * @brief 卡尔曼滤波器
 * @date 2025-7-14
 * @note 适用于低幅噪声数据。卡尔曼滤波器特点是低延迟，但使用时间较长时可能会发散，需要重启。
 *       如果数据噪声较大，可以使用ECKF(嵌入式容积卡尔曼滤波器)。
 */


typedef struct
{
    float Q;            //预测过程协方差
    float R;            //观测过程协方差
    float P;            //初始状态协方差
    float X;            //初始状态
    float X_Predict;	//预测值
	float P_Predict;	//预测值协方差
	float K;			//卡尔曼增益
} KalmanFilter_t;  //滤波器参数

void KalmanFilter_reset(KalmanFilter_t* kf);
float KalmanFilter_calculte(KalmanFilter_t* kf, float data);

#ifdef __cplusplus
}
#endif

#endif
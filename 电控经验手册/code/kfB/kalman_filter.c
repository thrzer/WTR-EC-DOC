#include "kalman_filter.h"
#include <math.h>

KalmanFilter_t kf;

/**
 * @brief 卡尔曼滤波器重置
 * @note 调整参数只涉及QRPX，Q越大越灵敏，R越大越稳定，P和X是初值，用于减少起步误差
 */
void KalmanFilter_reset(KalmanFilter_t* kf)
{
	kf->Q = 0.5f;
	kf->R = 5.0f;
	kf->P = 0.5f;
	kf->X = -8.0f;
    kf->X_Predict = 0.0f;
	kf->P_Predict = 0.0f;
	kf->K = 0.0f;
}

/**
 * @brief 卡尔曼滤波器计算
 * @param data 输入数据
 * @return 最优估计值
 */
float KalmanFilter_calculte(KalmanFilter_t* kf, float data)
{
	//预测过程
	kf->X_Predict = kf->X;
	kf->P_Predict = kf->P + kf->Q;
	//更新过程
	kf->K = kf->P_Predict / (kf->P_Predict + kf->R);
	kf->X = kf->X_Predict + kf->K * (data - kf->X_Predict);
	kf->P = (1 - kf->K) * kf->P_Predict;
    //返回最优估计值
	return kf->X;
}
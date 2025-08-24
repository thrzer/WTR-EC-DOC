#ifndef KALMAN_2DFILTER_H
#define KALMAN_2DFILTER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file kalman_2Dfilter.h
 * @author thrzer
 * @brief 二阶卡尔曼滤波器
 * @date 2025-7-30
 * @note 适用于高频噪声数据。
 *       如果数据噪声较大，可以使用ECKF(嵌入式容积卡尔曼滤波器)。
 */

typedef struct {
    float Q[2][2];      // 过程噪声协方差矩阵（2x2）
    float R;            // 观测噪声标量
    float P[2][2];      // 状态协方差矩阵（2x2）
    float X[2];         // 状态向量：[位置, 速度]
    float F[2][2];      // 状态转移矩阵
    float H[2];         // 观测矩阵（将状态映射到观测）
    float K[2];         // 卡尔曼增益（向量）
} KalmanFilter2D_t;

void KalmanFilter2D_reset(KalmanFilter2D_t* kf, float dt);
float KalmanFilter2D_calculate(KalmanFilter2D_t* kf, float measurement);

extern KalmanFilter2D_t kf2dx,kf2dy;

#ifdef __cplusplus
}
#endif

#endif
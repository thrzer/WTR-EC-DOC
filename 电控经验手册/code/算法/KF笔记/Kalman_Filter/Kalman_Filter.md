# 一.递归算法
![alt text](image.png)
# 二.数学基础
## 数据融合
![alt text](image-1.png)
![alt text](image-2.png)
![alt text](image-3.png)

## 协方差矩阵
![alt text](image-4.png)
![alt text](image-5.png)
- **对于协方差矩阵，主对角线上的数据代表那个变量的方差，对于协方差的数据，大于零表示对应的两个变量正相关，且越大相关性越强（相关系数的分子）**
  
## 状态空间方程
![alt text](image-6.png)
![alt text](image-7.png)
- **其中Xk可以看成计算（估计）结果，Zk可以看成测量结果，重点是二者的Data Fusion**

# 三.Kalman **Gain数学推导**
**补充：先验是通过已有的知识建立数学模型，进而推出本时刻的状态，该状态就叫先验估计，由于模型的不精确，再使用测量得到的状态值（也不精确）做修正，进而只能得到状态的估计值（也即后验）**
- Zk于Xk之间是有着H的矩阵变化的，就像是电流x电阻=电压，所以此处的H可以理解为状态变换矩阵
![alt text](a467e7c45e4db795931fcd811e65b7a-1.png)
![alt text](7a639de1b46efc55857061496e198e6.png)
![alt text](4dc18bf77ba33170603a9b87e27b21f.png)

# 四 误差协方差矩阵数学推导_卡尔曼滤波器的五个公式
![alt text](image-8.png)
![alt text](image-9.png)
![alt text](image-10.png)
**预测完成之后更新**

# 调节超参数（Q，R）
![alt text](image-11.png)
- 噪声可以理解为你对这个信号的不信任程度，来源于自己的假设和现实的结合，在假设的时候并不是不可以改变的
- 如果运动过程中环境方面的不确定干扰小（运动模型精度高），则过程噪声协方差矩阵Q可以给小，若测量设备精度高，则测量噪声协方差矩阵R可以给小
![alt text](3ff70eb37131afe4d8c90c8709f65b0.png)
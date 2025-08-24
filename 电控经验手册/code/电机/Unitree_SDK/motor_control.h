#ifndef __MOTOR_CONTORL_H
#define __MOTOR_CONTORL_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "main.h" // stm32 hal
#include "ris_protocol.h"// 包含了自定义的电机协议相关结构体，如RIS_Mode_t, RIS_Fbk_t等
#include "usart.h"


#pragma pack(1)                  //���ṹ���ڱ���ǿ��1�ֽڶ���// 强制1字节对齐，确保内存布局紧凑


/**
 * ����������ݰ�
 */
typedef struct
{
    uint8_t head[2];    // ��ͷ         2Byte// 包头，2字节
    RIS_Mode_t mode;    // �������ģʽ  1Byte// 电机模式，1字节
    RIS_Fbk_t   fbk;    // ����������� 11Byte// 电机反馈数据，11字节
    uint16_t  CRC16;    // CRC          2Byte// CRC校验，2字节
} MotorData_t;  //��������

/**
 * �������������ݰ�
 */
typedef struct
{
    // ���� ��������������ݰ�
    uint8_t head[2];    // ��ͷ         2Byte// 包头，2字节
    RIS_Mode_t mode;    // �������ģʽ  1Byte// 电机模式，1字节
    RIS_Comd_t comd;    // ����������� 12Byte// 控制命令数据，12字节
    uint16_t   CRC16;   // CRC          2Byte// CRC校验，2字节
} ControlData_t;     //��������������ݰ�

#pragma pack()


/**
 * �������ݹ����
 */
typedef struct
{
    // ���� ���͸�ʽ������
    ControlData_t motor_send_data;   //����������ݽṹ��// 控制命令数据结构体
    int hex_len;                        //���͵�16�����������鳤��, 34// 发送的数据长度，通常是34字节
    long long send_time;                //���͸������ʱ��, ΢��(us)// 发送时间，单位为微秒（us）
    // �����͵ĸ�������
    unsigned short id;                  //���ID��0����ȫ�����// 电机ID，标识电机（0表示广播）
    unsigned short mode;                // 0:����, 5:����ת��, 10:�ջ�FOC����// 工作模式（如停止、FOC运行等）
    //ʵ�ʸ�FOC��ָ������Ϊ��
    // K_P*delta_Pos + K_W*delta_W + T
    float T;                            //�����ؽڵ�������أ������������أ���Nm�� // 扭矩，单位：Nm
    float W;                            //�����ؽ��ٶȣ����������ٶȣ�(rad/s)// 速度，单位：rad/s
    float Pos;                          //�����ؽ�λ�ã�rad��// 位置，单位：rad
    float K_P;                          //�ؽڸն�ϵ�� // 刚度系数/位置控制比例
    float K_W;                          //�ؽ��ٶ�ϵ�� // 阻尼系数/速度控制比例
} MOTOR_send;

/**
 * �������ݹ����
 */
typedef struct
{
    // ���� ��������
    MotorData_t motor_recv_data;    //����������ݽṹ�壬���motor_msg.h// 电机反馈数据结构体
    int hex_len;                        //���յ�16�����������鳤��, 78 // 接收数据长度，通常为78字节
    long long resv_time;                //���ո������ʱ��, ΢��(us)// 接收时间，单位为微秒（us）
    int correct;                        //���������Ƿ�������1������0��������// 数据校验结果，1表示成功，0表示失败
    //����ó��ĵ������
    unsigned char motor_id;             //���ID// 电机ID
    unsigned char mode;                 // 0:����, 5:����ת��, 10:�ջ�FOC����// 电机工作模式
    int Temp;                           //�¶�// 电机温度
    unsigned char MError;               //������// 电机错误码
    float T;                            // ��ǰʵ�ʵ���������// 当前实际扭矩
		float W;														// speed// 当前实际速度
    float Pos;                          // ��ǰ���λ�ã�����0������������ؽڻ����Ա�����0��Ϊ׼�� // 当前实际位置
		float footForce;												// �����ѹ���������� 12bit (0-4095)// 传感器力反馈，单位：12位

} MOTOR_recv;

uint32_t crc32_core(uint32_t *ptr, uint32_t len);
int modify_data(MOTOR_send *motor_s);
int extract_data(MOTOR_recv *motor_r);
HAL_StatusTypeDef SERVO_Send_recv(UART_HandleTypeDef *usartx,MOTOR_send *pData, MOTOR_recv *rData);
#ifdef __cplusplus
}
#endif
#endif

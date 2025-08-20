#ifndef __MOTOR_CONTORL_H
#define __MOTOR_CONTORL_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "main.h" // stm32 hal
#include "ris_protocol.h"
#include "usart.h"


#pragma pack(1)                  //���ṹ���ڱ���ǿ��1�ֽڶ���

/**
 * ����������ݰ�
 */
typedef struct
{
    uint8_t head[2];    // ��ͷ         2Byte
    RIS_Mode_t mode;    // �������ģʽ  1Byte
    RIS_Fbk_t   fbk;    // ����������� 11Byte
    uint16_t  CRC16;    // CRC          2Byte
} MotorData_t;  //��������

/**
 * �������������ݰ�
 */
typedef struct
{
    // ���� ��������������ݰ�
    uint8_t head[2];    // ��ͷ         2Byte
    RIS_Mode_t mode;    // �������ģʽ  1Byte
    RIS_Comd_t comd;    // ����������� 12Byte
    uint16_t   CRC16;   // CRC          2Byte
} ControlData_t;     //��������������ݰ�

#pragma pack()


/**
 * �������ݹ����
 */
typedef struct
{
    // ���� ���͸�ʽ������
    ControlData_t motor_send_data;   //����������ݽṹ��
    int hex_len;                        //���͵�16�����������鳤��, 34
    long long send_time;                //���͸������ʱ��, ΢��(us)
    // �����͵ĸ�������
    unsigned short id;                  //���ID��0����ȫ�����
    unsigned short mode;                // 0:����, 5:����ת��, 10:�ջ�FOC����
    //ʵ�ʸ�FOC��ָ������Ϊ��
    // K_P*delta_Pos + K_W*delta_W + T
    float T;                            //�����ؽڵ�������أ������������أ���Nm��
    float W;                            //�����ؽ��ٶȣ����������ٶȣ�(rad/s)
    float Pos;                          //�����ؽ�λ�ã�rad��
    float K_P;                          //�ؽڸն�ϵ��
    float K_W;                          //�ؽ��ٶ�ϵ��
} MOTOR_send;

/**
 * �������ݹ����
 */
typedef struct
{
    // ���� ��������
    MotorData_t motor_recv_data;    //����������ݽṹ�壬���motor_msg.h
    int hex_len;                        //���յ�16�����������鳤��, 78
    long long resv_time;                //���ո������ʱ��, ΢��(us)
    int correct;                        //���������Ƿ�������1������0��������
    //����ó��ĵ������
    unsigned char motor_id;             //���ID
    unsigned char mode;                 // 0:����, 5:����ת��, 10:�ջ�FOC����
    int Temp;                           //�¶�
    unsigned char MError;               //������
    float T;                            // ��ǰʵ�ʵ���������
		float W;														// speed
    float Pos;                          // ��ǰ���λ�ã�����0������������ؽڻ����Ա�����0��Ϊ׼��
		float footForce;												// �����ѹ���������� 12bit (0-4095)

} MOTOR_recv;

uint32_t crc32_core(uint32_t *ptr, uint32_t len);
int modify_data(MOTOR_send *motor_s);
int extract_data(MOTOR_recv *motor_r);
HAL_StatusTypeDef SERVO_Send_recv(UART_HandleTypeDef *usartx,MOTOR_send *pData, MOTOR_recv *rData);
#ifdef __cplusplus
}
#endif
#endif

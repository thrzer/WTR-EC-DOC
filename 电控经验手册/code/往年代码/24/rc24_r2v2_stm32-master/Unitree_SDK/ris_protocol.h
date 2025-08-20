/**
 * @file ris_protocol.h
 * @author Yichao Zhang (unitree@qq.com)
 * @brief Go-M8010-6�ؽڵ������ ����ͨѶָ�
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) unitree robotics .co.ltd. 2022
 */

#ifndef __RIS_PROTOCOL_H
#define __RIS_PROTOCOL_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#pragma pack(1)

/**
 * @brief ���ģʽ������Ϣ
 * 
 */
typedef struct
{
    uint8_t id     :4;      // ���ID: 0,1...,13,14 15��ʾ�����е���㲥����(��ʱ�޷���)
    uint8_t status :3;      // ����ģʽ: 0.���� 1.FOC�ջ� 2.������У׼ 3.����
    uint8_t none   :1;      // ����λ
} RIS_Mode_t;   // ����ģʽ 1Byte

/**
 * @brief ���״̬������Ϣ
 * 
 */
typedef struct
{
    int16_t tor_des;        // �����ؽ����Ť�� unit: N.m      (q8)
    int16_t spd_des;        // �����ؽ�����ٶ� unit: rad/s    (q8)
    int32_t pos_des;        // �����ؽ����λ�� unit: rad      (q15)
    int16_t k_pos;          // �����ؽڸն�ϵ�� unit: -1.0-1.0 (q15)
    int16_t k_spd;          // �����ؽ�����ϵ�� unit: -1.0-1.0 (q15)
    
} RIS_Comd_t;   // ���Ʋ��� 12Byte

/**
 * @brief ���״̬������Ϣ
 * 
 */
typedef struct
{
    int16_t  torque;        // ʵ�ʹؽ����Ť�� unit: N.m     (q8)
    int16_t  speed;         // ʵ�ʹؽ�����ٶ� unit: rad/s   (q8)
    int32_t  pos;           // ʵ�ʹؽ����λ�� unit: rad     (q15)
    int8_t   temp;          // ����¶�: -128~127��C
    uint8_t  MError :3;     // ��������ʶ: 0.���� 1.���� 2.���� 3.��ѹ 4.���������� 5-7.����
    uint16_t force  :12;    // �����ѹ���������� 12bit (0-4095)
    uint8_t  none   :1;     // ����λ
} RIS_Fbk_t;   // ״̬���� 11Byte


#pragma pack()
#ifdef __cplusplus
}
#endif
#endif

/* 
 * Actuator Communication Reduced Instruction Set
 * Unitree robotics (c) .Co.Ltd. 2022 All Rights Reserved.
 */

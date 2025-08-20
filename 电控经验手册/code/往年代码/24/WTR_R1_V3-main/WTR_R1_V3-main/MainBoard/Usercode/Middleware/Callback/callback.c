#include "ashining_as69.h"
#include "HWT101CT_sdk.h"
#include "ops_sdk.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t ucTemp = 0;
    if (huart->Instance == AS69_UART) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
    }
    if (huart->Instance == HUART_CURRENT.Instance) {
        WitSerialDataIn(ucTemp);
        HAL_UART_Receive_IT(huart, &ucTemp, 1);
    }
    if (huart->Instance == OPS_UART) {
        OPS_Decode();
    }
    if (huart->Instance == LASER_UART) {
        Laser_Decode();
    }
}

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    switch (msg->msgid) {
        case 209:
            mavlink_msg_joystick_air_decode(msg, &msg_joystick_air);
            break;
        default:
            break;
    }
}
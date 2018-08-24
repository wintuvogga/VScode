#ifndef BT121_H
#define BT112_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void);
void BluetoothProcess(void *params);
void on_message_send(uint8_t msg_len, uint8_t* msg_data, uint16_t data_len, uint8_t* data);

#endif

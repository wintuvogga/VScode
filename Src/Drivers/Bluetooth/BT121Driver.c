#include "BT121.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dumo_bglib.h"

#define MAX_TIMEOUT 1000   //ticks
BGLIB_DEFINE();
unsigned char byte[20] = "";

void MX_USART1_UART_Init(void)
{
	// TXD
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// RXD
	GPIO_InitStructure.Pin = GPIO_PIN_10;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	__HAL_RCC_USART1_CLK_ENABLE();

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	NVIC_EnableIRQ(USART1_IRQn);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	//NVIC_SetPriority(USART1_IRQn, 0);

	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	
}

void BluetoothProcess(void *params)
{
	// Intialize USART
	//unsigned char buffer[5] = "wintu";
	MX_USART1_UART_Init();
	BGLIB_INITIALIZE(on_message_send);
	

	while(1)
	{
		//HAL_UART_Transmit(&huart1, buffer, 5, MAX_TIMEOUT);
		vTaskDelay(1000);
	}
}
 
void on_message_send(uint8_t msg_len, uint8_t* msg_data, uint16_t data_len, uint8_t* data)
{
	HAL_UART_Transmit(&huart1, msg_data, msg_len, MAX_TIMEOUT);
	if(data_len &&  data)
	{
		HAL_UART_Transmit(&huart1, msg_data, msg_len, MAX_TIMEOUT);
	}
}

void USART1_IRQHandler()
{
	//HAL_UART_Receive_IT(&huart1, byte, 20);
	return;
}

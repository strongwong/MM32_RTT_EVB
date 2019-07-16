////////////////////////////////////////////////////////////////////////////////
#define _UART_C_
////////////////////////////////////////////////////////////////////////////////

#include "stdio.h"	
#include "string.h"	

#include "sys.h" 
#include "HAL_device.h"

#include "main.h"
#include "sys.h"
#include "i2c.h"
#include "uart.h"	  


////////////////////////////////////////////////////////////////////////////////
void initGPIO_UART(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//UART2_TX   GPIOA.2 	//UART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9;;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	//UART2_RX	  GPIOA.3 	//UART1_RX	  GPIOA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void NVIC_UART(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);								

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = UART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);								
} 
	
////////////////////////////////////////////////////////////////////////////////
void initUART(void)
{
	UART_InitTypeDef UART_InitStructure;
	
// UART1
	UART_InitStructure.UART_BaudRate = BAUDRATE;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
	UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

	UART_Init(UART1, &UART_InitStructure);
	
	UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);
	UART_Cmd(UART1, ENABLE);

// UART2
	UART_InitStructure.UART_BaudRate = BAUDRATE;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
	UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

	UART_Init(UART2, &UART_InitStructure);
	
	UART_ITConfig(UART2, UART_IT_RXIEN, ENABLE);
	UART_Cmd(UART2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void UART1_IRQHandler()
{
	if(UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET) {
		UART_ClearITPendingBit(UART1,UART_IT_RXIEN);
		memmove(&uart1RxBuf[0], &uart1RxBuf[1], 7);
		uart1RxBuf[7] = UART_ReceiveData(UART1);
		rf.uart1 = TRUE;
		error.uart1 = FALSE;
		rfOverCnt.uart1 = 0;
	} 
} 

////////////////////////////////////////////////////////////////////////////////
void UART2_IRQHandler()
{
	if(UART_GetITStatus(UART2, UART_IT_RXIEN)  != RESET) {
		UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
		memmove(&uart2RxBuf[0], &uart2RxBuf[1], 7);
		uart2RxBuf[7] = UART_ReceiveData(UART2);
		rf.uart2 = TRUE;
		error.uart2 = FALSE;
		rfOverCnt.uart2 = 0;
	} 
}

////////////////////////////////////////////////////////////////////////////////

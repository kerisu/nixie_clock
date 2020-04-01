#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart_init(u32 bound);
void USART1_IRQHandler(void);
//void UartOneSendChar(char tt);
void usart_config(u32 bound);
void usart2_Printf (char *fmt, ...);
void sendFun(u8 *str);
void UartTwoSendChar(char tt);
void USART2_IRQHandler(void);
void uart3_init(u32 bound);
void UartThreeSendChar(char tt);

#endif



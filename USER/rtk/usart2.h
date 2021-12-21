#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "common.h" 
#include <string.h>
#include <stdlib.h>

#define USART2_REC_LEN  			100  	//�����������ֽ��� 200
#define USART2_TEC_LEN  			100  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����

extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART2_TX_BUF[USART2_TEC_LEN];
extern u16 USART2_RX_STA;         		//����״̬���	
extern u8 rec_complete;
extern u8 state_machine;
extern u8 qf;
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart2_init(u32 bound);

//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
//void Usart_SendStr( USART_TypeDef * pUSARTx, const uint8_t* ch);

void data_analysis(char *buff, const char* delim, float* dest);

#endif



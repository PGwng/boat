#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "common.h" 
#include <string.h>
#include <stdlib.h>

#define USART2_REC_LEN  			100  	//定义最大接收字节数 200
#define USART2_TEC_LEN  			100  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收

extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART2_TX_BUF[USART2_TEC_LEN];
extern u16 USART2_RX_STA;         		//接收状态标记	
extern u8 rec_complete;
extern u8 state_machine;
extern u8 qf;
//如果想串口中断接收，请不要注释以下宏定义
void uart2_init(u32 bound);

//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
//void Usart_SendStr( USART_TypeDef * pUSARTx, const uint8_t* ch);

void data_analysis(char *buff, const char* delim, float* dest);

#endif



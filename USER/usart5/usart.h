#ifndef __UART5_H
#define __UART5_H
#include "stm32f4xx.h"
#include "stdio.h"	
#include "common.h" 
#include <string.h>
#include <stdlib.h>

#define USART5_REC_LEN  			20  	//定义最大接收字节数 200
#define USART5_TEC_LEN  			20  	//定义最大接收字节数 200
#define EN_USART5_RX 			1		//使能（1）/禁止（0）串口2接收
extern u8 rec5_complete;
extern u8  USART5_RX_BUF[USART5_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART5_TX_BUF[USART5_TEC_LEN];
extern u16 USART5_RX_STA;         		//接收状态标记	

extern Float_data dis,yaw,yaw_ref;

#define USART5_Rx_LEN  			200  	//定义最大接收字节数 200
#define USART5_Tx_LEN        200

#ifdef  __UART5_GLOBALS
#define __UART5_EXT
#else
#define __UART5_EXT  extern
#endif

void uart5_init(u32 bound);
void POS_DATA(void);

#endif

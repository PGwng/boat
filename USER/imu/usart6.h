#ifndef __USART6_H
#define __USART6_H
#include "stdio.h"	
#include "common.h"
#include <string.h>
#include <stdlib.h>

#define USART6_REC_LEN  			100  	//�����������ֽ��� 200
#define USART6_TEC_LEN  			100  	//�����������ֽ��� 200

extern u8  USART6_RX_BUF[USART6_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART6_TX_BUF[USART6_TEC_LEN];
extern u16 USART6_RX_STA;         		//����״̬���	
extern u8  imu_mutex_lock;


void uart6DMA_init(u32 bound,u32 packebuff);

#endif



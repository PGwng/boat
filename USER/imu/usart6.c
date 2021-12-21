#include "./imu/usart6.h"
#include "../BaseDrive/DMA/dma.h"
#include "./imu/imu.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#define USEDMA

u8 USART6_RX_BUF[USART6_REC_LEN]={0};     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART6_RX_CNT=0;


u8 USART6_RX_BUF[USART6_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART6_TX_BUF[USART6_TEC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.


u16 USART6_RX_STA = 0;       //����״̬���
u8 imu_mutex_lock=0;

extern IMU imu;
extern u8 DMA2packBuf[40];

void uart6DMA_init(u32 bound,u32 packebuff)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);	//ʹ��USART6��GPIOCʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC2����ΪUSART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC3����ΪUSART6
	
	/*USART6_TX ->PC6*//*USART6_RX ->PC7*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PC6��PC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��

  //USART6 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ


  USART_Init(USART6, &USART_InitStructure); //��ʼ������2
	//USART_ClearFlag(USART6, USART_FLAG_TC);//���������ɱ�־
#ifndef USEDMA
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART6, ENABLE);
#endif


	//ʹ�ܴ���6
#ifdef USEDMA
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART6, ENABLE);
  
	//MYDMA_TXConfig(DMA2_Stream6,DMA_Channel_5,(u32)&USART6->DR,(u32)packebuff,sizeof(packebuff));
	MYDMA_RXConfigPtoM(DMA2_Stream1,DMA_Channel_5,(u32)&USART6->DR,(u32)packebuff,sizeof(packebuff));
	//USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);  //ʹ�ܴ���2��DMA����   
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);  //ʹ�ܴ���2��DMA���� 
#endif
}

//����6�����жϷ�����
void USART6_IRQHandler(void)
{
  if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)//��⵽�����ж�
	{	
      DMA_Cmd(DMA2_Stream1,DISABLE);
			imu_mutex_lock=1;   //�ϻ�����
      USART6_RX_CNT=USART6->SR;//����жϱ�־λ
      USART6_RX_CNT=USART6->DR;
			DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
      USART6_RX_CNT=USART6_REC_LEN-DMA_GetCurrDataCounter(DMA2_Stream1);  //�յ������ݳ���
			memcpy(imu.rawIMUBuf, DMA2packBuf, USART6_RX_CNT);
			imuAnalysisData(&imu);
			USART6_RX_STA=1;
			//if(USART6_RX_STA==1)
			//{
				//USART6_RX_STA=0;
				//printf("read inter yaw:%f\r\n",imuReadYaw(&imu));
//				printf("read pitch:%f\r\n",Eular[1]);
//				printf("read roll:%f\r\n",Eular[0]);
//				
//				printf("read ax:%f\r\n",Acc[0]);
//				printf("read ay:%f\r\n",Acc[1]);
//				printf("read az:%f\r\n",Acc[2]);
//				
//				printf("read gx:%f\r\n",Gyr[0]);
//				printf("read gy:%f\r\n",Gyr[1]);
//				printf("read gz:%f\r\n",Gyr[2]);
			//}
			//printf("---------------------------\r\n");
      //memset(, 0, );    //��ս�����
      DMA2_Stream1->NDTR=USART6_REC_LEN;//����װ��
      DMA_Cmd(DMA2_Stream1,ENABLE);    //���Ŵ���
			USART_ClearITPendingBit(USART6,USART_IT_IDLE);
   }											 
} 


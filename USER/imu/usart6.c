#include "./imu/usart6.h"
#include "../BaseDrive/DMA/dma.h"
#include "./imu/imu.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

#define USEDMA

u8 USART6_RX_BUF[USART6_REC_LEN]={0};     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART6_RX_CNT=0;


u8 USART6_RX_BUF[USART6_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART6_TX_BUF[USART6_TEC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.


u16 USART6_RX_STA = 0;       //接收状态标记
u8 imu_mutex_lock=0;

extern IMU imu;
extern u8 DMA2packBuf[40];

void uart6DMA_init(u32 bound,u32 packebuff)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);	//使能USART6，GPIOC时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC2复用为USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC3复用为USART6
	
	/*USART6_TX ->PC6*//*USART6_RX ->PC7*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PC6、PC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化

  //USART6 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式


  USART_Init(USART6, &USART_InitStructure); //初始化串口2
	//USART_ClearFlag(USART6, USART_FLAG_TC);//清除发送完成标志
#ifndef USEDMA
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART6, ENABLE);
#endif


	//使能串口6
#ifdef USEDMA
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART6, ENABLE);
  
	//MYDMA_TXConfig(DMA2_Stream6,DMA_Channel_5,(u32)&USART6->DR,(u32)packebuff,sizeof(packebuff));
	MYDMA_RXConfigPtoM(DMA2_Stream1,DMA_Channel_5,(u32)&USART6->DR,(u32)packebuff,sizeof(packebuff));
	//USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);  //使能串口2的DMA发送   
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);  //使能串口2的DMA接收 
#endif
}

//串口6接收中断服务函数
void USART6_IRQHandler(void)
{
  if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)//检测到空闲中断
	{	
      DMA_Cmd(DMA2_Stream1,DISABLE);
			imu_mutex_lock=1;   //上互斥锁
      USART6_RX_CNT=USART6->SR;//清除中断标志位
      USART6_RX_CNT=USART6->DR;
			DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
      USART6_RX_CNT=USART6_REC_LEN-DMA_GetCurrDataCounter(DMA2_Stream1);  //收到的数据长度
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
      //memset(, 0, );    //清空接受区
      DMA2_Stream1->NDTR=USART6_REC_LEN;//重新装填
      DMA_Cmd(DMA2_Stream1,ENABLE);    //接着传输
			USART_ClearITPendingBit(USART6,USART_IT_IDLE);
   }											 
} 


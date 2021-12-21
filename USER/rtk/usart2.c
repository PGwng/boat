#include "common.h" 
#include "./rtk/usart2.h"	  
#include "./usart/bsp_debug_usart.h"  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif




#if EN_USART2_RX   //如果使能了接收


u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART2_TX_BUF[USART2_TEC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.


u16 USART2_RX_STA = 0;       //接收状态标记
u8 state_machine = 0;

u8 rec_complete = 0;
u8 data_count = 0;
u8 qf;
////发送一个字节
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* Transmit Data */
//  pUSARTx->DR = ch;
//	/* 等待发送数据寄存器为空 */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}

////发送字符串
//void Usart_SendStr( USART_TypeDef * pUSARTx, const uint8_t* ch)
//{
//	while(*ch!='\0')
//	{
//		Usart_SendByte(pUSARTx,*ch++);
//	}
//}
	
void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_AHB1Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
  
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	//USART2_TX GPIOA.2    USART2_TX GPIOA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2、PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化

  //Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
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


  USART_Init(USART2, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口2

}

void USART2_IRQHandler(void)                	//串口2中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			 //printf("state_machine %d\r\n",state_machine);
				USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志
				Res =USART_ReceiveData(USART2);	//读取接收到的数据
				if(Res != ' ')                //去空格
				{
					switch(state_machine)      // 协议解析状态机
					{
						// 接收到帧头第一个数据
						case 0:	
								if(Res == '$')       // 接收到帧头第一个数据
									state_machine = 1;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								}					
								break;
								
						// 接收到帧头第二个数据		
						case 1:
								if(Res == 'G')       // 接收到帧头第二个数据
									state_machine = 2;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								} 					
								break;
						
						// 接收到帧头第三个数据
						case 2:
								if(Res == 'P')       // 接收到帧头第三个数据
									state_machine = 3;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						// 接收到帧头第四个数据
						case 3:
								if(Res == 'G')       // 接收到帧头第四个数据
									state_machine = 4;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						// 接收到帧头第五个数据
						case 4:
								if(Res == 'G')       // 接收到帧头第五个数据
									state_machine = 5;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
								
						// 接收到帧头第六个数据
						case 5:
								if(Res == 'A')       // 接收到帧头第六个数据
									state_machine = 6;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						// 接收到帧头第七个数据					
						case 6:
								if(Res == ',')       // 接收到帧头第七个数据
									state_machine = 7;
								else
								{
									state_machine = 0;      // 状态机复位
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						//接收有效帧数据				
						case 7:
								USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res;		 
								USART2_RX_STA++;
								if(Res == ',')
								{
									data_count++;
								}
								else if(data_count == 5)
								{
									qf = Res;
								}
								if(Res == '*')						     //数据接收完成
								{
									rec_complete = 1;
									state_machine = 0;
									data_count = 0;
								}        
								else
								{
									state_machine = 7;					//继续接收数据
								}
								if(USART2_RX_STA>(USART2_REC_LEN-1))    //超过最长，数据接收错误
								{
									state_machine = 0;
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;	
						default:
								break;
					}					
				}
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif

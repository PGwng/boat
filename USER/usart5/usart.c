#define  __UART5_GLOBALS

#include "./usart5/usart.h"

Float_data dis,yaw,yaw_ref;

u8 USART5_RX_BUF[USART5_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART5_TX_BUF[USART5_TEC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.


u16 USART5_RX_STA = 0;       //接收状态标记
u8 state_machine5 = 0;
const char* delim5 = ","; //分割點

u8 rec5_complete = 0;


/**
  * @brief  初始化UART5
  * @param  void
  * @retval void
  * @notes  UART5_TX-->PG14      UART5_RX-->PG9---->YOLO信息接收        UART5_TX---->串口打印
  */
void uart5_init(u32 bound)
{
	
  GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	/*enabe clocks*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	/*open the alternative function*/
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);
	
	/*Configure PC12  TX*/
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
		/*Configure PD2 RX*/
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate            = bound;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART5,&USART_InitStruct);
	
	USART_Cmd(UART5,ENABLE);		
	USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel                    = UART5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority  = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority         = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd                 = ENABLE ;
	NVIC_Init(&NVIC_InitStruct);
}


//static uint8_t upload_flag=0;
////串口接收上位机信息
void UART5_IRQHandler(void)    
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		//printf("state_machine5 %d\r\n",state_machine5);
		USART_ClearITPendingBit(UART5,USART_IT_RXNE); //清除中断标志
		Res =USART_ReceiveData(UART5);	//读取接收到的数据
		//if(Res != ' ')                //去空格
		{
				switch(state_machine5)      // 协议解析状态机
				{
						// 接收到帧头第一个数据
						case 0:	
								if(Res == '$')       		// 接收到帧头
									state_machine5 = 1;
								else
								{
									state_machine5 = 0;      // 状态机复位
									USART5_RX_STA = 0;
								}					
								break;
						// 接收到帧头第一个数据
						case 1:	
								if(Res == '$')       		// 接收到帧头
									state_machine5 = 2;
								else
								{
									state_machine5 = 0;      // 状态机复位
									USART5_RX_STA = 0;
								}					
								break;								
						// 接收到有用数据								
						case 2:
								USART5_RX_BUF[USART5_RX_STA&0X3FFF]=Res;		 
								USART5_RX_STA++;
								//printf("Res:%d\r\n",state_machine5);
								if((USART5_RX_STA&0X3FFF) == 12)						     //数据接收完成
								{
									rec5_complete = 1;
									state_machine5 = 0;
									USART5_RX_STA = 0;
								}        
								else
								{
									state_machine5 = 2;					//继续接收数据
								}
								if(USART5_RX_STA>(USART5_REC_LEN-1))    //超过最长，数据接收错误
								{
									state_machine5 = 0;
									USART5_RX_STA = 0;
								}
								break;	
						default:
								break;
			}					
		}
  }     
}


void data_analysis(char *buff, const char* delim5, float* dest);

//解析方位数据
void POS_DATA(void)
{
		int i=0;
		if(rec5_complete == 1)
		{
			USART5_RX_STA = 0;
			rec5_complete = 0;
		 //data_analysis((char*)USART5_RX_BUF,delim5,&dis.f_data);
		 //data_analysis(NULL,delim5,&yaw.f_data);
		 //data_analysis(NULL,delim5,&yaw_ref.f_data);
		 
			for(i=0; i<4; i++)
			{
				dis.byte[i] = USART5_RX_BUF[i];
				yaw.byte[i] = USART5_RX_BUF[i+4];
				yaw_ref.byte[i] = USART5_RX_BUF[i+8];
			}
			//printf("%s\r\n",USART5_RX_BUF);	
			memset(USART5_RX_BUF,0,USART5_REC_LEN);				
			printf("dis:%0.4f yaw:%0.4f ref_yaw:%0.4f\r\n",dis.f_data,yaw.f_data,yaw_ref.f_data);
		}
}

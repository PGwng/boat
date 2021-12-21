#include "common.h" 
#include "./rtk/usart2.h"	  
#include "./usart/bsp_debug_usart.h"  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif




#if EN_USART2_RX   //���ʹ���˽���


u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_TEC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.


u16 USART2_RX_STA = 0;       //����״̬���
u8 state_machine = 0;

u8 rec_complete = 0;
u8 data_count = 0;
u8 qf;
////����һ���ֽ�
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* Transmit Data */
//  pUSARTx->DR = ch;
//	/* �ȴ��������ݼĴ���Ϊ�� */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}

////�����ַ���
//void Usart_SendStr( USART_TypeDef * pUSARTx, const uint8_t* ch)
//{
//	while(*ch!='\0')
//	{
//		Usart_SendByte(pUSARTx,*ch++);
//	}
//}
	
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_AHB1Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
  
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART2_TX GPIOA.2    USART2_TX GPIOA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2��PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��

  //Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
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


  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2

}

void USART2_IRQHandler(void)                	//����2�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			 //printf("state_machine %d\r\n",state_machine);
				USART_ClearITPendingBit(USART2,USART_IT_RXNE); //����жϱ�־
				Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
				if(Res != ' ')                //ȥ�ո�
				{
					switch(state_machine)      // Э�����״̬��
					{
						// ���յ�֡ͷ��һ������
						case 0:	
								if(Res == '$')       // ���յ�֡ͷ��һ������
									state_machine = 1;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								}					
								break;
								
						// ���յ�֡ͷ�ڶ�������		
						case 1:
								if(Res == 'G')       // ���յ�֡ͷ�ڶ�������
									state_machine = 2;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								} 					
								break;
						
						// ���յ�֡ͷ����������
						case 2:
								if(Res == 'P')       // ���յ�֡ͷ����������
									state_machine = 3;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						// ���յ�֡ͷ���ĸ�����
						case 3:
								if(Res == 'G')       // ���յ�֡ͷ���ĸ�����
									state_machine = 4;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						// ���յ�֡ͷ���������
						case 4:
								if(Res == 'G')       // ���յ�֡ͷ���������
									state_machine = 5;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
								
						// ���յ�֡ͷ����������
						case 5:
								if(Res == 'A')       // ���յ�֡ͷ����������
									state_machine = 6;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						// ���յ�֡ͷ���߸�����					
						case 6:
								if(Res == ',')       // ���յ�֡ͷ���߸�����
									state_machine = 7;
								else
								{
									state_machine = 0;      // ״̬����λ
									USART2_RX_STA = 0;
									data_count = 0;
								}
								break;
						
						//������Ч֡����				
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
								if(Res == '*')						     //���ݽ������
								{
									rec_complete = 1;
									state_machine = 0;
									data_count = 0;
								}        
								else
								{
									state_machine = 7;					//������������
								}
								if(USART2_RX_STA>(USART2_REC_LEN-1))    //����������ݽ��մ���
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
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif

#include "user.h"
#include "./usart5/usart.h"
#include "./tim/tim4.h"

int main(void)
{ 
	
	Debug_USART_Config();
	uart2_init(115200);
	uart5_init(115200);
	delay_init();		  //��ʼ����ʱ����
	SystemInitUser();					//�ȳ�ʼ��������ʹ�ܶ�ʱ�����������printf��������ѭ��
	TIM4_Configuration();	
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)&ltc2944.remainder_charge,1);//��flash

	//delay_ms(5);		//��lt8920�����ȶ�
	//RF_Init();
//	printf("222\r\n");
	Pwm_Set(TIM2,3,4500);
	Pwm_Set(TIM2,4,4500);
	servo_Init();
	NRF24L01_Init();    		//��ʼ��NRF24L01 
	while(NRF24L01_Check())
	{
		printf("NRF24L01 Error\r\n");
		delay_ms(200);	
	}
	printf("NRF24L01 OK\r\n");
	NRF24L01_RT_Mode();
	
	delay_ms(1500); //imu��ʼ��ǰ������ʱ
	uart6DMA_init(115200,(u32)DMA2packBuf);	 //����6��ʼ��������Ϊ115200,DMA����
	imuInit(USART6,&imu);	
	imuStart(&imu);
	
//	u8 test1[5]={4,'A','A','A','A'};
//	u8 test2[5]={4,'B','B','B','B'};
	//Usart_SendByte(UART5,'D');
	while(1)
	{
//		get_tx_info();											//��ȡByte��ʽ�ķ�������
//		gen_tx_packets();										//��Ҫ���͵���������������ݰ�		
//		SEND_BUF(test1);
//		SEND_BUF(test2);
		
//		SEND_BUF(tx_packets.TX_DATA1);			//�������ݰ�1 (Acc & Gyr)
//		delay_ms(100);
//		SEND_BUF(tx_packets.TX_DATA2);			//�������ݰ�2	(Eular & RTK)
		
//		printf("Eular:%f\r\n",Eular[2]);
//		printf("ref_AS:%f\r\n",Ref_Angular_Speed);
		//delay_ms(100);//ע�͵����ң����ռ�ձȳ���
		
		Loop();
		key_scan(0);
		if(keyup_data==KEY0_DATA)
		{
			u16 full_charge = 0xffff;
			STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)&full_charge,1);
			LED1_ON();
			delay_ms(100);
			LED1_OFF();
			PEout(2) = ~PEout(2);
		}
	}	
}
/***********************************************************************************/







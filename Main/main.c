#include "user.h"
#include "./usart5/usart.h"
#include "./tim/tim4.h"

int main(void)
{ 
	
	Debug_USART_Config();
	uart2_init(115200);
	uart5_init(115200);
	delay_init();		  //初始化延时函数
	SystemInitUser();					//先初始化串口再使能定时器，否则会在printf函数中死循环
	TIM4_Configuration();	
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)&ltc2944.remainder_charge,1);//读flash

	//delay_ms(5);		//待lt8920晶振稳定
	//RF_Init();
//	printf("222\r\n");
	Pwm_Set(TIM2,3,4500);
	Pwm_Set(TIM2,4,4500);
	servo_Init();
	NRF24L01_Init();    		//初始化NRF24L01 
	while(NRF24L01_Check())
	{
		printf("NRF24L01 Error\r\n");
		delay_ms(200);	
	}
	printf("NRF24L01 OK\r\n");
	NRF24L01_RT_Mode();
	
	delay_ms(1500); //imu初始化前必须延时
	uart6DMA_init(115200,(u32)DMA2packBuf);	 //串口6初始化波特率为115200,DMA传输
	imuInit(USART6,&imu);	
	imuStart(&imu);
	
//	u8 test1[5]={4,'A','A','A','A'};
//	u8 test2[5]={4,'B','B','B','B'};
	//Usart_SendByte(UART5,'D');
	while(1)
	{
//		get_tx_info();											//获取Byte格式的发送数据
//		gen_tx_packets();										//将要发送的数据组成两个数据包		
//		SEND_BUF(test1);
//		SEND_BUF(test2);
		
//		SEND_BUF(tx_packets.TX_DATA1);			//发送数据包1 (Acc & Gyr)
//		delay_ms(100);
//		SEND_BUF(tx_packets.TX_DATA2);			//发送数据包2	(Eular & RTK)
		
//		printf("Eular:%f\r\n",Eular[2]);
//		printf("ref_AS:%f\r\n",Ref_Angular_Speed);
		//delay_ms(100);//注释掉后测遥控器占空比出错？
		
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







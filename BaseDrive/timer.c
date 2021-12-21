#include "timer.h"
#include "./tim/tim4.h"

/*时间片结构体定义*/
typedef struct{	
	volatile u8 Hz100;
	volatile u8 Hz50;
	volatile u8 Hz20;
	volatile u8 Hz10;
	volatile u8 Hz5;
	volatile u8 Hz1;
}TimeSlice;

/*初始变量定义*/
volatile u16 system_pulse;
volatile TimeSlice Mission;

//uint16_t time_flag[5] = {0,0,0,0,0};

//extern int test_i;

////TIM1
//void PWM1_Init(void){
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
//	
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
////	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
////	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOE,&GPIO_InitStructure);
//	
//	TIM_TimeBaseStructure.TIM_Period = 59999;
//	TIM_TimeBaseStructure.TIM_Prescaler = 55;
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse = 0;
//	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;
//	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
//	
//	TIM_OC1Init(TIM1, &TIM_OCInitStructure );
//	TIM_OC2Init(TIM1, &TIM_OCInitStructure );	
//	TIM_OC3Init(TIM1, &TIM_OCInitStructure );
//	TIM_OC4Init(TIM1, &TIM_OCInitStructure );
//	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable );
//	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable );
//	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable );
//	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable );
//	TIM_ARRPreloadConfig(TIM1, ENABLE);
//	TIM_Cmd(TIM1, ENABLE);
//	TIM_CtrlPWMOutputs(TIM1,ENABLE);
//}

//TIM2 CH3 CH4	电机
void PWM2_Init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_TIM2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 59999;
	TIM_TimeBaseStructure.TIM_Prescaler = 27;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dut_M;
	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure );
	TIM_OC4Init(TIM2, &TIM_OCInitStructure );
	
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable );
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable );
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
}

//TIM3 CH1 CH2	舵机
void PWM3_Init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 59999;
	TIM_TimeBaseStructure.TIM_Prescaler = 27;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dut_M;
	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure );
	TIM_OC2Init(TIM3, &TIM_OCInitStructure );
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable );
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable );
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
}

void Loop(void)
{
	/**********************************************10ms,100Hz时间片*******************************************/
	if(Mission.Hz100)
	{
		/**************任务***************/

		
		Mission.Hz100 = 0;
	}
	
	/**********************************************20ms,50Hz时间片*******************************************/
	if(Mission.Hz50)
	{
		/**************任务***************/
		RTK_DATA();							//rtk接收数据
		
		Mission.Hz50 = 0;
	}
	
	/**********************************************50ms,20Hz时间片*******************************************/
	if(Mission.Hz20)
	{
		/**************任务***************/
		
		Voltage_Storage();//电压采集
		//Temperature_Storage();
		//auto_flag = 1;
		
		POS_DATA();
		calculate_error(dis.f_data,yaw_ref.f_data,yaw.f_data);
		//printf("pos_err:%0.4f yaw_err:%0.4f \r\n",pos_err,yaw_err);
		Yaw_Control();	
		angular_speed_Control();//大船速度、角速度控制
		Pos_Control();
		spin_Control();
		servo_Control();
		
		Mission.Hz20 = 0;
	}

	/**********************************************100ms,10Hz时间片*******************************************/
	if(Mission.Hz10)
	{
		/**************任务***************/
//		static u8 i = 0;
//		get_tx_info();											//获取Byte格式的发送数据
//		gen_tx_packets();										//将要发送的数据组成两个数据包			
//		if(i == 0)
//		{
//			SEND_BUF(tx_packets.TX_DATA1);			//发送数据包1 (Acc & Gyr)
//			i = 1;
//		}
//		else
//		{
//			SEND_BUF(tx_packets.TX_DATA2);			//发送数据包2	(Eular & RTK)
//			i = 0;
//		}
		
		Mission.Hz10 = 0;
	}
	
	/**********************************************200ms,5Hz时间片*******************************************/
	if(Mission.Hz5)
	{
		/**************任务***************/
//		if(USART6_RX_STA==1)
//		{
//			USART6_RX_STA=0;
//			printf("read yaw:%f\r\n",Eular[2]);
//			//printf("read lock yaw:%f\r\n",360.0-imuReadYaw(&imu));
//			printf("read az:%f\r\n",Acc[2]);
//			printf("read gz:%f\r\n",Gyr[2]);
//		}
//		printf("---------------------------\r\n");
		
		Mission.Hz5 = 0;
	}
	
	/**********************************************1000ms,1Hz时间片*******************************************/
	if(Mission.Hz1)
	{
		//IWDG_Feed();//喂看门狗
		//CCR3_flag = 0;
		/**************任务***************/
		
		sendActYaw();//给上位机发送实际航向角		//每1s发送一次
		//CHARGR_DATA(&ltc2944);	
		
		Mission.Hz1 = 0;
	}	
	
}

//TIM5 10ms timer 84M
void Timer_Init(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1; 
	TIM_TimeBaseInitStructure.TIM_Prescaler = 84-1;				//84M/10000/420=20 溢出20次
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
	TIM_ARRPreloadConfig(TIM5,ENABLE);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE );
	TIM_Cmd(TIM5, ENABLE);
	system_pulse = 0;
	Mission.Hz1 = 0;Mission.Hz5 = 0;Mission.Hz10 = 0;Mission.Hz20 = 0;Mission.Hz50 = 0;Mission.Hz100 = 0;
}
//10ms定时器
void TIM5_IRQHandler(void){   
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)){
		system_pulse++;
		if((system_pulse)%(1) == 0)
		{
			if((system_pulse)%(2) == 0)
			{
				if((system_pulse)%(5) == 0)
				{
					if((system_pulse)%(10) == 0)
					{
						if((system_pulse)%(20) == 0)
						{
							if((system_pulse)%(100) == 0)
							{
								Mission.Hz1 = 1;
							}
							Mission.Hz5 = 1;
						}
						Mission.Hz10 = 1;
					}
					IWDG_Feed();//喂看门狗
					Mission.Hz20 = 1;
				}
				Mission.Hz50 = 1;
			}
			Mission.Hz100 = 1;
		}
		
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
	}
}

void Pwm_Set(TIM_TypeDef* TIMx, uint8_t i, uint16_t duty){
	switch(i){
		case 1:
				TIMx->CCR1 = duty;
				break;
		case 2:
				TIMx->CCR2 = duty;
				break;
		case 3:
				TIMx->CCR3 = duty;
				break;
		case 4:
				TIMx->CCR4 = duty;
				break;
	}
}

uint16_t Pwm_Read(TIM_TypeDef* TIMx, uint8_t i){
	uint16_t duty = 0;
	switch(i){
		case 1:
				duty = TIMx->CCR1;
				break;
		case 2:
				duty = TIMx->CCR2;
				break;
		case 3:
				duty = TIMx->CCR3;
				break;
		case 4:
				duty = TIMx->CCR4;
				break;
	}
	return duty;
}

#include "init.h"

u8 DMA2packBuf[100]={0};
IMU imu;

void SystemInitUser(void){
	IWDG_Init(4,500);//0.8s
	SystemInit();
	delay_init();
	KEY_Init();				//KEY:PF6/7/8/9
	LED_Init();      //LED: PG13 PG14
	MOS_Init();      //MOS: PD3/4/5/6
	LTC2944_Init();  //I2C: SCL PB6  SDA PB7
	
//	Relay1_TOG();
//	delay_ms(500);
//	Relay1_TOG();
//	Relay2_TOG();
//	delay_ms(500);
//	Relay2_TOG();
//	Relay3_TOG();
//	delay_ms(500);
//	Relay3_TOG();
	
	GPIO_standby();  //PE2/3/4/5/6
	USER_ADC_Init(); //ADC:PC5 DMA PB0,PB1
	
//	PWM1_Init();
	PWM2_Init();//电机
	PWM3_Init();//舵机
	Timer_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级分组2
	NVIC_Config();
//	EXTI8_Init();
//	EXTI0_Init();
	EXTI4_Init();
}

void NVIC_Config(void){
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;   //10ms 定时器5中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;//子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn; //PC
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn; //GPS
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel=USART6_IRQn; //imu
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
//		NVIC_InitStructure.NVIC_IRQChannel=UART5_IRQn; //imu
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream7_IRQn;           
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0x01;          
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
////  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;  //CAN
////  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
////  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;  
////  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
////  	NVIC_Init(&NVIC_InitStructure);
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel= EXTI9_5_IRQn; //外部中断8（PG8）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    NVIC_InitStructure.NVIC_IRQChannel= EXTI0_IRQn; //外部中断0（PD0）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);		
		
		NVIC_InitStructure.NVIC_IRQChannel= EXTI4_IRQn; //外部中断4（PA4）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);		
}

void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG, GPIO_Pin_13 | GPIO_Pin_14);

//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOF, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOF, GPIO_Pin_9| GPIO_Pin_10);
}

void MOS_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
}

void GPIO_standby(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
}

/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   通用定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "user.h"
#include "./tim/tim4.h"
#include "./usart/bsp_debug_usart.h"
#include "motor.h"

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */

__IO int IC1RisingValue = 0;
__IO int IC1FallingValue = 0;
__IO int IC2RisingValue = 0;
__IO int IC2FallingValue = 0;
__IO float DutyTime3 = 0;
__IO float DutyTime4 = 0;
int CCR2_flag = 0;		//0表示上升沿，1下降沿
int CCR3_flag = 0;		//0表示上升沿，1下降沿
int CCR4_flag = 0;		//0表示上升沿，1下降沿
float	unit_time = 1.0f/100;		//单位：ms 定时器4计数频率为100kHz，即计一个数用时1/100000s
float Speed_toset = 0;
float angularSpeed_toset = 0;
u8 remote_device_flag = 0;

static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (TIM4_ICPWM_GPIO_CLK, ENABLE); 
	RCC_AHB1PeriphClockCmd (TIM4_IC3PWM_GPIO_CLK, ENABLE);	

	/* 定时器复用引脚 */
	GPIO_PinAFConfig(TIM4_ICPWM_GPIO_PORT,TIM4_IC1PWM_PINSOURCE,TIM4_ICPWM_AF);		//PB8
  GPIO_PinAFConfig(TIM4_ICPWM_GPIO_PORT,TIM4_IC2PWM_PINSOURCE,TIM4_ICPWM_AF); 	//PB9
	GPIO_PinAFConfig(TIM4_IC3PWM_GPIO_PORT,TIM4_IC3PWM_PINSOURCE,TIM4_ICPWM_AF); 	//PD13
	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_InitStructure.GPIO_Pin = TIM4_IC1PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(TIM4_ICPWM_GPIO_PORT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = TIM4_IC2PWM_PIN;
	GPIO_Init(TIM4_ICPWM_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = TIM4_IC3PWM_PIN;
	GPIO_Init(TIM4_IC3PWM_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @brief  定时器 TIMx中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_TIM_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_PWMINPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	// 开启TIMx_CLK,
	// 开启定时器4时钟	//通用定时器时钟源TIMxCLK = 2*PCLK=84MHz 
  RCC_APB1PeriphClockCmd(TIM4_TIM_CLK, ENABLE); 

  TIM_TimeBaseStructure.TIM_Period = 50000-1;			//0xFFFF-1; 			//计数器重装载值50000//65535
	
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;											//设置预分频值	最大计数周期T=(1/84M)*840*50000 = 500ms
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//采样时钟分频
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(TIM4_TIM, &TIM_TimeBaseStructure);
	
	/* IC1捕获：上升沿触发 TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = TIM4_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM4_TIM, &TIM_ICInitStructure);
	
	/* IC2捕获：上升沿触发 TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = TIM4_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;		//TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM4_TIM, &TIM_ICInitStructure);
	/* IC3捕获：上升沿触发 TI1FP2*/
	TIM_ICInitStructure.TIM_Channel = TIM4_IC3PWM_CHANNEL;
	TIM_ICInit(TIM4_TIM, &TIM_ICInitStructure);
	
  /* 使能定时器 */
  TIM_Cmd(TIM4_TIM, ENABLE);

  /* 使能捕获/比较2 3 4中断请求 */
  TIM_ITConfig(TIM4_TIM, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM4_TIM, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM4_TIM, TIM_IT_CC2, ENABLE);
}


/**
  * @brief  初始化高级控制定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIM4_Configuration(void)
{
	TIMx_GPIO_Config();
	
	TIMx_NVIC_Configuration();	
	
	TIM_PWMINPUT_Config();
}

void  TIM4_TIM_IRQHandler (void)
{
		/*通道2*/
	if(TIM_GetITStatus(TIM4_TIM,TIM_IT_CC2) != RESET)
	{
		
		if(CCR2_flag == 0)
		{
			TIM_SetCounter(TIM4_TIM,0); //计数器清空		从0开始计数高电平持续时间
			TIM_OC2PolarityConfig(TIM4_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(TIM4_TIM,ENABLE);
			CCR2_flag = 1;
		}
		else
		{
			int temp = TIM_GetCapture2(TIM4_TIM);
			if(temp==101 || temp==100)
			{			
				remote_device_flag = 0; //遥控器上锁
				//printf("IC3FallingValue:%d\r\n",temp);
			}
			else if(temp==201 || temp==202)
			{
				remote_device_flag = 1; //遥控器解锁
				//printf("IC3FallingValue:%d\r\n",temp);
			}				
			TIM_OC2PolarityConfig(TIM4_TIM,TIM_ICPolarity_Rising);
			//TIM_Cmd(TIM4_TIM,ENABLE );
			CCR2_flag = 0;
		}	
		TIM_ClearITPendingBit(TIM4_TIM, TIM_IT_CC2);
	}
	/*通道3*/
	if(TIM_GetITStatus(TIM4_TIM,TIM_IT_CC3) != RESET)
	{
		
		if(CCR3_flag == 0)
		{
			TIM_SetCounter(TIM4_TIM,0); //计数器清空		从0开始计数高电平持续时间
			TIM_OC3PolarityConfig(TIM4_TIM,TIM_ICPolarity_Falling);
			//TIM_Cmd(TIM4_TIM,ENABLE );
			CCR3_flag = 1;
		}
		else
		{
			int temp = TIM_GetCapture3(TIM4_TIM);
			if((temp >= 101)&&(temp <= 202))
			{	
				IC1FallingValue = temp;
				
				Speed_toset = (IC1FallingValue -151)*2.0f;
				if(remote_device_flag)
				{
					//printf("armed");
					auto_flag = 0;			//失能自动控制
					if(abs(Speed_toset) <= 10 && abs(angularSpeed_toset) <= 10)Stop_Control();//0	//AS、S太小，停止控制
					else Set_Loop_Para(Speed_toset,angularSpeed_toset);
				}
				else
				{
					//printf("disarmed");
				}
			}
			printf("IC1FallingValue:%d\r\n",IC1FallingValue);			
//			DutyTime3 = IC1FallingValue * unit_time;
//			printf("高电平时间: %fms\r\n",DutyTime3);
			//printf("速度:%f\r\n",Speed_ref);
			TIM_OC3PolarityConfig(TIM4_TIM,TIM_ICPolarity_Rising);
			//TIM_Cmd(TIM4_TIM,ENABLE );
			CCR3_flag = 0;
		}	
		TIM_ClearITPendingBit(TIM4_TIM, TIM_IT_CC3);
	}
	
	/*通道4*/
	if(TIM_GetITStatus(TIM4_TIM,TIM_IT_CC4) != RESET)
	{
		
		if(CCR4_flag == 0)
		{
			TIM_SetCounter(TIM4_TIM,0); //计数器清空		从0开始计数高电平持续时间
			TIM_OC4PolarityConfig(TIM4_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(TIM4_TIM,ENABLE );
			CCR4_flag = 1;
		}
		else
		{
			int temp = TIM_GetCapture4(TIM4_TIM);
			if((temp >= 101)&&(temp <= 202))
			{			
				IC2FallingValue = temp;
				
				angularSpeed_toset = (IC2FallingValue -151)*2.0f;
		//			DutyTime4 = IC2FallingValue * unit_time;
				//printf("IC2FallingValue:%d\r\n",IC2FallingValue);
				//printf("角速度:%f\r\n",Ref_Angular_Speed);
			}
//			printf("高电平时间: %fms\r\n",DutyTime4);
			TIM_OC4PolarityConfig(TIM4_TIM,TIM_ICPolarity_Rising);
			//TIM_Cmd(TIM4_TIM,ENABLE );	 
			CCR4_flag = 0;
		}

		TIM_ClearITPendingBit(TIM4_TIM, TIM_IT_CC4);
	}

}
/*********************************************END OF FILE**********************/

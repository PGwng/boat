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
  
#include "./tim/bsp_advance_tim.h"
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
int CCR3_flag = 0;		//0表示上升沿，1下降沿
int CCR4_flag = 0;		//0表示上升沿，1下降沿
float	unit_time = 1.0f/100;		//单位：ms 定时器4计数频率为100kHz，即计一个数用时1/100000s

static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	//RCC_AHB1PeriphClockCmd (GENERAL_OCPWM_GPIO_CLK, ENABLE); 
	RCC_AHB1PeriphClockCmd (ADVANCE_ICPWM_GPIO_CLK, ENABLE); 	

	/* 定时器复用引脚 */
	//GPIO_PinAFConfig(GENERAL_OCPWM_GPIO_PORT,GENERAL_OCPWM_PINSOURCE,GENERAL_OCPWM_AF); 
	GPIO_PinAFConfig(ADVANCE_ICPWM_GPIO_PORT,ADVANCE_IC1PWM_PINSOURCE,ADVANCE_ICPWM_AF);
  GPIO_PinAFConfig(ADVANCE_ICPWM_GPIO_PORT,ADVANCE_IC2PWM_PINSOURCE,ADVANCE_ICPWM_AF); 	

//	/* 通用定时器PWM输出引脚 */															   
//	GPIO_InitStructure.GPIO_Pin = GENERAL_OCPWM_PIN;	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
//	GPIO_Init(GENERAL_OCPWM_GPIO_PORT, &GPIO_InitStructure);
	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_InitStructure.GPIO_Pin = ADVANCE_IC1PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(ADVANCE_ICPWM_GPIO_PORT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = ADVANCE_IC2PWM_PIN;
	GPIO_Init(ADVANCE_ICPWM_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @brief  高级控制定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQn; 	
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
//static void TIM_PWMOUTPUT_Config(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//  TIM_OCInitTypeDef  TIM_OCInitStructure;
//	
//	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
//  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

//  /* 累计 TIM_Period个后产生一个更新或者中断*/		
//  //当定时器从0计数到9999，即为10000次，为一个定时周期
//  TIM_TimeBaseStructure.TIM_Period = 10000-1;       
//	
//	// 通用定时器2时钟源TIMxCLK = HCLK/2=84MHz 
//	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz  10Hz
//  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;	
//  // 采样时钟分频
//  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//  // 计数方式
//  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
//	
//	// 初始化定时器TIMx, x[1,8]
//	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
//	
//	/* PWM输出模式配置 */
//	/* 配置为PWM模式1 */
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
//	/* PWM脉冲宽度 */
//  TIM_OCInitStructure.TIM_Pulse = 3000-1;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	/* 使能通道1 */
//  TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);	
//	/*使能通道1重载*/
//	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
//	// 使能定时器
//	TIM_Cmd(GENERAL_TIM, ENABLE);	
//}

static void TIM_PWMINPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	// 开启TIMx_CLK,x[1,8] 
	// 开启定时器4时钟	//通用定时器时钟源TIMxCLK = 2*PCLK=84MHz 
  RCC_APB1PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

  TIM_TimeBaseStructure.TIM_Period = 50000-1;			//0xFFFF-1; 			//计数器重装载值50000//65535
	
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;											//设置预分频值	最大计数周期T=1/84M*840*50000 = 500ms
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//采样时钟分频
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
	
	/* IC1捕获：上升沿触发 TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = ADVANCE_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(ADVANCE_TIM, &TIM_ICInitStructure);
  //TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);
	
	/* IC2捕获：下降沿触发 TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = ADVANCE_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;		//TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(ADVANCE_TIM, &TIM_ICInitStructure);
  //TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);
	
//	/* 选择定时器输入触发: TI1FP1 */
//  TIM_SelectInputTrigger(ADVANCE_TIM, TIM_TS_TI1FP1);		

//  /* 选择从模式: 复位模式 */
//  TIM_SelectSlaveMode(ADVANCE_TIM, TIM_SlaveMode_Reset);
//  TIM_SelectMasterSlaveMode(ADVANCE_TIM,TIM_MasterSlaveMode_Enable);

  /* 使能定时器 */
  TIM_Cmd(ADVANCE_TIM, ENABLE);

  /* 使能捕获/比较3 4中断请求 */
  TIM_ITConfig(ADVANCE_TIM, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(ADVANCE_TIM, TIM_IT_CC4, ENABLE);
}


/**
  * @brief  初始化高级控制定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{
	TIMx_GPIO_Config();
	
	TIMx_NVIC_Configuration();	
  
  //TIM_PWMOUTPUT_Config();
	
	TIM_PWMINPUT_Config();
}

void  ADVANCE_TIM_IRQHandler (void)
{
	
	/*通道3*/
	if(TIM_GetITStatus(ADVANCE_TIM,TIM_IT_CC3) != RESET)
	{
		
		if(CCR3_flag == 0)
		{
			TIM_SetCounter(ADVANCE_TIM,0); //计数器清空		从0开始计数高电平持续时间
//			IC1RisingValue = TIM_GetCapture3(ADVANCE_TIM);		//上升沿时计数器的值
//			printf("IC1RisingValue:%d\r\n",IC1RisingValue);
			TIM_OC3PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR3_flag = 1;
		}
		else
		{
			IC1FallingValue = TIM_GetCapture3(ADVANCE_TIM);
			
			/*	参考速度给定
			Speed_ref = IC1FallingValue -101;
			*/
			
			printf("IC1FallingValue:%d\r\n",IC1FallingValue);			
//			DutyTime3 = IC1FallingValue * unit_time;
//			printf("高电平时间: %fms\r\n",DutyTime3);
			TIM_OC3PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Rising);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR3_flag = 0;
		}
//		if(IC1RisingValue < IC1FallingValue)
//		{
//			DutyTime = IC1FallingValue - IC1RisingValue;
//			DutyTime = DutyTime/100000;
//			printf("高电平时间：%f\r\n",DutyTime);	
//		}		
		TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC3);
	}
	
	/*通道4*/
	if(TIM_GetITStatus(ADVANCE_TIM,TIM_IT_CC4) != RESET)
	{
		
		if(CCR4_flag == 0)
		{
			TIM_SetCounter(ADVANCE_TIM,0); //计数器清空		从0开始计数高电平持续时间
			TIM_OC4PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR4_flag = 1;
		}
		else
		{
			IC2FallingValue = TIM_GetCapture4(ADVANCE_TIM);
			/*	参考角速度给定
			AngularSpeed_ref = IC2FallingValue -101;
			*/			
			DutyTime4 = IC2FallingValue * unit_time;
			printf("IC2FallingValue:%d\r\n",IC2FallingValue);
//			printf("高电平时间: %fms\r\n",DutyTime4);
			TIM_OC4PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Rising);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR4_flag = 0;
		}

		TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC4);
	}

}
/*********************************************END OF FILE**********************/

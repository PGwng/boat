/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ͨ�ö�ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_advance_tim.h"
#include "./usart/bsp_debug_usart.h"
#include "motor.h"

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */

__IO int IC1RisingValue = 0;
__IO int IC1FallingValue = 0;
__IO int IC2RisingValue = 0;
__IO int IC2FallingValue = 0;
__IO float DutyTime3 = 0;
__IO float DutyTime4 = 0;
int CCR3_flag = 0;		//0��ʾ�����أ�1�½���
int CCR4_flag = 0;		//0��ʾ�����أ�1�½���
float	unit_time = 1.0f/100;		//��λ��ms ��ʱ��4����Ƶ��Ϊ100kHz������һ������ʱ1/100000s

static void TIMx_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	//RCC_AHB1PeriphClockCmd (GENERAL_OCPWM_GPIO_CLK, ENABLE); 
	RCC_AHB1PeriphClockCmd (ADVANCE_ICPWM_GPIO_CLK, ENABLE); 	

	/* ��ʱ���������� */
	//GPIO_PinAFConfig(GENERAL_OCPWM_GPIO_PORT,GENERAL_OCPWM_PINSOURCE,GENERAL_OCPWM_AF); 
	GPIO_PinAFConfig(ADVANCE_ICPWM_GPIO_PORT,ADVANCE_IC1PWM_PINSOURCE,ADVANCE_ICPWM_AF);
  GPIO_PinAFConfig(ADVANCE_ICPWM_GPIO_PORT,ADVANCE_IC2PWM_PINSOURCE,ADVANCE_ICPWM_AF); 	

//	/* ͨ�ö�ʱ��PWM������� */															   
//	GPIO_InitStructure.GPIO_Pin = GENERAL_OCPWM_PIN;	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
//	GPIO_Init(GENERAL_OCPWM_GPIO_PORT, &GPIO_InitStructure);
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
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
  * @brief  �߼����ƶ�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
//static void TIM_PWMOUTPUT_Config(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//  TIM_OCInitTypeDef  TIM_OCInitStructure;
//	
//	// ����TIMx_CLK,x[2,3,4,5,12,13,14] 
//  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

//  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
//  //����ʱ����0������9999����Ϊ10000�Σ�Ϊһ����ʱ����
//  TIM_TimeBaseStructure.TIM_Period = 10000-1;       
//	
//	// ͨ�ö�ʱ��2ʱ��ԴTIMxCLK = HCLK/2=84MHz 
//	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz  10Hz
//  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;	
//  // ����ʱ�ӷ�Ƶ
//  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
//  // ������ʽ
//  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
//	
//	// ��ʼ����ʱ��TIMx, x[1,8]
//	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
//	
//	/* PWM���ģʽ���� */
//	/* ����ΪPWMģʽ1 */
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
//	/* PWM������ */
//  TIM_OCInitStructure.TIM_Pulse = 3000-1;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	/* ʹ��ͨ��1 */
//  TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);	
//	/*ʹ��ͨ��1����*/
//	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
//	// ʹ�ܶ�ʱ��
//	TIM_Cmd(GENERAL_TIM, ENABLE);	
//}

static void TIM_PWMINPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	// ����TIMx_CLK,x[1,8] 
	// ������ʱ��4ʱ��	//ͨ�ö�ʱ��ʱ��ԴTIMxCLK = 2*PCLK=84MHz 
  RCC_APB1PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

  TIM_TimeBaseStructure.TIM_Period = 50000-1;			//0xFFFF-1; 			//��������װ��ֵ50000//65535
	
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;											//����Ԥ��Ƶֵ	����������T=1/84M*840*50000 = 500ms
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//����ʱ�ӷ�Ƶ
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
	
	/* IC1���������ش��� TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = ADVANCE_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(ADVANCE_TIM, &TIM_ICInitStructure);
  //TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);
	
	/* IC2�����½��ش��� TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = ADVANCE_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;		//TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(ADVANCE_TIM, &TIM_ICInitStructure);
  //TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);
	
//	/* ѡ��ʱ�����봥��: TI1FP1 */
//  TIM_SelectInputTrigger(ADVANCE_TIM, TIM_TS_TI1FP1);		

//  /* ѡ���ģʽ: ��λģʽ */
//  TIM_SelectSlaveMode(ADVANCE_TIM, TIM_SlaveMode_Reset);
//  TIM_SelectMasterSlaveMode(ADVANCE_TIM,TIM_MasterSlaveMode_Enable);

  /* ʹ�ܶ�ʱ�� */
  TIM_Cmd(ADVANCE_TIM, ENABLE);

  /* ʹ�ܲ���/�Ƚ�3 4�ж����� */
  TIM_ITConfig(ADVANCE_TIM, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(ADVANCE_TIM, TIM_IT_CC4, ENABLE);
}


/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
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
	
	/*ͨ��3*/
	if(TIM_GetITStatus(ADVANCE_TIM,TIM_IT_CC3) != RESET)
	{
		
		if(CCR3_flag == 0)
		{
			TIM_SetCounter(ADVANCE_TIM,0); //���������		��0��ʼ�����ߵ�ƽ����ʱ��
//			IC1RisingValue = TIM_GetCapture3(ADVANCE_TIM);		//������ʱ��������ֵ
//			printf("IC1RisingValue:%d\r\n",IC1RisingValue);
			TIM_OC3PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR3_flag = 1;
		}
		else
		{
			IC1FallingValue = TIM_GetCapture3(ADVANCE_TIM);
			
			/*	�ο��ٶȸ���
			Speed_ref = IC1FallingValue -101;
			*/
			
			printf("IC1FallingValue:%d\r\n",IC1FallingValue);			
//			DutyTime3 = IC1FallingValue * unit_time;
//			printf("�ߵ�ƽʱ��: %fms\r\n",DutyTime3);
			TIM_OC3PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Rising);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR3_flag = 0;
		}
//		if(IC1RisingValue < IC1FallingValue)
//		{
//			DutyTime = IC1FallingValue - IC1RisingValue;
//			DutyTime = DutyTime/100000;
//			printf("�ߵ�ƽʱ�䣺%f\r\n",DutyTime);	
//		}		
		TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC3);
	}
	
	/*ͨ��4*/
	if(TIM_GetITStatus(ADVANCE_TIM,TIM_IT_CC4) != RESET)
	{
		
		if(CCR4_flag == 0)
		{
			TIM_SetCounter(ADVANCE_TIM,0); //���������		��0��ʼ�����ߵ�ƽ����ʱ��
			TIM_OC4PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR4_flag = 1;
		}
		else
		{
			IC2FallingValue = TIM_GetCapture4(ADVANCE_TIM);
			/*	�ο����ٶȸ���
			AngularSpeed_ref = IC2FallingValue -101;
			*/			
			DutyTime4 = IC2FallingValue * unit_time;
			printf("IC2FallingValue:%d\r\n",IC2FallingValue);
//			printf("�ߵ�ƽʱ��: %fms\r\n",DutyTime4);
			TIM_OC4PolarityConfig(ADVANCE_TIM,TIM_ICPolarity_Rising);
			TIM_Cmd(ADVANCE_TIM,ENABLE );
			CCR4_flag = 0;
		}

		TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC4);
	}

}
/*********************************************END OF FILE**********************/

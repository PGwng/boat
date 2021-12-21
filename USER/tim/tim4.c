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
  
#include "user.h"
#include "./tim/tim4.h"
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
int CCR2_flag = 0;		//0��ʾ�����أ�1�½���
int CCR3_flag = 0;		//0��ʾ�����أ�1�½���
int CCR4_flag = 0;		//0��ʾ�����أ�1�½���
float	unit_time = 1.0f/100;		//��λ��ms ��ʱ��4����Ƶ��Ϊ100kHz������һ������ʱ1/100000s
float Speed_toset = 0;
float angularSpeed_toset = 0;
u8 remote_device_flag = 0;

static void TIMx_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (TIM4_ICPWM_GPIO_CLK, ENABLE); 
	RCC_AHB1PeriphClockCmd (TIM4_IC3PWM_GPIO_CLK, ENABLE);	

	/* ��ʱ���������� */
	GPIO_PinAFConfig(TIM4_ICPWM_GPIO_PORT,TIM4_IC1PWM_PINSOURCE,TIM4_ICPWM_AF);		//PB8
  GPIO_PinAFConfig(TIM4_ICPWM_GPIO_PORT,TIM4_IC2PWM_PINSOURCE,TIM4_ICPWM_AF); 	//PB9
	GPIO_PinAFConfig(TIM4_IC3PWM_GPIO_PORT,TIM4_IC3PWM_PINSOURCE,TIM4_ICPWM_AF); 	//PD13
	
	/* �߼����ƶ�ʱ��PWM���벶������ */
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
  * @brief  ��ʱ�� TIMx�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_TIM_IRQn; 	
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
static void TIM_PWMINPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	// ����TIMx_CLK,
	// ������ʱ��4ʱ��	//ͨ�ö�ʱ��ʱ��ԴTIMxCLK = 2*PCLK=84MHz 
  RCC_APB1PeriphClockCmd(TIM4_TIM_CLK, ENABLE); 

  TIM_TimeBaseStructure.TIM_Period = 50000-1;			//0xFFFF-1; 			//��������װ��ֵ50000//65535
	
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;											//����Ԥ��Ƶֵ	����������T=(1/84M)*840*50000 = 500ms
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//����ʱ�ӷ�Ƶ
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(TIM4_TIM, &TIM_TimeBaseStructure);
	
	/* IC1���������ش��� TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = TIM4_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM4_TIM, &TIM_ICInitStructure);
	
	/* IC2���������ش��� TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = TIM4_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;		//TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM4_TIM, &TIM_ICInitStructure);
	/* IC3���������ش��� TI1FP2*/
	TIM_ICInitStructure.TIM_Channel = TIM4_IC3PWM_CHANNEL;
	TIM_ICInit(TIM4_TIM, &TIM_ICInitStructure);
	
  /* ʹ�ܶ�ʱ�� */
  TIM_Cmd(TIM4_TIM, ENABLE);

  /* ʹ�ܲ���/�Ƚ�2 3 4�ж����� */
  TIM_ITConfig(TIM4_TIM, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM4_TIM, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM4_TIM, TIM_IT_CC2, ENABLE);
}


/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIM4_Configuration(void)
{
	TIMx_GPIO_Config();
	
	TIMx_NVIC_Configuration();	
	
	TIM_PWMINPUT_Config();
}

void  TIM4_TIM_IRQHandler (void)
{
		/*ͨ��2*/
	if(TIM_GetITStatus(TIM4_TIM,TIM_IT_CC2) != RESET)
	{
		
		if(CCR2_flag == 0)
		{
			TIM_SetCounter(TIM4_TIM,0); //���������		��0��ʼ�����ߵ�ƽ����ʱ��
			TIM_OC2PolarityConfig(TIM4_TIM,TIM_ICPolarity_Falling);
			TIM_Cmd(TIM4_TIM,ENABLE);
			CCR2_flag = 1;
		}
		else
		{
			int temp = TIM_GetCapture2(TIM4_TIM);
			if(temp==101 || temp==100)
			{			
				remote_device_flag = 0; //ң��������
				//printf("IC3FallingValue:%d\r\n",temp);
			}
			else if(temp==201 || temp==202)
			{
				remote_device_flag = 1; //ң��������
				//printf("IC3FallingValue:%d\r\n",temp);
			}				
			TIM_OC2PolarityConfig(TIM4_TIM,TIM_ICPolarity_Rising);
			//TIM_Cmd(TIM4_TIM,ENABLE );
			CCR2_flag = 0;
		}	
		TIM_ClearITPendingBit(TIM4_TIM, TIM_IT_CC2);
	}
	/*ͨ��3*/
	if(TIM_GetITStatus(TIM4_TIM,TIM_IT_CC3) != RESET)
	{
		
		if(CCR3_flag == 0)
		{
			TIM_SetCounter(TIM4_TIM,0); //���������		��0��ʼ�����ߵ�ƽ����ʱ��
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
					auto_flag = 0;			//ʧ���Զ�����
					if(abs(Speed_toset) <= 10 && abs(angularSpeed_toset) <= 10)Stop_Control();//0	//AS��S̫С��ֹͣ����
					else Set_Loop_Para(Speed_toset,angularSpeed_toset);
				}
				else
				{
					//printf("disarmed");
				}
			}
			printf("IC1FallingValue:%d\r\n",IC1FallingValue);			
//			DutyTime3 = IC1FallingValue * unit_time;
//			printf("�ߵ�ƽʱ��: %fms\r\n",DutyTime3);
			//printf("�ٶ�:%f\r\n",Speed_ref);
			TIM_OC3PolarityConfig(TIM4_TIM,TIM_ICPolarity_Rising);
			//TIM_Cmd(TIM4_TIM,ENABLE );
			CCR3_flag = 0;
		}	
		TIM_ClearITPendingBit(TIM4_TIM, TIM_IT_CC3);
	}
	
	/*ͨ��4*/
	if(TIM_GetITStatus(TIM4_TIM,TIM_IT_CC4) != RESET)
	{
		
		if(CCR4_flag == 0)
		{
			TIM_SetCounter(TIM4_TIM,0); //���������		��0��ʼ�����ߵ�ƽ����ʱ��
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
				//printf("���ٶ�:%f\r\n",Ref_Angular_Speed);
			}
//			printf("�ߵ�ƽʱ��: %fms\r\n",DutyTime4);
			TIM_OC4PolarityConfig(TIM4_TIM,TIM_ICPolarity_Rising);
			//TIM_Cmd(TIM4_TIM,ENABLE );	 
			CCR4_flag = 0;
		}

		TIM_ClearITPendingBit(TIM4_TIM, TIM_IT_CC4);
	}

}
/*********************************************END OF FILE**********************/

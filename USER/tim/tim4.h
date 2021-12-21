#ifndef TIM4_H
#define	TIM4_H

#include "stm32f4xx.h"

extern int CCR3_flag;

/* ��ʱ��PWM���벶�� */
/* PWM���벶������ */
#define TIM4_IC1PWM_PIN             GPIO_Pin_8  
#define TIM4_IC2PWM_PIN             GPIO_Pin_9 
#define TIM4_ICPWM_GPIO_PORT       	GPIOB                      
#define TIM4_ICPWM_GPIO_CLK        	RCC_AHB1Periph_GPIOB
#define TIM4_IC1PWM_PINSOURCE				GPIO_PinSource8
#define TIM4_IC2PWM_PINSOURCE				GPIO_PinSource9
#define TIM4_ICPWM_AF								GPIO_AF_TIM4
#define TIM4_IC1PWM_CHANNEL        	TIM_Channel_3
#define TIM4_IC2PWM_CHANNEL        	TIM_Channel_4

#define TIM4_IC3PWM_PIN             GPIO_Pin_13  
#define TIM4_IC3PWM_GPIO_PORT       GPIOD                      
#define TIM4_IC3PWM_GPIO_CLK        RCC_AHB1Periph_GPIOD
#define TIM4_IC3PWM_PINSOURCE				GPIO_PinSource13
#define TIM4_IC3PWM_CHANNEL        	TIM_Channel_2

/* �߼����ƶ�ʱ�� */
#define TIM4_TIM           		    	TIM4
#define TIM4_TIM_CLK       		   	 	RCC_APB1Periph_TIM4

/* ����/�Ƚ��ж� */
#define TIM4_TIM_IRQn					    	TIM4_IRQn
#define TIM4_TIM_IRQHandler        	TIM4_IRQHandler

//extern float unit_time;
void TIM4_Configuration(void);

#endif 


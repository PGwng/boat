#ifndef __TIM_H
#define	__TIM_H

#include "stm32f4xx.h"

///* ͨ�ö�ʱ��PWM��� */
///* PWM������� */
//#define GENERAL_OCPWM_PIN             GPIO_Pin_5              
//#define GENERAL_OCPWM_GPIO_PORT       GPIOA                      
//#define GENERAL_OCPWM_GPIO_CLK        RCC_AHB1Periph_GPIOA
//#define GENERAL_OCPWM_PINSOURCE				GPIO_PinSource5
//#define GENERAL_OCPWM_AF							GPIO_AF_TIM2

///* ͨ�ö�ʱ�� */
//#define GENERAL_TIM           		    TIM2
//#define GENERAL_TIM_CLK       		    RCC_APB1Periph_TIM2

/* �߼����ƶ�ʱ��PWM���벶�� */
/* PWM���벶������ */
#define GENERAL_IC1PWM_PIN             GPIO_Pin_8  
#define GENERAL_IC2PWM_PIN             GPIO_Pin_9 
#define GENERAL_ICPWM_GPIO_PORT       GPIOB                      
#define GENERAL_ICPWM_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define GENERAL_IC1PWM_PINSOURCE				GPIO_PinSource8
#define GENERAL_IC2PWM_PINSOURCE				GPIO_PinSource9
#define GENERAL_ICPWM_AF							GPIO_AF_TIM4
#define GENERAL_IC1PWM_CHANNEL        TIM_Channel_3
#define GENERAL_IC2PWM_CHANNEL        TIM_Channel_4

/* �߼����ƶ�ʱ�� */
#define GENERAL_TIM           		    TIM4
#define GENERAL_TIM_CLK       		    RCC_APB1Periph_TIM4

/* ����/�Ƚ��ж� */
#define GENERAL_TIM_IRQn					    TIM4_IRQn
#define GENERAL_TIM_IRQHandler        TIM4_IRQHandler

//extern float unit_time;
void TIM4_Configuration(void);

#endif 


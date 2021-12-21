#ifndef __BSP_ADVANCE_TIM_H
#define	__BSP_ADVANCE_TIM_H

#include "stm32f4xx.h"

///* 通用定时器PWM输出 */
///* PWM输出引脚 */
//#define GENERAL_OCPWM_PIN             GPIO_Pin_5              
//#define GENERAL_OCPWM_GPIO_PORT       GPIOA                      
//#define GENERAL_OCPWM_GPIO_CLK        RCC_AHB1Periph_GPIOA
//#define GENERAL_OCPWM_PINSOURCE				GPIO_PinSource5
//#define GENERAL_OCPWM_AF							GPIO_AF_TIM2

///* 通用定时器 */
//#define GENERAL_TIM           		    TIM2
//#define GENERAL_TIM_CLK       		    RCC_APB1Periph_TIM2

/* 高级控制定时器PWM输入捕获 */
/* PWM输入捕获引脚 */
#define ADVANCE_IC1PWM_PIN             GPIO_Pin_8  
#define ADVANCE_IC2PWM_PIN             GPIO_Pin_9 
#define ADVANCE_ICPWM_GPIO_PORT       GPIOB                      
#define ADVANCE_ICPWM_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define ADVANCE_IC1PWM_PINSOURCE				GPIO_PinSource8
#define ADVANCE_IC2PWM_PINSOURCE				GPIO_PinSource9
#define ADVANCE_ICPWM_AF							GPIO_AF_TIM4
#define ADVANCE_IC1PWM_CHANNEL        TIM_Channel_3
#define ADVANCE_IC2PWM_CHANNEL        TIM_Channel_4

/* 高级控制定时器 */
#define ADVANCE_TIM           		    TIM4
#define ADVANCE_TIM_CLK       		    RCC_APB1Periph_TIM4

/* 捕获/比较中断 */
#define ADVANCE_TIM_IRQn					    TIM4_IRQn
#define ADVANCE_TIM_IRQHandler        TIM4_IRQHandler

//extern float unit_time;
void TIMx_Configuration(void);

#endif 


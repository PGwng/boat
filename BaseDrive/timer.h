#ifndef __TIMER_H
#define __TIMER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "user.h"
#include "common.h"


//extern uint16_t gl_jsflag[5];
//extern void PWM1_Init(void);
extern void PWM2_Init(void);
extern void PWM3_Init(void);
void Loop(void);
extern void Timer_Init(void);
extern void TIM5_IRQHandler(void);

extern void Pwm_Set(TIM_TypeDef* TIMx, uint8_t i, uint16_t duty);
extern uint16_t Pwm_Read(TIM_TypeDef* TIMx, uint8_t i);

extern uint8_t S_B_Flag;//S_B_Flag=0:不发送  S_B_Flag=1:向左船发送  S_B_Flag=2:向右船发送   
#ifdef __cplusplus
}
#endif
#endif

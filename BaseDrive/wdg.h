#ifndef WDG_H
#define WDG_H
#ifdef __cplusplus
extern "C" {
#endif
#include"stm32f4xx_iwdg.h"

extern void IWDG_Init(uint8_t prer,uint16_t rlr);//IWDG��ʼ��
extern void IWDG_Feed(void);  //ι������
#ifdef __cplusplus
}
#endif
#endif

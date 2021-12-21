#include "wdg.h"
void IWDG_Init(uint8_t prer,uint16_t rlr){
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ�ܶ�IWDG->PR IWDG->RLR��д
	IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��
	IWDG_SetReload(rlr); //����IWDGװ��ֵ
	IWDG_ReloadCounter(); //reload
	IWDG_Enable(); //ʹ�ܿ��Ź�
}
void IWDG_Feed(void){
	IWDG_ReloadCounter();//reload
}

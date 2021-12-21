#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "user.h"

extern void USER_ADC_Init(void);
void ADC_GPIO_Configuration(void);
void ADC1Init(void);
void ADCInit_DMA(void);
	
void ADC3_GPIO_Configuration(void);
void ADC3Init(void);
static void ADC3Init_DMA(void);
	
extern void Voltage_Storage(void);
extern void Temperature_Storage(void);
#ifdef __cplusplus
}
#endif
#endif /* USER_ADC_H_ */

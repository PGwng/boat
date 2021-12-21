#ifndef INIT_H
#define INIT_H
#ifdef __cplusplus
extern "C" {
#endif
#include "user.h"
#include "../USER/imu/imu.h"

#define LED1_ON()  	GPIO_ResetBits(GPIOG,GPIO_Pin_13)
#define LED1_OFF()  GPIO_SetBits(GPIOG,GPIO_Pin_13)
#define LED1_TOG()  GPIO_ToggleBits(GPIOG,GPIO_Pin_13)

#define LED2_ON() 	GPIO_ResetBits(GPIOG,GPIO_Pin_14)
#define LED2_OFF()	GPIO_SetBits(GPIOG,GPIO_Pin_14)
#define LED2_TOG()  GPIO_ToggleBits(GPIOG,GPIO_Pin_14)
	
#define MOS_1							GPIO_Pin_3//greed
#define MOS_1_GPIO_Port		GPIOD
	
#define MOS_2							GPIO_Pin_4//yellow
#define MOS_2_GPIO_Port		GPIOD
	
#define MOS_3							GPIO_Pin_5//red
#define MOS_3_GPIO_Port		GPIOD
	
#define MOS_4							GPIO_Pin_6
#define MOS_4_GPIO_Port		GPIOD

#define Relay1_On()  {GPIO_SetBits(MOS_1_GPIO_Port,MOS_1);\
											GPIO_ResetBits(MOS_2_GPIO_Port,MOS_2);\
											GPIO_ResetBits(MOS_3_GPIO_Port,MOS_3);}

#define Relay1_Off()	{GPIO_ResetBits(MOS_1_GPIO_Port,MOS_1);\
											GPIO_ResetBits(MOS_2_GPIO_Port,MOS_2);\
											GPIO_ResetBits(MOS_3_GPIO_Port,MOS_3);}

#define Relay2_On()  {GPIO_ResetBits(MOS_1_GPIO_Port,MOS_1);\
											GPIO_SetBits(MOS_2_GPIO_Port,MOS_2);\
											GPIO_ResetBits(MOS_3_GPIO_Port,MOS_3);}

#define Relay2_Off()	{GPIO_ResetBits(MOS_1_GPIO_Port,MOS_1);\
											GPIO_ResetBits(MOS_2_GPIO_Port,MOS_2);\
											GPIO_ResetBits(MOS_3_GPIO_Port,MOS_3);}

#define Relay3_On()  {GPIO_ResetBits(MOS_1_GPIO_Port,MOS_1);\
											GPIO_ResetBits(MOS_2_GPIO_Port,MOS_2);\
											GPIO_SetBits(MOS_3_GPIO_Port,MOS_3);}

#define Relay3_Off()	{GPIO_ResetBits(MOS_1_GPIO_Port,MOS_1);\
											GPIO_ResetBits(MOS_2_GPIO_Port,MOS_2);\
											GPIO_ResetBits(MOS_3_GPIO_Port,MOS_3);}



#define Relay4_On()  GPIO_SetBits(MOS_4_GPIO_Port,MOS_4)//»µmos
#define Relay4_Off() GPIO_ResetBits(MOS_4_GPIO_Port,MOS_4)
#define Relay4_TOG() GPIO_ToggleBits(MOS_4_GPIO_Port,MOS_4)

extern void SystemInitUser(void);
void NVIC_Config(void);
void LED_Init(void);
void MOS_Init(void);
void GPIO_standby(void);

extern IMU imu;
extern u8 DMA2packBuf[100];
	
#ifdef __cplusplus
}
#endif
#endif

#ifndef __LT8920_H__
#define __LT8920_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "user.h"
#include "stm32f4xx_spi.h"

//#define RF_EMITTER               0  //1--遥控器   0 -- 接收器

typedef struct
{
	uint8_t subaddr;
	uint16_t value;
}RFSetting_Data_Type; 

//typedef enum
//{
//	Nothing = 0,
//	dimRGB,
//	dimCCT,
//	dimBright,
//	dimSpeed,
//}RF_DIM_Type;

//typedef union
//{
//	struct
//	{
//		RF_DIM_Type SliderDIM :4;
//		RF_DIM_Type RingDIM   :4;
//	}BIT;
//	uint8_t All;
//}RF_Ring_Slider_DIM_Type;

//typedef union
//{
//	struct
//	{
//		uint8_t  Channel : 8;
//		uint32_t UID     : 24;
//	}BIT;
//	uint32_t Data;
//}RF_DEVICE_ID_Type;

//typedef union 
//{
//	struct
//	{
//		uint8_t                 Header;
//		RF_DEVICE_ID_Type       Device_UID;
//		RF_Ring_Slider_DIM_Type Ring_Slider_DimType;
//		uint8_t                 RingValue;
//		uint8_t                 SliderValue;
//		uint8_t                 KeyValue;
//		uint8_t                 WaterCode;
//	}BIT;
//	uint8_t Data[10];
//}RF_Data_Type;

#define SPI_RST_Pin            GPIO_Pin_15
#define SPI_RST_GPIO_Port      GPIOD

#define SPI_NSS_Pin            GPIO_Pin_11
#define SPI_NSS_GPIO_Port      GPIOD

#define SPI_CLK_Pin            GPIO_Pin_12
#define SPI_CLK_GPIO_Port      GPIOD

#define SPI_MISO_Pin           GPIO_Pin_14
#define SPI_MISO_GPIO_Port     GPIOD

#define SPI_MOSI_Pin           GPIO_Pin_13
#define SPI_MOSI_GPIO_Port     GPIOD

#define NSS_EN                 GPIO_ResetBits(SPI_NSS_GPIO_Port,SPI_NSS_Pin)
#define NSS_DIS                GPIO_SetBits(SPI_NSS_GPIO_Port,SPI_NSS_Pin)

#define CLK_H                  GPIO_SetBits(SPI_CLK_GPIO_Port,SPI_CLK_Pin)
#define CLK_L                  GPIO_ResetBits(SPI_CLK_GPIO_Port,SPI_CLK_Pin)

#define MISO                   GPIO_ReadInputDataBit(SPI_MISO_GPIO_Port,SPI_MISO_Pin)

#define MOSI_H                 GPIO_SetBits(SPI_MOSI_GPIO_Port,SPI_MOSI_Pin)
#define MOSI_L                 GPIO_ResetBits(SPI_MOSI_GPIO_Port,SPI_MOSI_Pin)

extern uint8_t RX_LT8920_RAW[10];
extern uint8_t RX_LT8920[10];

extern void LT8920_SPI_Init(void);
extern void LT8920_Init(void);
extern void LT8920_Set_Reg(uint8_t reg, uint16_t value);
extern void LT8920_Read_Reg(uint8_t reg, uint8_t* pdata, uint16_t length);
extern uint16_t LT8920_ReadByte_Reg(uint8_t reg);
void RF_Receive_Scan(void);
extern void RF_Init(void);
extern uint8_t RF_Receive_Data(void);
extern void RF_Receive_Scan(void);
#ifdef __cplusplus
}
#endif

#endif

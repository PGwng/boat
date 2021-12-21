#include "adc.h"

uint16_t uAD_Buff[4] = {0,0,0,0};

float voltage_battery = 0;
float mcu_temperature = 0;

void USER_ADC_Init(void){
//	ADC_GPIO_Configuration();
//	ADCInit_DMA();
//	ADC1Init();	
	ADC3_GPIO_Configuration();
	ADC3Init_DMA();
	ADC3Init();
}/*
//PC5:ADC1 ADC_Channel_15 uAD_Buff[0]      改成 PF5 ADC3 ADC_Channel_15 uAD_Buff[0]
//mcu温度传感器 ADC1 ADC_Channel_16 uAD_Buff[1]
//PB0:ADC1 ADC_Channel_8  uAD_Buff[2]
//PB1:ADC1 ADC_Channel_9  uAD_Buff[3]
void ADC_GPIO_Configuration(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void ADC1Init(void){
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);//复位结束	
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_TempSensorVrefintCmd(ENABLE);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//开启扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//开启连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 4;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_84Cycles); //PC5  电压
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_84Cycles); //与mcu内部的温度传感器连在一起
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_84Cycles); //PB0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_84Cycles); //PB1
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
}
//ADC1:DMA2_Stream4 DMA_Channel_0
static void ADCInit_DMA(void){
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(u16 *)(&uAD_Buff[0]);//&uAD_Buff[0] = uAD_Buff 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 1 * 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//半字 16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//半字 16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream4, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream4, ENABLE);
}
*/
#define VOLTAGE_RATE1	21.0
void Voltage_Storage(void){
	uint16_t temp = 0;
	float temp1 = 0,sum_vl=0;
	static float vl[20] = {0};
	static uint8_t i=0,j=0;
	uint8_t k =0;
	
	temp = uAD_Buff[0];
	vl[i] = temp * 3.30 / 4095 * VOLTAGE_RATE1;
	if(i<20)
	{
		i++;
		if(j<20)j++;
	}
	if(i>=20)i=0;
	for(k=0;k<j;k++)sum_vl += vl[k];
	if(j)temp1 = sum_vl/j;
	voltage_battery = temp1;
	if(voltage_battery>22.2f)				{Relay1_On();}
	else if(voltage_battery>20.4f)		{Relay2_On();}
	else 													{Relay3_On();}
	
}

void Temperature_Storage(void){
	uint16_t temp = 0;
	float temp1 = 0, sum_vl = 0, temp2 = 0;
	static float vl[20] = {0};
	static uint8_t i=0,j=0;
	uint8_t k =0;
	
	temp = uAD_Buff[1];
	vl[i] = temp * 3.30 / 4095;
	if(i<20)
	{
		i++;
		if(j<20)j++;
	}
	if(i>=20)i=0;
	for(k=0;k<j;k++)sum_vl += vl[k];
	if(j)temp1 = sum_vl/j;
	temp2 = (temp1-0.76f)/0.0025f+25;
	mcu_temperature = ((int)(temp2*10))/10.0f;
}
//PF5 ADC3 ADC_Channel_15 uAD_Buff[0]
void ADC3_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化结构体
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PF5 端口5         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不带上下拉  
	GPIO_Init(GPIOF, &GPIO_InitStructure);//GPIOF初始化
}
void ADC3Init(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;//ADC3通用配置结构体
	ADC_InitTypeDef ADC_InitStructure;//ADC3初始化结构体
	//开启ADC3相关时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);//开启ADC3时钟
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, ENABLE);//ADC3复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, DISABLE);//ADC3复位结束	
	//ADC3通用配置
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;//两个采样阶段之间的延时为10个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;//预分频为8分频
	ADC_CommonInit(&ADC_CommonInitStructure);//初始化ADC3函数
	
	ADC_TempSensorVrefintCmd(ENABLE);//启用温度传感器和Vrefint通道
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//关闭扫描模式	 目前只采集一个通道 并不需要扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//开启连续转换 如果不开启连续转换 那么每次转换前都要代码开启一次  连续转换相比单次转换会耗费更多资源 但是使用DMA的话 并不影响CPU效率
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//不使用外部触发检测 使用软件触发 ADC_SoftwareStartConv(ADC3);
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;//如果使用外部触发检测 这里是配置外部触发的类型    这里未用到外部触发，实际上这个可以不配置
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;//ADC3规则转换通道的长度 这里是单次转换 只有1个通道    如果是扫描模式，则改成相应的数量即可 比如有4个规则通道 扫描模式 就改成4
	ADC_Init(ADC3, &ADC_InitStructure);//ADC3初始化
	ADC_RegularChannelConfig(ADC3, ADC_Channel_15, 1, ADC_SampleTime_84Cycles); //PF5  电压  //ADC3,ADC通道,84个周期,提高采样时间可以提高精确度		采电池电压 不需要这么精确 快一点没问题
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 2, ADC_SampleTime_84Cycles); //如果用到扫描模式 则在这里加上ADC通道即可
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 3, ADC_SampleTime_84Cycles); //
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 4, ADC_SampleTime_84Cycles); //
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);//在最后一次传输后启用禁用ADC DMA请求(单ADC模式)
	ADC_DMACmd(ADC3, ENABLE);//启用ADC3 DMA
	ADC_Cmd(ADC3, ENABLE);//启用ADC3
	ADC_SoftwareStartConv(ADC3);//开启ADC3转化
}
//ADC3:DMA2_Stream0 DMA_Channel_2
static void ADC3Init_DMA(void)
{
	DMA_InitTypeDef  DMA_InitStructure;//DMA初始化结构体 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2时钟使能

	DMA_InitStructure.DMA_Channel = DMA_Channel_2;//通道2
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC3->DR);//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(u16 *)(&uAD_Buff[0]);//&uAD_Buff[0] = uAD_Buff 内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//DMA方向 外设->内存
	DMA_InitStructure.DMA_BufferSize = 1;//数据传输量 1个  只用了1个通道才电压
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//内存地址不变
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//半字 16位  因为ADC采样的返回值是16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//半字 16位 因为ADC采样的返回值是16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式  循环接收
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//优先级中
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//失能FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//使能FIFO时才需要设置 这里失能FIFO 实际上可以不用设置
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);//DMA初始化
	DMA_Cmd(DMA2_Stream0, ENABLE);//使能DMA
}

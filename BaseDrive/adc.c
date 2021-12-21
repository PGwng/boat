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
//PC5:ADC1 ADC_Channel_15 uAD_Buff[0]      �ĳ� PF5 ADC3 ADC_Channel_15 uAD_Buff[0]
//mcu�¶ȴ����� ADC1 ADC_Channel_16 uAD_Buff[1]
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
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);//ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);//��λ����	
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_TempSensorVrefintCmd(ENABLE);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//����ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//��������ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 4;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_84Cycles); //PC5  ��ѹ
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_84Cycles); //��mcu�ڲ����¶ȴ���������һ��
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
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���� 16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���� 16λ
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
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO��ʼ���ṹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PF5 �˿�5         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����������  
	GPIO_Init(GPIOF, &GPIO_InitStructure);//GPIOF��ʼ��
}
void ADC3Init(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;//ADC3ͨ�����ýṹ��
	ADC_InitTypeDef ADC_InitStructure;//ADC3��ʼ���ṹ��
	//����ADC3���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);//����ADC3ʱ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, ENABLE);//ADC3��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, DISABLE);//ADC3��λ����	
	//ADC3ͨ������
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;//���������׶�֮�����ʱΪ10��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMAʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;//Ԥ��ƵΪ8��Ƶ
	ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��ADC3����
	
	ADC_TempSensorVrefintCmd(ENABLE);//�����¶ȴ�������Vrefintͨ��
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//�ر�ɨ��ģʽ	 Ŀǰֻ�ɼ�һ��ͨ�� ������Ҫɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//��������ת�� �������������ת�� ��ôÿ��ת��ǰ��Ҫ���뿪��һ��  ����ת����ȵ���ת����ķѸ�����Դ ����ʹ��DMA�Ļ� ����Ӱ��CPUЧ��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ʹ���ⲿ������� ʹ��������� ADC_SoftwareStartConv(ADC3);
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;//���ʹ���ⲿ������� �����������ⲿ����������    ����δ�õ��ⲿ������ʵ����������Բ�����
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;//ADC3����ת��ͨ���ĳ��� �����ǵ���ת�� ֻ��1��ͨ��    �����ɨ��ģʽ����ĳ���Ӧ���������� ������4������ͨ�� ɨ��ģʽ �͸ĳ�4
	ADC_Init(ADC3, &ADC_InitStructure);//ADC3��ʼ��
	ADC_RegularChannelConfig(ADC3, ADC_Channel_15, 1, ADC_SampleTime_84Cycles); //PF5  ��ѹ  //ADC3,ADCͨ��,84������,��߲���ʱ�������߾�ȷ��		�ɵ�ص�ѹ ����Ҫ��ô��ȷ ��һ��û����
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 2, ADC_SampleTime_84Cycles); //����õ�ɨ��ģʽ �����������ADCͨ������
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 3, ADC_SampleTime_84Cycles); //
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 4, ADC_SampleTime_84Cycles); //
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);//�����һ�δ�������ý���ADC DMA����(��ADCģʽ)
	ADC_DMACmd(ADC3, ENABLE);//����ADC3 DMA
	ADC_Cmd(ADC3, ENABLE);//����ADC3
	ADC_SoftwareStartConv(ADC3);//����ADC3ת��
}
//ADC3:DMA2_Stream0 DMA_Channel_2
static void ADC3Init_DMA(void)
{
	DMA_InitTypeDef  DMA_InitStructure;//DMA��ʼ���ṹ�� 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2ʱ��ʹ��

	DMA_InitStructure.DMA_Channel = DMA_Channel_2;//ͨ��2
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC3->DR);//�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(u16 *)(&uAD_Buff[0]);//&uAD_Buff[0] = uAD_Buff �ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//DMA���� ����->�ڴ�
	DMA_InitStructure.DMA_BufferSize = 1;//���ݴ����� 1��  ֻ����1��ͨ���ŵ�ѹ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���� 16λ  ��ΪADC�����ķ���ֵ��16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���� 16λ ��ΪADC�����ķ���ֵ��16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ģʽ  ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//���ȼ���
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//ʧ��FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//ʹ��FIFOʱ����Ҫ���� ����ʧ��FIFO ʵ���Ͽ��Բ�������
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);//DMA��ʼ��
	DMA_Cmd(DMA2_Stream0, ENABLE);//ʹ��DMA
}

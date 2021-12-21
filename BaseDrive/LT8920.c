#include "LT8920.h" 

static const RFSetting_Data_Type LT8920InitValue[] ={
{ 0,  0x6FE0 },
{ 1,  0x5681 },
{ 2,  0x6617 },
{ 4,  0x9CC9 },
{ 5,  0x6637 },
{ 7,  0x0030 },//芯片状态和RF频率
{ 8,  0x6C90 },
{ 9,  0x4800 },//设置发射功率
{ 10, 0x7FFD },
{ 11, 0x0008 },
{ 12, 0x0000 },
{ 13, 0x48BD },

{ 22, 0x00FF },
{ 23, 0x8005 },//8005
{ 24, 0x0067 },
{ 25, 0x1659 },
{ 26, 0x19E0 },
{ 27, 0x1300 },
{ 28, 0x1800 },

{ 32, 0x4800 },//15:13,12:11,10:8,7:6,3:1
{ 33, 0x3FC7 },
{ 34, 0x2000 },
{ 35, 0x0000 },
{ 36, 0x0380 },
{ 37, 0x0380 },
{ 38, 0x5A5A },
{ 39, 0x0380 },

{ 40, 0x4402 },//0x4402
{ 41, 0xB400 },//0xB400
{ 42, 0xFDB0 },
{ 43, 0x000F },
{ 44, 0x1000 },
{ 45, 0x0552 },
};

//static const uint16_t RFChTable[3] = {0x00A2,0x00CA,0x0F2};
//static uint8_t RF_Scan_Channel = 1;

//static uint8_t RF_Scan_Channel = 1;
static uint8_t sync[2]={0x10,0x01};
static uint8_t ff;

uint8_t RX_LT8920_RAW[10];
uint8_t RX_LT8920[10];

extern int test_i;
uint16_t tt[5]={0};
uint8_t tp[10]={0};
uint16_t reg48;

void LT8920_SPI_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin = SPI_NSS_Pin;//SPI片选 低有效
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(SPI_NSS_GPIO_Port, &GPIO_InitStructure);//初始化
	GPIO_SetBits(SPI_NSS_GPIO_Port, SPI_NSS_Pin);//默认高
	
	GPIO_InitStructure.GPIO_Pin = SPI_CLK_Pin;//SPI时钟
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(SPI_CLK_GPIO_Port, &GPIO_InitStructure);//初始化
	GPIO_ResetBits(SPI_CLK_GPIO_Port, SPI_CLK_Pin);//默认低电平

	GPIO_InitStructure.GPIO_Pin = SPI_MISO_Pin;//SPI MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输出
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(SPI_MISO_GPIO_Port, &GPIO_InitStructure);//初始化
//	GPIO_ResetBits(SPI_MISO_GPIO_Port, SPI_MISO_Pin);//默认低电平

	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_Pin;//SPI MOSI
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(SPI_MOSI_GPIO_Port, &GPIO_InitStructure);//初始化
	GPIO_ResetBits(SPI_MOSI_GPIO_Port, SPI_MOSI_Pin);//默认低电平
	
	GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin;//SPI使能 高有效
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(SPI_RST_GPIO_Port, &GPIO_InitStructure);//初始化
	GPIO_SetBits(SPI_RST_GPIO_Port, SPI_RST_Pin);//默认高
}

void spi_delay(uint32_t t){
	uint32_t i=8;
	while(t){
		t=t-1;
		while(i){
			i=i-1;
		}
	}
}
void LT8920_Write(uint8_t reg, uint8_t* pData, uint16_t size){
	uint8_t i,j;
	uint8_t temp;
	reg&=0x7F;
	NSS_EN;
	//spi_delay(10);
	//delay_ms(3);
	delay_us(1);
	for(i=0;i<8;i++){
		CLK_H;
		if(reg&0x80)MOSI_H;
		else MOSI_L;
		reg = reg<<1;
		//delay_ms(1);
		//spi_delay(10);
		delay_us(1);
		CLK_L;
		//delay_ms(1);
		//spi_delay(10);
		delay_us(1);
	}
	for(j=0;j<size;j++){
		//delay_ms(2);
		//spi_delay(20);
		delay_us(2);
		temp=*pData;
		pData++;
		for(i=0;i<8;i++){
			CLK_H;
			if(temp&0x80)MOSI_H;
			else MOSI_L;
			temp = temp<<1;
			//delay_ms(1);
			//spi_delay(10);
			delay_us(1);
			CLK_L;
			//delay_ms(1);
			//spi_delay(10);
			delay_us(1);
		}
		MOSI_L;
	}
	//delay_ms(1);
	//spi_delay(10);
	delay_us(1);
	NSS_DIS;
}

void LT8920_Read_Reg(uint8_t reg, uint8_t* pData, uint16_t size){
	uint8_t i,j;
	uint8_t temp;
	reg|=0x80;
	NSS_EN;
	//delay_ms(3);
	//spi_delay(10);
	delay_us(1);
	for(i=0;i<8;i++){
		CLK_H;
		if(reg&0x80)MOSI_H;
		else MOSI_L;
		reg=(reg<<1)&0xFE;
		//delay_ms(1);
		//spi_delay(10);
		delay_us(1);
		CLK_L;
		//delay_ms(1);
		//spi_delay(10);
		delay_us(1);
	}
	MOSI_L;
	for(j=0;j<size;j++){
		//delay_ms(2);
		//spi_delay(20);
		delay_us(2);
		temp=0;
		for(i=0;i<8;i++){
			temp=(temp<<1)&0xFE;
			CLK_H;
			//delay_ms(1);
			//spi_delay(10);
			delay_us(1);
			CLK_L;
			temp|=MISO;
			//delay_ms(1);
			//spi_delay(10);
			delay_us(1);
		}
		*pData=temp;
		pData++;
	}
	//delay_ms(1);
	//spi_delay(10);
	delay_us(1);
	NSS_DIS;
}

void LT8920_Set_Reg(uint8_t reg, uint16_t value){
	uint8_t w_data[2] = {(uint8_t)((value&0xFF00)>>8),(uint8_t)(value&0x00FF)};
	LT8920_Write(reg, w_data, 2);
}

uint16_t LT8920_ReadByte_Reg(uint8_t reg){
	uint8_t r_data[2] = {0};
	LT8920_Read_Reg(reg, r_data, 2);
	return (uint16_t)(r_data[0]<<8|r_data[1]);
}

void LT8920_Init(void){
	uint8_t i;
	uint8_t size;
	static uint16_t RegValue = 0;
	size = sizeof(LT8920InitValue)/sizeof(RFSetting_Data_Type);
	for (i = 0;i<size; i++) //上电初始化RF
	{
		do{
			LT8920_Set_Reg(LT8920InitValue[i].subaddr, LT8920InitValue[i].value);
			RegValue = LT8920_ReadByte_Reg(LT8920InitValue[i].subaddr);
		}while(LT8920InitValue[i].value != RegValue && LT8920InitValue[i].subaddr != 44);
		//test_i++;
	}
}

void pd(void){
	ff = ((sync[0]&0xF0) + (sync[0]&0x0F)*16 + sync[1])%40 + 1;
	int i=0;
	uint8_t pdd[6]={0x05,0x04,0x00,sync[0],0x00,sync[1]};
	for(i=0;i<3;i++){
		LT8920_Set_Reg(7,0x007F);
		LT8920_Set_Reg(52,0x8080);
		LT8920_Write(50,pdd,6);
		delay_ms(10);
		LT8920_Set_Reg(7,0x017F);
		delay_ms(100);
	}
	delay_ms(1000);
	LT8920_Set_Reg(36,(((uint16_t)sync[0])<<8) + sync[0]);
	LT8920_Set_Reg(39,(((uint16_t)sync[1])<<8) + sync[1]);
}

void RF_Init(void){
	LT8920_SPI_Init();
	LT8920_Init();
	LT8920_Set_Reg(50,0x0000);
	pd();
	RF_Receive_Scan();
}

void RF_Receive_Scan(void){	
	LT8920_Set_Reg(7, (uint16_t)ff);
	LT8920_Set_Reg(52,0x0000);
	LT8920_Set_Reg(52,0x8080);//Clear RX FIFO
	LT8920_Set_Reg(7, (uint16_t)ff|0x0080);
	//if(RF_Scan_Channel++ > 2) RF_Scan_Channel = 0;	
}
void LT8920_Key_Control(uint8_t key){
	
	switch(key){
	case 0xA1: //Key1长按
		Key_flag = 0;
		break;
	case 0xA3: //Key2长按
		Set_Spin(left_fore, right_back);//顺时针
		break;
	case 0xA5: //Key3长按
		Set_Spin(left_back, right_fore);//逆时针
		break;
	case 0xA7: //Key4长按
		Set_Servo(left_L, right_L);Relay4_Off();Key_flag = 1;
		break;
	case 0xA9: //Key5长按
		Set_Servo(left_H, right_H);Relay4_On();Key_flag = 1;
		break;
	case 0xA2: //Key1短按
		Key_flag = 0;
		break;
	case 0xA4: //Key2短按
		break;
	case 0xA6: //Key3短按
		break;
	case 0xA8: //Key4短按
		Set_Servo(left_L, right_L);Relay4_Off();Key_flag = 1;
		break;
	case 0xAB: //Key5短按
		Set_Servo(left_H, right_H);Relay4_On();Key_flag = 1;	
		break;	
	default:
		break;
	}

}

uint8_t RF_DEAL_RX(uint8_t* R_Data,uint8_t* D_Data){
	uint8_t j=0;
	auto_flag = 0;
	int temp[7]={0};
	if(R_Data[0]==0x07 && R_Data[1]==0x06){
		for(j=2;j<5;j++){
			if(R_Data[j] != R_Data[j+3])return 0;
		}
		memcpy(D_Data,R_Data,8);
		temp[0]=(int)D_Data[2]-0x64;//AS								//Data[2]：angular speed	
		temp[1]=(int)D_Data[3]-0x65;//S									//Data[3]：speed
		if(R_Data[4] != 0xA0)LT8920_Key_Control(R_Data[4]);					//Data[4]：Servo control
		if(abs(temp[0]) <= 10 && abs(temp[1]) <= 10)Stop_Control();//0
		else Set_Loop_Para((float)temp[1],(float)temp[0]);//1
		return 1;
	}
	else return 0;
}


//放到PKT外部中断里,处理接收功能
uint8_t RF_Receive_Data(void){

	reg48 = LT8920_ReadByte_Reg(48);
	if((reg48&0xC0FF)==0x0020){  		
		//RF_Scan_Channel = 0;
		printf("123\r\n");
		LT8920_Read_Reg(50, RX_LT8920_RAW, 8);
		RF_DEAL_RX(RX_LT8920_RAW,RX_LT8920);
		printf("%c",RX_LT8920_RAW[2]);
		RF_Receive_Scan();
		//test_i++;
		return 1;
	}
	if((reg48&0x8000)==0x8000 || (reg48&0x4000)==0x4000){
		RF_Receive_Scan();
	}
	return 0;
}


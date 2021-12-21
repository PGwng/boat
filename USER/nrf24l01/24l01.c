#include "./nrf24l01/24l01.h"
#include "common.h"
#include "./spi/spi.h"
#include "user.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//NRF24L01驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/5/9 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //发送地址    
//const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
//const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址

//0x07,0x06,0x64,0x64,0xA0,0x64,0x64,0xA0

u8 RX_DATA_RAW[11] = {0x0A,0x07,0x06,0x01,0x64,0x64,0xA0,0x01,0x64,0x64,0xA0};		//接收的原始数据	[0]保存数据帧字节数，[1]-[8]保存8字节数据
u8 RX_DATA[11] = {0x0A,0x07,0x06,0x01,0x64,0x64,0xA0,0x01,0x64,0x64,0xA0};				//copy后的数据
int receive_flag = 0;
void NRF24L01_SPI_Init(void)
{
	
 	SPI_InitTypeDef  SPI_InitStructure;
	
	SPI_Cmd(SPI1, DISABLE); //使能SPI外设
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第1个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
}
 
//初始化24L01的IO口
void NRF24L01_Init(void)
{  
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOB,G时钟
	/* 使能 SYSCFG 时钟 ，使用 GPIO 外部中断时必须使能 SYSCFG 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
  //GPIOB14初始化设置:推挽输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB14
	
	//GPIOG6,9推挽输出（！！此处PG7改成了PG9）
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化PG6,9
////////////////////////////////EXTI gpio口初始化////////////////////////////////	
	//GPIOG.8上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化PG8
	
//PA0 PA4	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA0,PA4
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化PD0	
	
	
//////////////////////////////////////////////////////////////////////////////////
  GPIO_SetBits(GPIOB,GPIO_Pin_14);//PB14输出1,防止SPI FLASH干扰NRF的通信 
  
 	SPI1_Init();    		//初始化SPI1  
	
	NRF24L01_SPI_Init();//针对NRF的特点修改SPI的设置

	NRF24L01_CE=0; 			//使能24L01
	NRF24L01_CSN=1;			//SPI片选取消	 		 	 
}
////////////////////////////////////EXTI初始化/////////////////////////////////////
void EXTI8_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,GPIO_PinSource8);//连接EXTI中断源到PG8引脚
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;//选择中断源
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能中断线
	EXTI_Init(&EXTI_InitStructure);
	//EXTI_ClearITPendingBit(EXTI_Line8);//清除EXTI8线路标志位
}

void EXTI0_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,GPIO_PinSource0);//连接EXTI中断源到PD0引脚
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//选择中断源
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能中断线
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line0);//清除EXTI0线路标志位
}

void EXTI4_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,GPIO_PinSource4);//连接EXTI中断源到PA4引脚
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;//选择中断源
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能中断线
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line4);//清除EXTI4线路标志位
}
//////////////////////////////////////////////////////////////////////////////////////
//检测24L01是否存在
//返回值:0，成功;1，失败	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}

/*********************************************/
/* 函数功能：给24L01的寄存器写值（一个字节） 	*/
/* 入口参数：reg   要写的寄存器地址          */
/*           value 给寄存器写的值            */
/* 出口参数：status 状态值                   */
/*********************************************/
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
  NRF24L01_CSN=0;                 //使能SPI传输
  status =SPI1_ReadWriteByte(reg);//发送寄存器号 
  SPI1_ReadWriteByte(value);      //写入寄存器的值
  NRF24L01_CSN=1;                 //禁止SPI传输	   
  return(status);       			//返回状态值
}

/*************************************************/
/* 函数功能：读24L01的寄存器值 （一个字节）      */
/* 入口参数：reg  要读的寄存器地址               */
/* 出口参数：value 读出寄存器的值                */
/*************************************************/
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
  NRF24L01_CSN = 0;          //使能SPI传输		
  SPI1_ReadWriteByte(reg);   //发送寄存器号
  reg_val=SPI1_ReadWriteByte(0XFF);//读取寄存器内容
  NRF24L01_CSN = 1;          //禁止SPI传输		    
  return(reg_val);           //返回状态值
}	
/*********************************************/
/* 函数功能：读24L01的寄存器值（多个字节）   */
/* 入口参数：reg   寄存器地址                */
/*           *pBuf 读出寄存器值的存放数组    */
/*           len   数组字节长度              */
/* 出口参数：status 状态值                   */
/*********************************************/
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
  u8 status,u8_ctr;	       
  NRF24L01_CSN = 0;           //使能SPI传输
  status=SPI1_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI1_ReadWriteByte(0XFF);//读出数据
  NRF24L01_CSN=1;       //关闭SPI传输
  return status;        //返回读到的状态值
}

/**********************************************/
/* 函数功能：给24L01的寄存器写值（多个字节）  */
/* 入口参数：reg  要写的寄存器地址            */
/*           *pBuf 值的存放数组               */
/*           len   数组字节长度               */
/* 出口参数：status 状态值                   */
/**********************************************/
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //使能SPI传输
  status = SPI1_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI1_ReadWriteByte(*pBuf++); //写入数据	 
  NRF24L01_CSN = 1;       //关闭SPI传输
  return status;          //返回读到的状态值
}				   

/**********************************************/
/* 函数功能：启动NRF24L01发送一次数据            */
/* 入口参数：txbuf  发送数据数组              */
/* 返回值； 0x10    达到最大重发次数，发送失败*/
/*          0x20    成功发送完成              */
/*          0xff    发送失败                  */
/**********************************************/
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送	   
	while(NRF24L01_IRQ!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(NRF_READ_REG+STATUS);  //读取状态寄存器的值	   
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta);//清除MAX_RT中断标志
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta);//清除TX_DS中断标志
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
/*********************************************/
/* 函数功能：接收数据                   */
/* 入口参数：rxbuf 接收数据数组              */
/* 返回值： 0   成功收到数据                 */
/*          1   没有收到数据                 */
/*********************************************/
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(NRF_READ_REG+STATUS);  //读取状态寄存器的值    	 
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除RX_RD中断标志
	if(sta&RX_OK)//接收到数据
	{
		receive_flag = 1;
		//printf("first_flag:%d\r\n",receive_flag);
		
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除RX_RD中断标志
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		NRF_DEAL_RX(RX_DATA_RAW,RX_DATA);
		
		return 0; 
	}	   
	return 1;//没收到任何数据
}		


//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{
  NRF24L01_CE=0;	  
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,0);	     //设置RF通信频率		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  NRF24L01_CE = 1; //CE为高,进入接收模式 
}

//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE=0;	   //CE低电平待机模式，高电平接收模式，高电平>10us发送模式 
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,0);       //设置RF通道为0
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE=1;//CE为高,10us后启动发送
}

void NRF24L01_RT_Mode(void)
{
	NRF24L01_CE = 0;
	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度
	NRF24L01_Write_Reg(FLUSH_RX,0xff);									//清除RX FIFO寄存器  
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址(P0：通道0)
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址
	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,0);       //设置RF通道为0
	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0f);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE=1;//CE为高,10us后启动发送
}

void SEND_BUF(u8 *buf)
{
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0e);	//配置寄存器，发射模式
	NRF24L01_CE=1;
	delay_us(15);
	NRF24L01_TxPacket(buf);
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);	//配置寄存器，接收模式
	NRF24L01_CE=1;
	delay_us(15);
}

u8 RECEIVE_BUF(void)
{
	NRF24L01_RxPacket(RX_DATA_RAW);	
	//NRF_DEAL_RX(RX_DATA_RAW,RX_DATA);
	return 0;
}
///////////////////////////////////////中断处理函数//////////////////////////////////////
//void EXTI9_5_IRQHandler(void)
//{
//	printf("%d",111);
//	delay_ms(1); //重要，延时等待AKC发送完成，再处理数据，时间自行调试和把握
//	if (EXTI_GetITStatus(EXTI_Line8)!=RESET)//判断是否为中断线路8引起的中断
//	{
//		NRF24L01_RxPacket(RX_DATA_RAW);		
//		NRF_DEAL_RX(RX_DATA_RAW,RX_DATA);
//		EXTI_ClearITPendingBit(EXTI_Line8);//清除stm32中断标志位
//	}
//}

void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0)!=RESET)
	{
//		printf("TEST\r\n");
		
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

//void test(void)
//{
//	receive_flag = 1;
//	printf("first_flag:%d\r\n",receive_flag);
//}

void EXTI4_IRQHandler(void)		//PA4
{
	
	delay_ms(1); //重要，延时等待AKC发送完成，再处理数据，时间自行调试和把握
	if (EXTI_GetITStatus(EXTI_Line4)!=RESET)
	{
		NRF24L01_RxPacket(RX_DATA_RAW);
//		test();
//		printf("interupt_flag:%d\r\n",receive_flag);
		if(receive_flag == 1)
		{
//		printf("外部中断333\r\n");
		//NRF_DEAL_RX(RX_DATA_RAW,RX_DATA);
		//receive_flag = 0;
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

///////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//按键：控制闸门、小幅度转向
//
//////////////////////////////////////////////////////////////////////
void Key_Control(uint8_t key){
	
	switch(key){
	case 0xA1: //Key1长按											//复位闸门控制
		Key_flag = 0;												//闸门异常控制标志位置0
		break;
	case 0xA3: //Key2长按
		Set_Spin(left_fore, right_back);//顺时针					//小幅度顺时针转动
		break;
	case 0xA5: //Key3长按
		Set_Spin(left_back, right_fore);//逆时针					//小幅度逆时针转动
		break;
	case 0xA7: //Key4长按
		Set_Servo(left_L, right_L);Relay4_Off();Key_flag = 1;		//伺服电机控制闸门下降
		break;														//闸门异常控制标志位置1
	case 0xA9: //Key5长按
		Set_Servo(left_H, right_H);Relay4_On();Key_flag = 1;		//伺服电机控制闸门上升
		break;														//闸门异常控制标志位置1
	
	case 0xB0:		//机械装置控制按钮
		PEout(2) = ~PEout(2);
		RX_DATA_RAW[6] = 0xA0;
		break;
	/*
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
	*/
	default:
		break;
	}

}

///*校验提取PID参数*/
//float yaw_P = 1.6f, yaw_I = 0.0f, yaw_D = 1.6f;
//float position_P = 0.8f, position_I = 0.0f, position_D = 0.8f;
//u8 PID_para_raw[15] = {0x0e,0x97,0x98,0x10,0x01,0x10,0x08,0x01,0x08,0x10,0x01,0x10,0x08,0x01,0x08};
//u8 PID_para[15] = {0x0e,0x97,0x98,0x10,0x01,0x10,0x08,0x01,0x08,0x10,0x01,0x10,0x08,0x01,0x08};
//u8 set_PID_para(u8* R_Data,u8* D_Data)
//{
// u8 i = 0;
// if(R_Data[1] == 0x97 && R_Data[2]==0x98)
// {
//  for(i = 3; i < 9; i++)
//  {
//   if(R_Data[i] != R_Data[i+6])return 0;
//  }
//  memcpy(D_Data,R_Data,15);
// }
// /*航向角PID【3】【4】【5】*/
// yaw_P = ((int)D_Data[3])/10.0f;
// yaw_I = ((int)D_Data[4])/1000.0f;
// yaw_D = ((int)D_Data[5])/10.0f;
// 
// /*位置PID 【6】【7】【8】*/
// position_P = ((int)D_Data[6])/10.0f;
// position_I = ((int)D_Data[7])/1000.0f;
// position_D = ((int)D_Data[8])/10.0f;
//}

//校验&提取遥控指令
uint8_t NRF_DEAL_RX(uint8_t* R_Data,uint8_t* D_Data){
	uint8_t j=0;
	int temp[7]={0};
	if(R_Data[1]==0x07 && R_Data[2]==0x06){								//检验帧头(首字节是数据的Byte数，)
		for(j=3;j<7;j++){												//
			if(R_Data[j] != R_Data[j+4])return 0;						//检验数据
		}
		memcpy(D_Data,R_Data,11);
		switch(R_Data[3]){
			
			case 0x01:							//遥控				
				auto_flag = 0;
				temp[0]=(int)D_Data[4]-0x64;//AS								//Data[3]：angular speed	
				temp[1]=(int)D_Data[5]-0x66;//S									//Data[4]：speed
				if(R_Data[6] != 0xA0)Key_Control(R_Data[6]);					//Data[4]：Servo control
		
				if(abs(temp[0]) <= 10 && abs(temp[1]) <= 10)Stop_Control();	//AS、S太小，停止控制
				else Set_Loop_Para((float)temp[1],(float)temp[0]);//1			//给定速度、角速度
				break;
			
			case 0x02:							//停止制动
				Stop_Control();
				break;
			case 0x03:							//自动航向
				Stop_Control();
				auto_flag = 1;				//自动航向标志位
			
//				Kp = ((int)D_Data[4])/10.0;
//				Ki = ((int)D_Data[5])/1000.0;
//				Kd = ((int)D_Data[6])/10.0;
			
				pos_Kp = ((int)D_Data[4])/10.0;
				pos_Ki = ((int)D_Data[5])/1000.0;
				pos_Kd = ((int)D_Data[6])/10.0;
			
				if(fabs(Ref_yaw - Eular[2]) > 1) Ref_Angular_Speed = Yaw_PID();
				
				Set_Loop_Para(0,Ref_Angular_Speed);		//A =0 ; AS = Yaw_PID()	初始化速度和角速度
				
				break;
			
			default:
				break;
		}
		
		return 1;
	}
	else return 0;
}
/////////////////////////////////////////////////////////////////////////////////
u8 actual_yaw[4];
u8 actual_pos[4];
u8 send_act[19] = {0x12, 'A', 'B' };
void sendActYaw(void)
{
	//printf("%c%c%c%c%c%c%c%c%c%c%c",0x0A,'A','B',send_act_yaw[3],send_act_yaw[4],send_act_yaw[5],send_act_yaw[6],send_act_yaw[7],send_act_yaw[8],send_act_yaw[9],send_act_yaw[10]);
	int i;
	
	//
	//float current_yaw = Eular[2];
	float current_yaw = yaw_err;
	float current_pos = pos_err;
	
	float2byte(&current_yaw,actual_yaw,sizeof(current_yaw));	//转为字节数组
	float2byte(&current_pos,actual_pos,sizeof(current_pos));	//转为字节数组
	
	for(i = 3; i <= 6; i++)
	{
		send_act[i] = actual_yaw[i-3];
	}
	for(i = 7; i <= 10; i++)
	{
		send_act[i] = actual_pos[i-7];
	}
	for(i = 11; i <= 14; i++)
	{
		send_act[i] = actual_yaw[i-11];
	}
	for(i = 15; i <= 18; i++)
	{
		send_act[i] = actual_pos[i-15];
	}
	
	SEND_BUF(send_act);
}


TX_PACKETS tx_packets;
TX_INFO	tx_info;

void get_tx_info(void)													//获取byte格式的发送信息
{
	tx_info.Lat_dir = latdir;
	tx_info.Lon_dir = londir;
	
//	memcpy(tx_info.Acc_Byte,(u8 *)Acc,12);
//	memcpy(tx_info.Gyr_Byte,(u8 *)Gyr,12);
//	memcpy(tx_info.Eular_Byte,(u8 *)Eular,12);
//	memcpy(tx_info.Lat_Byte,(u8 *)&lat.f_data,4);
//	memcpy(tx_info.Lon_Byte,(u8 *)&lon.f_data,4);
	float2byte(Acc,tx_info.Acc_Byte, sizeof(Acc));
	float2byte(Gyr,tx_info.Gyr_Byte, sizeof(Gyr));
	float2byte(Eular,tx_info.Eular_Byte, sizeof(Eular));
	float2byte(&lat.f_data, tx_info.Lat_Byte, sizeof(lat.f_data));
	float2byte(&lon.f_data, tx_info.Lon_Byte, sizeof(lon.f_data));	
}

void gen_tx_packets(void)
{
	int i,j;
	tx_packets.TX_DATA1[0] = TX_DATA1_LEN;
	tx_packets.TX_DATA2[0] = TX_DATA2_LEN;
	
	for(i=1;i<=12;i++)
	{
		tx_packets.TX_DATA1[i] = tx_info.Acc_Byte[i-1];		
	}
	
	for(i=13;i<=24;i++)
	{
		tx_packets.TX_DATA1[i] = tx_info.Gyr_Byte[i-13];
	}
	
	for(j=1;j<=12;j++)
	{
		tx_packets.TX_DATA2[j] = tx_info.Eular_Byte[j-1];	
	}
	
	tx_packets.TX_DATA2[13] = tx_info.Lat_dir;
	
	for(j=14;j<=17;j++)
	{
		tx_packets.TX_DATA2[j] = tx_info.Lat_Byte[j-14];
	}
	
	tx_packets.TX_DATA2[18] = tx_info.Lon_dir;
	
	for(j=19;j<=22;j++)
	{
		tx_packets.TX_DATA2[j] = tx_info.Lon_Byte[j-19];
	}
}

//src:要拷贝的数组 dest：目标数组 byte_num：拷贝字节数
int float2byte(float src[], u8 dest[], int byte_num)
{
	memcpy(dest, src, byte_num);					
	return 0;
}

int byte2float(u8 *src, float* dest, int byte_num)
{
	memcpy(dest, (float*)src, 12);				
	return 0;
}





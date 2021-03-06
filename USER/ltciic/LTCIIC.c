#include "LTCIIC.h"


/**
 * @brief Switch data bus input/output state.
 */
void LSDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



/**
 * @brief Switch data bus input/output state.
 */
void LSDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



//产生IIC起始信号
void LIIC_Start(void)
{
	LSDA_OUT();     //sda线输出
	LIIC_SDA=1;	  	  
	LIIC_SCL=1;
	delay_us(4);
	
 	LIIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	LIIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	

//产生IIC停止信号
void LIIC_Stop(void)
{
	LSDA_OUT();//sda线输出
	LIIC_SCL=0;
	LIIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	LIIC_SCL=1; 
	LIIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 LIIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	LIIC_SDA=1;delay_us(1);	
	
	LIIC_SCL=1;delay_us(1);	 /*SCL为高 进行读取操作 */  
   LSDA_IN();               //SDA设置为输入  
//	Usart_SendString(USART1,"----->应答\n");
	while(LREAD_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			LIIC_Stop();
			return 1;
		}
	}

	LIIC_SCL=0;//时钟输出0 	  	
	return 0;  
} 


//产生ACK应答
void LIIC_Ack(void)
{
	LIIC_SCL=0;
	LSDA_OUT();
	LIIC_SDA=0;
	delay_us(2);
	LIIC_SCL=1;
	delay_us(2);
	LIIC_SCL=0;
}
//不产生ACK应答		    
void LIIC_NAck(void)
{
	LIIC_SCL=0;
	LSDA_OUT();
	LIIC_SDA=1;
	delay_us(2);
	LIIC_SCL=1;
	delay_us(2);
	LIIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void LIIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	 LSDA_OUT(); 	    
    LIIC_SCL=0;//拉低时钟开始数据传输
	
    for(t=0;t<8;t++)
    {     
		/* I2C数据按照大端传输的，也就是高位先传输*/  
       LIIC_SDA=(txd&0x80)>>7;
		  
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		LIIC_SCL=1;
		delay_us(2); 
		LIIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 LIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	LSDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        LIIC_SCL=0; 
        delay_us(2);
			LIIC_SCL=1;
		
		receive <<= 1;
		
		if(LREAD_SDA)
		   receive++;
//		else
//			receive=receive&0xFE;
		
		delay_us(1); 
    }	

    if (!ack)
        LIIC_Ack(); //发送ACK  
    else
       LIIC_NAck();//发送nACK
//	 
//	 RS485_SendString("22222\n"); 		
    return receive;
}









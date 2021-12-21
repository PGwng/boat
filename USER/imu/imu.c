#include "./imu/imu.h"
//解析IMU数据

#define DEBUG

float Acc[3];// = {3.14,0,0};
float Gyr[3];
float Mag[3];
float Eular[3];// = {0,0,6.28};


/**@brife 初始化IMU设备
	*@param[in] 绑定串口
	*@param[in] 代初始化odrive设备
	*/
void imuInit(USART_TypeDef *pUSARTx,IMU *imu)
{
	memset(imu->rawIMUBuf,0,48);
	imu->port=pUSARTx;	
}

/**@brife START
	*/
void imuStart(IMU *imu)
{
	char starCmd[7]={0xa5,0x5a,0x04,0x01,0x05,0xaa};
//	char starCmd2[7]={0xa5,0x5a,0x05,0xa8,0x0a,0xb7,0xaa};

	Usart_SendString(imu->port,starCmd);
	//delay_ms(100);
	//Usart_SendString(imu->port,starCmd2);
}

/**@brife STOP
	*/
void imuStop(IMU *imu)
{
	char stopCmd[7]={0xa5,0x5a,0x04,0x02,0x06,0xaa};
	Usart_SendString(imu->port,stopCmd);
}

/**@brife 磁力计标定
	*/
void imuCalibration(IMU *imu)
{
	char calibCmd[7]={0xa5,0x5a,0x04,0xe3,0xe7,0xaa};
	Usart_SendString(imu->port,calibCmd);
}

/**@brife 磁力计保存标定
	*/
void imuSaceCfg(IMU *imu)
{
	char saceCmd[7]={0xa5,0x5a,0x04,0xe1,0xe5,0xaa};
	Usart_SendString(imu->port,saceCmd);
}



/**@brife 获取IMU数据
	*@param[in] imu设备
	*/
void imuAnalysisData(IMU *imu)
{
	int16_t yaw,pitch,roll;
	int16_t ax,ay,az;
	int16_t gx,gy,gz;
	int16_t hx,hy,hz;
	
	imu_mutex_lock=1;//互斥锁
	
	//Eular
	yaw=(imu->rawIMUBuf[3]<<8)+imu->rawIMUBuf[4];     //航向角，单位为0.1°
	imu->eular.yaw.f_data=yaw*0.1;
	Eular[2]=360.0-imu->eular.yaw.f_data;
	pitch=(imu->rawIMUBuf[5]<<8)+imu->rawIMUBuf[6];   //俯仰角，单位为0.1°
	imu->eular.pitch.f_data=pitch*0.1;
	Eular[1]=imu->eular.pitch.f_data;
	roll=(imu->rawIMUBuf[7]<<8)+imu->rawIMUBuf[8];   //横滚角，单位为0.1°
	imu->eular.roll.f_data=roll*0.1;
	Eular[0]=imu->eular.roll.f_data;

	//Acc
	ax=(imu->rawIMUBuf[9]<<8) +imu->rawIMUBuf[10];    //X轴加速度，除以16384后单位为g
	imu->acc.ax.f_data=ax/16384.0;
	Acc[0]=imu->acc.ax.f_data;
	ay=(imu->rawIMUBuf[11]<<8)+imu->rawIMUBuf[12];    //Y轴加速度，除以16384后单位为g
	imu->acc.ay.f_data=ay/16384.0;
	Acc[1]=imu->acc.ay.f_data;
	az=(imu->rawIMUBuf[13]<<8)+imu->rawIMUBuf[14];    //Z轴加速度，除以16384后单位为g
	imu->acc.az.f_data=az/16384.0;
	Acc[2]=imu->acc.az.f_data;

	//Gyr
	gx=(imu->rawIMUBuf[15]<<8)+imu->rawIMUBuf[16];    //X轴角速度，除以32.8后单位为（°/s）
	imu->gyr.gx.f_data=gx/32.8;
	Gyr[0]=imu->gyr.gx.f_data;
	gy=(imu->rawIMUBuf[17]<<8)+imu->rawIMUBuf[18];    //Y轴角速度，除以32.8后单位为（°/s）
	imu->gyr.gy.f_data=gy/32.8;
	Gyr[1]=imu->gyr.gy.f_data;
	gz=(imu->rawIMUBuf[19]<<8)+imu->rawIMUBuf[20];    //Z轴角速度，除以32.8后单位为（°/s）
	imu->gyr.gz.f_data=gz/32.8;
	Gyr[2]=imu->gyr.gz.f_data;
	
	//Mag
	hx=(imu->rawIMUBuf[21]<<8)+imu->rawIMUBuf[22];    //X磁强度，除以333.3后单位为Guass
	imu->mag.hx.f_data=hx/333.3;
	Mag[0]=imu->mag.hx.f_data;
	hy=(imu->rawIMUBuf[23]<<8)+imu->rawIMUBuf[24];    //Y磁强度，除以333.3后单位为Guass
	imu->mag.hy.f_data=hy/333.3;
	Mag[1]=imu->mag.hy.f_data;
	hz=(imu->rawIMUBuf[25]<<8)+imu->rawIMUBuf[26];    //Z磁强度，除以333.3后单位为Guass
	imu->mag.hz.f_data=hz/333.3;
	Mag[2]=imu->mag.hz.f_data;

	imu->alt=(imu->rawIMUBuf[27]<<24)+(imu->rawIMUBuf[28]<<16)+(imu->rawIMUBuf[29]<<8)+imu->rawIMUBuf[30];      //气压高度，单位为cm
	imu->tempr=(imu->rawIMUBuf[31]<<8)+imu->rawIMUBuf[32];                                        //内部温度，单位为0.01℃
	imu->alltime=(imu->rawIMUBuf[33]<<24)+(imu->rawIMUBuf[34]<<16)+(imu->rawIMUBuf[35]<<8)+imu->rawIMUBuf[36];  //运行时间，单位为ms
	imu->mageffect=imu->rawIMUBuf[37];   //磁状态标志 
	
	imu_mutex_lock=0;

#ifdef DEBUG
	//printf("Yaw:%f pitch：%f roll：%f\r\n",imuReadYaw(imu),imuReadPitch(imu),imuReadRoll(imu));
#endif		
}

/**@brife 读取eular Yaw数据
	*@param[in] imu设备
	*/
float imuReadYaw(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return 360.0-imu->eular.yaw.f_data;	
}

/**@brife 读取eular pitch数据
	*@param[in] imu设备
	*/
float imuReadPitch(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->eular.pitch.f_data;	
}

/**@brife 读取eular roll数据
	*@param[in] imu设备
	*/
float imuReadRoll(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->eular.roll.f_data;	
}

/**@brife 读取acc.ax数据
	*@param[in] imu设备
	*/
float imuReadAx(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->acc.ax.f_data;	
}

/**@brife 读取acc.ay数据
	*@param[in] imu设备
	*/
float imuReadAy(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->acc.ay.f_data;	
}

/**@brife 读取acc.az数据
	*@param[in] imu设备
	*/
float imuReadAz(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->acc.az.f_data;	
}

/**@brife 读取gyr.gx数据
	*@param[in] imu设备
	*/
float imuReadGx(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->gyr.gx.f_data;	
}

/**@brife 读取gyr.gx数据
	*@param[in] imu设备
	*/
float imuReadGy(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->gyr.gy.f_data;	
}

/**@brife 读取gyr.gx数据
	*@param[in] imu设备
	*/
float imuReadGz(IMU *imu)
{
		while(imu_mutex_lock==1);//等待解锁 
		return imu->gyr.gz.f_data;	
}


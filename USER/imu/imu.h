#ifndef __IMU_H_
#define __IMU_H_

#include "./imu/usart6.h"
#include "./usart/bsp_debug_usart.h"

/**@brife EULAR
	*
	*/
typedef struct
{
	Float_data yaw; 	//航向角，单位为0.1°
	Float_data pitch;	//俯仰角，单位为0.1°
	Float_data roll;	//横滚角，单位为0.1°
}EULAR;

/**@brife Acc
	*
	*/
typedef struct
{
	Float_data ax;	//X轴加速度，除以16384后单位为g
	Float_data ay;	//Y轴加速度，除以16384后单位为g
	Float_data az;	//Z轴加速度，除以16384后单位为g
}ACC;

/**@brife Gyr
	*
	*/
typedef struct
{
	Float_data gx;	//X轴角速度，除以32.8后单位为（°/s）
	Float_data gy;	//Y轴角速度，除以32.8后单位为（°/s）
	Float_data gz;	//Z轴角速度，除以32.8后单位为（°/s）
}GYR;

/**@brife Mag
	*
	*/
typedef struct
{
	Float_data hx;	//X磁强度，除以333.3后单位为Guass
	Float_data hy;	//Y磁强度，除以333.3后单位为Guass
	Float_data hz;	//Z磁强度，除以333.3后单位为Guass
}MAG;



/**@brife IMU设备
	*
	*/
typedef struct
{
	EULAR eular;
	ACC acc;
	GYR gyr;
	MAG mag;
	long int alt;					 		//气压高度，单位为cm
	long int tempr;				 		//内部温度，单位为0.01℃
	unsigned long int alltime;	//运行时间，单位为ms
	char mageffect;		 //磁状态标志
	char rawIMUBuf[48];
	USART_TypeDef *port;
}IMU;

void imuInit(USART_TypeDef *pUSARTx,IMU *imu);
void imuStart(IMU *imu);
void imuStop(IMU *imu);
void imuCalibration(IMU *imu);
void imuSaceCfg(IMU *imu);
void imuAnalysisData(IMU *imu);
float imuReadYaw(IMU *imu);
float imuReadPitch(IMU *imu);
float imuReadRoll(IMU *imu);
float imuReadAx(IMU *imu);
float imuReadAy(IMU *imu);
float imuReadAz(IMU *imu);
float imuReadGx(IMU *imu);
float imuReadGy(IMU *imu);
float imuReadGz(IMU *imu);

extern	float Acc[3];
extern	float Gyr[3];
extern  float Mag[3];
extern	float Eular[3];

#endif

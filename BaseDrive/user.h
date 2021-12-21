#ifndef USER_H
#define USER_H

#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG  1
//#define  RX_BUFFER_SIZE 100
#define dut_H 6000    //
#define dut_M 4500    //
#define dut_L 3000    //
#define left_H 2300    //	
#define left_L 6300    //	
#define right_L 2700    //	
#define right_H 6700    //	
#define left_fore 6000    //	
#define left_back 3000    //	
#define right_fore 6000    //	
#define right_back 3000    //	
//#define Prope_H 1000    //
//#define Prope_M 500    //
//#define Prope_L 0    //
//限制x在（min,max）区间内
#define lim(x,min,max) x=(x<=min)?min:((x>=max)?max:x)	//低于min则取min，高于min时判断x与max大小关系，高于max则取max（上下限）
//y的值在（x-p,x+p）区间外时，限制x的值为区间端点
#define pre(x,y,p) x=(y>x+p)?(x+p):((y<x-p)?(x-p):y)		//y大于x+p则取x+p，小于x+p时判断y于x-p的大小，小于x-p则取x-p
/**************system include ******************/
#include "stm32f4xx.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <stdint.h>
//#include <stdbool.h>
/***************user-define include*************/
#include "./usart/bsp_debug_usart.h"
#include "init.h"
#include "timer.h"
#include "common.h"
#include "packet.h"
//#include "imu.h"
//#include "step.h"
#include "motor.h"
#include "adc.h"
//#include "gps.h"
#include "wdg.h"
#include "./spi/spi.h"
#include "./nrf24l01/24l01.h"
#include "./rtk/usart2.h"
#include "./rtk/rtk.h"

#include "./key/key.h"
#include "./led/led.h"
#include "./ltc2944/ltc2944.h"
#include "./ltciic/LTCIIC.h"
#include "./stmflash/stmflash.h"
//#include "can.h"
//#include "i2c.h"
//#include "hardiic.h"
//#include "small_boat.h"
//#include "pc.h"
//#include "hardserial.h"
//#include "ringbuf.h"
//#include "maincpp.h"
//#include "pwm_dc.h"
//#include "limit_motor.h"

#include "./imu/imu.h"
#include "../BaseDrive/DMA/dma.h"
#include "./imu/usart6.h"
#include "./usart5/usart.h"

#ifdef __cplusplus
}
#endif
#endif

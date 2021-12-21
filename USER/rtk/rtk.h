#ifndef __RTK_H
#define __RTK_H	 

#include "./rtk/usart2.h"


extern const char* delim; //分割點
extern char *token;	 
//数据格式utc,lat,latdir,lon,londir,QF,satNo,hdop,alt,aunits,age,stnID;
extern Float_data utc,lat,lon;
extern char latdir,londir;

void data_analysis(char *buff, const char* delim, float* dest);
void RTK_DATA(void);



#endif

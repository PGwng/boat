#include "./rtk/usart2.h"
#include "./rtk/rtk.h"

const char* delim = ","; //分割點
char *token;	 
//数据格式utc,lat,latdir,lon,londir,QF,satNo,hdop,alt,aunits,age,stnID;
Float_data utc,lat,lon;
char latdir,londir;

//协议解析
void data_analysis(char *buff, const char* delim, float* dest)
{
			char *token;
			token = strtok((char*)buff, delim);	
			*dest =	atof(token);//字符转浮点
}

//接收数据
void RTK_DATA(void)
{
	 if(rec_complete == 1)
	 {
		 USART2_RX_STA = 0;
		 rec_complete = 0;
//		 if(qf == '0')
//		 {
//				printf("无效解\r\n");
//		 }
//		 else
//		 {
			 data_analysis((char*)USART2_RX_BUF,delim,&utc.f_data);
			 data_analysis(NULL,delim,&lat.f_data);
			 token = strtok(NULL, delim);
			 latdir = *token;
			 data_analysis(NULL,delim,&lon.f_data);
			 token = strtok(NULL, delim);
			 londir = *token;
			 memset(USART2_RX_BUF,0,USART2_REC_LEN);
			 
		 //printf("经度：%0.4f %c 纬度：%0.4f %c\r\n",lat.f_data,latdir,lon.f_data,londir); //lat:纬度 lon:经度
//		 } 
	 }
}

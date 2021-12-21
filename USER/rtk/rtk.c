#include "./rtk/usart2.h"
#include "./rtk/rtk.h"

const char* delim = ","; //�ָ��c
char *token;	 
//���ݸ�ʽutc,lat,latdir,lon,londir,QF,satNo,hdop,alt,aunits,age,stnID;
Float_data utc,lat,lon;
char latdir,londir;

//Э�����
void data_analysis(char *buff, const char* delim, float* dest)
{
			char *token;
			token = strtok((char*)buff, delim);	
			*dest =	atof(token);//�ַ�ת����
}

//��������
void RTK_DATA(void)
{
	 if(rec_complete == 1)
	 {
		 USART2_RX_STA = 0;
		 rec_complete = 0;
//		 if(qf == '0')
//		 {
//				printf("��Ч��\r\n");
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
			 
		 //printf("���ȣ�%0.4f %c γ�ȣ�%0.4f %c\r\n",lat.f_data,latdir,lon.f_data,londir); //lat:γ�� lon:����
//		 } 
	 }
}

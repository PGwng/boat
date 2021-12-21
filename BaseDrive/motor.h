#ifndef MOTOR_H
#define MOTOR_H
/******************************
*�����󴬸��������˵��: �������*3  �ҽ���*3  ֱ�����*3
*����:ֹͣ/��ʼ������  
*����:0:ֹͣ������   1:��ʼ������
*1���ҽ���(��ˮ����):PWMתDA���Ʒ�ʽ PE13�ܽ� TIM1-CH3    Prope_Pwm_Set(uint8_t i, uint16_t duty)
*1���������(����������):������� ͨ����������Ƶ�� �ı��ٶ�
*******************************/

#ifdef __cplusplus
extern "C" {
#endif
#include "user.h"

extern float Ref_Angular_Speed;
extern bool Closed_Loop_Control;
extern bool Spin_flag;
extern bool Key_flag;
extern bool auto_flag;
extern int rubblish_flag;//�����á�
	
extern void Set_Loop_Para(float speed_set, float angular_speed_set);
extern void angular_speed_Control(void);
extern void Set_Spin(uint16_t spin_left_set, uint16_t spin_right_set);
extern void spin_Control(void);
extern void Set_Servo(uint16_t servo_left_set, uint16_t servo_right_set);
extern void servo_Control(void);
extern void servo_Init(void);
extern void Single_Control(float motor_left, float motor_right);
extern void Stop_Control(void);
//�����á�
//extern void Set_Pullband_Para(float leftband,float rightband);
//extern void stop_rubbish(void);
//extern void start_rubbish(void);
//extern void rubbishControl(float target,float speed);//������������������ص� ��ˮ���(�ҽ���) �������(�����) ֱ�����(������)
//extern void oneKeyVoyage(void);//һ������С����ˮ��
//�����á�

float Yaw_PID(void);
void Yaw_Control(void);

float Pos_PID(void);
void Pos_Control(void);

void calculate_error(float dis, float ref_yaw, float act_yaw);
//extern float Kp;
//extern float Ki;
//extern float Kd;
extern float Ref_yaw;

extern float pos_Kp;
extern float pos_Ki;
extern float pos_Kd;
//extern float Ref_pos;

extern float pos_err;
extern float yaw_err;

#ifdef __cplusplus
}
#endif
#endif

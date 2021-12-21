#include "motor.h"
#include <math.h>
#include "pid.h"

float Kp = 1.6, Ki = 0, Kd = 1.6;
float Ref_yaw;

//λ��pid����
float pos_Kp = 1.6, pos_Ki = 0, pos_Kd = 1.6;
//float Ref_pos;

float yaw_err = 0, pos_err = 0;

float vel_x=0, Vel_y=0, Vel_w=0;
int kk = (dut_H-dut_L)/200;				//���ٶȱ仯1����λ����Ӧpwmռ�ձȱ仯��
int d_dut = (dut_H-dut_L)/2;			//���ٶȿ��������Χ����

float d_angularSpeed = 100;							//���ٶ��źŷ�ΧΪ��-100��+100��

float d_Speed = 50;								//�����źŷ�ΧΪ��-100��+100��

static float err_sum = 0, last_err = 0;			//�ǶȻ�pid_buf

static float error_sum = 0, last_error = 0;		//���ٶȻ�pid_buf

static float distance_sum = 0, last_distance = 0;			//λ�ÿ���pid_buf

uint16_t motor_left_pwm = dut_M, motor_right_pwm = dut_M;//ҡ�˿���ʱ�����ҵ��pwm

//��������
uint16_t servo_left_pwm = left_H, servo_right_pwm = right_H;	//�����բ�ţ�pwm��ʼֵ
uint16_t spin_left_pwm = dut_M, spin_right_pwm = dut_M;		//��ʼ�����ҵ��PWM����ֵΪ��ֵ
uint16_t spin_left_duty = dut_M;			//��ʼ�����ҵ��PWM���ŵ����ֵΪ��ֵ
uint16_t spin_right_duty = dut_M;			//

static float Ref_Speed = 0;						//�û������ٶ�
float Ref_Angular_Speed = 0;					//�������ٶ�

bool Closed_Loop_Control = 0;					//��ҡ�ˣ�ѭ�����Ʊ�־λ������һ��ҡ��ֵ��ı�ref_S��ref_AS��������ʱ��������ֵ�ջ����ƣ�
bool Spin_flag = 0;										//�������Ʊ�־λ
bool Key_flag = 0;										//բ���쳣���Ʊ�־λ
bool auto_flag = 0;										//�Զ������־λ

uint16_t float2pwm(float x){			//�����á�
	uint16_t result = dut_M;
	lim(x,-100,100);
  result = result + (uint16_t)(x*(dut_H-dut_M)/100.0f);
	return result;
}

//������������dut_L��dut_H֮��
static void motor_pwm_adjust(uint16_t *x, uint16_t *y, uint16_t min, uint16_t max){
	if(*x > *y){
		if(*x > max){
			*y -= (*x -max);
			*x = max;
		}
		else if(*y < min){
			*x += (min - *y);
			*y = min;
		}
	}
	else{
		if(*y > max){
			*x -= (*y - max);
			*y = max;
		}
		else if(*x < min){
			*y += (min - *x);
			*x = min;
		}
	}
	lim(*x,dut_L,dut_H);
	lim(*y,dut_L,dut_H);
}

//����ѭ�����Ʋ������ٶȡ����ٶ�
void Set_Loop_Para(float speed_set, float angular_speed_set){
	Closed_Loop_Control = 1;
	Spin_flag = 0;
	Ref_Speed = speed_set;
	Ref_Angular_Speed = angular_speed_set;
	//printf("�ο��ٶȣ�%f\r\n�ο����ٶȣ�%f\r\n",Ref_Speed,Ref_Angular_Speed);
}

//�������ҵ��PWM ����������С����ת��
void Set_Spin(uint16_t spin_left_set, uint16_t spin_right_set){
	Closed_Loop_Control = 0;
	Spin_flag = 1;
	spin_left_pwm=spin_left_set;
	spin_right_pwm=spin_right_set;
}

//�������Ҷ����բ�ţ�PWM		����������բ�ţ�
void Set_Servo(uint16_t servo_left_set, uint16_t servo_right_set){
	servo_left_pwm=servo_left_set;
	servo_right_pwm=servo_right_set;
}

static void clear_angular_pid_buf(void){
	err_sum = 0;		//���֮��
	last_error = 0;		//��һʱ�����
}
                                  
static void clear_pid_buf(void){
	error_sum = 0;		//���֮��
	last_error = 0;		//��һʱ�����
}

static void clear_pos_pid_buf(void){
	distance_sum = 0;		//���֮��
	last_distance = 0;		//��һʱ�����
}

static int angular_speed_PID_Control(void){
	float result = 0, error = 0, d_error = 0, dk = 0;
	
	static float last_result = 0;

	error = (Ref_Angular_Speed - Vel_w) * 10;  //imu������gps�����෴
	error = (Ref_Angular_Speed + Gyr[2]*5);		//���ٶ����=�������ٶ�-z����ٶȣ�������ʱ��Ϊ������������ʱ��Ϊ����
	d_error = error - last_error;							//���仯��
	
	error_sum = error_sum + PID_I(error, d_error) * error;
	lim(error_sum,(-d_dut),d_dut);
	
	result = PID_P(error, d_error) * error + error_sum + PID_D(error, d_error) * d_error;
	/*���ٶȾ���ֵ����25ʱ�����Ƶ�������������25���������Ƶ���������Χ������6*/
	if(abs((int)last_result)>(kk*25)) dk =kk*25;	//��һ�ε�result�Ƚϴ������õ��������޷�dk = 375
	else dk =kk*6; 																//kk = 15, dk = 90
	pre(last_result,result,dk);
	last_error = error;
	result = last_result;
	lim(result,(-d_dut),d_dut);
	return (int)result;
}

void angular_speed_Control(void){
	int u = 0;
	if(Closed_Loop_Control){	
		lim(Ref_Speed,-100,100);											//�������ٶ������ڣ�-100��100��
		lim(Ref_Angular_Speed,-100,100);							//���������ٶ������ڣ�-100��100��
		if(fabs(Ref_Speed) >= 1 || fabs(Ref_Angular_Speed) >= 1) u = angular_speed_PID_Control();
		else clear_pid_buf();
		if(Ref_Speed>0 && !Key_flag) {Set_Servo(left_H,right_H);Relay4_On();}						//û�а�������բ��ʱĬ��ǰ��ʱբ���ڸ�λ
		else if(Ref_Speed<0 && !Key_flag){Set_Servo(left_L,right_L);Relay4_Off();}			//û�а�������բ��ʱĬ�Ϻ���ʱբ���ڵ�λ
		motor_left_pwm = (uint16_t)(kk * Ref_Speed + dut_M + u);
		motor_right_pwm = (uint16_t)(kk * Ref_Speed + dut_M - u);
		motor_pwm_adjust(&motor_left_pwm, &motor_right_pwm, dut_L, dut_H);							//�������Ҳ��ٲ������ҵ��������(dut_L,dut_H)��Χ��
		Pwm_Set(TIM2,3, motor_left_pwm);//��ҽ���?
		Pwm_Set(TIM2,4, motor_right_pwm);//�ҹҽ���		
	}
}

//���ҵ��pwm���ſ���	����������С����ת��
void spin_Control(void){
	uint8_t dp=150;
	if(Spin_flag){															//�ж��Ƿ�ΪС������
	spin_left_duty=Pwm_Read(TIM2,3);						//��ȡ��ʱ��2��3,4ͨ����pwm���ֵ
	spin_right_duty=Pwm_Read(TIM2,4);						//
	pre(spin_left_duty, spin_left_pwm, dp);			//������ֵ(pwm)�ڵ�ǰֵ(duty)��dp֮�⣬���޷�����
	pre(spin_right_duty, spin_right_pwm, dp);		//
	Pwm_Set(TIM2,3, spin_left_duty);						//���ö�ʱ��2��3,4ͨ����pwmֵ
	Pwm_Set(TIM2,4, spin_right_duty);						//
	if(!Key_flag)Set_Servo(left_H, right_H);		//բ���쳣���Ʊ�־λ
	}
}

//�������		����������բ�ţ�
void servo_Control(void){
	uint8_t dp=100;
	uint16_t servo_left_duty = Pwm_Read(TIM3,1);
	uint16_t servo_right_duty = Pwm_Read(TIM3,2);
	pre(servo_left_duty, servo_left_pwm, dp);
	pre(servo_right_duty, servo_right_pwm, dp);
	Pwm_Set(TIM3, 1, servo_left_duty);
	Pwm_Set(TIM3, 2, servo_right_duty);
}

//�����ʼ��
void servo_Init(void){
	uint8_t dp=40;
	uint16_t servo_left_duty = Pwm_Read(TIM3,1);
	uint16_t servo_right_duty = Pwm_Read(TIM3,2);
	do{
	pre(servo_left_duty, left_H, dp);
	pre(servo_right_duty, right_H, dp);
	Pwm_Set(TIM3, 1, servo_left_duty);
	Pwm_Set(TIM3, 2, servo_right_duty);
	servo_left_duty = Pwm_Read(TIM3,1);
	servo_right_duty = Pwm_Read(TIM3,2);}
	while (servo_left_duty!=left_H && servo_right_duty!=right_H);
}

//void Single_Control(float motor_left, float motor_right){
//	Closed_Loop_Control = 0;
//	clear_pid_buf();
//	Pwm_Set(TIM2,3, float2pwm(motor_left));
//	Pwm_Set(TIM2,4, float2pwm(motor_right));
//}

//ɲ������
void Stop_Control(void){
  //printf("stop\r\n");
	Closed_Loop_Control = 0;
	auto_flag = 0;
	clear_angular_pid_buf();
	clear_pid_buf();
	if(!Spin_flag){
		Pwm_Set(TIM2,3, dut_M);
		Pwm_Set(TIM2,4, dut_M);
	}
}

/***********************************************************************
			Ref_yaw Ϊ�Ƕ��Ʋ���
			Ref__Angular_Speed = Kp*e(k) + Ki*��e(k) + Kd*[e��k��-e(k-1)]
			Ref__Angular_Speed �ʣ�-100��100��(����ֵresult��Ref__Angular_Speed)
************************************************************************/
//float Yaw_PID(void)
//{
//	//float Kp = 0.8, Ki = 0.001, Kd = 0.001;
//	static float err = 0, result = 0, dk = 10;
//	static float last_result = 0;
//	err = Ref_yaw - Eular[2];
//	err = (err > 180) ? (err - 360) : ((err < -180) ? (360 + err) : err);		//ѡ�ӻ�����
//	err_sum += Ki*err;
//	lim(err_sum, -d_speed, d_speed);
//	result = Kp*err + err_sum + Kd*(err - last_err);
//	last_err = err;
////	printf("err_sum:%f\r\n err:%f\r\n", err_sum, err);
//	////////////���ƽ��ٶ�����////////////
//	pre(last_result, result, dk);															//�����޷�
//	result = last_result;
//	//////////////////////////////////////
//	lim(result, -d_speed, d_speed);														//���ֵ�޷�
//	return -result;			//�������ʱ�뷽�����󣬽��ٶ���ʱ�뷽���С�����صĽ��ٶȸ���ֵӦ����
//}
float Yaw_PID(void)
{
	static float err = 0, result = 0, dk = 6, pid_output = 0, pid_output_max = 0;
	static float last_result = 0;
	err = yaw_err;
	err = (err > 180) ? (err - 360) : ((err < -180) ? (360 + err) : err);		//ѡ�ӻ�����
	pid_output_max = Kp*180;
	err_sum += Ki*err;
	lim(err_sum, -pid_output_max, pid_output_max);
	//lim(err_sum, -d_angularSpeed, d_angularSpeed);
	//////////////////////////////////////////
	pid_output = Kp*err + err_sum + Kd*(err - last_err);
	
	result = (pid_output/pid_output_max)*d_angularSpeed;	//��һ�������ٶȵ�������
	//////////////////////////////////////////
	//result = Kp*err + err_sum + Kd*(err - last_err);
	last_err = err;
//	printf("err_sum:%f\r\n err:%f\r\n", err_sum, err);
	/*���ƽ��ٶ��������޸ĳɰ���ǰ�ٶȸ���dk������ʱ�ý�С��dk���ٶȽϴ�ʱ�ɷſ�����*/
//	pre(last_result, result, dk);															//�����޷�
//	result = last_result;
//	lim(result, -d_angularSpeed, d_angularSpeed);														//���ֵ�޷�
	
	if(abs((int)last_result)>25) dk =25;	//��һ�εĽ��ٶȱȽϴ������õ��������޷�dk = 25
	else dk = 6; 													//��С������dk = 6
	pre(last_result,result,dk);
	result = last_result;
	lim(result,(-d_angularSpeed),d_angularSpeed);	
	
	return -result;			//�������ʱ�뷽�����󣬽��ٶ���ʱ�뷽���С�����صĽ��ٶȸ���ֵӦ����
} 

void Yaw_Control(void)
{

	static int i = 0;		//�⻷����Ϊ200ms
	if(i == 3)
	{
		if(auto_flag)
		{
			printf("yaw_err:%f\r\n",yaw_err);
			if(fabs(yaw_err) > 1) 
			{
				Ref_Angular_Speed = Yaw_PID();
				
				printf("ref_AS:%f\r\n",Ref_Angular_Speed);
			}
			else 
			{
					clear_angular_pid_buf();
					Ref_Angular_Speed = 0;
			}	
		}
		i = 0;		
	}
	i++;
}


//float gteDistance(float* Ref_pos,float* Cur_pos)
//{
//		return ����;
//}

void calculate_error(float dis, float ref_yaw, float act_yaw)
{
		pos_err = dis;	
		yaw_err = ref_yaw - act_yaw;
}

//λ�ÿ���PID ����ֵ������
float Pos_PID(void)
{
	static float distance = 0, result = 0, dk = 6, pid_output = 0, pid_output_max = 0;
	static float last_result = 0;
	
	distance = pos_err;

	pid_output_max = pos_Kp*15;  //15���ڽ���pid����
	distance_sum += pos_Ki*distance;
	lim(distance_sum, -pid_output_max, pid_output_max);
	pid_output = pos_Kp*distance + distance_sum + pos_Kd*(distance - last_distance);
	
	result = (pid_output/pid_output_max)*d_Speed;	//��һ�������ŵ�������
	//////////////////////////////////////////
	//result = Kp*distance + distance_sum + Kd*(distance - last_distance);
	last_distance = distance;
//	printf("err_sum:%f\r\n err:%f\r\n", err_sum, err);
	/*���ƽ��ٶ��������޸ĳɰ���ǰ�ٶȸ���dk������ʱ�ý�С��dk���ٶȽϴ�ʱ�ɷſ�����*/
//	pre(last_result, result, dk);															//�����޷�
//	result = last_result;
//	lim(result, -d_angularSpeed, d_angularSpeed);														//���ֵ�޷�
	
	if(abs((int)last_result)>25) dk =25;	//��һ�εĽ��ٶȱȽϴ������õ��������޷�dk = 25
	else dk = 6; 													//��С������dk = 6
	pre(last_result,result,dk);
	result = last_result;
	lim(result,(-d_Speed),d_Speed);	
	
	return result;			//�������ʱ�뷽�����󣬽��ٶ���ʱ�뷽���С�����صĽ��ٶȸ���ֵӦ����
} 

//λ�ÿ���PID
void Pos_Control(void)
{
	if(auto_flag)
	{
		if(fabs(yaw_err) < 30) //yaw���Ϸ�Χ
		{
				if(fabs(pos_err)<15 && fabs(pos_err)>2) 
				{
					Ref_Speed = Pos_PID();
				}
				else if(fabs(pos_err) >= 15) 
				{
						clear_pos_pid_buf();
						Ref_Speed = 50;
				}
				else if(fabs(pos_err) <= 2)
				{
						clear_pos_pid_buf();
						Ref_Speed = 0;
				}
		}
		else if(fabs(yaw_err) >= 30)
		{
						clear_pos_pid_buf();
						Ref_Speed = 0;
		}
		printf("Ref_Speed:%f\r\n",Ref_Speed);		
	}
}


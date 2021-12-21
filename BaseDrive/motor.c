#include "motor.h"
#include <math.h>
#include "pid.h"

float Kp = 1.6, Ki = 0, Kd = 1.6;
float Ref_yaw;

//位置pid参数
float pos_Kp = 1.6, pos_Ki = 0, pos_Kd = 1.6;
//float Ref_pos;

float yaw_err = 0, pos_err = 0;

float vel_x=0, Vel_y=0, Vel_w=0;
int kk = (dut_H-dut_L)/200;				//角速度变化1个单位，对应pwm占空比变化量
int d_dut = (dut_H-dut_L)/2;			//角速度控制量最大范围限制

float d_angularSpeed = 100;							//角速度信号范围为（-100，+100）

float d_Speed = 50;								//油门信号范围为（-100，+100）

static float err_sum = 0, last_err = 0;			//角度环pid_buf

static float error_sum = 0, last_error = 0;		//角速度环pid_buf

static float distance_sum = 0, last_distance = 0;			//位置控制pid_buf

uint16_t motor_left_pwm = dut_M, motor_right_pwm = dut_M;//摇杆控制时的左右电机pwm

//按键控制
uint16_t servo_left_pwm = left_H, servo_right_pwm = right_H;	//舵机（闸门）pwm初始值
uint16_t spin_left_pwm = dut_M, spin_right_pwm = dut_M;		//初始化左右电机PWM设置值为中值
uint16_t spin_left_duty = dut_M;			//初始化左右电机PWM引脚的输出值为中值
uint16_t spin_right_duty = dut_M;			//

static float Ref_Speed = 0;						//用户期望速度
float Ref_Angular_Speed = 0;					//期望角速度

bool Closed_Loop_Control = 0;					//（摇杆）循环控制标志位（发送一次摇杆值后改变ref_S、ref_AS，后续定时器按期望值闭环控制）
bool Spin_flag = 0;										//按键控制标志位
bool Key_flag = 0;										//闸门异常控制标志位
bool auto_flag = 0;										//自动航向标志位

uint16_t float2pwm(float x){			//【无用】
	uint16_t result = dut_M;
	lim(x,-100,100);
  result = result + (uint16_t)(x*(dut_H-dut_M)/100.0f);
	return result;
}

//将差速限制在dut_L和dut_H之间
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

//设置循环控制参数：速度、角速度
void Set_Loop_Para(float speed_set, float angular_speed_set){
	Closed_Loop_Control = 1;
	Spin_flag = 0;
	Ref_Speed = speed_set;
	Ref_Angular_Speed = angular_speed_set;
	//printf("参考速度：%f\r\n参考角速度：%f\r\n",Ref_Speed,Ref_Angular_Speed);
}

//设置左右电机PWM （按键控制小幅度转向）
void Set_Spin(uint16_t spin_left_set, uint16_t spin_right_set){
	Closed_Loop_Control = 0;
	Spin_flag = 1;
	spin_left_pwm=spin_left_set;
	spin_right_pwm=spin_right_set;
}

//设置左右舵机（闸门）PWM		（按键控制闸门）
void Set_Servo(uint16_t servo_left_set, uint16_t servo_right_set){
	servo_left_pwm=servo_left_set;
	servo_right_pwm=servo_right_set;
}

static void clear_angular_pid_buf(void){
	err_sum = 0;		//误差之和
	last_error = 0;		//上一时刻误差
}
                                  
static void clear_pid_buf(void){
	error_sum = 0;		//误差之和
	last_error = 0;		//上一时刻误差
}

static void clear_pos_pid_buf(void){
	distance_sum = 0;		//误差之和
	last_distance = 0;		//上一时刻误差
}

static int angular_speed_PID_Control(void){
	float result = 0, error = 0, d_error = 0, dk = 0;
	
	static float last_result = 0;

	error = (Ref_Angular_Speed - Vel_w) * 10;  //imu锟斤拷锟斤拷锟斤拷gps锟斤拷锟斤拷锟洁反
	error = (Ref_Angular_Speed + Gyr[2]*5);		//角速度误差=期望角速度-z轴角速度（期望逆时针为负，陀螺仪逆时针为正）
	d_error = error - last_error;							//误差变化率
	
	error_sum = error_sum + PID_I(error, d_error) * error;
	lim(error_sum,(-d_dut),d_dut);
	
	result = PID_P(error, d_error) * error + error_sum + PID_D(error, d_error) * d_error;
	/*角速度绝对值大于25时，限制单次增量不大于25，否则限制单次增量范围不大于6*/
	if(abs((int)last_result)>(kk*25)) dk =kk*25;	//上一次的result比较大则设置单次增幅限幅dk = 375
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
		lim(Ref_Speed,-100,100);											//将期望速度限制在（-100，100）
		lim(Ref_Angular_Speed,-100,100);							//将期望角速度限制在（-100，100）
		if(fabs(Ref_Speed) >= 1 || fabs(Ref_Angular_Speed) >= 1) u = angular_speed_PID_Control();
		else clear_pid_buf();
		if(Ref_Speed>0 && !Key_flag) {Set_Servo(left_H,right_H);Relay4_On();}						//没有按键控制闸门时默认前进时闸门在高位
		else if(Ref_Speed<0 && !Key_flag){Set_Servo(left_L,right_L);Relay4_Off();}			//没有按键控制闸门时默认后退时闸门在低位
		motor_left_pwm = (uint16_t)(kk * Ref_Speed + dut_M + u);
		motor_right_pwm = (uint16_t)(kk * Ref_Speed + dut_M - u);
		motor_pwm_adjust(&motor_left_pwm, &motor_right_pwm, dut_L, dut_H);							//保持左右差速并将左右电机限制在(dut_L,dut_H)范围内
		Pwm_Set(TIM2,3, motor_left_pwm);//锟斤拷医锟斤拷锟?
		Pwm_Set(TIM2,4, motor_right_pwm);//锟揭挂斤拷锟斤拷		
	}
}

//左右电机pwm引脚控制	（按键控制小幅度转向）
void spin_Control(void){
	uint8_t dp=150;
	if(Spin_flag){															//判断是否为小幅控制
	spin_left_duty=Pwm_Read(TIM2,3);						//读取定时器2的3,4通道的pwm输出值
	spin_right_duty=Pwm_Read(TIM2,4);						//
	pre(spin_left_duty, spin_left_pwm, dp);			//若设置值(pwm)在当前值(duty)±dp之外，做限幅调整
	pre(spin_right_duty, spin_right_pwm, dp);		//
	Pwm_Set(TIM2,3, spin_left_duty);						//设置定时器2的3,4通道的pwm值
	Pwm_Set(TIM2,4, spin_right_duty);						//
	if(!Key_flag)Set_Servo(left_H, right_H);		//闸门异常控制标志位
	}
}

//舵机控制		（按键控制闸门）
void servo_Control(void){
	uint8_t dp=100;
	uint16_t servo_left_duty = Pwm_Read(TIM3,1);
	uint16_t servo_right_duty = Pwm_Read(TIM3,2);
	pre(servo_left_duty, servo_left_pwm, dp);
	pre(servo_right_duty, servo_right_pwm, dp);
	Pwm_Set(TIM3, 1, servo_left_duty);
	Pwm_Set(TIM3, 2, servo_right_duty);
}

//舵机初始化
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

//刹车控制
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
			Ref_yaw 为角度制参数
			Ref__Angular_Speed = Kp*e(k) + Ki*∑e(k) + Kd*[e（k）-e(k-1)]
			Ref__Angular_Speed ∈（-100，100）(返回值result即Ref__Angular_Speed)
************************************************************************/
//float Yaw_PID(void)
//{
//	//float Kp = 0.8, Ki = 0.001, Kd = 0.001;
//	static float err = 0, result = 0, dk = 10;
//	static float last_result = 0;
//	err = Ref_yaw - Eular[2];
//	err = (err > 180) ? (err - 360) : ((err < -180) ? (360 + err) : err);		//选劣弧方向
//	err_sum += Ki*err;
//	lim(err_sum, -d_speed, d_speed);
//	result = Kp*err + err_sum + Kd*(err - last_err);
//	last_err = err;
////	printf("err_sum:%f\r\n err:%f\r\n", err_sum, err);
//	////////////限制角速度增幅////////////
//	pre(last_result, result, dk);															//增量限幅
//	result = last_result;
//	//////////////////////////////////////
//	lim(result, -d_speed, d_speed);														//输出值限幅
//	return -result;			//航向角逆时针方向增大，角速度逆时针方向减小，返回的角速度跟踪值应反号
//}
float Yaw_PID(void)
{
	static float err = 0, result = 0, dk = 6, pid_output = 0, pid_output_max = 0;
	static float last_result = 0;
	err = yaw_err;
	err = (err > 180) ? (err - 360) : ((err < -180) ? (360 + err) : err);		//选劣弧方向
	pid_output_max = Kp*180;
	err_sum += Ki*err;
	lim(err_sum, -pid_output_max, pid_output_max);
	//lim(err_sum, -d_angularSpeed, d_angularSpeed);
	//////////////////////////////////////////
	pid_output = Kp*err + err_sum + Kd*(err - last_err);
	
	result = (pid_output/pid_output_max)*d_angularSpeed;	//归一化到角速度的区间上
	//////////////////////////////////////////
	//result = Kp*err + err_sum + Kd*(err - last_err);
	last_err = err;
//	printf("err_sum:%f\r\n err:%f\r\n", err_sum, err);
	/*限制角速度增幅，修改成按当前速度给定dk，启动时用较小的dk，速度较大时可放宽限制*/
//	pre(last_result, result, dk);															//增量限幅
//	result = last_result;
//	lim(result, -d_angularSpeed, d_angularSpeed);														//输出值限幅
	
	if(abs((int)last_result)>25) dk =25;	//上一次的角速度比较大则设置单次增幅限幅dk = 25
	else dk = 6; 													//较小则设置dk = 6
	pre(last_result,result,dk);
	result = last_result;
	lim(result,(-d_angularSpeed),d_angularSpeed);	
	
	return -result;			//航向角逆时针方向增大，角速度逆时针方向减小，返回的角速度跟踪值应反号
} 

void Yaw_Control(void)
{

	static int i = 0;		//外环周期为200ms
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
//		return 距离;
//}

void calculate_error(float dis, float ref_yaw, float act_yaw)
{
		pos_err = dis;	
		yaw_err = ref_yaw - act_yaw;
}

//位置控制PID 返回值油门量
float Pos_PID(void)
{
	static float distance = 0, result = 0, dk = 6, pid_output = 0, pid_output_max = 0;
	static float last_result = 0;
	
	distance = pos_err;

	pid_output_max = pos_Kp*15;  //15米内进入pid控制
	distance_sum += pos_Ki*distance;
	lim(distance_sum, -pid_output_max, pid_output_max);
	pid_output = pos_Kp*distance + distance_sum + pos_Kd*(distance - last_distance);
	
	result = (pid_output/pid_output_max)*d_Speed;	//归一化到油门的区间上
	//////////////////////////////////////////
	//result = Kp*distance + distance_sum + Kd*(distance - last_distance);
	last_distance = distance;
//	printf("err_sum:%f\r\n err:%f\r\n", err_sum, err);
	/*限制角速度增幅，修改成按当前速度给定dk，启动时用较小的dk，速度较大时可放宽限制*/
//	pre(last_result, result, dk);															//增量限幅
//	result = last_result;
//	lim(result, -d_angularSpeed, d_angularSpeed);														//输出值限幅
	
	if(abs((int)last_result)>25) dk =25;	//上一次的角速度比较大则设置单次增幅限幅dk = 25
	else dk = 6; 													//较小则设置dk = 6
	pre(last_result,result,dk);
	result = last_result;
	lim(result,(-d_Speed),d_Speed);	
	
	return result;			//航向角逆时针方向增大，角速度逆时针方向减小，返回的角速度跟踪值应反号
} 

//位置控制PID
void Pos_Control(void)
{
	if(auto_flag)
	{
		if(fabs(yaw_err) < 30) //yaw符合范围
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


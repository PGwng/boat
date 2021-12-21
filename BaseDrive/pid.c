#include "pid.h"
#include <math.h>

float e_d[6] = {-50.0,-20.0,-10.0,10.0,20.0,50.0};
float ec_d[4] = {-30.0,-10.0,10.0,30};

float f_P[5] = {20.0,15.0,10.0,5.0,0.0};
float f_I[5] = {1.0,0.50,0.25,0.10,0.0};
float f_D[5] = {50.0,30.0,20.0,10.0,0.0};

float PID_P(float e,float ec){
		if(e < e_d[0])
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
		else if(e < e_d[1])
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
		else if(e < e_d[2])
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
		else if(e < e_d[3])
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
		else if(e < e_d[4])
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
		else if(e < e_d[5])
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
		else
		{
			  if(ec < ec_d[0]) return f_P[1];
				else if(ec < ec_d[1]) return f_P[1];
				else if(ec < ec_d[2]) return f_P[1];
				else if(ec < ec_d[3]) return f_P[1];
				else return f_P[1];
		}
}

float PID_I(float e,float ec){
		if(e < e_d[0])
		{
			  if(ec < ec_d[0]) return f_I[4];
				else if(ec < ec_d[1]) return f_I[4];
				else if(ec < ec_d[2]) return f_I[4];
				else if(ec < ec_d[3]) return f_I[4];
				else return f_I[4];
		}
		else if(e < e_d[1])
		{
			  if(ec < ec_d[0]) return f_I[3];
				else if(ec < ec_d[1]) return f_I[3];
				else if(ec < ec_d[2]) return f_I[3];
				else if(ec < ec_d[3]) return f_I[3];
				else return f_I[3];
		}
		else if(e < e_d[2])
		{
			  if(ec < ec_d[0]) return f_I[3];
				else if(ec < ec_d[1]) return f_I[2];
				else if(ec < ec_d[2]) return f_I[2];
				else if(ec < ec_d[3]) return f_I[2];
				else return f_I[3];
		}
		else if(e < e_d[3])
		{
			  if(ec < ec_d[0]) return f_I[0];
				else if(ec < ec_d[1]) return f_I[0];
				else if(ec < ec_d[2]) return f_I[0];
				else if(ec < ec_d[3]) return f_I[0];
				else return f_I[0];
		}
		else if(e < e_d[4])
		{
			  if(ec < ec_d[0]) return f_I[2];
				else if(ec < ec_d[1]) return f_I[2];
				else if(ec < ec_d[2]) return f_I[2];
				else if(ec < ec_d[3]) return f_I[2];
				else return f_I[2];
		}
		else if(e < e_d[5])
		{
			  if(ec < ec_d[0]) return f_I[3];
				else if(ec < ec_d[1]) return f_I[3];
				else if(ec < ec_d[2]) return f_I[3];
				else if(ec < ec_d[3]) return f_I[3];
				else return f_I[3];
		}
		else
		{
			  if(ec < ec_d[0]) return f_I[4];
				else if(ec < ec_d[1]) return f_I[4];
				else if(ec < ec_d[2]) return f_I[4];
				else if(ec < ec_d[3]) return f_I[4];
				else return f_I[4];
		}
}

float PID_D(float e,float ec){
		if(e < e_d[0])
		{
			  if(ec < ec_d[0]) return f_D[2];
				else if(ec < ec_d[1]) return f_D[3];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[3];
				else return f_D[2];
		}
		else if(e < e_d[1])
		{
			  if(ec < ec_d[0]) return f_D[1];
				else if(ec < ec_d[1]) return f_D[2];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[2];
				else return f_D[1];
		}
		else if(e < e_d[2])
		{
			  if(ec < ec_d[0]) return f_D[0];
				else if(ec < ec_d[1]) return f_D[1];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[1];
				else return f_D[0];
		}
		else if(e < e_d[3])
		{
			  if(ec < ec_d[0]) return f_D[4];
				else if(ec < ec_d[1]) return f_D[4];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[4];
				else return f_D[4];
		}
		else if(e < e_d[4])
		{
			  if(ec < ec_d[0]) return f_D[0];
				else if(ec < ec_d[1]) return f_D[1];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[1];
				else return f_D[0];
		}
		else if(e < e_d[5])
		{
			  if(ec < ec_d[0]) return f_D[1];
				else if(ec < ec_d[1]) return f_D[2];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[2];
				else return f_D[1];
		}
		else
		{
			  if(ec < ec_d[0]) return f_D[2];
				else if(ec < ec_d[1]) return f_D[3];
				else if(ec < ec_d[2]) return f_D[4];
				else if(ec < ec_d[3]) return f_D[3];
				else return f_D[2];
		}
}

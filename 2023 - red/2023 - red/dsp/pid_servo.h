#ifndef __PID_SERVO_H__
#define __PID_SERVO_H__
#include "stm32f1xx.h"
 
typedef struct
{
	float SetPosition;//设定值
	float ActualPosition;//实际值
	float err;
	float err_last;
	float Kp;
	float Ki;
	float Kd;
	float out;//执行器的变量
	float integral;//积分值
}pid_typedef;
 
void PID_Init(void);
float PIDx_realize(float ActualPosition,float SetPosition);
float PIDy_realize(float ActualPosition,float SetPosition);
 
#endif


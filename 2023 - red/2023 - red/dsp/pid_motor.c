#include "main.h"
#include "pid_motor.h"
#include "stdio.h"

void PID_V_Init(PID* pid,float kp,float ki,float kd)
{
	pid->V.Kp = kp;
	pid->V.Ki = ki;
	pid->V.Kd = kd;
	pid->V.error_now = 0;
	pid->V.error_last = 0;
	pid->V.error_last_l = 0;
	pid->V.Target = 0;
	pid->V.mean = 0;
}

void PID_A_Init(PID* pid,float kp,float ki,float kd)
{
	pid->P.Kp = kp;
	pid->P.Ki = ki;
	pid->P.Kd = kd;
	pid->P.error_now = 0;
	pid->P.error_last = 0;
	pid->P.Integral = 0;
	pid->P.Target = 0;
}

float updataPID_V(PID* pid,float input)
{
	pid->V.error_last_l = pid->V.error_last;
	pid->V.error_last = pid->V.error_now;
	pid->V.error_now = pid->V.Target - input;
	pid->V.mean += (pid->V.Kp*(pid->V.error_now - pid->V.error_last))
		 +(pid->V.Ki*pid->V.error_now)
		 +(pid->V.Kd*(pid->V.error_now-2*pid->V.error_last+pid->V.error_last_l));
	return pid->V.mean;
}

float updataPID_A(PID* pid,float input)
{
	pid->P.error_last = pid->P.error_now;
	pid->P.error_now = pid->P.Target - input;
	if(pid->P.error_now<=-180)
	{
		pid->P.error_now += 360;
	}
	pid->P.Integral += pid->P.error_now;	
	if(pid->P.Integral>=180)
	pid->P.Integral = 180;
	if(pid->P.Integral<=-180)
	pid->P.Integral = -180;
	
	pid->P.mean = pid->P.Kp * pid->P.error_now
		+ pid->P.Ki * pid->P.Integral 
		+ pid->P.Kd * (pid->P.error_now - pid->P.error_last);
        //限幅
	// if(pid->P.mean>5)
	// {
	// 	pid->P.mean=5;
	// }
	// 	if(pid->P.mean<-5)
	// {
	// 	pid->P.mean=-5;
	// }
	return pid->P.mean;

}

void Set_PID_V_Target(PID* pid,float target)
{
	pid->V.Target = target;
}

void Set_PID_A_Target(PID* pid,float target)
{
	pid->P.Target = target;
}

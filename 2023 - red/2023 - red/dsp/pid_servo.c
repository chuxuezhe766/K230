#include "pid_servo.h"

 
pid_typedef pid1;
pid_typedef pid2;
 
void PID_Init(void)
{
    pid1.SetPosition=0;
    pid1.ActualPosition=0.0;
    pid1.err=0.0;
    pid1.err_last=0.0;
    pid1.out=0.0;
    pid1.integral=0.0;
    pid1.Kp=0.025;
    pid1.Ki=0;
    pid1.Kd=0.017;
  
	pid2.SetPosition=0;
    pid2.ActualPosition=0.0;
    pid2.err=0.0;
    pid2.err_last=0.0;
    pid2.out=0.0;
    pid2.integral=0.0;
    pid2.Kp=0.025;
    pid2.Ki=0;
    pid2.Kd=0.017;
}
 
 
 
float PIDx_realize(float ActualPosition,float SetPosition)
{
	pid1.ActualPosition=ActualPosition;
	pid1.SetPosition=SetPosition;
    pid1.err=pid1.SetPosition-pid1.ActualPosition;
    pid1.integral+=pid1.err;
    pid1.out=pid1.Kp*pid1.err+pid1.Ki*pid1.integral+pid1.Kd*(pid1.err-pid1.err_last);
    pid1.err_last=pid1.err;
 
    return pid1.out;
}
 
float PIDy_realize(float ActualPosition,float SetPosition)
{
	pid2.ActualPosition=ActualPosition;
	pid2.SetPosition=SetPosition;
    pid2.err=pid2.ActualPosition-pid2.SetPosition;
    pid2.integral+=pid2.err;
    pid2.out=pid2.Kp*pid2.err+pid2.Ki*pid2.integral+pid2.Kd*(pid2.err-pid2.err_last);
    pid2.err_last=pid2.err;
 
    return pid2.out;
}




#ifndef __PID_MOTOR_H__
#define __PID_MOTOR_H__

/***************速度环参数****************/
typedef struct {
	float Kp;
	float Ki;
	float Kd;
	float error_now;
	float error_last;
	float error_last_l;
	float Target;
	float mean;
}speed;
/***************角度环参数****************/
typedef struct {
	float Kp;
	float Ki;
	float Kd;
	float error_now;
	float error_last;
	float Integral;
	float Target;
	float mean;
}position;

typedef struct {
	speed V;
	position P;
}PID;
void PID_V_Init(PID* pid,float kp,float ki,float kd);
void PID_A_Init(PID* pid,float kp,float ki,float kd);
float updataPID_V(PID* pid,float input);
float updataPID_A(PID* pid,float input);
void Set_PID_V_Target(PID* pid,float target);
void Set_PID_A_Target(PID* pid,float target);
#endif


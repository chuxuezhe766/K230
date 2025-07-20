#include "pid_step.h"
#include "main.h"

void PID_Init(PID_TypeDef* pid, float kp, float ki, float kd, float out_min, float out_max) 
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
    pid->output_max = out_max;
    pid->output_min = out_min;
}

float PID_Compute(PID_TypeDef* pid, float error) 
{
    float derivative = (error - pid->prev_error) / 0.01;
    pid->integral += error * 0.01;

    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

    // 限幅
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    pid->output = output;
    pid->prev_error = error;

    return output;
}





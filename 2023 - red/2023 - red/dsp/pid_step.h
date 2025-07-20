#ifndef __PID_STEP_H__
#define __PID_STEP_H__

typedef struct {
    float kp;
    float ki;
    float kd;

    float prev_error;
    float integral;

    float output;  // PID输出（单位：mm，或 px，根据你使用的单位）
    float output_max;
    float output_min;
} PID_TypeDef;
void PID_Init(PID_TypeDef* pid, float kp, float ki, float kd, float out_min, float out_max);
float PID_Compute(PID_TypeDef* pid, float error);
#endif

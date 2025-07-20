#ifndef __TASK_H__
#define __TASK_H__
#include "stdint.h"
extern int32_t dx,dy;
extern int32_t abs_x,abs_y;//误差绝对值

void Reset_To_Origin(void);
uint32_t Calc_Delay_Time(float d, float pixel_per_degree, float step_angle, uint32_t freq);
void Task1();
void Task2();
void Task3();
void Task4();
void Task5();
#endif

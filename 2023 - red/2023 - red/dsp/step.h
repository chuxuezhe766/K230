#ifndef __STEP_H__
#define __STEP_H__
#include <stdint.h>


#define STEP_ANGLE 0.9f

void Step_Init();
void Turn_angle_A(uint8_t dir, uint16_t angle);
void Turn_angle_B(uint8_t dir, uint16_t angle);
void Motor1_Run(float angle);
void Motor2_Run(float angle);
void Tuen_Angle(uint16_t dir2,uint16_t angle2,uint16_t dir1,uint16_t angle1);



void Set_Dir_Y(uint8_t dir);
void Set_Dir_X(uint8_t dir);
void Set_Speed_Y(uint32_t speed_hz);
void Set_Speed_X(uint32_t speed_hz);

void Move_Angle_Y(uint8_t dir, float angle, uint32_t freq);
void Move_Angle_X(uint8_t dir, float angle, uint32_t freq);
#endif


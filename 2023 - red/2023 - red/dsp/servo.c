#include "servo.h"
#include "main.h"
#include "tim.h"
#include "pid_servo.h"
void Servo_Init()
{
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);//pa8
    __HAL_TIM_MOE_ENABLE(&htim1);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);//pa7
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);//pa6
}

void Servo_SetAngle(float angle)
{
    float temp;
    temp = angle / 180 * 2000.0 + 500;
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,temp);
}

void Set_Angle(float x,float y)
{
    //开环
    float temp1,temp2;
    temp1 = x / 180 * 2000.0 + 500;
    temp2 = y / 180 * 2000.0 + 500;
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,temp1);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,temp2);
    
    //闭环
    //cx为激光当前的x坐标，mid_x为中心点的坐标,servo为舵机0°的位置
    // servo1 += servo1 + PIDx_realize(cx,mid_x);
    // servo2 += servo2 + PIDx_realize(cy,mid_y);
    // //限幅
    // if(servo1 <= 25)
    // {
    //     servo1 = 25;
    // }
    // if(servo1 >= 125)
    // {
    //     servo1 = 125;
    // }

    // if(servo2 <= 25)
    // {
    //     servo2 = 25;
    // }
    // if(servo2 >= 125)
    // {
    //     servo2 = 125;
    // }
    // __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,servo1);
    // __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,servo2);
}

 
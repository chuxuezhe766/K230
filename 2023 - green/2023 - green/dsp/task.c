#include "main.h"
#include "step.h"
#include "servo.h"
#include "bsp_uart.h"
#include "pid_step.h"
#include "math.h"
#include "oled.h"
#include "key.h"

//位移对应系数
#define Kx 1.56
#define Ky 1.43
#define ver 80
double dx,dy = 0;//红绿激光的坐标差值
uint16_t Track_Flag = 0;//追踪标志位
int count_t = 0;



#define max_ver 400   // 最大速度
#define min_ver 16    // 最小速度

// 计算速度，距离越大速度越大，距离越小速度越小
int calc_speed(double dx, double dy) 
{
    double dist = sqrt(dx*dx + dy*dy); // 欧氏距离
    int speed = (int)(dist * 2); // 你可以调整这个系数
    if(speed > max_ver) speed = max_ver;
    if(speed < min_ver) speed = min_ver;
    return speed;
}

// //1.绿在红的右下角
// if(dx >= 10 && dy >= 10)
// {
//     Move_Angle_X(0, dx / Kx, ver_x);//x轴向左走
//     Move_Angle_Y(1, dy / Ky, ver_y);//y轴向上走
// }
// // 然后替换原来的 ver 参数
// Move_Angle_X(0, dx / Kx, ver_x);
// Move_Angle_Y(1, dy / Ky, ver_y);


//追踪红色激光
void Track_Red()
{
        //计算差值
        dx = Green_X - Red_X;
        dy = Green_Y - Red_Y;

        int ver_x = calc_speed(dx, 0); // x方向速度
        int ver_y = calc_speed(0, dy); // y方向速度

        //判断是否执行追踪函数
        if((fabs(dx) < 13) && (fabs(dy) < 13))
        {
                count_t++;
                if(count_t == 3)
                {
                        Track_Flag = 2;
                        return;
                }
        }
        //分类讨论，决定如何走

        if((fabs(dx) < 40) && (fabs(dy) < 40))
        {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
        }
        else
        {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
        }
        //1.绿在红的右下角
        if(dx >= 0 && dy >= 0)
        {
                Move_Angle_X(0,dx / Kx,ver_x);//x轴向左走
                Move_Angle_Y(0,dy / Ky,ver_y);//y轴向上走
        }

        //2.绿在红的右上角
        else if(dx >= 0 && dy <= 0)
        {
                Move_Angle_X(0,dx / Kx,ver_x);//x轴向左走
                Move_Angle_Y(1,fabs(dy / Ky),ver_y);//y轴向下走
        }

        //3.绿在红的左下角
        else if(dx <= 0 && dy >= 0)
        {
                Move_Angle_X(1,fabs(dx / Kx),ver_x);//x轴向右走
                Move_Angle_Y(0,dy / Ky,ver_y);//y轴向上走
        }

        //4.绿在红的左上角
        else if(dx <= 0 && dy <= 0)
        {
                Move_Angle_X(1,fabs(dx / Kx),ver_x);//x轴向右走
                Move_Angle_Y(1,fabs(dy / Ky),ver_y);//y轴向下走
        }
}


//第一问：追踪红色激光
void Task1()
{
        Track_Flag = 1;
}



void Task2()
{
}

void Task3()
{
}


void Task4()
{
        
}
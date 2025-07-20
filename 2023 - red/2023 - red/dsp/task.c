#include "main.h"
#include "usart.h"
#include "step.h"
#include "servo.h"
#include "bsp_uart.h"
#include "pid_step.h"
#include "math.h"
#include "oled.h"
#include "key.h"
#include "tim.h"
#include "timer.h"
static uint16_t state = 0;
int32_t dx,dy;//

int32_t max(uint32_t x,uint32_t y)
{
        if(x >= y)
        {
                return x;
        }
        else
        {
                return y;
        }
}

//第一问，回原点
void Reset_To_Origin(void)
{
        //计算误差值
        dx = current_x1 - Midpiont_X;
        dy = current_y1 - Midpiont_Y;
        //步进电机x轴移动1°，对应摄像头像素坐标位移0.84；
        //       y轴移动1°，对应摄像头像素坐标位移0.84;
        if(dx >= 0 && dy >= 0)//右下角
        {
                //x轴电机向左走，y轴电机向向上走
                Move_Angle_X(1,dx / 0.84,200);
                Move_Angle_Y(0,dy / 0.84,200);
        }
        if(dx >= 0 && dy < 0)//右上角
        {
                //x轴电机向左走，y轴电机向向下走
                Move_Angle_X(1,dx / 0.84,200);
                Move_Angle_Y(1,dy / 0.84,200);

        }
        if(dx < 0 && dy >= 0)//左下角
        {
                //x轴电机向右走，y轴电机向向上走
                Move_Angle_X(0,dx / 0.84,200);
                Move_Angle_Y(0,dy / 0.84,200);

        }
        if(dx < 0 && dy < 0)//左上角
        {
                //x轴电机向右走，y轴电机向向下走
                Move_Angle_X(0,dx / 0.84,200);
                Move_Angle_Y(1,dy / 0.84,200);
        }

}

//第二问沿着黑线行走
void Task1()
{
        //从左上角走到右上角 x = 216; y = 291;
        state = 0;
        if(state == 0)
        {
                Move_Angle_X(0,390,400);
                HAL_Delay(2000);
                state ++;
        }
        //从右上角走到右下角
        if(state == 1)
        {
                Move_Angle_Y(1,383,400);
                HAL_Delay(800);
                Move_Angle_X(0,2,180);
                HAL_Delay(1500);
                state ++;
        }
        //从右下角走到左下角
        if(state == 2)
        {
                Move_Angle_X(1,390,400);
                HAL_Delay(2000);
                state++;
        }
        // //从左下角走到左上角
        if(state == 3)
        {
                Move_Angle_Y(0,386,400);
                HAL_Delay(700);
                Move_Angle_X(1,1,200);
        }    
}



void Task2()
{
        //从左上角走到右上角
        Move_Angle_X(0,219,400);
        HAL_Delay(1000);
        //从右上角走到右下角
        Move_Angle_Y(1,151,400);
        HAL_Delay(1000);
        //从右下角走到左下角
        Move_Angle_X(1,220,400);
        HAL_Delay(1000);
        //从左下角走到左上角
        Move_Angle_Y(0,151,400);
        HAL_Delay(1000);
}
uint32_t arr[2][4] = {0};

void Task3()
{
        OLED_Clear();
        OLED_ShowNum(1,1,f,3);
        //计算黑胶布中心的各点坐标
        //拟合一条矩形框
        arr[0][0] = (W[0][0] + L[0][0]) / 2;//A点的x坐标
        arr[1][0] = (W[1][0] + L[1][0]) / 2;//A点的y坐标
        arr[0][1] = (W[0][1] + L[0][1]) / 2;//B点的x坐标
        arr[1][1] = (W[1][1] + L[1][1]) / 2;//B点的y坐标      
        arr[0][2] = (W[0][2] + L[0][2]) / 2;//C点的x坐标
        arr[1][2] = (W[1][2] + L[1][2]) / 2;//C点的y坐标       
        arr[0][3] = (W[0][3] + L[0][3]) / 2;//D点的x坐标
        arr[1][3] = (W[1][3] + L[1][3]) / 2;//D点的y坐标
        //A点为起始点  A ——> B ——> C ——> D
        double dx1,dy1,dx2,dy2,dx3,dy3,dx4,dy4 = 0;
        //A ——> B
        dx1 = arr[0][1] - arr[0][0];
        dy1 = arr[1][1] - arr[1][0];
        OLED_ShowNum(2,1,dx1,3);
        OLED_ShowNum(2,5,dy1,3);
        Move_Angle_X(0,(dx1 + 3) / 0.84,300.0  * (double)(dx1 / dy1));//向右走dx1的距离
        Move_Angle_Y(1,(dy1 + 3) / 0.84,300.0);//向下走dy1的距离
        HAL_Delay(2000);

        //B ——> C
        dx2 = arr[0][1] - arr[0][2];
        dy2 = arr[1][2] - arr[1][1];
        OLED_ShowNum(2,9,dx2,3);
        OLED_ShowNum(2,13,dy2,3);
        if(dy2 == 0)
        {
                Move_Angle_X(1,(dx2 + 4) / 0.84,300.0);//向左走dx2的距离
        }
        else
        {
                Move_Angle_X(1,dx2 / 0.84,300.0  * (double)(dx2 / dy2));//向左走dx2的距离
                Move_Angle_Y(1,dy2 / 0.84,300.0);//向下走dy2的距离
        }
        HAL_Delay(2000);

        //C ——> D
        dx3 = arr[0][2] - arr[0][3];
        dy3 = arr[1][2] - arr[1][3];
        Move_Angle_X(1,(dx3 + 3) / 0.84,300.0  * (double)(dx3 / dy3));//向左走dx3的距离
        Move_Angle_Y(0,(dy3 + 3) / 0.84,300.0);//向上走dy3的距离
        HAL_Delay(2000);

        //D ——> A
        dx4 = arr[0][0] - arr[0][3];
        dy4 = arr[1][3] - arr[1][0];
        if(dy4 == 0)
        {
                Move_Angle_X(0,dx4 / 0.84,300.0);//向右走dx4的距离
        }
        else
        {
                Move_Angle_X(0,dx4 / 0.84,300.0  * (double)(dx4 / dy4));//向右走dx4的距离
                Move_Angle_Y(0,dy4 / 0.84,300.0);//向上走dy4的距离
        }
        HAL_Delay(1000);
}


void Task4()
{
        //从左上角走到右上角
        Move_Angle_X(0,219,120);
        HAL_Delay(2000);
        //从右上角走到右下角
        Move_Angle_Y(1,151,120);
        HAL_Delay(1500);
        //从右下角走到左下角
        Move_Angle_X(1,220,120);
        HAL_Delay(2000);
        //从左下角走到左上角
        Move_Angle_Y(0,151,120);
}

void Task5()
{
        //计算黑胶布中心的各点坐标
        //拟合一条矩形框
        arr[0][0] = (W[0][0] + L[0][0]) / 2;//A点的x坐标
        arr[1][0] = (W[1][0] + L[1][0]) / 2;//A点的y坐标
        arr[0][1] = (W[0][1] + L[0][1]) / 2;//B点的x坐标
        arr[1][1] = (W[1][1] + L[1][1]) / 2;//B点的y坐标      
        arr[0][2] = (W[0][2] + L[0][2]) / 2;//C点的x坐标
        arr[1][2] = (W[1][2] + L[1][2]) / 2;//C点的y坐标       
        arr[0][3] = (W[0][3] + L[0][3]) / 2;//D点的x坐标
        arr[1][3] = (W[1][3] + L[1][3]) / 2;//D点的y坐标
        //A点为起始点  A ——> B ——> C ——> D
        double dx1,dy1,dx2,dy2,dx3,dy3,dx4,dy4 = 0;
        //A ——> B
        dx1 = arr[0][1] - arr[0][0];
        dy1 = arr[1][1] - arr[1][0];
        OLED_ShowNum(2,1,dx1,3);
        OLED_ShowNum(2,5,dy1,3);
        Move_Angle_X(0,(dx1 + 3) / 0.84,80.0  * (double)(dx1 / dy1));//向右走dx1的距离
        Move_Angle_Y(1,(dy1 + 3) / 0.84,80.0);//向下走dy1的距离
        HAL_Delay(dy1 * 16.8);

        //B ——> C
        dx2 = arr[0][1] - arr[0][2];
        dy2 = arr[1][2] - arr[1][1];
        OLED_ShowNum(2,9,dx2,3);
        OLED_ShowNum(2,13,dy2,3);
        if(dy2 == 0)
        {
                Move_Angle_X(1,(dx2 + 4) / 0.84,200.0);//向左走dx2的距离
        }
        else
        {
                Move_Angle_X(1,dx2 / 0.84,80.0  * (double)(dx2 / dy2));//向左走dx2的距离
                Move_Angle_Y(1,dy2 / 0.84,80.0);//向下走dy2的距离
        }
        HAL_Delay(dy2 * 17);

        //C ——> D
        dx3 = arr[0][2] - arr[0][3];
        dy3 = arr[1][2] - arr[1][3];
        Move_Angle_X(1,(dx3 + 3) / 0.84,80.0  * (double)(dx3 / dy3));//向左走dx3的距离
        Move_Angle_Y(0,(dy3 + 3) / 0.84,80.0);//向上走dy3的距离
        HAL_Delay(dy3 * 16.8);

        //D ——> A
        dx4 = arr[0][0] - arr[0][3];
        dy4 = arr[1][3] - arr[1][0];
        if(dy4 == 0)
        {
                Move_Angle_X(0,dx4 / 0.84,80.0);//向右走dx4的距离
        }
        else
        {
                Move_Angle_X(0,dx4 / 0.84,80.0  * (double)(dx4 / dy4));//向右走dx4的距离
                Move_Angle_Y(0,dy4 / 0.84,80.0);//向上走dy4的距离
        }

}



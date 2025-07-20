#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "tim.h"
void Step_Init()
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);
}

//dir  0——>顺时针  1——>逆时针
void Turn_angle_A(uint8_t dir, uint16_t angle)//上面的电机
{
    uint16_t n;
    n = (int) angle / 0.9;
    if(dir)
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_SET);
    }
    for(uint16_t i = 0;i < n; i++)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_Delay(2);
    }
}
void Turn_angle_B(uint8_t dir, uint16_t angle)//下面的电机
{
    uint16_t n;
    n = (int)angle / 0.9;
    if(dir)
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
    }
    for(uint16_t i = 0;i < n; i++)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_Delay(2);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        HAL_Delay(2);
    }
}

void Tuen_Angle(uint16_t dir2,uint16_t angle2,uint16_t dir1,uint16_t angle1)
{
    uint16_t n1,n2;
    n1 = (int)angle1 / 0.9;
    n2 = (int)angle2 / 0.9;
    if(dir1)
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_SET);
    }
    if(dir2)
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
    }
    for(uint16_t i = 0;i < n1; i++)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_Delay(1);
    }
    for(uint16_t i = 0;i < n2; i++)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        HAL_Delay(1);
    }

}


//PWM驱动步进电机
//设置Y轴方向电机转动方向
void Set_Dir_Y(uint8_t dir)
{
    // dir = 0: 顺时针; 1: 逆时针
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11,dir ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

//设置X轴方向电机转动方向
void Set_Dir_X(uint8_t dir)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13,dir ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

// speed_hz: 步进频率（步/秒）
void Set_Speed_Y(uint32_t speed_hz)
{
    uint32_t arr = 1000000 / speed_hz;  // 1MHz 定时器
    __HAL_TIM_SET_AUTORELOAD(&htim5, arr - 1);
    __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, arr / 2);
}

void Set_Speed_X(uint32_t speed_hz)
{
    uint32_t arr = 1000000 / speed_hz;
    __HAL_TIM_SET_AUTORELOAD(&htim2, arr - 1);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, arr / 2);
}

// 角度转步数: 每步 0.9°
#define STEP_ANGLE 0.9f
void Move_Angle_Y(uint8_t dir, float angle, uint32_t freq)
{
    // 1. 方向
    Set_Dir_Y(dir);
    // 2. 目标步数
    step_target_y = (uint32_t)(angle / STEP_ANGLE + 0.5f);
    step_count_y = 0;
    // 3. 设置频率
    Set_Speed_Y(freq);
    // 4. 启动 PWM 更新中断(定时器2中断回调函数中有对应操作)
    HAL_TIM_Base_Start_IT(&htim5);
    HAL_TIM_PWM_Start_IT(&htim5, TIM_CHANNEL_1);
}
void Move_Angle_X(uint8_t dir, float angle, uint32_t freq)
{
    // 1. 方向
    Set_Dir_X(dir);
    // 2. 目标步数
    step_target_x = (uint32_t)(angle / STEP_ANGLE + 0.5f);
    step_count_x = 0;
    // 3. 设置频率
    Set_Speed_X(freq);
    // 4. 启动 PWM 更新中断
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
}


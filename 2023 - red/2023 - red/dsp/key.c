#include "main.h"
#include "key.h"
#include "gpio.h"
#include "bsp_uart.h"
#include "task.h"
#include "step.h"

volatile uint16_t key_down_flag = 0;
volatile uint32_t key_press_time = 0;
volatile uint8_t key_long_press_flag = 0;
volatile uint16_t stateA = 0;//0：运动 1：暂停


void Key_Init()
{
    MX_GPIO_Init();
}

uint8_t Key_Getnum(void)
{
    uint8_t Keynum = 0;
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
        HAL_Delay(10);
        while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET); // 等待松开
        HAL_Delay(10);
        Keynum = 1;
    }
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET)
    {
        HAL_Delay(10);
        while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET);
        HAL_Delay(10);
        Keynum = 2;
    }
    return Keynum;
}


//外部中断
uint32_t current_x1,current_y1;


volatile uint8_t in_homing = 0;  // 回原点进行中标志

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_13)
    {
        current_x1 = current_x;
        current_y1 = current_y;
                    
        // 执行回原点
        Reset_To_Origin();

        // if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == GPIO_PIN_RESET)
        // {
        //     // 按键按下
        //     key_down_flag = 1;
        //     key_press_time = 0;
        //     key_long_press_flag = 0;
        // }
        // else
        // {
        //     // 按键释放
        //     if(key_down_flag)
        //     {
        //         key_down_flag = 0;
                
        //         // 短按处理：回原点（仅当未暂停且未在回原点过程中）
        //         if((key_press_time < 1500) && !stateA && !in_homing)
        //         {
        //             in_homing = 1;  // 设置回原点标志
                    
        //             // 保存当前位置（可选）
                    
        //             in_homing = 0;  // 清除回原点标志
        //         }
        //     }
        // }
    }
}
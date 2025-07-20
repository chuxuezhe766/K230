#include "main.h"
#include "key.h"
#include "gpio.h"
#include "bsp_uart.h"
#include "task.h"
#include "step.h"
uint16_t upda_flag = 0;
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    upda_flag = 0;
    if(GPIO_Pin == GPIO_PIN_13)
    {
    }
}



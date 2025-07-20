#include "main.h"
#include "led.h"
#include "gpio.h"

void LED_ON()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,GPIO_PIN_RESET);
}

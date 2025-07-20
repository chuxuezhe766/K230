#include "main.h"
#include "tim.h"
#include "timer.h"
#include "key.h"
#include "task.h"
//pwm控制步进电机的相关变量
volatile uint32_t step_count_y = 0;
volatile uint32_t step_target_y = 0;
volatile uint32_t step_count_x = 0;
volatile uint32_t step_target_x = 0;

//10ms定时器中断
void Timer_Init()
{
    HAL_TIM_Base_Start_IT(&htim6);
}

uint16_t t = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim6)  // 10ms定时器
    {
        // if(key_down_flag)
        // {
        //     key_press_time += 10;
            
        //     // 长按处理（≥1.5秒）
        //     if((key_press_time >= 1500) && (key_long_press_flag == 0))
        //     {
        //         key_long_press_flag = 1;
        //         stateA = !stateA;  // 切换暂停状态
        //         HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);  // 指示灯
                
        //         // 暂停时立即停止所有电机
        //         if(stateA) {
        //             HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);  // X轴
        //             HAL_TIM_PWM_Stop_IT(&htim5, TIM_CHANNEL_1);  // Y轴
        //         }
        //     }
        // }
    }    
    if (htim->Instance == TIM2) 
    {
        if (step_count_x < step_target_x) 
        {
            step_count_x++;
        } 
        else 
        {
            // 达到目标步数，停止 PWM
            HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_2);
        }
    }
    if (htim->Instance == TIM5) 
    {
        // TIM5 更新事件（ARR 计数完成）
        if (step_count_y < step_target_y) 
        {
            step_count_y++;
        } 
        else 
        {
            // 达到目标步数，停止 PWM
            HAL_TIM_PWM_Stop_IT(&htim5, TIM_CHANNEL_1);
        }
    }    
}




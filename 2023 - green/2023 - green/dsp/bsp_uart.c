#include "bsp_uart.h"
#include "main.h"
#include "usart.h"
#include "stdio.h"
void uart_send(UART_HandleTypeDef *huart,const uint8_t *pData, uint16_t Size)
{
    HAL_UART_Transmit(huart, pData, Size, HAL_MAX_DELAY);
}

static uint8_t RxData = 0;     
static uint8_t RxBuffer[40];      
uint16_t flag = 0;
uint16_t current_x,current_y;//激光当前的坐标
uint32_t W[2][4],L[2][4] = {0};//电工胶布外边框坐标与内边框坐标
uint16_t f;
// 初始化 UART 接收
void K230_UART2_Init(void) 
{
    HAL_UART_Receive_IT(&huart2, &RxData, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{

    static uint16_t Rx_Cnt = 0;
    if (huart->Instance == USART2) 
    {
        RxBuffer[Rx_Cnt] = RxData;
        // 检查数据头
        if (RxBuffer[0] != 0xA3 && RxBuffer[0] != 0xB3) 
        {
            Rx_Cnt = 0;
            HAL_UART_Receive_IT(&huart2, &RxData, 1);
            return;
        }
        Rx_Cnt++;
        if (Rx_Cnt == 10) 
        {
            if(RxBuffer[0] == 0xA3)
            {
                if(RxBuffer[9] == 0xC3)
                {
                    Rx_Cnt = 0;
                    flag ++;
                    Red_X = (uint16_t)RxBuffer[1] << 8 | RxBuffer[2];
                    Red_Y = (uint16_t)RxBuffer[3] << 8 | RxBuffer[4];
                    Green_X = (uint16_t)RxBuffer[5] << 8 | RxBuffer[6];
                    Green_Y = (uint16_t)RxBuffer[7] << 8 | RxBuffer[8];
                    // switch (flag)
                    // {
                    // case 1:
                    //     Top_Left_X = (uint16_t)RxBuffer[1] << 8 | RxBuffer[2];
                    //     Top_Left_Y = (uint16_t)RxBuffer[3] << 8 | RxBuffer[4];
                    //     break;
                    // case 2:
                    //     Top_Right_X = (uint16_t)RxBuffer[1] << 8 | RxBuffer[2];
                    //     Top_Right_Y = (uint16_t)RxBuffer[3] << 8 | RxBuffer[4];
                    //     break;
                    // case 3:
                    //     Lower_Left_X = (uint16_t)RxBuffer[1] << 8 | RxBuffer[2];
                    //     Lower_Left_Y = (uint16_t)RxBuffer[3] << 8 | RxBuffer[4];
                    //     break;
                    // case 4:
                    //     Lower_Right_X = (uint16_t)RxBuffer[1] << 8 | RxBuffer[2];
                    //     Lower_Right_Y = (uint16_t)RxBuffer[3] << 8 | RxBuffer[4];
                        // break;
                    // case 5:
                    //     Midpiont_X = (uint16_t)RxBuffer[1] << 8 | RxBuffer[2];
                    //     Midpiont_Y = (uint16_t)RxBuffer[3] << 8 | RxBuffer[4];
                    //     break;
                // default:
                //     break;
                //     }
                }
            }
        }
     }
    // 重新启用接收中断
    HAL_UART_Receive_IT(&huart2, &RxData, 1);
}


//     if (huart->Instance == USART1) 
//     {
//         static uint16_t RxState = 0;
// 	    static uint16_t pRxPacket = 0;
//         if(RxState == 0)
//         {
//             if(RxData == 0xA3)
//             {
//                 flag = 1;
//                 RxState = 1;
//                 pRxPacket = 0;
//                 Rx_Cnt ++;
//             }
//         }
//         if(RxState == 1)
//         {
//             RxBuffer[pRxPacket] = RxData;
//             pRxPacket++;
//                 Top_Left_X = RxBuffer[0] << 8 | RxBuffer[1];
//                 Top_Left_Y = RxBuffer[2] << 8 | RxBuffer[3];
   
//             if(RxBuffer[pRxPacket] == 0xC3)
//             {
//                 RxState = 2;
//             }
//         }
//         if(RxState == 2)
//         {
//             RxState = 0;
//         }
//         HAL_UART_Receive_IT(&huart1, &RxData, 1);
//     }
// }


 
int fputc(int ch,FILE *f)
{
//采用轮询方式发送1字节数据，超时时间设置为无限等待
HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,1000);
return ch;
}
int fgetc(FILE *f)
{
uint8_t ch;
// 采用轮询方式接收 1字节数据，超时时间设置为无限等待
HAL_UART_Receive( &huart1,(uint8_t*)&ch,1, 1000 );
return ch;
}

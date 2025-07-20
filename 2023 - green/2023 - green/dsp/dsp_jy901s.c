#include "main.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;

// 全局变量
static uint8_t ucData = 0;         // 单字节接收缓存
static uint8_t ucRxBuffer[11];      // 修正为9字节缓存
static uint8_t ucRxCnt = 0;        // 接收计数器
float Roll = 0, Pitch = 0, Yaw = 0; // 欧拉角

// 初始化 UART 接收
void JY901S_UART2_Init(void) 
{
    ucRxCnt = 0;
    HAL_UART_Receive_IT(&huart2, &ucData, 1);
}

// 校验和验证（前10字节求和，等于第11字节）
static uint8_t JY901S_CheckSum(uint8_t *data) {
    uint8_t sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += data[i];
    }
    return (sum == data[10]);
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//     if (huart->Instance == USART2) {
//         ucRxBuffer[ucRxCnt++] = ucData;

//         // 检查数据头
//         if (ucRxCnt == 1 && ucRxBuffer[0] != 0x55) {
//             ucRxCnt = 0;
//             HAL_UART_Receive_IT(&huart2, &ucData, 1);
//             return;
//         }

//         // 满11字节，处理数据包
//         if (ucRxCnt == 11) {
//             if (ucRxBuffer[1] == 0x53 && JY901S_CheckSum(ucRxBuffer)) {
//                 int16_t rawRoll  = (int16_t)((ucRxBuffer[3] << 8) | ucRxBuffer[2]);
//                 int16_t rawPitch = (int16_t)((ucRxBuffer[5] << 8) | ucRxBuffer[4]);
//                 int16_t rawYaw   = (int16_t)((ucRxBuffer[7] << 8) | ucRxBuffer[6]);
//                 Roll  = (float)rawRoll  / 32768.0f * 180.0f;
//                 Pitch = (float)rawPitch / 32768.0f * 180.0f;
//                 Yaw   = (float)rawYaw   / 32768.0f * 180.0f;
//             }
//             ucRxCnt = 0; // 重置计数器
//         }

//         // 重新启用接收中断
//         HAL_UART_Receive_IT(&huart2, &ucData, 1);
//     }
// }
// 获取欧拉角接口
float JY901S_GetRoll(void)  { return Roll; }
float JY901S_GetPitch(void) { return Pitch; }
float JY901S_GetYaw(void)   { return Yaw; }



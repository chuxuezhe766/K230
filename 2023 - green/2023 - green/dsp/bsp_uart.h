#ifndef __BSP_UART_H__
#define __BSP_UART_H__
#include "main.h"
extern uint16_t current_x,current_y;
extern uint32_t W[2][4],L[2][4];
extern uint16_t f;


void uart_send(UART_HandleTypeDef *huart,const uint8_t *pData, uint16_t Size);
void K230_UART2_Init(void) ;
#endif

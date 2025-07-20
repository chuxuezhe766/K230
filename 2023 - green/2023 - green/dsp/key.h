#ifndef __KEY_H__
#define __KEY_H__
#include <stdint.h>

void Key_Init();
uint8_t Key_Getnum(void);
extern uint16_t upda_flag;
extern uint32_t current_x1,current_y1;

#endif

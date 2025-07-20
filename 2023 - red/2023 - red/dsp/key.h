#ifndef __KEY_H__
#define __KEY_H__
#include <stdint.h>

extern volatile uint16_t key_down_flag;
extern volatile uint32_t key_press_time;
extern volatile uint8_t key_long_press_flag;
extern volatile uint16_t stateA;
extern volatile uint8_t in_homing;

void Key_Init();
uint8_t Key_Getnum(void);
extern uint32_t current_x1,current_y1;

#endif

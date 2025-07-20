#ifndef __TIMER_H__
#define __TIMER_H__
#include <stdint.h>
extern volatile uint32_t step_count_y;
extern volatile uint32_t step_target_y;
extern volatile uint32_t step_count_x;
extern volatile uint32_t step_target_x;


void Timer_Init();

extern uint16_t t;


#endif

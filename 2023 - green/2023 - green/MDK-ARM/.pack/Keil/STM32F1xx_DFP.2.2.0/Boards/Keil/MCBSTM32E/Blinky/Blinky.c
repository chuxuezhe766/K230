/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher for MCBSTM32E
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "stm32f10x.h"                  // Device header
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "RTE_Components.h"             // Component selection

extern int Init_Thread_LED(void);
extern int Init_Thread_ADC(void);
extern osThreadId tid_Thread_LED;

volatile uint32_t delay_val = 500;


/**
  * Main function
  */
int main (void) {

  osKernelInitialize();                     /* initialize CMSIS-RTOS          */

  LED_Initialize();                         /* Initialize LED                 */
  Buttons_Initialize();                     /* Initialize Buttons             */
  ADC_Initialize();                         /* Initialize ADC                 */

  Init_Thread_LED();                        /* Create LED thread              */
  Init_Thread_ADC();                        /* create ADC thread              */

  osKernelStart();                          /* start thread execution         */

  while (1) {
    while (Buttons_GetState() & 4U);        /* Wait while holding USER button */
    osSignalSet(tid_Thread_LED, 1U);        /* Signal LED thread              */
    osDelay(delay_val);                     /* Wait                           */
  }
}

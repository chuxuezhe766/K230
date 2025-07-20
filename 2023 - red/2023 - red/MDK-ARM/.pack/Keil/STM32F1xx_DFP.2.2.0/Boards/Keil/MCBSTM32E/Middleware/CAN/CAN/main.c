/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::CAN
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: CAN Example main module
 *----------------------------------------------------------------------------*/

#include "main.h"

osThreadDef(UI_Thread,  osPriorityNormal, 1U, NULL);
osThreadDef(CAN_Thread, osPriorityNormal, 1U, NULL);

osThreadId ui_thread_id;
osThreadId can_thread_id;

static void Error_Handler (void) {
  while (1);
}


int main (void) {

  if (UI_Initialize  () == false) { Error_Handler (); }
  if (CAN_Initialize () == false) { Error_Handler (); }

  ui_thread_id  = osThreadCreate (osThread(UI_Thread),  NULL);
  can_thread_id = osThreadCreate (osThread(CAN_Thread), NULL);

  while (1) {
    osSignalWait (0, osWaitForever);
  }  
}

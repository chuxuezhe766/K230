/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::CAN
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    main.h
 * Purpose: CAN Example main header
 *----------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"

extern uint8_t rx_data[8];
extern uint8_t tx_data[8];

extern bool CAN_Initialize (void);
extern void CAN_Thread     (void const *arg);

extern bool UI_Initialize  (void);
extern void UI_Thread      (void const *arg);

extern osThreadId ui_thread_id;
extern osThreadId can_thread_id;

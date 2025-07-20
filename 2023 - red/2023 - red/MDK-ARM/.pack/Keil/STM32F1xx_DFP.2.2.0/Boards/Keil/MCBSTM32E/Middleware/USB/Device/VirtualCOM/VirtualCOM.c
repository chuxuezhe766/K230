/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    VirtualCOM.c
 * Purpose: USB Device Virtual COM Port demonstration
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_usb.h"

#include "Board_GLCD.h"
#include "GLCD_Config.h"

extern    GLCD_FONT GLCD_Font_6x8;
extern    GLCD_FONT GLCD_Font_16x24;


int main (void) {

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0U, 0U*24U, "    USB Device      ");
  GLCD_DrawString         (0U, 1U*24U, "   CDC ACM Class    ");
  GLCD_DrawString         (0U, 2U*24U, " VirtualCOM Example ");
  GLCD_DrawString         (0U, 4U*24U, " USB <-> UART2(COM0)");
  GLCD_DrawString         (0U, 8U*24U, "  Keil Tools by ARM ");
  GLCD_DrawString         (0U, 9U*24U, "    www.keil.com    ");

  USBD_Initialize         (0U);         // USB Device 0 Initialization
  USBD_Connect            (0U);         // USB Device 0 Connect

  while (1) {
    osSignalWait (0U, osWaitForever);
  }  
}

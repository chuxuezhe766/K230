/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MassStorage.c
 * Purpose: USB Device - Mass Storage example
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_fs.h"                      /* FileSystem definitions             */
#include "rl_usb.h"                     /* RL-USB function prototypes         */

#include "Board_GLCD.h"
#include "GLCD_Config.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

int main (void) {

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, "    USB Device      ");
  GLCD_DrawString         (0, 1*24, "    MSC Class       ");
  GLCD_DrawString         (0, 2*24, "Mass Storage Example");
  GLCD_DrawString         (0, 4*24, "USBFS: Memory Disk  ");
  GLCD_DrawString         (0, 8*24, "  Keil Tools by ARM ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");

  finit          ("M0:");               /* Initialize SD Card 0               */
  fmount         ("M0:");               /* Mount SD Card 0                    */

  USBD_Initialize(0);                   /* USB Device 0 Initialization        */
  USBD_Connect   (0);                   /* USB Device 0 Connect               */

  while (1) {
    osDelay(1000);
  }
}

/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HID.c
 * Purpose: USB Device Human Interface Device example program
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_usb.h"

#include "Board_LED.h"
#include "Board_Joystick.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

int main (void) {
  uint8_t but;
  uint8_t buf[1];

  LED_Initialize     ();
  Joystick_Initialize();

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, "    USB Device      ");
  GLCD_DrawString         (0, 1*24, "    HID Class       ");
  GLCD_DrawString         (0, 2*24, "    HID Example     ");
  GLCD_DrawString         (0, 4*24, "USBFS: HID0         ");
  GLCD_DrawString         (0, 8*24, "  Keil Tools by ARM ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");

  USBD_Initialize    (0);               /* USB Device 0 Initialization        */
  USBD_Connect       (0);               /* USB Device 0 Connect               */

  while (1) {                           /* Loop forever                       */
    but = (uint8_t)(Joystick_GetState ());
    if (but ^ buf[0]) {
      buf[0] = but;
      USBD_HID_GetReportTrigger(0, 0, &buf[0], 1);
    }
    osDelay(100);                       /* 100 ms delay for sampling buttons  */
  }
}

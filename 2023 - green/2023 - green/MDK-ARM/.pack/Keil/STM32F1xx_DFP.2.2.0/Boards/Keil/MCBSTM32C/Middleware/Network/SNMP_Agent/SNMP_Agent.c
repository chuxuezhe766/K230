/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SNMP_Agent.c
 * Purpose: SNMP Agent example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "cmsis_os.h"                   /* CMSIS RTOS definitions            */
#include "rl_net.h"                     /* Network definitions               */

#include "Board_LED.h"
#include "Board_Buttons.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"

extern GLCD_FONT GLCD_Font_16x24;

bool LCDupdate;
char lcd_text[2][20+1];

static void Display (void const *arg);

osThreadDef(Display, osPriorityNormal, 1, 0);

/// Read digital inputs
uint8_t get_button (void) {
  return (Buttons_GetState ());
}

/// IP address change notification
void dhcp_client_notify (uint32_t if_num,
                         dhcpClientOption opt, const uint8_t *val, uint32_t len) {
  if (opt == dhcpClientIPaddress) {
    // IP address has changed
    sprintf (lcd_text[0],"IP address:");
    sprintf (lcd_text[1],"%s", ip4_ntoa (val));
    LCDupdate = true;
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static void Display (void const *arg) {
  char lcd_buf[20+1];

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 1*24, "       MDK-MW       ");
  GLCD_DrawString         (0, 2*24, " SNMP Agent example ");

  sprintf (lcd_text[0], "");
  sprintf (lcd_text[1], "Waiting for DHCP");
  LCDupdate = true;

  while(1) {
    if (LCDupdate == true) {
      sprintf (lcd_buf, "%-20s", lcd_text[0]);
      GLCD_DrawString (0, 5*24, lcd_buf);
      sprintf (lcd_buf, "%-20s", lcd_text[1]);
      GLCD_DrawString (0, 6*24, lcd_buf);
      LCDupdate = false;
    }
    osDelay (250);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {
  LED_Initialize ();
  Buttons_Initialize ();
  net_initialize ();

  osThreadCreate (osThread(Display), NULL);

  while (1) {
    net_main ();
    osThreadYield ();
  }
}

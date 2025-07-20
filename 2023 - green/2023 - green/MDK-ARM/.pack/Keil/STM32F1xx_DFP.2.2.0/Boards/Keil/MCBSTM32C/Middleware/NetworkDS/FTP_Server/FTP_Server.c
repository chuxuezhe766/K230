/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FTP_Server.c
 * Purpose: FTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "rl_fs.h"                      // Keil.MDK-Pro::File System:CORE

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCBSTM32C::Board Support:Graphic LCD

extern GLCD_FONT GLCD_Font_16x24;

// Thread IDs
osThreadId TID_Display;
osThreadId TID_Led;

// Thread definitions
static void BlinkLed (void const *arg);
static void Display  (void const *arg);

osThreadDef(BlinkLed, osPriorityNormal, 1, 0);
osThreadDef(Display,  osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static void Display (void const *arg) {
  static uint8_t ip_addr[NET_ADDR_IP6_LEN];
  static char    ip_ascii[40];
  static char    buf[24];

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 1*24, "       MDK-MW       ");
  GLCD_DrawString         (0, 2*24, " FTP Server example ");

  GLCD_DrawString (0, 4*24, "IP4:Waiting for DHCP");

  // Print Link-local IPv6 address
  netIF_GetOption (NET_IF_CLASS_ETH,
                   netIF_OptionIP6_LinkLocalAddress, ip_addr, sizeof(ip_addr));

  netIP_ntoa(NET_ADDR_IP6, ip_addr, ip_ascii, sizeof(ip_ascii));

  sprintf (buf, "IP6:%.16s", ip_ascii);
  GLCD_DrawString (0, 5*24, buf);
  sprintf (buf, "%s", ip_ascii+16);
  GLCD_DrawString (10*16, 6*24, buf);

  while(1) {
    /* Wait for signal from DHCP */
    osSignalWait (0x01, osWaitForever);
    /* Retrieve and print IPv4 address */
    netIF_GetOption (NET_IF_CLASS_ETH,
                     netIF_OptionIP4_Address, ip_addr, sizeof(ip_addr));

    netIP_ntoa (NET_ADDR_IP4, ip_addr, ip_ascii, sizeof(ip_ascii));

    sprintf (buf, "IP4:%-16s",ip_ascii);
    GLCD_DrawString (0, 4*24, buf);
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static void BlinkLed (void const *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  int cnt = 0;

  while(1) {
    // Every 100 ms
    LED_SetOut (led_val[cnt]);
    if (++cnt >= sizeof(led_val)) {
      cnt = 0;
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {
  LED_Initialize ();

  if (finit("") == fsOK) {
    fmount("");
  }

  netInitialize ();

  TID_Led     = osThreadCreate (osThread(BlinkLed), NULL);
  TID_Display = osThreadCreate (osThread(Display),  NULL);

  while(1) {
    osSignalWait (0, osWaitForever);
  }
}

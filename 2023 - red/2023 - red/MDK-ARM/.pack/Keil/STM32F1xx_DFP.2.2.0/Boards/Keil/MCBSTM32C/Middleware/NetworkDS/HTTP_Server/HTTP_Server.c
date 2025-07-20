/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCBSTM32C::Board Support:Graphic LCD

extern GLCD_FONT GLCD_Font_16x24;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

// Thread IDs
osThreadId TID_Display;
osThreadId TID_Led;

// Thread definitions
static void BlinkLed (void const *arg);
static void Display  (void const *arg);

osThreadDef(BlinkLed, osPriorityNormal, 1, 0);
osThreadDef(Display,  osPriorityNormal, 1, 0);

/// Read analog inputs
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }
  return (val);
}

/// Read digital inputs
uint8_t get_button (void) {
  return (Buttons_GetState ());
}

/// IP address change notification
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osSignalSet (TID_Display, 0x01);
  }
}

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
  GLCD_DrawString         (0, 2*24, "HTTP Server example ");

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

    /* Display user text lines */
    sprintf (buf, "%-20s", lcd_text[0]);
    GLCD_DrawString (0, 7*24, buf);
    sprintf (buf, "%-20s", lcd_text[1]);
    GLCD_DrawString (0, 8*24, buf);
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static void BlinkLed (void const *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  int cnt = 0;

  LEDrun = true;
  while(1) {
    // Every 100 ms
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {
  LED_Initialize     ();
  Buttons_Initialize ();
  ADC_Initialize     ();

  netInitialize      ();

  TID_Led     = osThreadCreate (osThread(BlinkLed), NULL);
  TID_Display = osThreadCreate (osThread(Display),  NULL);

  while(1) {
    osSignalWait (0, osWaitForever);
  }
}

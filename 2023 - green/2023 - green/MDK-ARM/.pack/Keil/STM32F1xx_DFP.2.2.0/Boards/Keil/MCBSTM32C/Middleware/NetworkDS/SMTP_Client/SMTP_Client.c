/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client.c
 * Purpose: SMTP Client example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "Board_ADC.h"                  // ::Board Support:A/D Converter
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCBSTM32C::Board Support:Graphic LCD

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//   <s.40>Remote IP Address
//   =======================
//   <i> IPv4 or IPv6 Address in text representation
//   <i>Default: "192.168.0.100" or "fe80::1c30:6cff:fea2:455e"
#define IP_ADDR        "fe80::1c30:6cff:fea2:455e"

//------------- <<< end of configuration section >>> -----------------------


extern GLCD_FONT GLCD_Font_16x24;

netSMTPc_Event smtp_event;
osThreadId     send_id;

static void BlinkLed (void const *arg);
static void SendMail (void const *arg);

osThreadDef(BlinkLed, osPriorityNormal, 1, 0);
osThreadDef(SendMail, osPriorityNormal, 1, 0);

/// Read analog inputs
uint16_t AD_in (uint32_t ch) {
         int32_t val;
  static int32_t last_val = 0;

  ADC_StartConversion();
  val = ADC_GetValue();
  if (val != -1) {
    last_val = val;
  }

  return (last_val);
}

/// Inform application about SMTP status
void smtp_inform (netSMTPc_Event event) {
  smtp_event = event;
  // Notify send thread of SMTP status
  osSignalSet (send_id, 0x0001);
}

/*----------------------------------------------------------------------------
  Thread 'SendMail': Send an email
 *---------------------------------------------------------------------------*/
static void SendMail (void const *arg) {
  NET_ADDR smtp_srv;
  netStatus retv;

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 1*24, "       MDK-MW       ");
  GLCD_DrawString         (0, 2*24, "SMTP Client example ");
  send_id = osThreadGetId ();

  /* Setup server parameters */
  if (netIP_aton (IP_ADDR, NET_ADDR_IP4, smtp_srv.addr)) {
    /* IP_ADDR specifies IPv4 address */
    smtp_srv.addr_type = NET_ADDR_IP4;
  }
  else if (netIP_aton (IP_ADDR, NET_ADDR_IP6, smtp_srv.addr)) {
    /* IP_ADDR specified IPv6 address */
    smtp_srv.addr_type = NET_ADDR_IP6;
  }
  smtp_srv.port = 0;

  while(1) {
    // After 5 seconds start sending email
    osDelay (5000);
    GLCD_DrawString (0, 5*24, "Sending email...");

    // Start SMTP client, use default SMTP port
    retv = netSMTPc_Connect (&smtp_srv);
    if (retv != netOK) {
      // Failed to start SMTP process
      GLCD_DrawString (0, 5*24, "Connect failed     ");
      continue;
    }
    osSignalWait (0x0001, osWaitForever);
    // Check the completion code
    switch (smtp_event) {
      case netSMTPc_EventSuccess:
        // Success, terminate thread
        GLCD_DrawString (0, 6*24, "Success, email sent");
        osThreadTerminate (send_id);
        break;
      case netSMTPc_EventTimeout:
        // Timeout, try again
        GLCD_DrawString (0, 5*24, "Server timeout      ");
      break;
      case netSMTPc_EventError:
        // Error, try again
        GLCD_DrawString (0, 5*24, "Operation failed    ");
        break;
    }
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
  ADC_Initialize ();
  
  netInitialize ();

  osThreadCreate (osThread(BlinkLed), NULL);
  osThreadCreate (osThread(SendMail), NULL);

  while(1) {
    osSignalWait (0, osWaitForever);
  }
}

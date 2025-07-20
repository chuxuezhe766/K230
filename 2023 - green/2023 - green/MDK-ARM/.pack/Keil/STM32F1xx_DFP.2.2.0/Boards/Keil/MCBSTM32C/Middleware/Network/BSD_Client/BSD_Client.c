/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    BSD_Client.c
 * Purpose: LED Client example using BSD sockets
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_net.h"                     /* Network definitions                */

#include "Board_LED.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//   <h>Remote IP Address
//   ====================
//
//     <o>IP1: Address byte 1 <0-255>
//     <i> Default: 192
#define IP1            192

//     <o>IP2: Address byte 2 <0-255>
//     <i> Default: 168
#define IP2            168

//     <o>IP3: Address byte 3 <0-255>
//     <i> Default: 0
#define IP3            0

//     <o>IP4: Address byte 4 <0-255>
//     <i> Default: 100
#define IP4            100

//   </h>

//   <o>Remote Port <1-65535>
//   <i> Do not set number of port too small,
//   <i> maybe it is already used.
//   <i> Default: 1001
#define PORT_NUM       1001

//   <o>Communication Protocol <0=> Stream (TCP) <1=> Datagram (UDP)
//   <i> Selecet a protocol for sending data.
#define PROTOCOL       1

//   <o>LED Blinking speed <1-100>
//   <i> Blinking speed = SPEED * 100ms
//   <i> Default: 2
#define SPEED          2

//------------- <<< end of configuration section >>> -----------------------

#define BLINKLED 0x01                   /* Command for blink the leds        */
#if (PROTOCOL == 0)
 #define SOCKTYPE   SOCK_STREAM
#else
 #define SOCKTYPE   SOCK_DGRAM
#endif

extern GLCD_FONT GLCD_Font_16x24;

static void Client (void const *arg);
osThreadDef(Client, osPriorityNormal, 2, 0);

/// Initialize display
static void init_display () {
  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 3*24, "       MDK-MW       ");
  GLCD_DrawString         (0, 4*24, " BSD Client example ");
}

/*----------------------------------------------------------------------------
  Thread 'Client': BSD Client socket process
 *---------------------------------------------------------------------------*/
static void Client (void const *arg) {
  SOCKADDR_IN addr;
  int sock, res;
  char dbuf[4];
  uint8_t p2val,lshf;

  while (1) {
    sock = socket (AF_INET, SOCKTYPE, 0);

    addr.sin_port      = htons(PORT_NUM);
    addr.sin_family    = PF_INET;
    addr.sin_addr.s_b1 = IP1;
    addr.sin_addr.s_b2 = IP2;
    addr.sin_addr.s_b3 = IP3;
    addr.sin_addr.s_b4 = IP4;

    connect (sock, (SOCKADDR *)&addr, sizeof (addr));

    lshf  = 1;
    p2val = 0x01;
    while (1) {
      // Shift the LEDs
      LED_SetOut (p2val);
      p2val = lshf ? (p2val << 1) : (p2val >> 1);
      if (p2val == 0x80) lshf = 0;
      if (p2val == 0x01) lshf = 1;
    
      // Send the data to LED Server.
      dbuf[0] = BLINKLED;
      dbuf[1] = p2val;
      res = send (sock, (char *)&dbuf, 2, 0);
      if (res < 0) {
        break;
      }
      osDelay (100 * SPEED);
    }
    closesocket (sock);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {

  LED_Initialize ();
  init_display ();
  net_initialize ();

  // Create networking task
  osThreadCreate (osThread(Client), NULL);
  while(1) {
    net_main ();
    osThreadYield ();
  }
}

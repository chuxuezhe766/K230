/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    BSD_Server.c
 * Purpose: LED Server example using BSD sockets
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_net.h"                     /* Network definitions                */

#include "Board_LED.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"

#define PORT_NUM 1001
#define BLINKLED 0x01                   /* Command for blink the leds         */

extern GLCD_FONT GLCD_Font_16x24;

static void Server (void const *arg);
osThreadDef(Server, osPriorityNormal, 2, 0);

/// Initialize display
static void init_display (void) {
  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 3*24, "       MDK-MW       ");
  GLCD_DrawString         (0, 4*24, " BSD Server example ");
}

/*----------------------------------------------------------------------------
  Thread 'Server': BSD Server socket process
 *---------------------------------------------------------------------------*/
static void Server (void const *arg) {
  SOCKADDR_IN addr;
  int sock, sd, res;
  int type = (int)arg;
  char dbuf[4];

  while (1) {
    sock = socket (AF_INET, type, 0);

    addr.sin_port        = htons(PORT_NUM);
    addr.sin_family      = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    bind (sock, (SOCKADDR *)&addr, sizeof(addr));

    if (type == SOCK_STREAM) {
      listen (sock, 1);
      sd = accept (sock, NULL, NULL);
      closesocket (sock);
      sock = sd;
    }

    while (1) {
      res = recv (sock, dbuf, sizeof (dbuf), 0);
      if (res <= 0) {
        break;
      }
      if (dbuf[0] == BLINKLED) {
        LED_SetOut (dbuf[1]);
      }
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

  osThreadCreate (osThread(Server), (void *)SOCK_STREAM);
  osThreadCreate (osThread(Server), (void *)SOCK_DGRAM);
  while(1) {
    net_main ();
    osThreadYield ();
  }
}

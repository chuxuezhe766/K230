/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::CAN
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    UI.c
 * Purpose: CAN Example User Interface module
 *----------------------------------------------------------------------------*/

#include "main.h"

// User Interface configuration ------------------------------------------------

#define USE_GLCD        1       // 0 = no graphic LCD, 1 = use graphic LCD

//------------------------------------------------------------------------------

#if    (USE_GLCD == 1)
#include "Board_GLCD.h"
#include "GLCD_Config.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
#endif


//------------------------------------------------------------------------------
//  User Interface Initialization
//------------------------------------------------------------------------------
bool UI_Initialize (void) {

#if (USE_GLCD == 1)
  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, "    CAN Example     ");
  GLCD_DrawString         (0, 2*24, "Tx data:            ");
  GLCD_DrawString         (0, 3*24, "                    ");
  GLCD_DrawString         (0, 5*24, "Rx data:            ");
  GLCD_DrawString         (0, 6*24, "                    ");
  GLCD_DrawString         (0, 8*24, "  Keil Tools by ARM ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");
#endif
  printf ("CAN Example\n");

  return true;  
}


//------------------------------------------------------------------------------
//  User Interface Thread Function
//------------------------------------------------------------------------------
void UI_Thread (void const *arg) {
  osEvent event;
#if (USE_GLCD == 1)
  char    disp_str[13];
#endif

  while (1) {
    event = osSignalWait(0U, osWaitForever);
    if (event.status == osEventSignal) {
      if (event.value.signals & 1U) {           // If receive event happened
        printf ("Rx data = %02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X\n", 
                rx_data[0], rx_data[1], rx_data[2], rx_data[3], 
                rx_data[4], rx_data[5], rx_data[6], rx_data[7]);
#if (USE_GLCD == 1)
        sprintf(disp_str, "%02X,%02X,%02X,%02X", rx_data[0], rx_data[1], rx_data[2], rx_data[3]);
        GLCD_DrawString(8*16, 5*24, disp_str);
        sprintf(disp_str, "%02X %02X %02X %02X", rx_data[4], rx_data[5], rx_data[6], rx_data[7]);
        GLCD_DrawString(8*16, 6*24, disp_str);
#endif
      }
      if (event.value.signals & (1U << 1)) {    // If transmit event happened
        printf ("Tx data = %02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X\n", 
                tx_data[0], tx_data[1], tx_data[2], tx_data[3], 
                tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
#if (USE_GLCD == 1)
        sprintf(disp_str, "%02X,%02X,%02X,%02X", tx_data[0], tx_data[1], tx_data[2], tx_data[3]);
        GLCD_DrawString(8*16, 2*24, disp_str);
        sprintf(disp_str, "%02X %02X %02X %02X", tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
        GLCD_DrawString(8*16, 3*24, disp_str);
#endif
      }
    }
  }
}

/*----------------------------------------------------------------------------
 * Name:    Demo.c
 * Purpose: Demo example for MCBSTM32C
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "Driver_I2C.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "Board_LED.h"
#include "Board_ADC.h"
#include "Board_Buttons.h"
#include "Board_Joystick.h"
#include "Board_Touch.h"

/* Bargraph position definitions */
#define BAR_ST      (8*6+3)
#define BAR_LEN     (217)
#define BAR_HGT     (20)
#define BAR_LN(ln)  ((ln)*24)

/* Joystick position definitions */
#define JOY_X       (13*16)
#define JOY_Y       ( 5*24 + 6)

/* Button position definitions */
#define BTN_X       (9)
#define BTN_Y       (190)
#define BTN_X_EVAL  (20)
#define BTN_Y_EVAL  (182)

/* Bulb position definition */
#define BULB_X       (209)
#define BULB_Y       (190)
#define BULB_X_EVAL  (9 * 16)
#define BULB_Y_EVAL  (8 * 24)

/* Keys bit masks */
#define KEY_WAKEUP  1
#define KEY_TAMPER  2
#define KEY_USER    4

/* I2C1 Driver */
extern ARM_DRIVER_I2C Driver_I2C1;

/* Mode switch timeout variable */
uint32_t KeyTick;

/* Extern variables */
extern unsigned char Arrows_16bpp_red[];
extern unsigned char Button_16bpp[];
extern unsigned char Bulb_16bpp[];
extern GLCD_FONT     GLCD_Font_16x24;

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  TOUCH_STATE tsc_state;
  uint32_t ofs;
  uint32_t btnMsk, joyMsk;
  int32_t adcVal;
  int32_t btn  = -1;
  int32_t joy  = -1;
  int32_t adc  = -1;

  LED_Initialize();                     /* LED Init                           */
  ADC_Initialize();                     /* A/D Converter Init                 */
  Buttons_Initialize();                 /* Keyboard Init                      */
  Joystick_Initialize();                /* Joystick Init                      */
  Touch_Initialize();                   /* Touchscreen Controller Init        */
  GLCD_Initialize();                    /* Graphical Display Init             */

  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen ();
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetFont            (&GLCD_Font_16x24);

  GLCD_DrawString (0, 0*24, "     MCBSTM32C      ");
  GLCD_DrawString (0, 1*24, "    Demo Example    ");
  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor (GLCD_COLOR_BLUE);

  while (1) {
    /* Show potentiometer, keys, joystick */
    btnMsk = Buttons_GetState();    /* Show buttons state                 */
    if (btn ^ btnMsk) {
      btn = btnMsk;

      GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
      if (btn & KEY_USER  )    { GLCD_DrawString (1*16, 4*24, "USER");   }
      if (btn & KEY_TAMPER)    { GLCD_DrawString (1*16, 5*24, "TAMPER"); }
      if (btn & KEY_WAKEUP  )  { GLCD_DrawString (1*16, 6*24, "WAKEUP"); }
    
      GLCD_SetForegroundColor (GLCD_COLOR_LIGHT_GREY);
      if (!(btn & KEY_USER  )) { GLCD_DrawString (1*16, 4*24, "USER");   }
      if (!(btn & KEY_TAMPER)) { GLCD_DrawString (1*16, 5*24, "TAMPER"); }
      if (!(btn & KEY_WAKEUP)) { GLCD_DrawString (1*16, 6*24, "WAKEUP"); }
    }

    joyMsk = Joystick_GetState();   /* Show joystick arrows               */
    if (joy ^ joyMsk) {
      joy = joyMsk;

      /* Left arrow white/red? */
      ofs = (joy & JOYSTICK_LEFT)   ? (2416) : (0);
      GLCD_DrawBitmap(JOY_X+ 0, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 0*532]);

      /* Right arrow white/red? */
      ofs = (joy & JOYSTICK_RIGHT)     ? (2416) : (0);
      GLCD_DrawBitmap(JOY_X+35, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 1*532]);

      /* Center dot white/red */
      ofs = (joy & JOYSTICK_CENTER) ? (2416) : (0);
      GLCD_DrawBitmap(JOY_X+21, JOY_Y+21, 12, 12, &Arrows_16bpp_red[ofs + 4*532]);

      /* Up arrow white/red? */
      ofs = (joy & JOYSTICK_UP)   ? (2416) : (0);
      GLCD_DrawBitmap(JOY_X+20, JOY_Y+ 0, 14, 19, &Arrows_16bpp_red[ofs + 2*532]);

      /* Down arrow white/red? */
      ofs = (joy & JOYSTICK_DOWN)  ? (2416) : (0);
      GLCD_DrawBitmap(JOY_X+20, JOY_Y+35, 14, 19, &Arrows_16bpp_red[ofs + 3*532]);
    }

    ADC_StartConversion();
    while (ADC_ConversionDone() != 0);
    adcVal = ADC_GetValue();
    if (adc ^ adcVal) {
      adc = adcVal;

      GLCD_SetForegroundColor (GLCD_COLOR_GREEN);
      GLCD_DrawBargraph (144, 3*24, 160, 20, (adcVal * 100) / ((1 << ADC_GetResolution()) - 1));
    }

    Touch_GetState (&tsc_state);    /* Get touch screen state             */

    if (!tsc_state.pressed) {
      tsc_state.x = 0;
      tsc_state.y = 0;
    }

    if (tsc_state.x >= 200 && tsc_state.x <= 1000 &&
        tsc_state.y >= 260 && tsc_state.y <= 1650) {
      GLCD_DrawBitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[1*11520]);
      GLCD_DrawBitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[1*2080]);
    }
    else {
      GLCD_DrawBitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[0*11520]);
      GLCD_DrawBitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[0*2080]);
    }
 }
}

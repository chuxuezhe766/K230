/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.36 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration

              Display:        Ampire 240320LG
              Controller:     Himax HX8347-D

              Display driver: GUIDRV_FlexColor
              Operation mode: 16bpp, 16-bit interface or SPI

---------------------------END-OF-HEADER------------------------------
*/

#ifdef _RTE_
#include "RTE_Components.h"
#endif
#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "LCD_X.h"

/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 240
#define YSIZE_PHYS 320

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

//
// Buffers / VScreens
//
#define NUM_BUFFERS   1
#define NUM_VSCREENS  1

//
// Display orientation
//
  #define DISPLAY_ORIENTATION  0
//#define DISPLAY_ORIENTATION               (GUI_MIRROR_X | GUI_MIRROR_Y)
//#define DISPLAY_ORIENTATION (GUI_SWAP_XY | GUI_MIRROR_Y)
//#define DISPLAY_ORIENTATION (GUI_SWAP_XY | GUI_MIRROR_X)

//
// Touch screen
//
#ifdef RTE_Graphics_Touchscreen
#define USE_TOUCH   1
#else
#define USE_TOUCH   0
#endif
//
// Touch screen calibration
#define TOUCH_X_MIN 0x0EF0
#define TOUCH_X_MAX 0x00C0
#define TOUCH_Y_MIN 0x0F40
#define TOUCH_Y_MAX 0x00A0

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif
#ifndef   DISPLAY_ORIENTATION
  #define DISPLAY_ORIENTATION  0
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
#define WIDTH       YSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      XSIZE_PHYS  /* Screen Height (in pixels)        */
#else
#define WIDTH       XSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      YSIZE_PHYS  /* Screen Height (in pixels)        */
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_TOP    TOUCH_X_MAX
    #define TOUCH_BOTTOM TOUCH_X_MIN
  #else
    #define TOUCH_TOP    TOUCH_X_MIN
    #define TOUCH_BOTTOM TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_LEFT   TOUCH_Y_MAX
    #define TOUCH_RIGHT  TOUCH_Y_MIN
  #else
    #define TOUCH_LEFT   TOUCH_Y_MIN
    #define TOUCH_RIGHT  TOUCH_Y_MAX
  #endif
#else
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_LEFT   TOUCH_X_MAX
    #define TOUCH_RIGHT  TOUCH_X_MIN
  #else
    #define TOUCH_LEFT   TOUCH_X_MIN
    #define TOUCH_RIGHT  TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_TOP    TOUCH_Y_MAX
    #define TOUCH_BOTTOM TOUCH_Y_MIN
  #else
    #define TOUCH_TOP    TOUCH_Y_MIN
    #define TOUCH_BOTTOM TOUCH_Y_MAX
  #endif
#endif

/*********************************************************************
*
*       Driver Port functions
*
**********************************************************************
*/
#ifdef RTE_Graphics_LCD_MCBQVGA_LG_SPI

/*********************************************************************
*
*       LCD_X_Write0_16
*
* Purpose:
*   Write to controller, with A0 = 0
*/
void LCD_X_Write0_16(U16 c) {
  U8 data[2];

  LCD_X_ClrCS();
  data[0] = 0x70;               // Start + WR Register
  data[1] = (U8)(c);
  LCD_X_WriteM(data, 2);
  LCD_X_SetCS();
}

/*********************************************************************
*
*       LCD_X_Write1_16
*
* Purpose:
*   Write to controller, with A0 = 1
*/
void LCD_X_Write1_16(U16 c) {
  U8 data[3];

  LCD_X_ClrCS();
  data[0] = 0x72;               // Start + WR Data
  data[1] = (U8)(c >> 8);
  data[2] = (U8)(c);
  LCD_X_WriteM(data, 3);
  LCD_X_SetCS();
}

/*********************************************************************
*
*       LCD_X_Read1_16
*
* Purpose:
*   Read from controller, with A0 = 1
*/
U16 LCD_X_Read1_16(void) {
  U8 data[3];

  LCD_X_ClrCS();
  data[0] = 0x73;               // Start + RD Data
  LCD_X_WriteM(data, 1);
  LCD_X_ReadM (data, 1);  
  LCD_X_SetCS();
  
  return (data[0]);
}

/*********************************************************************
*
*       LCD_X_WriteM1_16
*
* Purpose:
*   Write multiple bytes to controller, with A0 = 1
*/
void LCD_X_WriteM1_16(U16 * pData, int NumWords) {
  U8 data[2];

  LCD_X_ClrCS();
  data[0] = 0x72;               // Start + WR Data
  LCD_X_WriteM(data, 1);
  for (; NumWords; NumWords--) {
    data[0] = (U8)(*pData >> 8);
    data[1] = (U8)(*pData);
    LCD_X_WriteM(data, 2);
    pData++;
  }
  LCD_X_SetCS();
}

/*********************************************************************
*
*       LCD_X_ReadM1_16
*
* Purpose:
*   Read multiple bytes from controller, with A0 = 1
*/
void LCD_X_ReadM1_16(U16 * pData, int NumWords) {
  U8  data[4];
  U32 v;

  if (NumWords != 4) return;

  LCD_X_ClrCS();
  data[0] = 0x73;               // Start + RD Data
  LCD_X_WriteM(data, 1);
  LCD_X_ReadM (data, 4);
  v = (data[1] << 16) |
      (data[2] <<  8) |
       data[3];
  *pData++ = (v & 0x03F000) >> 10;
  *pData++ = (v & 0xFC0000) >> 16;
  *pData++ = (v & 0x000FC0) >>  4;    
  LCD_X_SetCS();
}

#endif

/*********************************************************************
*
*       Private code
*
**********************************************************************
*/
static void wr_reg (U16 reg, U16 dat) {
  LCD_X_Write0_16(reg);
  LCD_X_Write1_16(dat);
}

/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/
static void _InitController(void) {
#ifndef WIN32

  LCD_X_Init();

  /* Driving ability settings --------------------------------------*/
  wr_reg(0xEA, 0x00);         /* Power control internal used (1)    */
  wr_reg(0xEB, 0x20);         /* Power control internal used (2)    */
  wr_reg(0xEC, 0x0C);         /* Source control internal used (1)   */
  wr_reg(0xED, 0xC7);         /* Source control internal used (2)   */
  wr_reg(0xE8, 0x38);         /* Source output period Normal mode   */
  wr_reg(0xE9, 0x10);         /* Source output period Idle mode     */
  wr_reg(0xF1, 0x01);         /* RGB 18-bit interface ;0x0110       */
  wr_reg(0xF2, 0x10);       

  /* Adjust the Gamma Curve ----------------------------------------*/
  wr_reg(0x40, 0x01);
  wr_reg(0x41, 0x00);
  wr_reg(0x42, 0x00);
  wr_reg(0x43, 0x10);
  wr_reg(0x44, 0x0E);
  wr_reg(0x45, 0x24);
  wr_reg(0x46, 0x04);
  wr_reg(0x47, 0x50);
  wr_reg(0x48, 0x02);
  wr_reg(0x49, 0x13);
  wr_reg(0x4A, 0x19);
  wr_reg(0x4B, 0x19);
  wr_reg(0x4C, 0x16);

  wr_reg(0x50, 0x1B);
  wr_reg(0x51, 0x31);
  wr_reg(0x52, 0x2F);
  wr_reg(0x53, 0x3F);
  wr_reg(0x54, 0x3F);
  wr_reg(0x55, 0x3E);
  wr_reg(0x56, 0x2F);
  wr_reg(0x57, 0x7B);
  wr_reg(0x58, 0x09);
  wr_reg(0x59, 0x06);
  wr_reg(0x5A, 0x06);
  wr_reg(0x5B, 0x0C);
  wr_reg(0x5C, 0x1D);
  wr_reg(0x5D, 0xCC);

  /* Power voltage setting -----------------------------------------*/
  wr_reg(0x1B, 0x1B);
  wr_reg(0x1A, 0x01);
  wr_reg(0x24, 0x2F);
  wr_reg(0x25, 0x57);
  wr_reg(0x23, 0x88);

  /* Power on setting ----------------------------------------------*/
  wr_reg(0x18, 0x36);         /* Internal oscillator frequency adj  */
  wr_reg(0x19, 0x01);         /* Enable internal oscillator         */
  wr_reg(0x01, 0x00);         /* Normal mode, no scrool             */
  wr_reg(0x1F, 0x88);         /* Power control 6 - DDVDH Off        */
  GUI_Delay(200);
  wr_reg(0x1F, 0x82);         /* Power control 6 - Step-up: 3 x VCI */
  GUI_Delay(50);                 
  wr_reg(0x1F, 0x92);         /* Power control 6 - Step-up: On      */
  GUI_Delay(50);
  wr_reg(0x1F, 0xD2);         /* Power control 6 - VCOML active     */
  GUI_Delay(50);

  /* Color selection -----------------------------------------------*/
  wr_reg(0x17, 0x55);         /* RGB, System interface: 16 Bit/Pixel*/
  wr_reg(0x00, 0x00);         /* Scrolling off, no standby          */

  /* Interface config ----------------------------------------------*/
  wr_reg(0x2F, 0x11);         /* LCD Drive: 1-line inversion        */
  wr_reg(0x31, 0x00);
  wr_reg(0x32, 0x00);         /* DPL=0, HSPL=0, VSPL=0, EPL=0       */

  /* Display on setting --------------------------------------------*/
  wr_reg(0x28, 0x38);         /* PT(0,0) active, VGL/VGL            */
  GUI_Delay(200);
  wr_reg(0x28, 0x3C);         /* Display active, VGL/VGL            */

//wr_reg(0x16, 0x00);         /* Mem Access Control (MX/Y/V/L,BGR)  */

  /* Display scrolling settings ------------------------------------*/
  wr_reg(0x0E, 0x00);         /* TFA MSB                            */
  wr_reg(0x0F, 0x00);         /* TFA LSB                            */
  wr_reg(0x10, 320 >> 8);     /* VSA MSB                            */
  wr_reg(0x11, 320 & 0xFF);   /* VSA LSB                            */
  wr_reg(0x12, 0x00);         /* BFA MSB                            */
  wr_reg(0x13, 0x00);         /* BFA LSB                            */

#endif  /* WIN32 */
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  GUI_DEVICE * pDevice;
  GUI_PORT_API PortAPI = {0}; 
  CONFIG_FLEXCOLOR Config = {0};
  
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  //
  // Common display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
  //
  // Configuration of GUIDRV_FLEX_COLOR
  //
  //
  // Select hardware interface
  //
  GUIDRV_FlexColor_SetReadFunc66712_B16(pDevice, GUIDRV_FLEXCOLOR_READ_FUNC_II);
  //
  // Set controller and operation mode
  //
  PortAPI.pfWrite16_A0  = LCD_X_Write0_16;
  PortAPI.pfWrite16_A1  = LCD_X_Write1_16;
  PortAPI.pfWriteM16_A1 = LCD_X_WriteM1_16;
  PortAPI.pfReadM16_A1  = LCD_X_ReadM1_16;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66712, GUIDRV_FLEXCOLOR_M16C0B16);
  //
  // Orientation and offset of SEG/COM lines
  //
  Config.Orientation   = DISPLAY_ORIENTATION;
  Config.RegEntryMode  = 0;
  GUIDRV_FlexColor_Config(pDevice, &Config);

  #if (USE_TOUCH == 1)
    //
    // Set orientation of touch screen
    //
    GUI_TOUCH_SetOrientation(DISPLAY_ORIENTATION);
    //
    // Calibrate touch screen
    //
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, WIDTH  - 1, TOUCH_LEFT, TOUCH_RIGHT);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, HEIGHT - 1, TOUCH_TOP,  TOUCH_BOTTOM);
  #endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    _InitController();
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/

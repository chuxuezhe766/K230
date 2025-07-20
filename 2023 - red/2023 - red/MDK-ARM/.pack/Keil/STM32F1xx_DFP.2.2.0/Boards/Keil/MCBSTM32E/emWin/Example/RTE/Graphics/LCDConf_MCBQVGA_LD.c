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

              Display:        Ampire 240320L8
              Controller:     Ilitek ILI9320

              Display:        Ampire 240320LD
              Controller:     OriseTech SPFD5408,
                              Raydium RM68050

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
#ifdef RTE_Graphics_LCD_MCBQVGA_LD_SPI

/*********************************************************************
*
*       LCD_X_Write0_16
*
* Purpose:
*   Write to controller, with A0 = 0
*/
void LCD_X_Write0_16(U16 c) {
  U8 data[3];

  LCD_X_ClrCS();
  data[0] = 0x70;               // Start + WR Register
  data[1] = (U8)(c >> 8);
  data[2] = (U8)(c);
  LCD_X_WriteM(data, 3);
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
  LCD_X_ReadM (data, 3);  
  LCD_X_SetCS();
  
  return ((data[1] << 8) | data[2]);
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
  U8 data[5];

  LCD_X_ClrCS();
  data[0] = 0x73;               // Start + RD Data
  LCD_X_WriteM(data, 1);
  LCD_X_ReadM (data, 5);        // Dummy RD
  for (; NumWords; NumWords--) {
    LCD_X_ReadM(data, 2);
    *pData = (data[0] << 8) |
              data[1];
    pData++;
  }
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

static U16 rd_reg (U16 reg) {
  LCD_X_Write0_16(reg);
  return LCD_X_Read1_16();
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
  U16 driverCode;

  LCD_X_Init();

  driverCode = rd_reg(0x00);

  /* Start Initial Sequence ----------------------------------------*/
  wr_reg(0x01, 0x0100);       /* Set SS bit                         */
  wr_reg(0x02, 0x0700);       /* Set 1 line inversion               */
  wr_reg(0x04, 0x0000);       /* Resize register                    */
  wr_reg(0x08, 0x0207);       /* 2 lines front, 7 back porch        */
  wr_reg(0x09, 0x0000);       /* Set non-disp area refresh cyc ISC  */
  wr_reg(0x0A, 0x0000);       /* FMARK function                     */
  wr_reg(0x0C, 0x0000);       /* RGB interface setting              */
  wr_reg(0x0D, 0x0000);       /* Frame marker Position              */
  wr_reg(0x0F, 0x0000);       /* RGB interface polarity             */

  /* Power On sequence ---------------------------------------------*/
  wr_reg(0x10, 0x0000);       /* Reset Power Control 1              */
  wr_reg(0x11, 0x0000);       /* Reset Power Control 2              */
  wr_reg(0x12, 0x0000);       /* Reset Power Control 3              */
  wr_reg(0x13, 0x0000);       /* Reset Power Control 4              */
  GUI_Delay(200);             /* Discharge cap power voltage (200ms)*/
  wr_reg(0x10, 0x12B0);       /* SAP, BT[3:0], AP, DSTB, SLP, STB   */
  wr_reg(0x11, 0x0007);       /* DC1[2:0], DC0[2:0], VC[2:0]        */
  GUI_Delay(50);              /* Delay 50 ms                        */
  wr_reg(0x12, 0x01BD);       /* VREG1OUT voltage                   */
  GUI_Delay(50);              /* Delay 50 ms                        */
  wr_reg(0x13, 0x1400);       /* VDV[4:0] for VCOM amplitude        */
  wr_reg(0x29, 0x000E);       /* VCM[4:0] for VCOMH                 */
  GUI_Delay(50);              /* Delay 50 ms                        */
  wr_reg(0x20, 0x0000);       /* GRAM horizontal Address            */
  wr_reg(0x21, 0x0000);       /* GRAM Vertical Address              */

  /* Adjust the Gamma Curve ----------------------------------------*/
  switch (driverCode) {
    case 0x5408:              /* LCD with SPFD5408 LCD Controller   */
      wr_reg(0x30, 0x0B0D);
      wr_reg(0x31, 0x1923);
      wr_reg(0x32, 0x1C26);
      wr_reg(0x33, 0x261C);
      wr_reg(0x34, 0x2419);
      wr_reg(0x35, 0x0D0B);
      wr_reg(0x36, 0x1006);
      wr_reg(0x37, 0x0610);
      wr_reg(0x38, 0x0706);
      wr_reg(0x39, 0x0304);
      wr_reg(0x3A, 0x0E05);
      wr_reg(0x3B, 0x0E01);
      wr_reg(0x3C, 0x010E);
      wr_reg(0x3D, 0x050E);
      wr_reg(0x3E, 0x0403);
      wr_reg(0x3F, 0x0607);
      break;
    case 0x9325:              /* LCD with RM68050 LCD Controller    */
      wr_reg(0x30, 0x0000);
      wr_reg(0x31, 0x0607);
      wr_reg(0x32, 0x0305);
      wr_reg(0x35, 0x0000);
      wr_reg(0x36, 0x1604);
      wr_reg(0x37, 0x0204);
      wr_reg(0x38, 0x0001);
      wr_reg(0x39, 0x0707);
      wr_reg(0x3C, 0x0000);
      wr_reg(0x3D, 0x000F);
      break;
    case 0x9320:              /* LCD with ILI9320 LCD Controller    */
    default:                  /* LCD with other LCD Controller      */
      wr_reg(0x30, 0x0006);
      wr_reg(0x31, 0x0101);
      wr_reg(0x32, 0x0003);
      wr_reg(0x35, 0x0106);
      wr_reg(0x36, 0x0B02);
      wr_reg(0x37, 0x0302);
      wr_reg(0x38, 0x0707);
      wr_reg(0x39, 0x0007);
      wr_reg(0x3C, 0x0600);
      wr_reg(0x3D, 0x020B);
      break;
  }

  /* Set GRAM area -------------------------------------------------*/
  wr_reg(0x50, 0x0000);       /* Horizontal GRAM Start Address      */
  wr_reg(0x51, (HEIGHT-1));   /* Horizontal GRAM End   Address      */
  wr_reg(0x52, 0x0000);       /* Vertical   GRAM Start Address      */
  wr_reg(0x53, (WIDTH-1));    /* Vertical   GRAM End   Address      */

  /* Set Gate Scan Line --------------------------------------------*/
  switch (driverCode) {
    case 0x5408:              /* LCD with SPFD5408 LCD Controller   */
    case 0x9325:              /* LCD with RM68050 LCD Controller    */
      wr_reg(0x60, 0xA700);
      break;
    case 0x9320:              /* LCD with ILI9320 LCD Controller    */
    default:                  /* LCD with other LCD Controller      */
      wr_reg(0x60, 0x2700);
      break;
  }
  wr_reg(0x61, 0x0001);       /* NDL,VLE, REV                       */
  wr_reg(0x6A, 0x0000);       /* Set scrolling line                 */

  /* Partial Display Control ---------------------------------------*/
  wr_reg(0x80, 0x0000);
  wr_reg(0x81, 0x0000);
  wr_reg(0x82, 0x0000);
  wr_reg(0x83, 0x0000);
  wr_reg(0x84, 0x0000);
  wr_reg(0x85, 0x0000);

  /* Panel Control -------------------------------------------------*/
  wr_reg(0x90, 0x0010);
  wr_reg(0x92, 0x0000);
  wr_reg(0x93, 0x0003);
  wr_reg(0x95, 0x0110);
  wr_reg(0x97, 0x0000);
  wr_reg(0x98, 0x0000);

//wr_reg(0x03, 0x0000);       /* Entry Mode (AM,ID0/1,ORG,BGR)      */

  wr_reg(0x07, 0x0137);       /* 262K color and display ON          */

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
  // Set controller and operation mode
  //
  PortAPI.pfWrite16_A0  = LCD_X_Write0_16;
  PortAPI.pfWrite16_A1  = LCD_X_Write1_16;
  PortAPI.pfWriteM16_A1 = LCD_X_WriteM1_16;
  PortAPI.pfReadM16_A1  = LCD_X_ReadM1_16;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66708, GUIDRV_FLEXCOLOR_M16C0B16);
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

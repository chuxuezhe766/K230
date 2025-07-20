/*-----------------------------------------------------------------------------
 * Name:    Touch_STMPE811.c
 * Purpose: Touchscreen STMPE811 interface
 * Rev.:    1.1
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "STMPE811.h"
#include "Driver_I2C.h"
#include "Board_Touch.h"

#ifndef TSC_I2C_PORT
#define TSC_I2C_PORT    1               /* I2C Port number                    */
#endif

#define TSC_I2C_ADDR    0x41            /* 7-bit I2C address                  */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(TSC_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(TSC_I2C_PORT))


/**
  \fn          int32_t Touch_ReadRegister (uint8_t reg, uint8_t *val)
  \brief       Read register value from Touchscreen controller
  \param[in]   reg    Register address
  \param[out]  val    Pointer where data will be read from register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t Touch_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];

  data[0] = reg;
  ptrI2C->MasterTransmit(TSC_I2C_ADDR, data, 1, true);
  while (ptrI2C->GetStatus().busy);
  if (ptrI2C->GetDataCount() != 1) return -1;
  ptrI2C->MasterReceive (TSC_I2C_ADDR, val, 1, false);
  while (ptrI2C->GetStatus().busy);
  if (ptrI2C->GetDataCount() != 1) return -1;

  return 0;
}

/**
  \fn          int32_t Touch_WriteData (uint8_t reg, const uint8_t *val)
  \brief       Write value to Touchscreen controller register
  \param[in]   reg    Register address
  \param[in]   val    Pointer with data to write to register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t Touch_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];

  data[0] = reg;
  data[1] = val;
  ptrI2C->MasterTransmit(TSC_I2C_ADDR, data, 2, false);
  while (ptrI2C->GetStatus().busy);
  if (ptrI2C->GetDataCount() != 2) return -1;

  return 0;
}


/**
  \fn          int32_t Touch_Initialize (void)
  \brief       Initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Initialize (void) {
  uint32_t tick;
  
  ptrI2C->Initialize  (NULL);
  ptrI2C->PowerControl(ARM_POWER_FULL);
  ptrI2C->Control     (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  ptrI2C->Control     (ARM_I2C_BUS_CLEAR, 0);

  Touch_Write(STMPE811_SYS_CTRL1,      0x02); /* Reset Touch-screen controller */
  tick = osKernelSysTick();                   /* Wait 10ms                     */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

  Touch_Write(STMPE811_SYS_CTRL2,      0x0C); /* Enable TSC and ADC            */
  Touch_Write(STMPE811_ADC_CTRL1,      0x68); /* Set sample time , 12-bit mode */
  tick = osKernelSysTick();                   /* Wait 500us                    */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(500));

  Touch_Write(STMPE811_ADC_CTRL2,      0x01); /* ADC frequency 3.25 MHz        */
  Touch_Write(STMPE811_TSC_CFG,        0xC2); /* Detect delay 10us,
                                                 Settle time 500us             */
  Touch_Write(STMPE811_FIFO_TH,        0x01); /* Threshold for FIFO            */
  Touch_Write(STMPE811_FIFO_STA,       0x01); /* FIFO reset                    */
  Touch_Write(STMPE811_FIFO_STA,       0x00); /* FIFO not reset                */
  Touch_Write(STMPE811_TSC_FRACTION_Z, 0x07); /* Fraction z                    */
  Touch_Write(STMPE811_TSC_I_DRIVE,    0x01); /* Drive 50 mA typical           */
  Touch_Write(STMPE811_GPIO_AF,        0x00); /* Pins are used for touchscreen */
  Touch_Write(STMPE811_TSC_CTRL,       0x01); /* Enable TSC                    */

  return 0;
}

/**
  \fn          int32_t Touch_Uninitialize (void)
  \brief       De-initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Uninitialize (void) {
  Touch_Write(STMPE811_SYS_CTRL1, 0x02);  /* Reset Touch-screen controller    */
  return 0;
}

/**
  \fn          int32_t Touch_GetState (TOUCH_STATE *pState)
  \brief       Get touchscreen state
  \param[out]  pState  pointer to TOUCH_STATE structure
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_GetState (TOUCH_STATE *pState) {
  uint8_t val;
  uint8_t num;
  uint8_t xyz[4];
  int32_t res;

  /* Read touch status */
  res = Touch_Read(STMPE811_TSC_CTRL, &val);
  if (res < 0) return -1;
  pState->pressed = (val & (1 << 7)) ? 1 : 0;
  
  if (pState->pressed) {
    val = STMPE811_TSC_DATA;

    /* If FIFO overflow, discard all samples except the last one */
    res = Touch_Read(STMPE811_FIFO_SIZE, &num);
    if (res < 0 || num == 0) return -1;

    while (num--) {
      ptrI2C->MasterTransmit (TSC_I2C_ADDR, &val, 1, true);
      while (ptrI2C->GetStatus().busy);
      ptrI2C->MasterReceive (TSC_I2C_ADDR, xyz, 4, false);
      while (ptrI2C->GetStatus().busy);
    }
    pState->x =  (xyz[0] << 4) | ((xyz[1] & 0xF0) >> 4);
    pState->y =   xyz[2]       | ((xyz[1] & 0x0F) << 8);
  }  else {
    /* Clear all data in FIFO */
    Touch_Write(STMPE811_FIFO_STA, 0x1);
    Touch_Write(STMPE811_FIFO_STA, 0x0);
  }

  return 0;
}

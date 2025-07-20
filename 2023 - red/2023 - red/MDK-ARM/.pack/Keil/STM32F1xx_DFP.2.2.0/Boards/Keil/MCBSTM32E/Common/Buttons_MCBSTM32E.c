/*-----------------------------------------------------------------------------
 * Name:    Buttons_MCBSTM32E.c
 * Purpose: Buttons interface for MCBSTM32E evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2015 ARM LIMITED

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

#include "Board_Buttons.h"
#include "GPIO_STM32F10x.h"

/* Buttons for MCBSTM32E */
#define BUTTONS_COUNT   3               /* Number of available buttons        */
#define WAKEUP_MASK     (1<<0)
#define WAKEUP_PORT     GPIOA
#define WAKEUP_BIT      0
#define TAMPER_MASK     (1<<1)
#define TAMPER_PORT     GPIOC
#define TAMPER_BIT      13
#define USER_MASK       (1<<2)
#define USER_PORT       GPIOG
#define USER_BIT        8


/**
  \fn          int32_t Buttons_Initialize (void)
  \brief       Initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Initialize (void) {

  /* Configure WAKEUP pin */
  GPIO_PortClock   (WAKEUP_PORT, true);
  GPIO_PinConfigure(WAKEUP_PORT, WAKEUP_BIT,
                    GPIO_IN_PULL_DOWN,
                    GPIO_MODE_INPUT);
  
  /* Configure TAMPER pin */
  GPIO_PortClock   (TAMPER_PORT, true);
  GPIO_PinConfigure(TAMPER_PORT, TAMPER_BIT,
                    GPIO_IN_PULL_UP,
                    GPIO_MODE_INPUT);

  /* Configure USER pin */
  GPIO_PortClock   (USER_PORT, true);
  GPIO_PinConfigure(USER_PORT, USER_BIT,
                    GPIO_IN_PULL_UP,
                    GPIO_MODE_INPUT);

  return 0;
}

/**
  \fn          int32_t Buttons_Uninitialize (void)
  \brief       De-initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Uninitialize (void) {
  return 0;
}

/**
  \fn          uint32_t Buttons_GetState (void)
  \brief       Get buttons state
  \returns     Buttons state
*/
uint32_t Buttons_GetState (void) {
  uint32_t val = 0;

  if (GPIO_PinRead(WAKEUP_PORT, WAKEUP_BIT) == 1) {
    /* Wakeup button */
    val |= WAKEUP_MASK;
  }
  if (GPIO_PinRead(TAMPER_PORT, TAMPER_BIT) == 0) {
    /* Tamper button */
    val |= TAMPER_MASK;
  }
  if (GPIO_PinRead(USER_PORT, USER_BIT) == 0) {
    /* User button */
    val |= USER_MASK;
  }
  return (val);
}

/**
  \fn          uint32_t Buttons_GetCount (void)
  \brief       Get number of available buttons
  \return      Number of available buttons
*/
uint32_t Buttons_GetCount (void) {
  return BUTTONS_COUNT;
}

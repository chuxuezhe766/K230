/*-----------------------------------------------------------------------------
 * Name:    Joystick_MCBSTM32E.c
 * Purpose: Joystick interface for MCBSTM32E evaluation board
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

#include "Board_Joystick.h"
#include "GPIO_STM32F10x.h"

typedef struct {
  GPIO_PIN_ID Pin;
  uint8_t     Button;
} JOYSTICK_MAP;


const JOYSTICK_MAP Joystick[] = {
  {{ GPIOG, 14 }, JOYSTICK_LEFT},
  {{ GPIOG, 13 }, JOYSTICK_RIGHT},
  {{ GPIOG,  7 }, JOYSTICK_CENTER},
  {{ GPIOG, 15 }, JOYSTICK_UP},
  {{ GPIOD,  3 }, JOYSTICK_DOWN}
};

#define PIN_COUNT (sizeof(Joystick)/sizeof(JOYSTICK_MAP))

/**
  \fn          int32_t Joystick_Initialize (void)
  \brief       Initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Initialize (void) {
  uint32_t idx;
  
  /* Configure Pins */
  for (idx = 0; idx < PIN_COUNT; idx++) {
    GPIO_PortClock   (Joystick[idx].Pin.port, true);
    GPIO_PinConfigure(Joystick[idx].Pin.port, Joystick[idx].Pin.num,
                      GPIO_IN_FLOATING, 
                      GPIO_MODE_INPUT);
  }
  return 0;
}

/**
  \fn          int32_t Joystick_Uninitialize (void)
  \brief       De-initialize joystick
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Joystick_Uninitialize (void) {
  uint32_t idx;
  
  for (idx = 0; idx < PIN_COUNT; idx++) {
    GPIO_PinConfigure(Joystick[idx].Pin.port, Joystick[idx].Pin.num,
                      GPIO_OUT_PUSH_PULL, 
                      GPIO_MODE_OUT2MHZ);
  }
  return 0;
}

/**
  \fn          uint32_t Joystick_GetState (void)
  \brief       Get joystick state
  \returns     Joystick state
*/
uint32_t Joystick_GetState (void) {
  uint32_t pd;
  uint32_t idx;
  uint32_t btn = 0;
  
  for (idx = 0; idx < PIN_COUNT; idx++) { 
    pd = GPIO_PortRead(Joystick[idx].Pin.port);
    if ( (pd & (1 << Joystick[idx].Pin.num)) == 0) {
      btn |= Joystick[idx].Button;
    }
  }  
  return btn;
}

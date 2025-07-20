/*-----------------------------------------------------------------------------
 * Name:    ADC_MCBSTM32E.c
 * Purpose: A/D Converter interface for MCBSTM32E evaluation board
 * Rev.:    1.0.0
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

#include "STM32F10x.h"
#include "Board_ADC.h"

#define ADC_RESOLUTION        12        /* Number of A/D converter bits       */

static volatile uint16_t AD_last;       /* Last converted value               */
static volatile uint8_t  AD_done;       /* AD conversion done flag            */


/**
  \fn          int32_t ADC_Initialize (void)
  \brief       Initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Initialize (void) {

  RCC->APB2ENR |= ( 1UL <<  4);           /* Enable GPIOC peripheral clock    */
  GPIOC->CRL &= ~0x000F0000;              /* Configure PC4 as ADC.14 input    */

  /* Setup and initialize ADC converter                                       */
  RCC->CFGR    |= ( 3UL << 14);           /* ADC clk = PCLK2 / 8              */

  RCC->APB2ENR |= ( 1UL <<  9);           /* Enable ADC1 peripheral clock     */

  ADC1->SQR1    =  0;                     /* Regular chn. Sequence length = 1 */
  ADC1->SQR2    =  0;                     /* Clear register                   */
  ADC1->SQR3    = (14UL <<  0);           /* 1. conversion = channel 14       */
  ADC1->SMPR1   = ( 5UL << 12);           /* sample time channel 14 55,5 cyc. */
  ADC1->CR1     = ( 1UL <<  5) |          /* enable for EOC Interrupt         */
                  ( 1UL <<  8) ;          /* Scan mode on                     */
  ADC1->CR2     = ( 7UL << 17) |          /* select SWSTART                   */
                  ( 1UL << 20) ;          /* enable external Trigger          */

  NVIC_EnableIRQ(ADC1_2_IRQn);            /* enable ADC Interrupt             */

  ADC1->CR2    |= ( 1UL <<  0);           /* ADC enable                       */

  ADC1->CR2    |=  1 <<  3;               /* Initialize calibration registers */
  while (ADC1->CR2 & (1 << 3));           /* Wait for init to finish          */
  ADC1->CR2    |=  1 <<  2;               /* Start calibration                */
  while (ADC1->CR2 & (1 << 2));           /* Wait for calibration to finish   */

  return 0;
}

/**
  \fn          int32_t ADC_Uninitialize (void)
  \brief       De-initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Uninitialize (void) {
  return 0;
}

/**
  \fn          int32_t ADC_StartConversion (void)
  \brief       Start conversion
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_StartConversion (void) {
  AD_done = 0;
  ADC1->CR2    |=  (1UL << 22);           /* Start A/D conversion             */ 
  return 0;
}

/**
  \fn          int32_t ADC_ConversionDone (void)
  \brief       Check if conversion finished
  \returns
   - \b  0: conversion finished
   - \b -1: conversion in progress
*/
int32_t ADC_ConversionDone (void) {
  return (AD_done) ? 0 : -1;
}

/**
  \fn          int32_t ADC_GetValue (void)
  \brief       Get converted value
  \returns
   - <b> >=0</b>: converted value
   - \b -1: conversion in progress or failed
*/
int32_t ADC_GetValue (void) {
  return (AD_done) ? AD_last : -1;
}

/**
  \fn          uint32_t ADC_GetResolution (void)
  \brief       Get resolution of Analog-to-Digital Converter
  \returns     Resolution (in bits)
*/
uint32_t ADC_GetResolution (void) {
  return ADC_RESOLUTION;
}

/**
  \fn          ADC1_2_IRQHandler (void)
  \brief       A/D IRQ: Executed when A/D Conversion is done
*/
void ADC1_2_IRQHandler (void) {

  if (ADC1->SR & (1 << 1)) {            /* ADC1 EOC interrupt?                */
    AD_last = ADC1->DR;
    AD_done = 1;
    ADC1->SR &= ~(1 << 1);              /* clear EOC interrupt                */
  }
}

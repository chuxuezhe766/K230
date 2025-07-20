/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2015 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        22. September 2015
 * $Revision:    V2.0
 *
 * Project:      I2C Driver definitions for STMicroelectronics STM32F10x
 * -------------------------------------------------------------------------- */

#ifndef __I2C_STM32F10X_H
#define __I2C_STM32F10X_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f10x.h"
#include "GPIO_STM32F10x.h"
#include "DMA_STM32F10x.h"

#include "Driver_I2C.h"

#include "RTE_Components.h"
#include "RTE_Device.h"

#if ((defined(RTE_Drivers_I2C1) || \
      defined(RTE_Drivers_I2C2))   \
     && (RTE_I2C1 == 0)            \
     && (RTE_I2C2 == 0))
  #error "I2C not configured in RTE_Device.h!"
#endif

#define RCC_APB_I2C1_MASK       RCC_APB1ENR_I2C1EN  /* same for Clock/Reset */
#define RCC_APB_I2C2_MASK       RCC_APB1ENR_I2C2EN  /* same for Clock/Reset */
#define RCC_APB_I2C3_MASK       RCC_APB1ENR_I2C3EN  /* same for Clock/Reset */


/* I2C1 configuration definitions */
#if defined (RTE_I2C1) && (RTE_I2C1 == 1)
  #if (((RTE_I2C1_RX_DMA != 0) && (RTE_I2C1_TX_DMA == 0)) || \
       ((RTE_I2C1_RX_DMA == 0) && (RTE_I2C1_TX_DMA != 0)))
    #error "I2C1 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h!"
  #endif

  #define USE_I2C1

  #if (RTE_I2C1_RX_DMA == 1) && (RTE_I2C1_TX_DMA == 1)
    #define USE_I2C1_DMA

    /* Rx channel */
    #define I2C1_RX_DMA_Instance    DMAx_CHANNELy(RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_CHANNEL)
    #define I2C1_RX_DMA_Number      RTE_I2C1_RX_DMA_NUMBER
    #define I2C1_RX_DMA_Channel     RTE_I2C1_RX_DMA_CHANNEL
    #define I2C1_RX_DMA_Priority    RTE_I2C1_RX_DMA_PRIORITY

    #define I2C1_RX_DMA_Handler     DMAx_CHANNELy_EVENT (RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_CHANNEL)

    /* Tx channel */
    #define I2C1_TX_DMA_Instance    DMAx_CHANNELy(RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_CHANNEL)
    #define I2C1_TX_DMA_Number      RTE_I2C1_TX_DMA_NUMBER
    #define I2C1_TX_DMA_Channel     RTE_I2C1_TX_DMA_CHANNEL
    #define I2C1_TX_DMA_Priority    RTE_I2C1_TX_DMA_PRIORITY

    #define I2C1_TX_DMA_Handler     DMAx_CHANNELy_EVENT (RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_CHANNEL)
  #endif

  #define I2C1_SCL_GPIOx            RTE_I2C1_SCL_PORT
  #define I2C1_SCL_GPIO_Pin         RTE_I2C1_SCL_BIT
  #define I2C1_SDA_GPIOx            RTE_I2C1_SDA_PORT
  #define I2C1_SDA_GPIO_Pin         RTE_I2C1_SDA_BIT

  #define I2C1_AF_REMAP             RTE_I2C1_AF_REMAP
#endif

/* I2C2 configuration definitions */
#if defined (RTE_I2C2) && (RTE_I2C2 == 1)

  #if !defined(I2C2)
    #error "I2C2 not available for selected device!"
  #endif

  #if (((RTE_I2C2_RX_DMA != 0) && (RTE_I2C2_TX_DMA == 0)) || \
       ((RTE_I2C2_RX_DMA == 0) && (RTE_I2C2_TX_DMA != 0)))
    #error "I2C2 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h!"
  #endif

  #define USE_I2C2

  #if (RTE_I2C2_RX_DMA == 1) && (RTE_I2C2_TX_DMA == 1)
    #define USE_I2C2_DMA

    /*  Rx channel */
    #define I2C2_RX_DMA_Instance    DMAx_CHANNELy(RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_CHANNEL)
    #define I2C2_RX_DMA_Number      RTE_I2C2_RX_DMA_NUMBER
    #define I2C2_RX_DMA_Channel     RTE_I2C2_RX_DMA_CHANNEL
    #define I2C2_RX_DMA_Priority    RTE_I2C2_RX_DMA_PRIORITY

    #define I2C2_RX_DMA_Handler     DMAx_CHANNELy_EVENT (RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_CHANNEL)

    /* Tx channel */
    #define I2C2_TX_DMA_Instance    DMAx_CHANNELy(RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_CHANNEL)
    #define I2C2_TX_DMA_Number      RTE_I2C2_TX_DMA_NUMBER
    #define I2C2_TX_DMA_Channel     RTE_I2C2_TX_DMA_CHANNEL
    #define I2C2_TX_DMA_Priority    RTE_I2C2_TX_DMA_PRIORITY

    #define I2C2_TX_DMA_Handler     DMAx_CHANNELy_EVENT (RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_CHANNEL)
  #endif

  #define I2C2_SCL_GPIOx            RTE_I2C2_SCL_PORT
  #define I2C2_SCL_GPIO_Pin         RTE_I2C2_SCL_BIT
  #define I2C2_SDA_GPIOx            RTE_I2C2_SDA_PORT
  #define I2C2_SDA_GPIO_Pin         RTE_I2C2_SDA_BIT

  #define I2C2_AF_REMAP             RTE_I2C2_AF_REMAP
#endif

/* Current driver status flag definition */
#define I2C_INIT            ((uint8_t)0x01)   // I2C initialized
#define I2C_POWER           ((uint8_t)0x02)   // I2C powered on
#define I2C_SETUP           ((uint8_t)0x04)   // I2C configured

/* Transfer status flags definitions */
#define XFER_CTRL_XPENDING  ((uint8_t)0x01)   // Transfer pending
#define XFER_CTRL_RSTART    ((uint8_t)0x02)   // Generate repeated start and readdress
#define XFER_CTRL_ADDR_DONE ((uint8_t)0x04)   // Addressing done
#define XFER_CTRL_DMA_DONE  ((uint8_t)0x08)   // DMA transfer done
#define XFER_CTRL_WAIT_BTF  ((uint8_t)0x10)   // Wait for byte transfer finished
#define XFER_CTRL_XACTIVE   ((uint8_t)0x20)   // Transfer active


/* DMA Information definitions */
typedef struct _I2C_DMA {
  DMA_Channel_TypeDef  *reg;                  // DMA register interface
  uint8_t               dma_num;              // DMA number
  uint8_t               channel;              // Channel number
  uint8_t               priority;             // Channel priority
} const I2C_DMA;


/* I2C Input/Output Configuration */
typedef const struct _I2C_IO {
  GPIO_TypeDef         *scl_port;             // SCL IO Port
  GPIO_TypeDef         *sda_port;             // SDA IO Port
  uint16_t              scl_pin;              // SCL IO Pin
  uint16_t              sda_pin;              // SDA IO Pin
  AFIO_REMAP            remap;
} I2C_IO;


/* I2C Transfer Information (Run-Time) */
typedef struct _I2C_TRANSFER_INFO {
  uint32_t              num;                  // Number of data to transfer
  uint32_t              cnt;                  // Data transfer counter
  uint8_t              *data;                 // Data pointer
  uint16_t              addr;                 // Device address
  uint8_t               ctrl;                 // Transfer control flags
} I2C_TRANSFER_INFO;


/* I2C Information (Run-Time) */
typedef struct _I2C_INFO {
  ARM_I2C_SignalEvent_t cb_event;             // Event Callback
  ARM_I2C_STATUS        status;               // Status flags
  I2C_TRANSFER_INFO     xfer;                 // Transfer information
  uint8_t               flags;                // Current I2C state flags
} I2C_INFO;


/* I2C Resources definition */
typedef struct {
  I2C_TypeDef          *reg;                  // I2C peripheral register interface
  I2C_DMA              *dma_rx;               // I2C DMA Configuration
  I2C_DMA              *dma_tx;               // I2C DMA Configuration
  I2C_IO                io;                   // I2C Input/Output pins
  IRQn_Type             ev_irq_num;           // I2C Event IRQ Number
  IRQn_Type             er_irq_num;           // I2C Error IRQ Number
  uint32_t              apb_mask;             // APB Clock/Reset register mask
  I2C_INFO             *info;                 // Run-Time information
} const I2C_RESOURCES;

#endif /* __I2C_STM32F10X_H */

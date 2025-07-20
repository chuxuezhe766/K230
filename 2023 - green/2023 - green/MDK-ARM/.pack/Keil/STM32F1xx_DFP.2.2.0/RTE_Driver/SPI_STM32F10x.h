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
 * $Date:        29. August 2016
 * $Revision:    V2.2
 *
 * Project:      SPI Driver definitions for STMicroelectronics STM32F10x
 * -------------------------------------------------------------------------- */

#ifndef __SPI_STM32F10X_H
#define __SPI_STM32F10X_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f10x.h"
#include "GPIO_STM32F10x.h"
#include "DMA_STM32F10x.h"

#include "Driver_SPI.h"

#include "RTE_Components.h"
#include "RTE_Device.h"

#if ((defined(RTE_Drivers_SPI1) || \
      defined(RTE_Drivers_SPI2) || \
      defined(RTE_Drivers_SPI3))   \
     && (RTE_SPI1 == 0)   \
     && (RTE_SPI2 == 0)   \
     && (RTE_SPI3 == 0))
  #error "SPI not configured in RTE_Device.h!"
#endif

// SPI1 configuration definitions
#if (RTE_SPI1 == 1)
  #define MX_SPI1

  #if (RTE_SPI1_RX_DMA == 1)
    #define MX_SPI1_RX_DMA_Instance DMAx_CHANNELy(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_CHANNEL)
    #define MX_SPI1_RX_DMA_Number   RTE_SPI1_RX_DMA_NUMBER
    #define MX_SPI1_RX_DMA_Channel  RTE_SPI1_RX_DMA_CHANNEL
    #define MX_SPI1_RX_DMA_Priority RTE_SPI1_RX_DMA_PRIORITY

    #define SPI1_RX_DMA_Handler     DMAx_CHANNELy_EVENT(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_CHANNEL)
  #endif
  #if (RTE_SPI1_TX_DMA == 1)
    #define MX_SPI1_TX_DMA_Instance DMAx_CHANNELy(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_CHANNEL)
    #define MX_SPI1_TX_DMA_Number   RTE_SPI1_TX_DMA_NUMBER
    #define MX_SPI1_TX_DMA_Channel  RTE_SPI1_TX_DMA_CHANNEL
    #define MX_SPI1_TX_DMA_Priority RTE_SPI1_TX_DMA_PRIORITY

    #define SPI1_TX_DMA_Handler     DMAx_CHANNELy_EVENT(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_CHANNEL)
  #endif

  #if (RTE_SPI1_MISO == 1)
    #define MX_SPI1_MISO_Pin        1U
    #define MX_SPI1_MISO_GPIOx      RTE_SPI1_MISO_PORT
    #define MX_SPI1_MISO_GPIO_Pin   RTE_SPI1_MISO_BIT
  #endif

  #if (RTE_SPI1_MOSI == 1)
    #define MX_SPI1_MOSI_Pin        1U
    #define MX_SPI1_MOSI_GPIOx      RTE_SPI1_MOSI_PORT
    #define MX_SPI1_MOSI_GPIO_Pin   RTE_SPI1_MOSI_BIT
  #endif

  #define MX_SPI1_SCK_Pin           1U
  #define MX_SPI1_SCK_GPIOx         RTE_SPI1_SCK_PORT
  #define MX_SPI1_SCK_GPIO_Pin      RTE_SPI1_SCK_BIT

  #if (RTE_SPI1_NSS_PIN == 1)
    #define MX_SPI1_NSS_Pin         1U
    #define MX_SPI1_NSS_GPIOx       RTE_SPI1_NSS_PORT
    #define MX_SPI1_NSS_GPIO_Pin    RTE_SPI1_NSS_BIT
  #endif

  #define MX_SPI1_REMAP_DEF         AFIO_SPI1_NO_REMAP
  #define MX_SPI1_REMAP             RTE_SPI1_AF_REMAP
#endif

// SPI2 configuration definitions
#if (RTE_SPI2 == 1)

  #if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL)
    #error "SPI2 not available for selected device!"
  #endif

  #define MX_SPI2

  #if (RTE_SPI2_RX_DMA == 1)
    #define MX_SPI2_RX_DMA_Instance DMAx_CHANNELy(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_CHANNEL)
    #define MX_SPI2_RX_DMA_Number   RTE_SPI2_RX_DMA_NUMBER
    #define MX_SPI2_RX_DMA_Channel  RTE_SPI2_RX_DMA_CHANNEL
    #define MX_SPI2_RX_DMA_Priority RTE_SPI2_RX_DMA_PRIORITY

    #define SPI2_RX_DMA_Handler     DMAx_CHANNELy_EVENT(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_CHANNEL)
  #endif
  #if (RTE_SPI2_TX_DMA == 1)
    #define MX_SPI2_TX_DMA_Instance DMAx_CHANNELy(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_CHANNEL)
    #define MX_SPI2_TX_DMA_Number   RTE_SPI2_TX_DMA_NUMBER
    #define MX_SPI2_TX_DMA_Channel  RTE_SPI2_TX_DMA_CHANNEL
    #define MX_SPI2_TX_DMA_Priority RTE_SPI2_TX_DMA_PRIORITY

    #define SPI2_TX_DMA_Handler     DMAx_CHANNELy_EVENT(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_CHANNEL)
  #endif

  #if (RTE_SPI2_MISO == 1)
    #define MX_SPI2_MISO_Pin        1U
    #define MX_SPI2_MISO_GPIOx      RTE_SPI2_MISO_PORT
    #define MX_SPI2_MISO_GPIO_Pin   RTE_SPI2_MISO_BIT
  #endif

  #if (RTE_SPI2_MOSI == 1)
    #define MX_SPI2_MOSI_Pin        1U
    #define MX_SPI2_MOSI_GPIOx      RTE_SPI2_MOSI_PORT
    #define MX_SPI2_MOSI_GPIO_Pin   RTE_SPI2_MOSI_BIT
  #endif

  #define MX_SPI2_SCK_Pin           1U
  #define MX_SPI2_SCK_GPIOx         RTE_SPI2_SCK_PORT
  #define MX_SPI2_SCK_GPIO_Pin      RTE_SPI2_SCK_BIT

  #if (RTE_SPI2_NSS_PIN == 1)
    #define MX_SPI2_NSS_Pin         1U
    #define MX_SPI2_NSS_GPIOx       RTE_SPI2_NSS_PORT
    #define MX_SPI2_NSS_GPIO_Pin    RTE_SPI2_NSS_BIT
  #endif

  #define MX_SPI2_REMAP_DEF         AFIO_UNAVAILABLE_REMAP
  #define MX_SPI2_REMAP             AFIO_UNAVAILABLE_REMAP
#endif

// SPI3 configuration definitions
#if (RTE_SPI3 == 1)

  #if !defined(STM32F10X_HD) && !defined(STM32F10X_CL) && !defined(STM32F10X_XL) && !defined(STM32F10X_HD_VL)
    #error "SPI3 is not available for selected device!"
  #endif

  #define MX_SPI3

  #if (RTE_SPI3_RX_DMA == 1)
    #define MX_SPI3_RX_DMA_Instance DMAx_CHANNELy(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_CHANNEL)
    #define MX_SPI3_RX_DMA_Number   RTE_SPI3_RX_DMA_NUMBER
    #define MX_SPI3_RX_DMA_Channel  RTE_SPI3_RX_DMA_CHANNEL
    #define MX_SPI3_RX_DMA_Priority RTE_SPI3_RX_DMA_PRIORITY

    #define SPI3_RX_DMA_Handler     DMAx_CHANNELy_EVENT(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_CHANNEL)
  #endif
  #if (RTE_SPI3_TX_DMA == 1)
    #define MX_SPI3_TX_DMA_Instance DMAx_CHANNELy(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_CHANNEL)
    #define MX_SPI3_TX_DMA_Number   RTE_SPI3_TX_DMA_NUMBER
    #define MX_SPI3_TX_DMA_Channel  RTE_SPI3_TX_DMA_CHANNEL
    #define MX_SPI3_TX_DMA_Priority RTE_SPI3_TX_DMA_PRIORITY

    #define SPI3_TX_DMA_Handler     DMAx_CHANNELy_EVENT(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_CHANNEL)
  #endif

  #if (RTE_SPI3_MISO == 1)
    #define MX_SPI3_MISO_Pin        1U
    #define MX_SPI3_MISO_GPIOx      RTE_SPI3_MISO_PORT
    #define MX_SPI3_MISO_GPIO_Pin   RTE_SPI3_MISO_BIT
  #endif

  #if (RTE_SPI3_MOSI == 1)
    #define MX_SPI3_MOSI_Pin        1U
    #define MX_SPI3_MOSI_GPIOx      RTE_SPI3_MOSI_PORT
    #define MX_SPI3_MOSI_GPIO_Pin   RTE_SPI3_MOSI_BIT
  #endif

  #define MX_SPI3_SCK_Pin           1U
  #define MX_SPI3_SCK_GPIOx         RTE_SPI3_SCK_PORT
  #define MX_SPI3_SCK_GPIO_Pin      RTE_SPI3_SCK_BIT

  #if (RTE_SPI3_NSS_PIN == 1)
    #define MX_SPI3_NSS_Pin         1U
    #define MX_SPI3_NSS_GPIOx       RTE_SPI3_NSS_PORT
    #define MX_SPI3_NSS_GPIO_Pin    RTE_SPI3_NSS_BIT
  #endif

  #if defined(STM32F10X_CL)
    // SPI3 remap is available only in connectivity line devices
    #define MX_SPI3_REMAP_DEF       AFIO_SPI3_NO_REMAP
    #define MX_SPI3_REMAP           RTE_SPI3_AF_REMAP
  #else
    #if (RTE_SPI3_REMAP != 0)
      #error "SPI3 remap is available only in connectivity line devices!"
    #endif
    #define MX_SPI3_REMAP_DEF       AFIO_UNAVAILABLE_REMAP
    #define MX_SPI3_REMAP           AFIO_UNAVAILABLE_REMAP
  #endif
#endif


#ifdef MX_SPI1
#if (defined(MX_SPI1_RX_DMA_Instance) || defined(MX_SPI1_TX_DMA_Instance))
#ifndef MX_SPI1_RX_DMA_Instance
  #error "SPI1 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI1_TX_DMA_Instance
  #error "SPI1 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI2
#if (defined(MX_SPI2_RX_DMA_Instance) || defined(MX_SPI2_TX_DMA_Instance))
#ifndef MX_SPI2_RX_DMA_Instance
  #error "SPI2 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI2_TX_DMA_Instance
  #error "SPI2 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif

#ifdef MX_SPI3
#if (defined(MX_SPI3_RX_DMA_Instance) || defined(MX_SPI3_TX_DMA_Instance))
#ifndef MX_SPI3_RX_DMA_Instance
  #error "SPI3 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#ifndef MX_SPI3_TX_DMA_Instance
  #error "SPI3 using DMA requires Rx and Tx DMA channel enabled in RTE_Device.h or MX_Device.h!"
#endif
#endif
#endif


#if ((defined(MX_SPI1) && defined(MX_SPI1_RX_DMA_Instance)) || \
     (defined(MX_SPI2) && defined(MX_SPI2_RX_DMA_Instance)) || \
     (defined(MX_SPI3) && defined(MX_SPI3_RX_DMA_Instance)))
#define __SPI_DMA_RX
#endif
#if ((defined(MX_SPI1) && defined(MX_SPI1_TX_DMA_Instance)) || \
     (defined(MX_SPI2) && defined(MX_SPI2_TX_DMA_Instance)) || \
     (defined(MX_SPI3) && defined(MX_SPI3_TX_DMA_Instance)))
#define __SPI_DMA_TX
#endif
#if (defined(__SPI_DMA_RX) && defined(__SPI_DMA_TX))
#define __SPI_DMA
#endif


// Current driver status flag definition
#define SPI_INITIALIZED           ((uint8_t)(1U))           // SPI initialized
#define SPI_POWERED               ((uint8_t)(1U << 1))     // SPI powered on
#define SPI_CONFIGURED            ((uint8_t)(1U << 2))     // SPI configured
#define SPI_DATA_LOST             ((uint8_t)(1U << 3))     // SPI data lost occurred
#define SPI_MODE_FAULT            ((uint8_t)(1U << 4))     // SPI mode fault occurred


// SPI DMA
typedef struct _SPI_DMA {
  DMA_Channel_TypeDef *instance;        // Channel registry interface
  uint8_t              dma_num;         // DMA number
  uint8_t              ch_num;          // Channel number
  uint8_t              priority;        // Channel priority
} SPI_DMA;

// SPI pin
typedef const struct _SPI_PIN {
  GPIO_TypeDef         *port;           // Port
  uint32_t              pin;            // Pin
} SPI_PIN;

// SPI Input/Output Configuration
typedef const struct _SPI_IO {
  SPI_PIN              *mosi;           // Pointer to MOSI pin configuration
  SPI_PIN              *miso;           // Pointer to MISO pin configuration
  SPI_PIN              *nss;            // Pointer to NSS pin configuration
  SPI_PIN              *sck;            // Pointer to SCK pin configuration
  AFIO_REMAP            afio_def;       // Default pin remapping setting
  AFIO_REMAP            afio;           // Pin remapping information
} SPI_IO;

typedef struct _SPI_STATUS {
  uint8_t busy;                         // Transmitter/Receiver busy flag
  uint8_t data_lost;                    // Data lost: Receive overflow / Transmit underflow (cleared on start of transfer operation)
  uint8_t mode_fault;                   // Mode fault detected; optional (cleared on start of transfer operation)
} SPI_STATUS;

// SPI Information (Run-time)
typedef struct _SPI_INFO {
  ARM_SPI_SignalEvent_t cb_event;       // Event Callback
  SPI_STATUS            status;         // Status flags
  uint8_t               state;          // Current SPI state
  uint32_t              mode;           // Current SPI mode
} SPI_INFO;

// SPI Transfer Information (Run-Time)
typedef struct _SPI_TRANSFER_INFO {
  uint32_t              num;            // Total number of transfers
  uint8_t              *rx_buf;         // Pointer to in data buffer
  uint8_t              *tx_buf;         // Pointer to out data buffer
  uint32_t              rx_cnt;         // Number of data received
  uint32_t              tx_cnt;         // Number of data sent
  uint32_t              dump_val;       // Variable for dumping DMA data
  uint16_t              def_val;        // Default transfer value
} SPI_TRANSFER_INFO;


// SPI Resources definition
typedef struct {
  SPI_TypeDef          *reg;            // SPI peripheral pointer
  uint32_t              pclk;           // Peripheral clock frequency
  SPI_IO                io;             // SPI Input/Output pins
  IRQn_Type             irq_num;        // SPI IRQ Number
  SPI_DMA              *rx_dma;         // Receive stream register interface
  SPI_DMA              *tx_dma;         // Transmit stream register interface
  SPI_INFO             *info;           // Run-Time Information
  SPI_TRANSFER_INFO    *xfer;           // SPI transfer information
} SPI_RESOURCES;

#endif /* __SPI_STM32F10X_H */

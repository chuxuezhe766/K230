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
 * $Date:        26. October 2015
 * $Revision:    V2.2
 *
 * Driver:       Driver_SPI1, Driver_SPI2, Driver_SPI3
 * Configured:   via RTE_Device.h configuration file
 * Project:      SPI Driver for STMicroelectronics STM32F10x
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   SPI Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_SPI# = 1       use SPI1
 *   Connect to hardware via Driver_SPI# = 2       use SPI2
 *   Connect to hardware via Driver_SPI# = 3       use SPI3
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.2
 *    Corrected SPI Peripheral Reset and Clock enable/disable
 *      - Added checking if peripheral is available on selected device
 *  Version 2.1
 *    Corrected Bus Speed configuration
 *    Corrected 8bit/16bit Data register access, regarding the Data frame size
 *  Version 2.0
 *    Updated to CMSIS Driver API V2.01
 *  Version 1.3
 *    Event send_data_event added to capabilities
 *    SPI IRQ handling corrected
 *  Version 1.2
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.1
 *    Corrections for configuration without DMA
 *  Version 1.0
 *    Initial release
 */

#include "SPI_STM32F10x.h"

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,2)

// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = { ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION };

// Driver Capabilities
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
  0,  /* Simplex Mode (Master and Slave) */
  0,  /* TI Synchronous Serial Interface */
  0,  /* Microwire Interface */
  1   /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};


#ifdef MX_SPI1

// SPI1 Run-Time Information
static SPI_INFO          SPI1_Info         = { 0U };
static SPI_TRANSFER_INFO SPI1_TransferInfo = { 0U };

#ifdef MX_SPI1_MOSI_Pin
  static SPI_PIN SPI1_mosi = {MX_SPI1_MOSI_GPIOx, MX_SPI1_MOSI_GPIO_Pin};
#endif
#ifdef MX_SPI1_MISO_Pin
  static SPI_PIN SPI1_miso = {MX_SPI1_MISO_GPIOx, MX_SPI1_MISO_GPIO_Pin};
#endif
#ifdef MX_SPI1_NSS_Pin
  static SPI_PIN SPI1_nss  = {MX_SPI1_NSS_GPIOx,  MX_SPI1_NSS_GPIO_Pin};
#endif
  static SPI_PIN SPI1_sck  = {MX_SPI1_SCK_GPIOx,  MX_SPI1_SCK_GPIO_Pin};


#ifdef MX_SPI1_RX_DMA_Instance
static SPI_DMA SPI1_DMA_Rx = {
  MX_SPI1_RX_DMA_Instance,
  MX_SPI1_RX_DMA_Number,
  MX_SPI1_RX_DMA_Channel,
  MX_SPI1_RX_DMA_Priority
};
#endif

#ifdef MX_SPI1_TX_DMA_Instance
static SPI_DMA SPI1_DMA_Tx = {
  MX_SPI1_TX_DMA_Instance,
  MX_SPI1_TX_DMA_Number,
  MX_SPI1_TX_DMA_Channel,
  MX_SPI1_TX_DMA_Priority
};
#endif

// SPI1 Resources
static const SPI_RESOURCES SPI1_Resources = {
  SPI1,
  RTE_PCLK2,
  // PINS
  {
#ifdef MX_SPI1_MOSI_Pin
    &SPI1_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI1_MISO_Pin
    &SPI1_miso,
#else
    NULL,
#endif
#ifdef MX_SPI1_NSS_Pin 
    &SPI1_nss,
#else
    NULL,
#endif
    &SPI1_sck,
    MX_SPI1_REMAP_DEF,
    MX_SPI1_REMAP
  },

  SPI1_IRQn,

#ifdef MX_SPI1_RX_DMA_Instance
  &SPI1_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI1_TX_DMA_Instance
  &SPI1_DMA_Tx,
#else
  NULL,
#endif

  &SPI1_Info,
  &SPI1_TransferInfo
};
#endif /* MX_SPI1 */

#ifdef MX_SPI2

// SPI2 Run-Time Information
static SPI_INFO          SPI2_Info         = { 0U };
static SPI_TRANSFER_INFO SPI2_TransferInfo = { 0U };


#ifdef MX_SPI2_MOSI_Pin
  static SPI_PIN SPI2_mosi = {MX_SPI2_MOSI_GPIOx, MX_SPI2_MOSI_GPIO_Pin};
#endif
#ifdef MX_SPI2_MISO_Pin
  static SPI_PIN SPI2_miso = {MX_SPI2_MISO_GPIOx, MX_SPI2_MISO_GPIO_Pin};
#endif
#ifdef MX_SPI2_NSS_Pin
  static SPI_PIN SPI2_nss  = {MX_SPI2_NSS_GPIOx,  MX_SPI2_NSS_GPIO_Pin};
#endif
  static SPI_PIN SPI2_sck  = {MX_SPI2_SCK_GPIOx,  MX_SPI2_SCK_GPIO_Pin};

#ifdef MX_SPI2_RX_DMA_Instance
static SPI_DMA SPI2_DMA_Rx = {
  MX_SPI2_RX_DMA_Instance,
  MX_SPI2_RX_DMA_Number,
  MX_SPI2_RX_DMA_Channel,
  MX_SPI2_RX_DMA_Priority
};
#endif
#ifdef MX_SPI2_TX_DMA_Instance
static SPI_DMA SPI2_DMA_Tx = {
  MX_SPI2_TX_DMA_Instance,
  MX_SPI2_TX_DMA_Number,
  MX_SPI2_TX_DMA_Channel,
  MX_SPI2_TX_DMA_Priority
};
#endif

// SPI2 Resources
static const SPI_RESOURCES SPI2_Resources = {
  SPI2,
  RTE_PCLK1,
  // PINS
  {
#ifdef MX_SPI2_MOSI_Pin
    &SPI2_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI2_MISO_Pin
    &SPI2_miso,
#else
    NULL,
#endif
#ifdef MX_SPI2_NSS_Pin 
    &SPI2_nss,
#else
    NULL,
#endif
    &SPI2_sck,
    MX_SPI2_REMAP_DEF,
    MX_SPI2_REMAP
  },

  SPI2_IRQn,

#ifdef MX_SPI2_RX_DMA_Instance
  &SPI2_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI2_TX_DMA_Instance
  &SPI2_DMA_Tx,
#else
  NULL,
#endif

  &SPI2_Info,
  &SPI2_TransferInfo
};
#endif /* MX_SPI2 */

#ifdef MX_SPI3

// SPI3 Run-Time Information
static SPI_INFO          SPI3_Info         = { 0U };
static SPI_TRANSFER_INFO SPI3_TransferInfo = { 0U };


#ifdef MX_SPI3_MOSI_Pin
  static SPI_PIN SPI3_mosi = {MX_SPI3_MOSI_GPIOx, MX_SPI3_MOSI_GPIO_Pin};
#endif
#ifdef MX_SPI3_MISO_Pin
  static SPI_PIN SPI3_miso = {MX_SPI3_MISO_GPIOx, MX_SPI3_MISO_GPIO_Pin};
#endif
#ifdef MX_SPI3_NSS_Pin
  static SPI_PIN SPI3_nss  = {MX_SPI3_NSS_GPIOx,  MX_SPI3_NSS_GPIO_Pin};
#endif
  static SPI_PIN SPI3_sck  = {MX_SPI3_SCK_GPIOx,  MX_SPI3_SCK_GPIO_Pin};

#ifdef MX_SPI3_RX_DMA_Instance
static SPI_DMA SPI3_DMA_Rx = {
  MX_SPI3_RX_DMA_Instance,
  MX_SPI3_RX_DMA_Number,
  MX_SPI3_RX_DMA_Channel,
  MX_SPI3_RX_DMA_Priority
};
#endif
#ifdef MX_SPI3_TX_DMA_Instance
static SPI_DMA SPI3_DMA_Tx = {
  MX_SPI3_TX_DMA_Instance,
  MX_SPI3_TX_DMA_Number,
  MX_SPI3_TX_DMA_Channel,
  MX_SPI3_TX_DMA_Priority
  };
#endif

// SPI3 Resources
static const SPI_RESOURCES SPI3_Resources = {
  SPI3,
  RTE_PCLK1,
  // PINS
  {
#ifdef MX_SPI3_MOSI_Pin
    &SPI3_mosi,
#else
    NULL,
#endif
#ifdef MX_SPI3_MISO_Pin
    &SPI3_miso,
#else
    NULL,
#endif
#ifdef MX_SPI3_NSS_Pin 
    &SPI3_nss,
#else
    NULL,
#endif
    &SPI3_sck,
    MX_SPI3_REMAP_DEF,
    MX_SPI3_REMAP
  },

  SPI3_IRQn,

#ifdef MX_SPI3_RX_DMA_Instance
  &SPI3_DMA_Rx,
#else
  NULL,
#endif

#ifdef MX_SPI3_TX_DMA_Instance
  &SPI3_DMA_Tx,
#else
  NULL,
#endif

  &SPI3_Info,
  &SPI3_TransferInfo
};
#endif /* MX_SPI3 */


/**
  \fn          void SPI_PeripheralReset (const SPI_TypeDef *spi)
  \brief       SPI Reset
*/
static void SPI_PeripheralReset (const SPI_TypeDef *spi) {

  if (spi == SPI1) { RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST; }
#if !defined(STM32F10X_LD) && !defined(STM32F10X_LD_VL)
  if (spi == SPI2) { RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST; }
#endif
#if defined(STM32F10X_HD) || defined(STM32F10X_CL) || defined(STM32F10X_XL) || defined(STM32F10X_HD_VL)
  if (spi == SPI3) { RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST; }
#endif

  __NOP(); __NOP(); __NOP(); __NOP();

  if (spi == SPI1) { RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST; }
#if !defined(STM32F10X_LD) && !defined(STM32F10X_LD_VL)
  if (spi == SPI2) { RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST; }
#endif
#if defined(STM32F10X_HD) || defined(STM32F10X_CL) || defined(STM32F10X_XL) || defined(STM32F10X_HD_VL)
  if (spi == SPI3) { RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST; }
#endif
}

/**
  \fn          ARM_DRIVER_VERSION SPIX_GetVersion (void)
  \brief       Get SPI driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION SPIX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_SPI_CAPABILITIES SPI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_SPI_CAPABILITIES
*/
static ARM_SPI_CAPABILITIES SPIX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t SPI_Initialize (ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi)
  \brief       Initialize SPI Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SPI_SignalEvent
  \param[in]   spi       Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Initialize (ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi) {
  bool ok = true;

  if (spi->info->state & SPI_INITIALIZED) { return ARM_DRIVER_OK; }

  // Initialize SPI Run-Time Resources
  spi->info->cb_event = cb_event;
  spi->info->status.busy       = 0U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Clear transfer information
  memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));

  // Setup pin remap
  GPIO_AFConfigure(spi->io.afio);

  // Configure SPI SCK pin
  GPIO_PortClock (spi->io.sck->port, true);
  ok = GPIO_PinConfigure(spi->io.sck->port, spi->io.sck->pin, GPIO_AF_PUSHPULL,
                                                              GPIO_MODE_OUT50MHZ);
  if (ok && (spi->io.mosi != NULL)) {
    // Configure SPI MOSI pin
    GPIO_PortClock (spi->io.mosi->port, true);
    ok = GPIO_PinConfigure(spi->io.mosi->port, spi->io.mosi->pin, GPIO_AF_PUSHPULL,
                                                                  GPIO_MODE_OUT50MHZ);
  }
  if (ok && (spi->io.miso != NULL)) {
    // Configure SPI MISO pin
    GPIO_PortClock (spi->io.miso->port, true);
    ok = GPIO_PinConfigure(spi->io.miso->port, spi->io.miso->pin, GPIO_AF_PUSHPULL,
                                                                  GPIO_MODE_INPUT);
  }

#ifdef __SPI_DMA
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    if (spi->rx_dma != NULL) {
      DMA_ChannelInitialize (spi->rx_dma->dma_num, spi->rx_dma->ch_num);
    }
    if (spi->tx_dma != NULL) {
      DMA_ChannelInitialize (spi->tx_dma->dma_num, spi->tx_dma->ch_num);
    }
  }
#endif

  spi->info->state = SPI_INITIALIZED;

  return (ok) ? (ARM_DRIVER_OK) : (ARM_DRIVER_ERROR);
}

/**
  \fn          int32_t SPI_Uninitialize (const SPI_RESOURCES *spi)
  \brief       De-initialize SPI Interface.
  \param[in]   spi  Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Uninitialize (const SPI_RESOURCES *spi) {

  // Uninitialize SPI pins
  GPIO_PinConfigure (spi->io.sck->port, spi->io.sck->pin, GPIO_IN_ANALOG, GPIO_MODE_INPUT);

  if (spi->io.miso != NULL) {
    GPIO_PinConfigure (spi->io.miso->port, spi->io.miso->pin, GPIO_IN_ANALOG, GPIO_MODE_INPUT);
  }
  if (spi->io.mosi != NULL) {
    GPIO_PinConfigure (spi->io.mosi->port, spi->io.mosi->pin, GPIO_IN_ANALOG, GPIO_MODE_INPUT);
  }
  if (spi->io.nss != NULL) {
    GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_IN_ANALOG, GPIO_MODE_INPUT);
  }

  // Uncofigure pin remapping
  GPIO_AFConfigure(spi->io.afio_def);

#ifdef __SPI_DMA
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    if (spi->rx_dma != NULL) {
      DMA_ChannelUninitialize (spi->rx_dma->dma_num, spi->rx_dma->ch_num);
    }
    if (spi->tx_dma != NULL) {
      DMA_ChannelUninitialize (spi->tx_dma->dma_num, spi->tx_dma->ch_num);
    }
  }
#endif

  // Clear SPI state
  spi->info->state = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_PowerControl (ARM_POWER_STATE state, const SPI_RESOURCES *spi)
  \brief       Control SPI Interface Power.
  \param[in]   state  Power state
  \param[in]   spi    Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_PowerControl (ARM_POWER_STATE state, const SPI_RESOURCES *spi) {

  switch (state) {
    case ARM_POWER_OFF:
      // SPI peripheral reset
      SPI_PeripheralReset (spi->reg);

      NVIC_DisableIRQ (spi->irq_num);

      // Disable SPI clock
      if (spi->reg == SPI1) { RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN; }
#if !defined(STM32F10X_LD) && !defined(STM32F10X_LD_VL)
      if (spi->reg == SPI2) { RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN; }
#endif
#if defined(STM32F10X_HD) || defined(STM32F10X_CL) || defined(STM32F10X_XL) || defined(STM32F10X_HD_VL)
      if (spi->reg == SPI3) { RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN; }
#endif

      // Clear status flags
      spi->info->status.busy       = 0U;
      spi->info->status.data_lost  = 0U;
      spi->info->status.mode_fault = 0U;

      // Clear powered flag
      spi->info->state &= ~SPI_POWERED;
      break;

    case ARM_POWER_FULL:
      if ((spi->info->state & SPI_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((spi->info->state & SPI_POWERED)     != 0U) {
        return ARM_DRIVER_OK;
      }

      // Clear status flags
      spi->info->status.busy       = 0U;
      spi->info->status.data_lost  = 0U;
      spi->info->status.mode_fault = 0U;

      spi->xfer->def_val           = 0U;

      // Ready for operation - set powered flag
      spi->info->state |= SPI_POWERED;

      // Enable SPI clock
      if (spi->reg == SPI1) { RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; }
#if !defined(STM32F10X_LD) && !defined(STM32F10X_LD_VL)
      if (spi->reg == SPI2) { RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; }
#endif
#if defined(STM32F10X_HD) || defined(STM32F10X_CL) || defined(STM32F10X_XL) || defined(STM32F10X_HD_VL)
      if (spi->reg == SPI3) { RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; }
#endif

      // SPI peripheral reset
      SPI_PeripheralReset (spi->reg);

      // Clear and Enable SPI IRQ
      NVIC_ClearPendingIRQ(spi->irq_num);
      NVIC_EnableIRQ(spi->irq_num);
      break;

    case ARM_POWER_LOW:
    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Send (const void *data, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start sending data to SPI transmitter.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter
  \param[in]   num   Number of data items to send
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Send (const void *data, uint32_t num, const SPI_RESOURCES *spi) {
#ifdef __SPI_DMA
  uint32_t cfg;
#endif

  if ((data == NULL) || (num == 0U))             { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((spi->info->state & SPI_CONFIGURED) == 0U) { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                   { return ARM_DRIVER_ERROR_BUSY; }

  // Check if transmit pin available
  if ((((spi->io.mosi != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER)) ||
       ((spi->io.miso != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE ))) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  // Update SPI statuses
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Save transfer info
  spi->xfer->rx_buf = NULL;
  spi->xfer->tx_buf = (uint8_t *)data;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

#ifdef __SPI_DMA_RX
  if (spi->rx_dma != NULL) {
    // Configure and enable rx DMA channel
    cfg = ((spi->rx_dma->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
          DMA_PERIPHERAL_TO_MEMORY       |
          DMA_TRANSFER_COMPLETE_INTERRUPT;

    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_16BIT | DMA_MEMORY_DATA_16BIT;
    } else {
      //  8 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_8BIT | DMA_MEMORY_DATA_8BIT;
    }

    DMA_ChannelConfigure(spi->rx_dma->instance,
                         cfg,
                         (uint32_t)(&spi->reg->DR),
                         (uint32_t)(&spi->xfer->dump_val),
                         num);
    DMA_ChannelEnable(spi->rx_dma->instance);

    // RX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_RXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // RX Buffer not empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_RXNEIE;
  }

#ifdef __SPI_DMA_TX
  if (spi->tx_dma != NULL) {
    cfg = ((spi->tx_dma->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
          DMA_READ_MEMORY                 |
          DMA_MEMORY_INCREMENT            |
          DMA_TRANSFER_COMPLETE_INTERRUPT ;

    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_16BIT | DMA_MEMORY_DATA_16BIT;
    } else {
      //  8 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_8BIT | DMA_MEMORY_DATA_8BIT;
    }

    DMA_ChannelConfigure(spi->tx_dma->instance,
                         cfg,
                         (uint32_t)&spi->reg->DR,
                         (uint32_t)data,
                         num);
    DMA_ChannelEnable(spi->tx_dma->instance);

    // TX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_TXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // TX Buffer empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_TXEIE;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Receive (void *data, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start receiving data from SPI receiver.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   num   Number of data items to receive
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Receive (void *data, uint32_t num, const SPI_RESOURCES *spi) {
#ifdef __SPI_DMA
  uint32_t cfg;
#endif

  if ((data == NULL) || (num == 0U))             { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((spi->info->state & SPI_CONFIGURED) == 0U) { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                   { return ARM_DRIVER_ERROR_BUSY; }

  // Check if receive pin available
  if ((((spi->io.miso != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER)) ||
       ((spi->io.mosi != NULL) && ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE ))) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  // Update SPI statuses
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Save transfer info
  spi->xfer->rx_buf = (uint8_t *)data;
  spi->xfer->tx_buf = NULL;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

#ifdef __SPI_DMA_RX
  // DMA mode
  if (spi->rx_dma != NULL) {
    cfg = ((spi->rx_dma->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
          DMA_PERIPHERAL_TO_MEMORY        |
          DMA_MEMORY_INCREMENT            |
          DMA_TRANSFER_COMPLETE_INTERRUPT ;

    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_16BIT | DMA_MEMORY_DATA_16BIT;
    } else {
      //  8 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_8BIT | DMA_MEMORY_DATA_8BIT;
    }

    DMA_ChannelConfigure(spi->rx_dma->instance,
                         cfg,
                         (uint32_t)(&spi->reg->DR),
                         (uint32_t)data,
                         num);
    DMA_ChannelEnable(spi->rx_dma->instance);

    // RX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_RXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // RX Buffer not empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_RXNEIE;
  }

#ifdef __SPI_DMA_TX
  // DMA mode
  if (spi->tx_dma != NULL) {
    cfg = ((spi->tx_dma->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
          DMA_READ_MEMORY                |
          DMA_TRANSFER_COMPLETE_INTERRUPT;

    if (spi->reg->CR1 & SPI_CR1_DFF) {
      // 16 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_16BIT | DMA_MEMORY_DATA_16BIT;
    } else {
      //  8 - bit data frame
      cfg |= DMA_PERIPHERAL_DATA_8BIT | DMA_MEMORY_DATA_8BIT;
    }

    DMA_ChannelConfigure(spi->tx_dma->instance,
                         cfg,
                         (uint32_t)(&spi->reg->DR),
                         (uint32_t)(&spi->xfer->def_val),
                         num);
    DMA_ChannelEnable(spi->tx_dma->instance);

    // TX Buffer DMA enable
    spi->reg->CR2 |= SPI_CR2_TXDMAEN;
  } else
#endif
  {
    // Interrupt mode
    // TX buffer empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_TXEIE;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start sending/receiving data to/from SPI transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   spi       Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t num, const SPI_RESOURCES *spi) {
#ifdef __SPI_DMA
  uint32_t cfg;
#endif

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((spi->info->state & SPI_CONFIGURED) == 0U)              { return ARM_DRIVER_ERROR; }
  if ( spi->info->status.busy)                                { return ARM_DRIVER_ERROR_BUSY; }

  // Check if receive and transmit pins available
  if ((spi->io.miso == NULL) || (spi->io.mosi == NULL)) {
    return ARM_DRIVER_ERROR;
  }

  // Update SPI statuses
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Save transfer info
  spi->xfer->rx_buf = (uint8_t *)data_in;
  spi->xfer->tx_buf = (uint8_t *)data_out;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

#ifdef __SPI_DMA
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    // DMA mode

    if (spi->rx_dma != NULL) {
      cfg = ((spi->rx_dma->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
            DMA_PERIPHERAL_TO_MEMORY       |
            DMA_MEMORY_INCREMENT           |
            DMA_TRANSFER_COMPLETE_INTERRUPT;

      if (spi->reg->CR1 & SPI_CR1_DFF) {
        // 16 - bit data frame
        cfg |= DMA_PERIPHERAL_DATA_16BIT | DMA_MEMORY_DATA_16BIT;
      } else {
        //  8 - bit data frame
        cfg |= DMA_PERIPHERAL_DATA_8BIT | DMA_MEMORY_DATA_8BIT;
      }

      DMA_ChannelConfigure(spi->rx_dma->instance,
                          cfg,
                          (uint32_t)(&spi->reg->DR),
                          (uint32_t)data_in,
                          num);
      DMA_ChannelEnable(spi->rx_dma->instance);

      // RX Buffer DMA enable
      spi->reg->CR2 |= SPI_CR2_RXDMAEN;
    }

    if (spi->tx_dma != NULL) {
      cfg = ((spi->tx_dma->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
            DMA_READ_MEMORY                |
            DMA_MEMORY_INCREMENT           |
            DMA_TRANSFER_COMPLETE_INTERRUPT;

      DMA_ChannelConfigure(spi->tx_dma->instance,
                           cfg,
                           (uint32_t)(&spi->reg->DR),
                           (uint32_t)data_out,
                           num);
      DMA_ChannelEnable(spi->tx_dma->instance);

      // TX Buffer DMA enable
      spi->reg->CR2 |= SPI_CR2_TXDMAEN;
    }
  } else
#endif
  {
    // Interrupt mode
    // TX Buffer empty and RX Buffer not empty interrupt enable
    spi->reg->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_TXEIE;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t SPI_GetDataCount (const SPI_RESOURCES *spi)
  \brief       Get transferred data count.
  \param[in]   spi  Pointer to SPI resources
  \return      number of data items transferred
*/
static uint32_t SPI_GetDataCount (const SPI_RESOURCES *spi) {
  return (spi->xfer->rx_cnt);
}

/**
  \fn          int32_t SPI_Control (uint32_t control, uint32_t arg, const SPI_RESOURCES *spi)
  \brief       Control SPI Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \param[in]   spi      Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Control (uint32_t control, uint32_t arg, const SPI_RESOURCES *spi) {
  uint32_t           mode, val, pclk;
  uint32_t           cr1, cr2;

  mode  = 0U;
  val   = 0U;
  cr1   = 0U;
  cr2   = 0U;

  if ((spi->info->state & SPI_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER) {
    // Send abort
    if (spi->tx_dma != NULL) {
      // DMA mode
      // TX buffer DMA disable
      spi->reg->CR2 &= ~SPI_CR2_TXDMAEN;

      // Disable TX DMA transfer
      DMA_ChannelDisable (spi->tx_dma->instance);
    } else {
      // Interrupt mode
      // Disable TX buffer empty interrupt
      spi->reg->CR2 &= ~SPI_CR2_TXEIE;
    }

    // Receive abort
    if (spi->rx_dma != NULL) {
      // DMA mode
      // RX buffer DMA disable
      spi->reg->CR2 &= ~SPI_CR2_RXDMAEN;

      // Disable RX DMA transfer
      DMA_ChannelDisable (spi->rx_dma->instance);
    } else {
      // Interrupt mode
      // Disable RX buffer not empty interrupt
      spi->reg->CR2 &= ~SPI_CR2_RXNEIE;
    }

    memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));
    spi->info->status.busy = 0U;
    return ARM_DRIVER_OK;
  }

  // Check for busy flag
  if (spi->info->status.busy) { return ARM_DRIVER_ERROR_BUSY; }

  switch (control & ARM_SPI_CONTROL_Msk) {
    case ARM_SPI_MODE_INACTIVE:
      mode |= ARM_SPI_MODE_INACTIVE;
      break;

    case ARM_SPI_MODE_MASTER:
      mode |= ARM_SPI_MODE_MASTER;

      // Master enabled
      cr1 |= SPI_CR1_MSTR;
      break;

    case ARM_SPI_MODE_SLAVE:
      mode |= ARM_SPI_MODE_SLAVE;
      break;

    case ARM_SPI_MODE_MASTER_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_MODE_SLAVE_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_SET_BUS_SPEED:
      // Set SPI Bus Speed 
      pclk = spi->pclk;
      for (val = 0U; val < 8U; val++) {
        if (arg >= (pclk >> (val + 1U))) { break; }
      }
      if ((val == 8U) || (arg < (pclk >> (val + 1U)))) {
        // Requested Bus Speed can not be configured
        return ARM_DRIVER_ERROR;
      }
      // Disable SPI, update prescaler and enable SPI
      spi->reg->CR1 &= ~SPI_CR1_SPE;
      spi->reg->CR1  =  (spi->reg->CR1 & ~SPI_CR1_BR) | (val << 3U);
      spi->reg->CR1 |=  SPI_CR1_SPE;
      return ARM_DRIVER_OK;

    case ARM_SPI_GET_BUS_SPEED:
      // Return current bus speed
      return (spi->pclk >> (((spi->reg->CR1 & SPI_CR1_BR) >> 3U) + 1U));

    case ARM_SPI_SET_DEFAULT_TX_VALUE:
      spi->xfer->def_val = (uint16_t)(arg & 0xFFFFU);
      return ARM_DRIVER_OK;

    case ARM_SPI_CONTROL_SS:
      val = (spi->info->mode & ARM_SPI_CONTROL_Msk);
      // Master modes
      if (val == ARM_SPI_MODE_MASTER) {
        val = spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk;
        // Check if NSS pin is available and
        // software slave select master is selected
        if ((spi->io.nss != NULL) && (val == ARM_SPI_SS_MASTER_SW)) {
          // Set/Clear NSS pin
          if (arg == ARM_SPI_SS_INACTIVE)
            GPIO_PinWrite (spi->io.nss->port, spi->io.nss->pin, 1);
          else
            GPIO_PinWrite (spi->io.nss->port, spi->io.nss->pin, 0);
        } else return ARM_DRIVER_ERROR;
        return ARM_DRIVER_OK;
      }
      // Slave modes
      else if (val == ARM_SPI_MODE_SLAVE) {
        val = spi->info->mode & ARM_SPI_SS_SLAVE_MODE_Msk;
        // Check if slave select slave mode is selected
        if (val == ARM_SPI_SS_MASTER_SW) {
          if (arg == ARM_SPI_SS_ACTIVE) {
            spi->reg->CR1 |= SPI_CR1_SSI;
          }
          else {
            spi->reg->CR1 &= ~SPI_CR1_SSI;
          }
          return ARM_DRIVER_OK;
        } else { return ARM_DRIVER_ERROR; }
      } else { return ARM_DRIVER_ERROR; }

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // Frame format:
  switch (control & ARM_SPI_FRAME_FORMAT_Msk) {
    case ARM_SPI_CPOL0_CPHA0:
      break;
    case ARM_SPI_CPOL0_CPHA1:
     cr1 |= SPI_CR1_CPHA;
      break;
    case ARM_SPI_CPOL1_CPHA0:
      cr1 |= SPI_CR1_CPOL;
      break;
    case ARM_SPI_CPOL1_CPHA1:
      cr1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
      break;
    case ARM_SPI_TI_SSI:
    case ARM_SPI_MICROWIRE:
    default:
      return ARM_SPI_ERROR_FRAME_FORMAT;
  }

  // Data Bits
  switch (control & ARM_SPI_DATA_BITS_Msk) {
    case ARM_SPI_DATA_BITS(8U):
      break;
    case ARM_SPI_DATA_BITS(16U):
      cr1 |= SPI_CR1_DFF;
      break;
    default: return ARM_SPI_ERROR_DATA_BITS;
  }

  // Bit order
  if ((control & ARM_SPI_BIT_ORDER_Msk) == ARM_SPI_LSB_MSB) {
    cr1 |= SPI_CR1_LSBFIRST;
  }

  // Slave select master modes
  if (mode == ARM_SPI_MODE_MASTER) {
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk) {
      case ARM_SPI_SS_MASTER_UNUSED:
        if (spi->io.nss != NULL) {
          // Unconfigure NSS pin
          GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_IN_ANALOG,
                                                                  GPIO_MODE_INPUT);
        }
        // Software slave management
        // Internal NSS always active, IO value is ignored
        cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;
        mode |= ARM_SPI_SS_MASTER_UNUSED;
        break;

      case ARM_SPI_SS_MASTER_HW_INPUT:
        if (spi->io.nss) {
          // Configure NSS pin
          GPIO_PortClock (spi->io.nss->port, true);

          GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_AF_PUSHPULL,
                                                                  GPIO_MODE_OUT50MHZ);
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        mode |= ARM_SPI_SS_MASTER_HW_INPUT;
        break;

      case ARM_SPI_SS_MASTER_SW:
        if (spi->io.nss) {
          // Configure NSS pin as GPIO output
          GPIO_PortClock (spi->io.nss->port, true);

          GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_OUT_PUSH_PULL,
                                                                  GPIO_MODE_OUT50MHZ);
          // Software slave management
          cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;

          mode |= ARM_SPI_SS_MASTER_SW;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_MASTER_HW_OUTPUT:
        if (spi->io.nss) {
          // Configure NSS pin - SPI NSS alternative function
          GPIO_PortClock (spi->io.nss->port, true);

          GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_AF_PUSHPULL,
                                                                  GPIO_MODE_OUT50MHZ);
          // Slave select output enable
          cr2 |= SPI_CR2_SSOE;

          mode |= ARM_SPI_SS_MASTER_HW_OUTPUT;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;
        default: return ARM_SPI_ERROR_SS_MODE;
    }
  }

  // Slave select slave modes
  if (mode ==  ARM_SPI_MODE_SLAVE) {
    switch (control & ARM_SPI_SS_SLAVE_MODE_Msk) {
      case ARM_SPI_SS_SLAVE_HW:
        if (spi->io.nss) {
          // Configure NSS pin - SPI NSS alternative function
          GPIO_PortClock (spi->io.nss->port, true);

          GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_AF_PUSHPULL,
                                                                  GPIO_MODE_OUT50MHZ);
          mode |= ARM_SPI_SS_SLAVE_HW;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_SLAVE_SW:
        if (spi->io.nss) {
          // Unconfigure NSS pin
          GPIO_PinConfigure (spi->io.nss->port, spi->io.nss->pin, GPIO_IN_ANALOG,
                                                                  GPIO_MODE_INPUT);
        }
        // Enable software slave management
        cr1 |= SPI_CR1_SSM;
        mode |= ARM_SPI_SS_SLAVE_SW;
        break;
      default: return ARM_SPI_ERROR_SS_MODE;
    }
  }

  // Set SPI Bus Speed 
  pclk = spi->pclk;
  for (val = 0U; val < 8U; val++) {
    if (arg >= (pclk >> (val + 1U))) break;
  }
  if ((val == 8U) || (arg < (pclk >> (val + 1U)))) {
    // Requested Bus Speed can not be configured
    return ARM_DRIVER_ERROR;
  }
  // Save prescaler value
  cr1 |= (val << 3U);

  spi->info->mode = mode;

  // Configure registers
  spi->reg->CR1 &= ~SPI_CR1_SPE;
  spi->reg->CR2  = cr2 | SPI_CR2_ERRIE;
  spi->reg->CR1  = cr1;

  if ((mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_INACTIVE) {
    spi->info->state &= ~SPI_CONFIGURED;
  } else {
    spi->info->state |=  SPI_CONFIGURED;
  }

  // Enable SPI
  spi->reg->CR1 |= SPI_CR1_SPE;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_SPI_STATUS SPI_GetStatus (const SPI_RESOURCES *spi)
  \brief       Get SPI status.
  \param[in]   spi  Pointer to SPI resources
  \return      SPI status \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_GetStatus (const SPI_RESOURCES *spi) {
  ARM_SPI_STATUS status;

  status.busy       = spi->info->status.busy;
  status.data_lost  = spi->info->status.data_lost;
  status.mode_fault = spi->info->status.mode_fault;

  return status;
}

/*SPI IRQ Handler */
void SPI_IRQHandler (const SPI_RESOURCES *spi) {
  uint8_t  data_8bit;
  uint16_t data_16bit, sr;
  uint32_t event;

  // Save status register
  sr = spi->reg->SR;

  event = 0U;

  if ((sr & SPI_SR_OVR) != 0U) {
    // Clear Overrun flag by reading data and status register
    if ((spi->reg->CR1 & SPI_CR1_DFF) == 0U) {
      // 8-bit data frame
      data_8bit = *(volatile uint8_t *)(&spi->reg->DR);
      if (spi->xfer->rx_cnt < spi->xfer->num) {
        if (spi->xfer->rx_buf != NULL) {
          *(spi->xfer->rx_buf++) = data_8bit;
        }
      }
    } else {
      // 16-bit data frame
      data_16bit = *(volatile uint16_t *)(&spi->reg->DR);
      if (spi->xfer->rx_cnt < spi->xfer->num) {
        if (spi->xfer->rx_buf != NULL) {
          *(spi->xfer->rx_buf++) = (uint8_t) data_16bit;
          *(spi->xfer->rx_buf++) = (uint8_t)(data_16bit >> 8U);
        }
      }
    }
    spi->xfer->rx_cnt++;
    sr = spi->reg->SR;

    spi->info->status.data_lost = 1U;
    event |=ARM_SPI_EVENT_DATA_LOST;
  }
  if ((sr & SPI_SR_UDR) != 0U) {
    // Underrun flag is set
    spi->info->status.data_lost = 1U;
    event |= ARM_SPI_EVENT_DATA_LOST;
  }
  if ((sr & SPI_SR_MODF) != 0U) {
    // Mode fault flag is set
    spi->info->status.mode_fault = 1U;

    // Write CR1 register to clear MODF flag
    spi->reg->CR1 = spi->reg->CR1;
    event |= ARM_SPI_EVENT_MODE_FAULT;
  }

  if (((sr & SPI_SR_RXNE) != 0U) && ((spi->reg->CR2 & SPI_CR2_RXNEIE) != 0U)) {
    // Receive Buffer Not Empty

    if (spi->xfer->rx_cnt < spi->xfer->num) {
      if ((spi->reg->CR1 & SPI_CR1_DFF) == 0U) {
        // 8-bit data frame
        data_8bit = *(volatile uint8_t *)(&spi->reg->DR);
        if (spi->xfer->rx_buf != NULL) {
          *(spi->xfer->rx_buf++) = data_8bit;
        }
      } else {
        // 16-bit data frame
        data_16bit = *(volatile uint16_t *)(&spi->reg->DR);
        if (spi->xfer->rx_buf != NULL) {
          *(spi->xfer->rx_buf++) = (uint8_t) data_16bit;
          *(spi->xfer->rx_buf++) = (uint8_t)(data_16bit >> 8U);
        }
      }

      spi->xfer->rx_cnt++;

      if (spi->xfer->rx_cnt == spi->xfer->num) {

        // Disable RX Buffer Not Empty Interrupt
        spi->reg->CR2 &= ~SPI_CR2_RXNEIE;

        // Clear busy flag
        spi->info->status.busy = 0U;

        // Transfer completed
        event |= ARM_SPI_EVENT_TRANSFER_COMPLETE;
      }
    }
    else {
      // Unexpected transfer, data lost
      event |= ARM_SPI_EVENT_DATA_LOST;
    }
  }

  if (((sr & SPI_SR_TXE) != 0U) && ((spi->reg->CR2 & SPI_CR2_TXEIE) != 0U)) {
    if (spi->xfer->tx_cnt < spi->xfer->num) {
      if ((spi->reg->CR1 & SPI_CR1_DFF) == 0U) {
        if (spi->xfer->tx_buf != NULL) {
          data_8bit = *(spi->xfer->tx_buf++);
        } else {
          data_8bit = (uint8_t)spi->xfer->def_val;
        }
        // Write data to data register
        *(volatile uint8_t *)(&spi->reg->DR) = data_8bit;
      } else {
        if (spi->xfer->tx_buf != NULL) {
          data_16bit  = *(spi->xfer->tx_buf++);
          data_16bit |= *(spi->xfer->tx_buf++) << 8U;
        } else {
          data_16bit  = (uint16_t)spi->xfer->def_val;
        }
        // Write data to data register
        *(volatile uint16_t *)(&spi->reg->DR) = data_16bit;
      }

      spi->xfer->tx_cnt++;

      if (spi->xfer->tx_cnt == spi->xfer->num) {
        // All data sent, disable TX Buffer Empty Interrupt
        spi->reg->CR2 &= ~SPI_CR2_TXEIE;
      }
    } else {
      // Unexpected transfer, data lost
      event |= ARM_SPI_EVENT_DATA_LOST;
    }
  }

  // Send event
  if ((event != 0U) && ((spi->info->cb_event != NULL))) {
    spi->info->cb_event(event);
  }
}

#ifdef __SPI_DMA_TX
void SPI_TX_DMA_Complete(uint32_t events, const SPI_RESOURCES *spi) {

  DMA_ChannelDisable(spi->tx_dma->instance);
  
  if ((DMA_ChannelTransferItemCount(spi->tx_dma->instance) != 0) && (spi->xfer->num != 0)) {
    // TX DMA Complete caused by transfer abort
    return;
  }

  spi->xfer->tx_cnt = spi->xfer->num;
}
#endif

#ifdef __SPI_DMA_RX
void SPI_RX_DMA_Complete(uint32_t events, const SPI_RESOURCES *spi) {

  DMA_ChannelDisable(spi->rx_dma->instance);

  if ((DMA_ChannelTransferItemCount(spi->rx_dma->instance) != 0) && (spi->xfer->num != 0)) {
    // RX DMA Complete caused by transfer abort
    return;
  }

  spi->xfer->rx_cnt = spi->xfer->num;

  spi->info->status.busy = 0U;
  if (spi->info->cb_event != NULL) {
    spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
  }
}
#endif

// SPI1
#ifdef MX_SPI1
static int32_t        SPI1_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI1_Resources); }
static int32_t        SPI1_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI1_Resources); }
static int32_t        SPI1_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI1_Resources); }
static int32_t        SPI1_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI1_Resources); }
static int32_t        SPI1_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI1_Resources); }
static int32_t        SPI1_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI1_Resources); }
static uint32_t       SPI1_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI1_Resources); }
static int32_t        SPI1_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI1_Resources); }
static ARM_SPI_STATUS SPI1_GetStatus           (void)                                              { return SPI_GetStatus (&SPI1_Resources); }
       void           SPI1_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI1_Resources); }

#ifdef MX_SPI1_TX_DMA_Instance
      void            SPI1_TX_DMA_Handler      (uint32_t events)                                   {        SPI_TX_DMA_Complete (events, &SPI1_Resources); }
#endif
#ifdef MX_SPI1_RX_DMA_Instance
      void            SPI1_RX_DMA_Handler      (uint32_t events)                                   {        SPI_RX_DMA_Complete (events, &SPI1_Resources); }
#endif

ARM_DRIVER_SPI Driver_SPI1 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI1_Initialize,
  SPI1_Uninitialize,
  SPI1_PowerControl,
  SPI1_Send,
  SPI1_Receive,
  SPI1_Transfer,
  SPI1_GetDataCount,
  SPI1_Control,
  SPI1_GetStatus
};
#endif

// SPI2
#ifdef MX_SPI2
static int32_t        SPI2_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI2_Resources); }
static int32_t        SPI2_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI2_Resources); }
static int32_t        SPI2_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI2_Resources); }
static int32_t        SPI2_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI2_Resources); }
static int32_t        SPI2_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI2_Resources); }
static int32_t        SPI2_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI2_Resources); }
static uint32_t       SPI2_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI2_Resources); }
static int32_t        SPI2_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI2_Resources); }
static ARM_SPI_STATUS SPI2_GetStatus           (void)                                              { return SPI_GetStatus (&SPI2_Resources); }
       void           SPI2_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI2_Resources); }

#ifdef MX_SPI2_TX_DMA_Instance
      void            SPI2_TX_DMA_Handler      (uint32_t events)                                   {        SPI_TX_DMA_Complete (events, &SPI2_Resources); }
#endif
#ifdef MX_SPI2_RX_DMA_Instance
      void            SPI2_RX_DMA_Handler      (uint32_t events)                                   {        SPI_RX_DMA_Complete (events, &SPI2_Resources); }
#endif

ARM_DRIVER_SPI Driver_SPI2 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI2_Initialize,
  SPI2_Uninitialize,
  SPI2_PowerControl,
  SPI2_Send,
  SPI2_Receive,
  SPI2_Transfer,
  SPI2_GetDataCount,
  SPI2_Control,
  SPI2_GetStatus
};
#endif

// SPI3
#ifdef MX_SPI3
static int32_t        SPI3_Initialize          (ARM_SPI_SignalEvent_t pSignalEvent)                { return SPI_Initialize (pSignalEvent, &SPI3_Resources); }
static int32_t        SPI3_Uninitialize        (void)                                              { return SPI_Uninitialize (&SPI3_Resources); }
static int32_t        SPI3_PowerControl        (ARM_POWER_STATE state)                             { return SPI_PowerControl (state, &SPI3_Resources); }
static int32_t        SPI3_Send                (const void *data, uint32_t num)                    { return SPI_Send (data, num, &SPI3_Resources); }
static int32_t        SPI3_Receive             (void *data, uint32_t num)                          { return SPI_Receive (data, num, &SPI3_Resources); }
static int32_t        SPI3_Transfer            (const void *data_out, void *data_in, uint32_t num) { return SPI_Transfer (data_out, data_in, num, &SPI3_Resources); }
static uint32_t       SPI3_GetDataCount        (void)                                              { return SPI_GetDataCount (&SPI3_Resources); }
static int32_t        SPI3_Control             (uint32_t control, uint32_t arg)                    { return SPI_Control (control, arg, &SPI3_Resources); }
static ARM_SPI_STATUS SPI3_GetStatus           (void)                                              { return SPI_GetStatus (&SPI3_Resources); }
       void           SPI3_IRQHandler          (void)                                              {        SPI_IRQHandler (&SPI3_Resources); }

#ifdef MX_SPI3_TX_DMA_Instance
      void            SPI3_TX_DMA_Handler      (uint32_t events)                                   {        SPI_TX_DMA_Complete (events, &SPI3_Resources); }
#endif
#ifdef MX_SPI3_RX_DMA_Instance
      void            SPI3_RX_DMA_Handler      (uint32_t events)                                   {        SPI_RX_DMA_Complete (events, &SPI3_Resources); }
#endif

ARM_DRIVER_SPI Driver_SPI3 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI3_Initialize,
  SPI3_Uninitialize,
  SPI3_PowerControl,
  SPI3_Send,
  SPI3_Receive,
  SPI3_Transfer,
  SPI3_GetDataCount,
  SPI3_Control,
  SPI3_GetStatus
};
#endif

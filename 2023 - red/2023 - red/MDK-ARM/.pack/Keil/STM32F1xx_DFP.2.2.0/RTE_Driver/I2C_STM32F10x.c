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
 * $Date:        21. October 2015
 * $Revision:    V2.1
 *
 * Driver:       Driver_I2C1, Driver_I2C2
 * Configured:   via RTE_Device.h configuration file
 * Project:      I2C Driver for STMicroelectronics STM32F10x
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   I2C Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_I2C# = 1       use I2C1
 *   Connect to hardware via Driver_I2C# = 2       use I2C2
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.1
 *    Corrected invalid __I2C_DMA field in I2C_DMA_TxEvent and I2C_DMA_RxEvent functions (corrected from instance into reg)
 *  Version 2.0
 *    Updated to the CMSIS Driver API V2.02
 *  Version 1.2
 *    Bugfix (corrected I2C register access)
 *  Version 1.1
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.0
 *    Initial release
 */


#include "I2C_STM32F10x.h"

#define ARM_I2C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,1)    /* driver version */


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_I2C_API_VERSION,
  ARM_I2C_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_I2C_CAPABILITIES DriverCapabilities = { 0U };


#if defined(USE_I2C1)
/* Function prototypes */
void I2C1_EV_IRQHandler (void);
void I2C1_ER_IRQHandler (void);

/* I2C1 DMA */
#if defined(I2C1_RX_DMA_Instance) && defined(I2C1_TX_DMA_Instance)
static const I2C_DMA I2C1_RX_DMA = {
  I2C1_RX_DMA_Instance,
  I2C1_RX_DMA_Number,
  I2C1_RX_DMA_Channel,
  I2C1_RX_DMA_Priority
};
static const I2C_DMA I2C1_TX_DMA = {
  I2C1_TX_DMA_Instance,
  I2C1_TX_DMA_Number,
  I2C1_TX_DMA_Channel,
  I2C1_TX_DMA_Priority
};
#endif

/* I2C1 Information (Run-Time) */
static I2C_INFO I2C1_Info;

/* I2C1 Resources */
static I2C_RESOURCES I2C1_Resources = {
  I2C1,
#if defined(I2C1_RX_DMA_Instance) && defined(I2C1_TX_DMA_Instance)
  &I2C1_RX_DMA,
  &I2C1_TX_DMA,
#else
  NULL,
  NULL,
#endif
  {
    I2C1_SCL_GPIOx,
    I2C1_SDA_GPIOx,
    I2C1_SCL_GPIO_Pin,
    I2C1_SDA_GPIO_Pin,
    I2C1_AF_REMAP,
  },
  I2C1_EV_IRQn,
  I2C1_ER_IRQn,
  RCC_APB_I2C1_MASK,
  &I2C1_Info
};

#endif /* USE_I2C1 */

#if defined(USE_I2C2)
/* Function prototypes */
void I2C2_EV_IRQHandler (void);
void I2C2_ER_IRQHandler (void);

/* I2C2 DMA */
#if defined(I2C2_RX_DMA_Instance) && defined(I2C2_TX_DMA_Instance)
static const I2C_DMA I2C2_RX_DMA = {
  I2C2_RX_DMA_Instance,
  I2C2_RX_DMA_Number,
  I2C2_RX_DMA_Channel,
  I2C2_RX_DMA_Priority
};
static const I2C_DMA I2C2_TX_DMA = {
  I2C2_TX_DMA_Instance,
  I2C2_TX_DMA_Number,
  I2C2_TX_DMA_Channel,
  I2C2_TX_DMA_Priority
};
#endif

/* I2C2 Information (Run-Time) */
static I2C_INFO I2C2_Info;

/* I2C2 Resources */
static I2C_RESOURCES I2C2_Resources = {
  I2C2,
#if defined(I2C2_RX_DMA_Instance) && defined(I2C2_TX_DMA_Instance)
  &I2C2_RX_DMA,
  &I2C2_TX_DMA,
#else
  NULL,
  NULL,
#endif
  {
    I2C2_SCL_GPIOx,
    I2C2_SDA_GPIOx,
    I2C2_SCL_GPIO_Pin,
    I2C2_SDA_GPIO_Pin,
    I2C2_AF_REMAP,
  },
  I2C2_EV_IRQn,
  I2C2_ER_IRQn,
  RCC_APB_I2C2_MASK,
  &I2C2_Info
};

#endif /* USE_I2C2 */


/**
  \fn          ARM_DRV_VERSION I2C_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION I2CX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_I2C_CAPABILITIES I2C_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_I2C_CAPABILITIES
*/
static ARM_I2C_CAPABILITIES I2CX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t I2C_Initialize (ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *i2c)
  \brief       Initialize I2C Interface.
  \param[in]   cb_event  Pointer to \ref ARM_I2C_SignalEvent
  \param[in]   i2c   Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static int32_t I2C_Initialize (ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *i2c) {

  if (i2c->info->flags & I2C_INIT) { return ARM_DRIVER_OK; }

  GPIO_AFConfigure(i2c->io.remap);

  /* Configure SCL Pin */
  GPIO_PortClock   (i2c->io.scl_port, true);
  GPIO_PinConfigure(i2c->io.scl_port, i2c->io.scl_pin, GPIO_AF_OPENDRAIN,
                                                       GPIO_MODE_OUT50MHZ);
  /* Configure SDA Pin */
  GPIO_PortClock   (i2c->io.sda_port, true);
  GPIO_PinConfigure(i2c->io.sda_port, i2c->io.sda_pin, GPIO_AF_OPENDRAIN,
                                                       GPIO_MODE_OUT50MHZ);

  /* Initialize DMA Channels */
  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
    if ((i2c->dma_rx != NULL) && (i2c->dma_tx != NULL)) {
      DMA_ChannelInitialize(i2c->dma_rx->dma_num, i2c->dma_rx->channel);
      DMA_ChannelInitialize(i2c->dma_tx->dma_num, i2c->dma_tx->channel);
    }
  #endif

  /* Reset Run-Time information structure */
  memset (i2c->info, 0x00, sizeof (I2C_INFO));

  i2c->info->cb_event = cb_event;
  i2c->info->flags    = I2C_INIT;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_Uninitialize (I2C_RESOURCES *i2c)
  \brief       De-initialize I2C Interface.
  \param[in]   i2c  Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_Uninitialize (I2C_RESOURCES *i2c) {

  /* Unconfigure SCL Pin */
  GPIO_PinConfigure(i2c->io.scl_port, i2c->io.scl_pin, GPIO_IN_ANALOG,
                                                       GPIO_MODE_OUT2MHZ);
  /* Unconfigure SDA Pin */
  GPIO_PinConfigure(i2c->io.sda_port, i2c->io.sda_pin, GPIO_IN_ANALOG,
                                                       GPIO_MODE_OUT2MHZ);
  GPIO_AFConfigure(AFIO_I2C1_NO_REMAP);

  /* Uninitialize DMA Channels */
  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
    if ((i2c->dma_rx != NULL) && (i2c->dma_tx != NULL)) {
      DMA_ChannelUninitialize(i2c->dma_rx->dma_num, i2c->dma_rx->channel);
      DMA_ChannelUninitialize(i2c->dma_tx->dma_num, i2c->dma_tx->channel);
    }
  #endif

  i2c->info->flags = 0U;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t ARM_I2C_PowerControl (ARM_POWER_STATE state, I2C_RESOURCES *i2c)
  \brief       Control I2C Interface Power.
  \param[in]   state  Power state
  \param[in]   i2c  Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_PowerControl (ARM_POWER_STATE state, I2C_RESOURCES *i2c) {

  switch (state) {
    case ARM_POWER_OFF:
      /* Enable I2C clock */
      RCC->APB1ENR |= i2c->apb_mask;

      /* Disable I2C peripheral */
      i2c->reg->CR1 = 0;

      #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
        if ((i2c->dma_rx != NULL) && (i2c->dma_tx != NULL)) {
          /* Abort DMA channels */
          DMA_ChannelDisable (i2c->dma_rx->reg);
          DMA_ChannelDisable (i2c->dma_tx->reg);
        }
      #endif

      /* Disable I2C IRQ */
      NVIC_DisableIRQ(i2c->ev_irq_num);
      NVIC_DisableIRQ(i2c->er_irq_num);

      /* Disable peripheral clock */
      RCC->APB1ENR &= ~i2c->apb_mask;

      i2c->info->status.busy             = 0U;
      i2c->info->status.mode             = 0U;
      i2c->info->status.direction        = 0U;
      i2c->info->status.general_call     = 0U;
      i2c->info->status.arbitration_lost = 0U;
      i2c->info->status.bus_error        = 0U;

      i2c->info->flags &= ~I2C_POWER;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((i2c->info->flags & I2C_INIT)  == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((i2c->info->flags & I2C_POWER) != 0U) {
        return ARM_DRIVER_OK;
      }

      /* Enable I2C clock */
      RCC->APB1ENR |= i2c->apb_mask;

      /* Clear and Enable I2C IRQ */
      NVIC_ClearPendingIRQ(i2c->ev_irq_num);
      NVIC_ClearPendingIRQ(i2c->er_irq_num);
      NVIC_EnableIRQ(i2c->ev_irq_num);
      NVIC_EnableIRQ(i2c->er_irq_num);

      /* Reset the peripheral */
      RCC->APB1RSTR |=  i2c->apb_mask;
      __NOP(); __NOP(); __NOP(); __NOP();
      RCC->APB1RSTR &= ~i2c->apb_mask;

      /* Enable event and error interrupts */
      i2c->reg->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
      /* Disable buffer interrupts */
      i2c->reg->CR2 &= ~I2C_CR2_ITBUFEN;

      /* Enable clock stretching */
      i2c->reg->CR1 &= ~I2C_CR1_NOSTRETCH;

      /* Enable I2C peripheral */
      i2c->reg->CR1 |= I2C_CR1_PE;

      /* Enable acknowledge */
      i2c->reg->CR1 |= I2C_CR1_ACK;

      /* Ready for operation */
      i2c->info->flags |= I2C_POWER;
      break;
  }

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_MasterTransmit (uint32_t       addr,
                                           const uint8_t *data,
                                           uint32_t       num,
                                           bool           xfer_pending,
                                           I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[in]   data          Pointer to buffer with data to send to I2C Slave
  \param[in]   num           Number of data bytes to send
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_MasterTransmit (uint32_t       addr,
                                   const uint8_t *data,
                                   uint32_t       num,
                                   bool           xfer_pending,
                                   I2C_RESOURCES *i2c) {

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((addr & ~(ARM_I2C_ADDRESS_10BIT | ARM_I2C_ADDRESS_GC)) > 0x3FFU) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  if ((i2c->info->xfer.ctrl & XFER_CTRL_XPENDING) == 0U) {
    /* New transfer */
    while (i2c->reg->SR2 & I2C_SR2_BUSY) {
      ; /* Wait until bus released */
    }
  }

  i2c->info->status.busy             = 1U;
  i2c->info->status.mode             = 1U;
  i2c->info->status.direction        = 0U;
  i2c->info->status.bus_error        = 0U;
  i2c->info->status.arbitration_lost = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = 0U;
  i2c->info->xfer.data = (uint8_t *)data;
  i2c->info->xfer.addr = (uint16_t)(addr);
  i2c->info->xfer.ctrl = 0U;

  if (xfer_pending) {
    i2c->info->xfer.ctrl |= XFER_CTRL_XPENDING;
  }

  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
  if (i2c->dma_tx) {
    /* Configure and enable DMA channel */
    DMA_ChannelConfigure (i2c->dma_tx->reg,
                          (i2c->dma_rx->priority << DMA_PRIORITY_POS)|
                          DMA_MEMORY_DATA_8BIT                       |
                          DMA_PERIPHERAL_DATA_8BIT                   |
                          DMA_MEMORY_INCREMENT                       |
                          DMA_READ_MEMORY                            |
                          DMA_TRANSFER_COMPLETE_INTERRUPT,
                         (uint32_t)&(i2c->reg->DR),
                         (uint32_t)data,
                          num);
    DMA_ChannelEnable (i2c->dma_tx->reg);
  }
  #endif

  /* Generate start and enable event interrupts */
  i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
  i2c->reg->CR1 |=  I2C_CR1_START;
  i2c->reg->CR2 |=  I2C_CR2_ITEVTEN;
  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_MasterReceive (uint32_t       addr,
                                          uint8_t       *data,
                                          uint32_t       num,
                                          bool           xfer_pending,
                                          I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[out]  data          Pointer to buffer for data to receive from I2C Slave
  \param[in]   num           Number of data bytes to receive
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_MasterReceive (uint32_t       addr,
                                  uint8_t       *data,
                                  uint32_t       num,
                                  bool           xfer_pending,
                                  I2C_RESOURCES *i2c) {

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if ((addr & ~(ARM_I2C_ADDRESS_10BIT | ARM_I2C_ADDRESS_GC)) > 0x3FFU) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  if ((i2c->info->xfer.ctrl & XFER_CTRL_XPENDING) == 0U) {
    /* New transfer */
    while (i2c->reg->SR2 & I2C_SR2_BUSY) {
      ; /* Wait until bus released */
    }
  }

  i2c->info->status.busy             = 1U;
  i2c->info->status.mode             = 1U;
  i2c->info->status.direction        = 1U;
  i2c->info->status.bus_error        = 0U;
  i2c->info->status.arbitration_lost = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = 0U;
  i2c->info->xfer.data = data;
  i2c->info->xfer.addr = (uint16_t)(addr);
  i2c->info->xfer.ctrl = 0U;

  if (xfer_pending) {
    i2c->info->xfer.ctrl |= XFER_CTRL_XPENDING;
  }

  /* Enable acknowledge generation */
  i2c->reg->CR1 |= I2C_CR1_ACK;

  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
  if (i2c->dma_rx) {
    /* Configure and enable DMA channel */
    DMA_ChannelConfigure (i2c->dma_rx->reg,
                          (i2c->dma_rx->priority << DMA_PRIORITY_POS)|
                          DMA_MEMORY_DATA_8BIT                       |
                          DMA_PERIPHERAL_DATA_8BIT                   |
                          DMA_MEMORY_INCREMENT                       |
                          DMA_PERIPHERAL_TO_MEMORY                   |
                          DMA_TRANSFER_COMPLETE_INTERRUPT,
                         (uint32_t)&(i2c->reg->DR),
                         (uint32_t)data,
                          num);

    DMA_ChannelEnable (i2c->dma_rx->reg);

    /* Permit generation of a NACK on the last received data */
    i2c->reg->CR2 |= I2C_CR2_LAST;
  }
  #endif

  /* Generate start and enable event interrupts */
  i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
  i2c->reg->CR1 |=  I2C_CR1_START;
  i2c->reg->CR2 |=  I2C_CR2_ITEVTEN;

  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_SlaveTransmit (const uint8_t *data, uint32_t num, I2C_RESOURCES *i2c)
  \brief       Start transmitting data as I2C Slave.
  \param[in]   data          Pointer to buffer with data to send to I2C Master
  \param[in]   num           Number of data bytes to send
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_SlaveTransmit (const uint8_t *data, uint32_t num, I2C_RESOURCES *i2c) {

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  i2c->info->status.bus_error    = 0U;
  i2c->info->status.general_call = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = 0U;
  i2c->info->xfer.data = (uint8_t *)data;
  i2c->info->xfer.ctrl = 0U;

  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
  if (i2c->dma_tx) {
    /* Configure and enable DMA channel */
    DMA_ChannelConfigure (i2c->dma_tx->reg,
                          (i2c->dma_tx->priority << DMA_PRIORITY_POS)|
                          DMA_MEMORY_DATA_8BIT                       |
                          DMA_PERIPHERAL_DATA_8BIT                   |
                          DMA_MEMORY_INCREMENT                       |
                          DMA_READ_MEMORY                            |
                          DMA_TRANSFER_COMPLETE_INTERRUPT,
                         (uint32_t)&(i2c->reg->DR),
                         (uint32_t)data,
                          num);
    DMA_ChannelEnable (i2c->dma_tx->reg);
  }
  #endif

  /* Enable acknowledge */
  i2c->reg->CR1 |= I2C_CR1_ACK;

  /* Enable event interrupts */
  i2c->reg->CR2 |= I2C_CR2_ITEVTEN;
  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_SlaveReceive (uint8_t *data, uint32_t num, I2C_RESOURCES *i2c)
  \brief       Start receiving data as I2C Slave.
  \param[out]  data          Pointer to buffer for data to receive from I2C Master
  \param[in]   num           Number of data bytes to receive
  \param[in]   i2c           Pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_SlaveReceive (uint8_t *data, uint32_t num, I2C_RESOURCES *i2c) {

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if (i2c->info->status.busy) {
    return (ARM_DRIVER_ERROR_BUSY);
  }

  i2c->info->status.bus_error    = 0U;
  i2c->info->status.general_call = 0U;

  i2c->info->xfer.num  = num;
  i2c->info->xfer.cnt  = 0U;
  i2c->info->xfer.data = data;
  i2c->info->xfer.ctrl = 0U;

  /* Enable acknowledge generation */
  i2c->reg->CR2 |= I2C_CR2_LAST;

  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
  if (i2c->dma_rx) {
    /* Configure and enable DMA channel */
    DMA_ChannelConfigure (i2c->dma_rx->reg,
                          (i2c->dma_rx->priority << DMA_PRIORITY_POS)|
                          DMA_MEMORY_DATA_8BIT                       |
                          DMA_PERIPHERAL_DATA_8BIT                   |
                          DMA_MEMORY_INCREMENT                       |
                          DMA_PERIPHERAL_TO_MEMORY                   |
                          DMA_TRANSFER_COMPLETE_INTERRUPT,
                         (uint32_t)&(i2c->reg->DR),
                         (uint32_t)data,
                          num);

    DMA_ChannelEnable (i2c->dma_rx->reg);
  }
  #endif

  /* Enable acknowledge */
  i2c->reg->CR1 |= I2C_CR1_ACK;

  /* Enable event interrupts */
  i2c->reg->CR2 |= I2C_CR2_ITEVTEN;
  return ARM_DRIVER_OK;
}


/**
  \fn          int32_t I2C_GetDataCount (void)
  \brief       Get transferred data count.
  \return      number of data bytes transferred; -1 when Slave is not addressed by Master
*/
static int32_t I2C_GetDataCount (I2C_RESOURCES *i2c) {
  return ((int32_t)i2c->info->xfer.cnt);
}


/**
  \fn          int32_t I2C_Control (uint32_t control, uint32_t arg, I2C_RESOURCES *i2c)
  \brief       Control I2C Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \param[in]   i2c      pointer to I2C resources
  \return      \ref execution_status
*/
static int32_t I2C_Control (uint32_t control, uint32_t arg, I2C_RESOURCES *i2c) {
  uint32_t state;
  uint32_t i, j, pclk;
  uint32_t ccr;
  uint32_t trise;

  if ((i2c->info->flags & I2C_POWER) == 0U) {
    /* I2C not powered */
    return ARM_DRIVER_ERROR;
  }

  switch (control) {
    case ARM_I2C_OWN_ADDRESS:
      /* Enable/Disable General call */
      if (arg & ARM_I2C_ADDRESS_GC) {
        i2c->reg->CR1 |=  I2C_CR1_ENGC;
      } else {
        i2c->reg->CR1 &= ~I2C_CR1_ENGC;
      }
      /* Set own address and its length */
      i2c->reg->OAR1 = ((arg << 1) & 0x03FFU) |
                       (1U << 14)             |
                       ((arg & ARM_I2C_ADDRESS_10BIT) ? (1U << 15) : (0U));
      break;

    case ARM_I2C_BUS_SPEED:
      pclk = RTE_PCLK1;

      switch (arg) {
        case ARM_I2C_BUS_SPEED_STANDARD:
          /* Clock = 100kHz,  Rise Time = 1000ns */
          if (pclk > 42000000U) { return ARM_DRIVER_ERROR_UNSUPPORTED; }
          if (pclk <  2000000U) { return ARM_DRIVER_ERROR_UNSUPPORTED; }
          ccr   = (pclk /  100000U) / 2U;
          trise = (pclk / 1000000U) + 1U;
          break;
        case ARM_I2C_BUS_SPEED_FAST:
          /* Clock = 400kHz,  Rise Time = 300ns */
          if (pclk > 42000000U) { return ARM_DRIVER_ERROR_UNSUPPORTED; }
          if (pclk <  4000000U) { return ARM_DRIVER_ERROR_UNSUPPORTED; }
          if ((pclk >= 10000000U) && ((pclk % 10000000U) == 0U)) {
            ccr = I2C_CCR_FS | I2C_CCR_DUTY | ((pclk / 400000U) / 25U);
          } else {
            ccr = I2C_CCR_FS |                ((pclk / 400000U) / 3U);
          }
          trise = (pclk / 333333U) + 1U;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      i2c->reg->CR1   &= ~I2C_CR1_PE;           /* Disable I2C peripheral */
      i2c->reg->CR2   &= ~I2C_CR2_FREQ;
      i2c->reg->CR2   |=  pclk / 1000000U;
      i2c->reg->CCR    =  ccr;
      i2c->reg->TRISE  =  trise;
      i2c->reg->CR1   |=  I2C_CR1_PE;           /* Enable I2C peripheral */
      i2c->reg->CR1   |=  I2C_CR1_ACK;          /* Enable acknowledge    */
      break;

    case ARM_I2C_BUS_CLEAR:
      i2c->reg->CR1 &= ~I2C_CR1_PE;

      SystemCoreClockUpdate();

      /* Frequency is set to approx. standard speed (100kHz) */
      pclk = SystemCoreClock / 100000 / 4;

      GPIO_PinWrite (i2c->io.scl_port, i2c->io.scl_pin, 1);
      GPIO_PinWrite (i2c->io.sda_port, i2c->io.sda_pin, 1);


      /* Configure SCL Pin as GPIO */
      GPIO_PinConfigure(i2c->io.scl_port, i2c->io.scl_pin, GPIO_OUT_OPENDRAIN,
                                                           GPIO_MODE_OUT50MHZ);
      
      /* Configure SDA Pin as GPIO */
      GPIO_PinConfigure(i2c->io.sda_port, i2c->io.sda_pin, GPIO_OUT_OPENDRAIN,
                                                           GPIO_MODE_OUT50MHZ);

      for (i = 0U; i < 9U; i++) {
        if (GPIO_PinRead (i2c->io.sda_port, i2c->io.sda_pin)) {
          /* Break if slave released SDA line */
          break;
        }
        /* Clock high */
        GPIO_PinWrite (i2c->io.scl_port, i2c->io.scl_pin, 1);
        for (j = 0; j < pclk; j++);

        /* Clock low */
        GPIO_PinWrite (i2c->io.scl_port, i2c->io.scl_pin, 0);
        for (j = 0; j < pclk; j++);
      }
      /* Check SDA state */
      state = GPIO_PinRead (i2c->io.sda_port, i2c->io.sda_pin);

      /* Generate STOP condition - SDA goes high while SCL is high */
      GPIO_PinWrite (i2c->io.sda_port, i2c->io.sda_pin, 0);
      for (j = 0; j < pclk; j++);
      GPIO_PinWrite (i2c->io.scl_port, i2c->io.scl_pin, 1);
      for (j = 0; j < pclk; j++);
      GPIO_PinWrite (i2c->io.sda_port, i2c->io.sda_pin, 1);

      /* Configure SCL Pin as I2C peripheral pin */
      GPIO_PinConfigure(i2c->io.scl_port, i2c->io.scl_pin, GPIO_AF_OPENDRAIN,
                                                           GPIO_MODE_OUT50MHZ);

      /* Configure SDA Pin as I2C peripheral pin */
      GPIO_PinConfigure(i2c->io.sda_port, i2c->io.sda_pin, GPIO_AF_OPENDRAIN,
                                                           GPIO_MODE_OUT50MHZ);

      i2c->reg->CR1 |=  I2C_CR1_PE;

      /* Send event */
      if (i2c->info->cb_event) {
        i2c->info->cb_event (ARM_I2C_EVENT_BUS_CLEAR);
      }

      return (state) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;

    case ARM_I2C_ABORT_TRANSFER:
      /* Disable DMA requests and I2C interrupts */
      i2c->reg->CR2 &= ~(I2C_CR2_DMAEN | I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN);

      #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
        if ((i2c->dma_rx != NULL) && (i2c->dma_tx != NULL)) {
          DMA_ChannelDisable(i2c->dma_rx->reg);
          DMA_ChannelDisable(i2c->dma_tx->reg);
        }
      #endif

      /* Generate stop */
      /* Master generates stop after the current byte transfer */
      /* Slave releases SCL and SDA after the current byte transfer */
      i2c->reg->CR1 |= I2C_CR1_STOP;

      i2c->info->xfer.num  = 0U;
      i2c->info->xfer.cnt  = 0U;
      i2c->info->xfer.data = NULL;
      i2c->info->xfer.addr = 0U;
      i2c->info->xfer.ctrl = 0U;

      i2c->info->status.busy             = 0U;
      i2c->info->status.mode             = 0U;
      i2c->info->status.direction        = 0U;
      i2c->info->status.general_call     = 0U;
      i2c->info->status.arbitration_lost = 0U;
      i2c->info->status.bus_error        = 0U;

      /* Disable and reenable peripheral to clear some flags */
      i2c->reg->CR1 &= ~I2C_CR1_PE;
      i2c->reg->CR1 |=  I2C_CR1_PE;
      /* Enable acknowledge */
      i2c->reg->CR1 |=  I2C_CR1_ACK;
      break;

    default: return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_OK;
}


/**
  \fn          ARM_I2C_STATUS I2C_GetStatus (I2C_RESOURCES *i2c)
  \brief       Get I2C status.
  \param[in]   i2c      pointer to I2C resources
  \return      I2C status \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_GetStatus (I2C_RESOURCES *i2c) {
  return (i2c->info->status);
}


/**
  \fn          void I2C_EV_IRQHandler (I2C_RESOURCES *i2c)
  \brief       I2C Event Interrupt handler.
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_EV_IRQHandler (I2C_RESOURCES *i2c) {
  I2C_TRANSFER_INFO *tr = &i2c->info->xfer;
  uint8_t  data;
  uint16_t sr1, sr2;
  uint32_t event;

  sr1 = (uint16_t)i2c->reg->SR1;

  if (sr1 & I2C_SR1_SB) {
    /* (EV5): start bit generated, send address */

    if (tr->addr & ARM_I2C_ADDRESS_10BIT) {
      /* 10-bit addressing mode */
      data = (uint8_t)(0xF0U | ((tr->addr >> 7) & 0x06U));
    }
    else {
      /* 7-bit addressing mode */
      data  = (uint8_t)tr->addr << 1;
      data |= (uint8_t)i2c->info->status.direction;
    }
    i2c->reg->DR = data;
  }
  else if (sr1 & I2C_SR1_ADD10) {
    /* (EV9): 10-bit address header sent, send device address LSB */
    i2c->reg->DR = (uint8_t)tr->addr;

    if (i2c->info->status.direction) {
      /* Master receiver generates repeated start in 10-bit addressing mode */
      tr->ctrl |= XFER_CTRL_RSTART;
    }
  }
  else if (sr1 & I2C_SR1_ADDR) {
    /* (EV6): addressing complete */
    if (tr->ctrl & XFER_CTRL_ADDR_DONE) {
      /* Restart condition, end previous transfer */
      i2c->info->status.busy = 0U;

      event = ARM_I2C_EVENT_TRANSFER_DONE;

      if (tr->cnt < tr->num) {
        event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
      }

      if (i2c->info->status.general_call) {
        event |= ARM_I2C_EVENT_GENERAL_CALL;
      }

      if (i2c->info->cb_event != NULL) {
        i2c->info->cb_event (event);
      }
    }

    if ((i2c->info->status.mode != 0U) && (i2c->info->status.direction != 0U)) {
      /* Master mode, receiver */
      if (tr->num == 1U) {
        i2c->reg->CR1 &= ~I2C_CR1_ACK;
      }

      /* Clear ADDR flag */
      i2c->reg->SR1;
      i2c->reg->SR2;

      if (tr->ctrl & XFER_CTRL_RSTART) {
        tr->ctrl &= ~XFER_CTRL_RSTART;
        /* Generate repeated start */
        i2c->reg->CR1 |= I2C_CR1_START;
      }
      else {
        if (tr->num == 1U) {
          if ((tr->ctrl & XFER_CTRL_XPENDING) == 0U) {
            i2c->reg->CR1 |= I2C_CR1_STOP;
          }
        }
        else if (tr->num == 2U) {
          i2c->reg->CR1 &= ~I2C_CR1_ACK;
          i2c->reg->CR1 |= I2C_CR1_POS;

          /* Wait until BTF == 1 */
          tr->ctrl |= XFER_CTRL_WAIT_BTF;
        }
        else {
          if (tr->num == 3U) {
            /* Wait until BTF == 1 */
            tr->ctrl |= XFER_CTRL_WAIT_BTF;
          }
        }
      }
    }
    else {
      /* Master transmitter or slave mode */
      sr2 = (uint16_t)i2c->reg->SR2;

      if (i2c->info->status.mode == 0U) {
        /* Slave mode */

        if (sr2 & I2C_SR2_GENCALL) {
          i2c->info->status.general_call = 1U;
        } else {
          i2c->info->status.general_call = 0U;
        }
        
        if (sr2 & I2C_SR2_TRA) {
          i2c->info->status.direction = 0U;
        } else {
          i2c->info->status.direction = 1U;
        }

        event = 0U;

        if (tr->data == NULL) {
          if (i2c->info->status.direction) {
            event |= ARM_I2C_EVENT_SLAVE_RECEIVE;
          }
          else {
            event |= ARM_I2C_EVENT_SLAVE_TRANSMIT;
          }
        }

        if (i2c->info->status.general_call) {
          event |= ARM_I2C_EVENT_GENERAL_CALL;
        }

        if ((event != 0U) && (i2c->info->cb_event != NULL)) {
          i2c->info->cb_event (event);
        }

        i2c->info->status.busy = 1U;
      }
    }

    tr->ctrl |= XFER_CTRL_ADDR_DONE | XFER_CTRL_XACTIVE;

    if ((i2c->dma_rx != NULL) && (i2c->dma_tx != NULL)) {
      /* Enable DMA data transfer */
      i2c->reg->CR2 |= I2C_CR2_DMAEN;
    }
    else {
      /* Enable IRQ data transfer */
      i2c->reg->CR2 |= I2C_CR2_ITBUFEN;
    }

  }
  else if (sr1 & I2C_SR1_STOPF) {
    /* STOP condition detected */
    tr->data = NULL;
    tr->ctrl = 0U;

    /* Reenable ACK */
    i2c->reg->CR1 |= I2C_CR1_ACK;

    i2c->info->status.busy = 0U;

    event = ARM_I2C_EVENT_TRANSFER_DONE;
    if (tr->cnt < tr->num) {
      event |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
    }
    if (i2c->info->status.general_call) {
      event |= ARM_I2C_EVENT_GENERAL_CALL;
    }

    if (i2c->info->cb_event) {
      i2c->info->cb_event (event);
    }
  }
  else if (tr->ctrl & XFER_CTRL_XACTIVE) {
    /* BTF, RxNE or TxE interrupt */
    if (tr->ctrl & XFER_CTRL_DMA_DONE) {
      /* BTF triggered this event */
      if (i2c->info->status.mode) {
        if (i2c->info->xfer.ctrl & XFER_CTRL_XPENDING) {
          /* Disable event interrupt */
          i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
        }
        else {
          /* Generate stop condition */
          i2c->reg->CR1 |= I2C_CR1_STOP;
        }
        tr->data  =  NULL;
        tr->ctrl &= ~XFER_CTRL_XACTIVE;

        i2c->info->status.busy = 0U;
        i2c->info->status.mode = 0U;

        if (i2c->info->cb_event) {
          i2c->info->cb_event (ARM_I2C_EVENT_TRANSFER_DONE);
        }
      }
    }
    else if (sr1 & I2C_SR1_TXE) {
      if (i2c->info->status.mode) {
        /* Master transmitter */
        if (tr->ctrl & XFER_CTRL_WAIT_BTF) {
          if (sr1 & I2C_SR1_BTF) {
            /* End master transmit operation */
            i2c->reg->CR2 &= ~I2C_CR2_ITBUFEN;

            if (tr->ctrl & XFER_CTRL_XPENDING) {
              i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
            }
            else {
              i2c->reg->CR1 |= I2C_CR1_STOP;
            }

            tr->data  = NULL;
            tr->ctrl &= ~XFER_CTRL_XACTIVE;

            i2c->info->status.busy = 0U;
            i2c->info->status.mode = 0U;

            if (i2c->info->cb_event) {
              i2c->info->cb_event (ARM_I2C_EVENT_TRANSFER_DONE);
            }
          }
        }
        else {
          i2c->reg->DR = tr->data[tr->cnt];

          tr->cnt++;
          if (tr->cnt == tr->num) {
            tr->ctrl |= XFER_CTRL_WAIT_BTF;
          }
        }
      }
      else {
        /* Slave transmitter */
        if (tr->data == NULL) {
          if (i2c->info->cb_event) {
            i2c->info->cb_event (ARM_I2C_EVENT_SLAVE_TRANSMIT);
          }
        }

        if (tr->data) {
          i2c->reg->DR = tr->data[tr->cnt];

          tr->cnt++;
          if (tr->cnt == tr->num) {
            tr->data = NULL;
          }
        }
        else {
          /* Master requests more data as we have */
          i2c->reg->DR = (uint8_t)0xFF;
        }
      }
    }
    else if (sr1 & I2C_SR1_RXNE) {
      if (i2c->info->status.mode) {
        /* Master receiver */
        if (tr->ctrl & XFER_CTRL_WAIT_BTF) {
          if (sr1 & I2C_SR1_BTF) {
            if ((tr->num == 2U) || (tr->cnt == (tr->num - 2U))) {
              /* Two bytes remaining */
              i2c->reg->CR2 &= ~I2C_CR2_ITBUFEN;

              if (tr->ctrl & XFER_CTRL_XPENDING) {
                i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
              }
              else {
                i2c->reg->CR1 |= I2C_CR1_STOP;
              }

              /* Read data N-1 and N */
              tr->data[tr->cnt++] = (uint8_t)i2c->reg->DR;
              tr->data[tr->cnt++] = (uint8_t)i2c->reg->DR;

              tr->data  = NULL;
              tr->ctrl &= ~XFER_CTRL_XACTIVE;

              i2c->info->status.busy = 0U;
              i2c->info->status.mode = 0U;

              i2c->reg->CR1 &= ~I2C_CR1_POS;

              if (i2c->info->cb_event) {
                i2c->info->cb_event (ARM_I2C_EVENT_TRANSFER_DONE);
              }
            }
            else {
              /* Three bytes remaining */
              i2c->reg->CR1 &= ~I2C_CR1_ACK;
              /* Read data N-2 */
              tr->data[tr->cnt++] = (uint8_t)i2c->reg->DR;
            }
          }
        }
        else {
          tr->data[tr->cnt++] = (uint8_t)i2c->reg->DR;

          if (tr->num == 1U) {
            /* Single byte transfer completed */
            i2c->reg->CR2 &= ~I2C_CR2_ITBUFEN;

            if (tr->ctrl & XFER_CTRL_XPENDING) {
              i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
            }
            /* (STOP was already sent during ADDR phase) */

            tr->data  = NULL;
            tr->ctrl &= ~XFER_CTRL_XACTIVE;

            i2c->info->status.busy = 0U;
            i2c->info->status.mode = 0U;

            if (i2c->info->cb_event) {
              i2c->info->cb_event (ARM_I2C_EVENT_TRANSFER_DONE);
            }
          }
          else {
            if (tr->cnt == (tr->num - 3U)) {
              /* N > 2 byte reception, begin N-2 data reception */
              i2c->reg->CR2 &= ~I2C_CR2_ITBUFEN;
              /* Wait until BTF == 1 */
              tr->ctrl |= XFER_CTRL_WAIT_BTF;
            }
          }
        }
      }
      else {
        /* Slave receiver */
        data = (uint8_t)i2c->reg->DR;

        if (tr->data == NULL) {
          /* Receive buffer full: Disable ACK */
          i2c->reg->CR1 &= ~I2C_CR1_ACK;
        }
        else {
          if (tr->cnt < tr->num) {
            tr->data[tr->cnt] = data;

            tr->cnt++;
            if (tr->cnt == tr->num) {
              tr->data = NULL;
            }
          }
        }
      }
    }
  }
}


/**
  \fn          void I2C_ER_IRQHandler (I2C_RESOURCES *i2c)
  \brief       I2C Error Interrupt handler.
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_ER_IRQHandler (I2C_RESOURCES *i2c) {
  uint32_t sr1 = i2c->reg->SR1;
  uint32_t evt = 0U;
  uint32_t err = 0U;

  if (sr1 & I2C_SR1_SMBALERT) {
    /* SMBus alert */
    err |= I2C_SR1_SMBALERT;
  }
  if (sr1 & I2C_SR1_TIMEOUT) {
    /* Timeout - SCL remained LOW for 25ms */
    err |= I2C_SR1_TIMEOUT;
  }
  if (sr1 & I2C_SR1_PECERR) {
    /* PEC Error in reception */
    err |= I2C_SR1_PECERR;
  }
  if (sr1 & I2C_SR1_OVR) {
    /* Overrun/Underrun */
    err |= I2C_SR1_OVR;
  }

  if (sr1 & I2C_SR1_AF) {
    /* Acknowledge failure */
    err |= I2C_SR1_AF;

    /* Reset the communication */
    i2c->reg->CR1 |= I2C_CR1_STOP;

    i2c->info->status.busy = 0U;
    i2c->info->status.mode = 0U;

    i2c->info->xfer.data = NULL;
    i2c->info->xfer.ctrl = 0U;

    evt = ARM_I2C_EVENT_TRANSFER_DONE;

    if ((i2c->info->xfer.ctrl & XFER_CTRL_ADDR_DONE) == 0U) {
      /* Addressing not done */
      evt |= ARM_I2C_EVENT_ADDRESS_NACK;
    }
  }

  if (sr1 & I2C_SR1_ARLO) {
    /* Arbitration lost */
    err |= I2C_SR1_ARLO;

    /* Switch to slave mode */
    i2c->info->status.busy             = 0U;
    i2c->info->status.mode             = 0U;
    i2c->info->status.arbitration_lost = 1U;

    i2c->info->xfer.data = NULL;
    i2c->info->xfer.ctrl = 0U;

    evt = ARM_I2C_EVENT_TRANSFER_DONE | ARM_I2C_EVENT_ARBITRATION_LOST;
  }

  if (sr1 & I2C_SR1_BERR) {
    /* Bus error - misplaced start/stop */
    err |= I2C_SR1_BERR;

    i2c->info->status.bus_error = 1U;

    if (i2c->info->status.mode == 0U) {
      /* Lines are released in slave mode */
      i2c->info->status.busy = 0U;

      i2c->info->xfer.data = NULL;
      i2c->info->xfer.ctrl = 0U;
    }

    evt = ARM_I2C_EVENT_TRANSFER_DONE | ARM_I2C_EVENT_BUS_ERROR;

  }
  /* Abort DMA channels */
  #if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
    if ((i2c->dma_rx != NULL) && (i2c->dma_tx != NULL)) {
      DMA_ChannelDisable(i2c->dma_tx->reg);
      DMA_ChannelDisable(i2c->dma_rx->reg);
    }
  #endif

  /* Clear error flags */
  i2c->reg->SR1 &= ~err;

  if ((evt != 0) && (i2c->info->cb_event != NULL)) {
    if (i2c->info->xfer.cnt < i2c->info->xfer.num) {
      evt |= ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
    }
    i2c->info->cb_event (evt);
  }
}


#if defined(USE_I2C1_DMA) || defined(USE_I2C2_DMA)
/**
  \fn          void I2C_DMA_TxEvent (uint32_t event, I2C_RESOURCES *i2c)
  \brief       I2C DMA Transmit Event handler
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_DMA_TxEvent (uint32_t event, I2C_RESOURCES *i2c) {
  i2c->reg->CR2 &= ~I2C_CR2_DMAEN;

  DMA_ChannelDisable(i2c->dma_tx->reg);

  i2c->info->xfer.cnt  = i2c->info->xfer.num - DMA_ChannelTransferItemCount(i2c->dma_tx->reg);
  i2c->info->xfer.data = NULL;

  if (i2c->info->status.mode) {
    /* Master transmitter: Wait for BTF in I2C EV IRQ handler */
    i2c->info->xfer.ctrl |= XFER_CTRL_DMA_DONE;
  }
}

/**
  \fn          void I2C_DMA_RxEvent (uint32_t event, I2C_RESOURCES *i2c)
  \brief       I2C DMA Receive Event handler
  \param[in]   i2c  Pointer to I2C resources
*/
static void I2C_DMA_RxEvent (uint32_t event, I2C_RESOURCES *i2c) {
  i2c->reg->CR2 &= ~I2C_CR2_DMAEN;

  DMA_ChannelDisable(i2c->dma_rx->reg);

  i2c->info->xfer.cnt  = i2c->info->xfer.num - DMA_ChannelTransferItemCount(i2c->dma_rx->reg);
  i2c->info->xfer.data = NULL;

  if (i2c->info->status.mode) {
    /* Master mode */
    if (i2c->info->xfer.ctrl & XFER_CTRL_XPENDING) {
      /* Transfer pending */
      i2c->reg->CR2 &= ~I2C_CR2_ITEVTEN;
    }
    else {
      if (i2c->info->xfer.num != 1U) {
        i2c->reg->CR1 |= I2C_CR1_STOP;
      }
    }

    i2c->info->status.busy = 0U;
    i2c->info->status.mode = 0U;

    if (i2c->info->cb_event) {
      i2c->info->cb_event (ARM_I2C_EVENT_TRANSFER_DONE);
    }
  }
}
#endif


#if defined(USE_I2C1)
/* I2C1 Driver wrapper functions */
static int32_t I2C1_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, &I2C1_Resources);
}
static int32_t I2C1_Uninitialize (void) {
  return I2C_Uninitialize(&I2C1_Resources);
}
static int32_t I2C1_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, &I2C1_Resources);
}
static int32_t I2C1_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterTransmit(addr, data, num, xfer_pending, &I2C1_Resources);
}
static int32_t I2C1_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterReceive(addr, data, num, xfer_pending, &I2C1_Resources);
}
static int32_t I2C1_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return I2C_SlaveTransmit(data, num, &I2C1_Resources);
}
static int32_t I2C1_SlaveReceive (uint8_t *data, uint32_t num) {
  return I2C_SlaveReceive(data, num, &I2C1_Resources);
}
static int32_t I2C1_GetDataCount (void) {
  return I2C_GetDataCount(&I2C1_Resources);
}
static int32_t I2C1_Control (uint32_t control, uint32_t arg) {
  return I2C_Control(control, arg, &I2C1_Resources);
}
static ARM_I2C_STATUS I2C1_GetStatus (void) {
  return I2C_GetStatus(&I2C1_Resources);
}
void I2C1_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(&I2C1_Resources);
}
void I2C1_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(&I2C1_Resources);
}

#if defined(USE_I2C1_DMA)
void I2C1_RX_DMA_Handler (uint32_t event) {
  I2C_DMA_RxEvent (event, &I2C1_Resources);
}

void I2C1_TX_DMA_Handler (uint32_t event) {
  I2C_DMA_TxEvent (event, &I2C1_Resources);
}
#endif

/* I2C1 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C1 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C1_Initialize,
  I2C1_Uninitialize,
  I2C1_PowerControl,
  I2C1_MasterTransmit,
  I2C1_MasterReceive,
  I2C1_SlaveTransmit,
  I2C1_SlaveReceive,
  I2C1_GetDataCount,
  I2C1_Control,
  I2C1_GetStatus
};
#endif


#if defined(USE_I2C2)
/* I2C2 Driver wrapper functions */
static int32_t I2C2_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, &I2C2_Resources);
}
static int32_t I2C2_Uninitialize (void) {
  return I2C_Uninitialize(&I2C2_Resources);
}
static int32_t I2C2_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, &I2C2_Resources);
}
static int32_t I2C2_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterTransmit(addr, data, num, xfer_pending, &I2C2_Resources);
}
static int32_t I2C2_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending) {
  return I2C_MasterReceive(addr, data, num, xfer_pending, &I2C2_Resources);
}
static int32_t I2C2_SlaveTransmit (const uint8_t *data, uint32_t num) {
  return I2C_SlaveTransmit(data, num, &I2C2_Resources);
}
static int32_t I2C2_SlaveReceive (uint8_t *data, uint32_t num) {
  return I2C_SlaveReceive(data, num, &I2C2_Resources);
}
static int32_t I2C2_GetDataCount (void) {
  return I2C_GetDataCount(&I2C2_Resources);
}
static int32_t I2C2_Control (uint32_t control, uint32_t arg) {
  return I2C_Control(control, arg, &I2C2_Resources);
}
static ARM_I2C_STATUS I2C2_GetStatus (void) {
  return I2C_GetStatus(&I2C2_Resources);
}
void I2C2_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(&I2C2_Resources);
}
void I2C2_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(&I2C2_Resources);
}

#if defined(USE_I2C2_DMA)
void I2C2_RX_DMA_Handler (uint32_t event) {
  I2C_DMA_RxEvent (event, &I2C2_Resources);
}

void I2C2_TX_DMA_Handler (uint32_t event) {
  I2C_DMA_TxEvent (event, &I2C2_Resources);
}
#endif


/* I2C2 Driver Control Block */
ARM_DRIVER_I2C Driver_I2C2 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C2_Initialize,
  I2C2_Uninitialize,
  I2C2_PowerControl,
  I2C2_MasterTransmit,
  I2C2_MasterReceive,
  I2C2_SlaveTransmit,
  I2C2_SlaveReceive,
  I2C2_GetDataCount,
  I2C2_Control,
  I2C2_GetStatus
};
#endif

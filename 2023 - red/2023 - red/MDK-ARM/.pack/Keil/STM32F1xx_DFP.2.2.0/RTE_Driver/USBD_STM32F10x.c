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
 * $Date:        2. October 2015
 * $Revision:    V2.0
 *
 * Driver:       Driver_USBD0
 * Configured:   via RTE_Device.h configuration file 
 * Project:      USB Full/Low-Speed Device Driver for ST STM32F10x
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                Value
 *   ---------------------                -----
 *   Connect to hardware via Driver_USBD# = 0
 * -------------------------------------------------------------------- */

/* History:
 *  Version 2.0
 *    Updated to CMSIS Driver API V2.01
 *  Version 1.00
 *    Initial release
 */

#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"
#include "stm32f10x.h"

#include "GPIO_STM32F10x.h"
#include "USBD_STM32F10x.h"

#include "Driver_USBD.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

// Do basic RTE configuration check
#if    (RTE_USB_DEVICE == 0)
#error  Enable USB Device in RTE_Device.h!
#endif

#ifndef USBD_MAX_ENDPOINT_NUM
#define USBD_MAX_ENDPOINT_NUM           8U
#endif
#if    (USBD_MAX_ENDPOINT_NUM > 8)
#error  Too many Endpoints, maximum IN/OUT Endpoint pairs that this driver supports is 8 !!!
#endif

// Endpoint buffer address

// Endpoint buffer sizes in bytes 
// (total available memory for Endpoint buffers is 512 Bytes - EP_BUF_ADDR)
#ifndef USB_EP0_RX_BUF_SIZE
#define USB_EP0_RX_BUF_SIZE     8U
#endif
#define USB_EP0_RX_BUF_OFFSET   0x80U

#ifndef USB_EP0_TX_BUF_SIZE
#define USB_EP0_TX_BUF_SIZE     8U
#endif
#define USB_EP0_TX_BUF_OFFSET   USB_EP0_RX_BUF_OFFSET + USB_EP0_RX_BUF_SIZE

#ifndef USB_EP1_RX_BUF_SIZE
#define USB_EP1_RX_BUF_SIZE     64U
#endif
#define USB_EP1_RX_BUF_OFFSET   USB_EP0_TX_BUF_OFFSET + USB_EP0_TX_BUF_SIZE

#ifndef USB_EP1_TX_BUF_SIZE
#define USB_EP1_TX_BUF_SIZE     64U
#endif
#define USB_EP1_TX_BUF_OFFSET   USB_EP1_RX_BUF_OFFSET + USB_EP1_RX_BUF_SIZE

#ifndef USB_EP2_RX_BUF_SIZE
#define USB_EP2_RX_BUF_SIZE     64U
#endif
#define USB_EP2_RX_BUF_OFFSET   USB_EP1_TX_BUF_OFFSET + USB_EP1_TX_BUF_SIZE

#ifndef USB_EP2_TX_BUF_SIZE
#define USB_EP2_TX_BUF_SIZE     64U
#endif
#define USB_EP2_TX_BUF_OFFSET   USB_EP2_RX_BUF_OFFSET + USB_EP2_RX_BUF_SIZE

#ifndef USB_EP3_RX_BUF_SIZE
#define USB_EP3_RX_BUF_SIZE     8U
#endif
#define USB_EP3_RX_BUF_OFFSET   USB_EP2_TX_BUF_OFFSET + USB_EP2_TX_BUF_SIZE

#ifndef USB_EP3_TX_BUF_SIZE
#define USB_EP3_TX_BUF_SIZE     8U
#endif
#define USB_EP3_TX_BUF_OFFSET   USB_EP3_RX_BUF_OFFSET + USB_EP3_RX_BUF_SIZE

#ifndef USB_EP4_RX_BUF_SIZE
#define USB_EP4_RX_BUF_SIZE     8U
#endif
#define USB_EP4_RX_BUF_OFFSET   USB_EP3_TX_BUF_OFFSET + USB_EP3_TX_BUF_SIZE

#ifndef USB_EP4_TX_BUF_SIZE
#define USB_EP4_TX_BUF_SIZE     8U
#endif
#define USB_EP4_TX_BUF_OFFSET   USB_EP4_RX_BUF_OFFSET + USB_EP4_RX_BUF_SIZE

#ifndef USB_EP5_RX_BUF_SIZE
#define USB_EP5_RX_BUF_SIZE     8U
#endif
#define USB_EP5_RX_BUF_OFFSET   USB_EP4_TX_BUF_OFFSET + USB_EP4_TX_BUF_SIZE

#ifndef USB_EP5_TX_BUF_SIZE
#define USB_EP5_TX_BUF_SIZE     8U
#endif
#define USB_EP5_TX_BUF_OFFSET   USB_EP5_RX_BUF_OFFSET + USB_EP5_RX_BUF_SIZE

#ifndef USB_EP6_RX_BUF_SIZE
#define USB_EP6_RX_BUF_SIZE     8U
#endif
#define USB_EP6_RX_BUF_OFFSET   USB_EP5_TX_BUF_OFFSET + USB_EP5_TX_BUF_SIZE

#ifndef USB_EP6_TX_BUF_SIZE
#define USB_EP6_TX_BUF_SIZE     8U
#endif
#define USB_EP6_TX_BUF_OFFSET   USB_EP6_RX_BUF_OFFSET + USB_EP6_RX_BUF_SIZE

#ifndef USB_EP7_RX_BUF_SIZE
#define USB_EP7_RX_BUF_SIZE     8U
#endif
#define USB_EP7_RX_BUF_OFFSET   USB_EP6_TX_BUF_OFFSET + USB_EP6_TX_BUF_SIZE

#ifndef USB_EP7_TX_BUF_SIZE
#define USB_EP7_TX_BUF_SIZE     8U
#endif
#define USB_EP7_TX_BUF_OFFSET   USB_EP7_RX_BUF_OFFSET + USB_EP7_RX_BUF_SIZE

#if ((USB_EP7_TX_BUF_OFFSET + USB_EP7_TX_BUF_SIZE) > 512U)
#error "The SUM of all Endpoint buffers sizes exceeds available dedicated RAM size (512 Bytes)."
#endif


// USBD Driver *****************************************************************

#define ARM_USBD_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,0)

// Driver Version
static const ARM_DRIVER_VERSION usbd_driver_version = { ARM_USBD_API_VERSION, ARM_USBD_DRV_VERSION };

// Driver Capabilities
static const ARM_USBD_CAPABILITIES usbd_driver_capabilities = {
  0U,   // VBUS Detection
  0U,   // Event VBUS On
  0U    // Event VBUS Off
};

#define EP_NUM(ep_addr)         ((ep_addr) & ARM_USB_ENDPOINT_NUMBER_MASK)
#define EP_ID(ep_addr)          ((EP_NUM(ep_addr) * 2U) + (((ep_addr) >> 7) & 1U))

typedef struct {                        // Endpoint structure definition
  uint8_t  *data;
  uint32_t  num;
  uint32_t  num_transferred_total;
  uint16_t  num_transferring;
  uint16_t  max_packet_size;
  uint8_t   active;
} ENDPOINT_t;

static ARM_USBD_SignalDeviceEvent_t   SignalDeviceEvent;
static ARM_USBD_SignalEndpointEvent_t SignalEndpointEvent;

static bool                hw_powered     = false;
static bool                hw_initialized = false;
static ARM_USBD_STATE      usbd_state;
static uint8_t             setup_packet[8];     // Setup packet data
static volatile uint8_t    setup_received;      // Setup packet received

// Endpoint buffer address
#define EP_BUF_ADDR (sizeof(EP_BUF_DSCR)*8)
// Pointer to Endpoint descriptor table
static EP_BUF_DSCR        *pBUF_DSCR   = (EP_BUF_DSCR *)USB_PMA_ADDR;

static const uint16_t EP_buff_offset[16] = { USB_EP0_RX_BUF_OFFSET, USB_EP0_TX_BUF_OFFSET,
                                             USB_EP1_RX_BUF_OFFSET, USB_EP1_TX_BUF_OFFSET,
                                             USB_EP2_RX_BUF_OFFSET, USB_EP2_TX_BUF_OFFSET,
                                             USB_EP3_RX_BUF_OFFSET, USB_EP3_TX_BUF_OFFSET,
                                             USB_EP4_RX_BUF_OFFSET, USB_EP4_TX_BUF_OFFSET,
                                             USB_EP5_RX_BUF_OFFSET, USB_EP5_TX_BUF_OFFSET,
                                             USB_EP6_RX_BUF_OFFSET, USB_EP6_TX_BUF_OFFSET,
                                             USB_EP7_RX_BUF_OFFSET, USB_EP7_TX_BUF_OFFSET };

// Endpoints runtime information
static volatile ENDPOINT_t ep[(USBD_MAX_ENDPOINT_NUM + 1U) * 2U];

#define IN_EP_RESET(num)  (EPxREG(EP_NUM(num)) = (EPxREG(EP_NUM(num)) & (EP_MASK | EP_STAT_TX | EP_DTOG_TX)) | EP_CTR_TX | EP_CTR_RX)
#define OUT_EP_RESET(num) (EPxREG(EP_NUM(num)) = (EPxREG(EP_NUM(num)) & (EP_MASK | EP_STAT_RX | EP_DTOG_RX)) | EP_CTR_TX | EP_CTR_RX)

// Auxiliary functions
/**
  \fn          static void IN_EP_Status (uint8_t ep_num, uint32_t stat)
  \brief       Called to update Endpoint status
*/
void IN_EP_Status (uint8_t ep_num, uint32_t stat) {
  uint32_t num, val, ep_reg;

  num         =  ep_num & ARM_USB_ENDPOINT_NUMBER_MASK;
  stat       &=  EP_STAT_TX;
  ep_reg      =  EPxREG(num);
  val         = (ep_reg & EP_STAT_TX) ^ stat;
  val         = (ep_reg & ~EP_STAT_TX) | val;
  EPxREG(num) = (val & (EP_MASK | EP_STAT_TX)) | EP_CTR_TX | EP_CTR_RX;
}

/**
  \fn          static void OUT_EP_Status (uint8_t ep_num, uint32_t stat)
  \brief       Called to update Endpoint status
*/
void OUT_EP_Status (uint8_t ep_num, uint32_t stat) {
  uint32_t num, val, ep_reg;

  num         =  ep_num & ARM_USB_ENDPOINT_NUMBER_MASK;
  stat       &=  EP_STAT_RX;
  ep_reg      =  EPxREG(num);
  val         = (ep_reg & EP_STAT_RX) ^ stat;
  val         = (ep_reg & ~EP_STAT_RX) | val;
  EPxREG(num) = (val & (EP_MASK | EP_STAT_RX)) | EP_CTR_TX | EP_CTR_RX;
}

/**
  \fn          void USBD_EP_HW_Read (uint8_t ep_addr)
  \brief       Read data from USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \return      number of data bytes read
*/
static void USBD_EP_HW_Read (uint8_t ep_addr) {
  volatile ENDPOINT_t *ptr_ep;
  uint32_t             cnt, i;
  __packed uint8_t    *ptr_dest_8;
  __packed uint16_t   *ptr_dest;
  volatile uint32_t   *ptr_src;
  uint8_t              ep_num;
  uint8_t              tmp_buf[4];

  ptr_ep = &ep[EP_ID(ep_addr)];
  ep_num = EP_NUM(ep_addr);

  cnt = (pBUF_DSCR + ep_num)->COUNT_RX & EP_COUNT_MASK;

  // Check for ZLP
  if (cnt          == 0U) { return; }

  // Check if buffer available
  if (ptr_ep->data == 0U) { return; }

  // Copy data from FIFO
  ptr_src  = (uint32_t *)(USB_PMA_ADDR + 2*((pBUF_DSCR + ep_num)->ADDR_RX));
  ptr_dest = (__packed uint16_t *)(ptr_ep->data + ptr_ep->num_transferred_total);

  i = cnt / 2U;
  while (i != 0U) {
    *ptr_dest++ = *ptr_src++;
    i--;
  }
  ptr_ep->num_transferred_total += cnt;

  // If data size is not equal n*2
  if ((cnt & 1U) != 0U) {
    ptr_dest_8 = (uint8_t *)(ptr_dest);
    *((__packed uint16_t *)tmp_buf) = *ptr_src;
    *ptr_dest_8 = tmp_buf[0];
  }

  if (cnt != ptr_ep->max_packet_size) { ptr_ep->num  = 0U;  }
  else                                { ptr_ep->num -= cnt; }
}

/**
  \fn          void USBD_EP_HW_Write (uint8_t ep_addr)
  \brief       Write data to Endpoint Buffer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
*/
static void USBD_EP_HW_Write (uint8_t ep_addr) {
  volatile ENDPOINT_t *ptr_ep;
  uint8_t              ep_num;
  uint16_t             num, i;
  volatile uint32_t   *ptr_dest;
  __packed uint16_t   *ptr_src;

  ptr_ep = &ep[EP_ID(ep_addr)];
  ep_num = EP_NUM(ep_addr);

  if (ptr_ep->num > ptr_ep->max_packet_size) { num = ptr_ep->max_packet_size; }
  else                                       { num = ptr_ep->num;             }

  ptr_src  = (__packed uint16_t *)(ptr_ep->data + ptr_ep->num_transferred_total);
  ptr_dest = (uint32_t *)(USB_PMA_ADDR + 2*((pBUF_DSCR + ep_num)->ADDR_TX));

  ptr_ep->num_transferring  = num;
  ptr_ep->num              -= num;

  // Copy data to EP Buffer
  i = (num + 1U) >> 1;
  while (i != 0U) {
    *ptr_dest++ = *ptr_src++;
    i--;
  }

  (pBUF_DSCR + ep_num)->COUNT_TX = num;

  if ((EPxREG(ep_num) & EP_STAT_TX) != EP_TX_STALL) {
    IN_EP_Status(ep_addr, EP_TX_VALID);     // do not make EP valid if stalled
  }
}


/**
  \fn          static void USBD_Reset (uint8_t ep_addr)
  \brief       Called after usbd reset interrupt to reset configuration
*/
static void USBD_Reset (void) {

  ISTR = 0;                                 // Clear Interrupt Status

  CNTR = CNTR_CTRM | CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM | CNTR_ERRM | CNTR_PMAOVRM;


  // Reset global variables
  setup_received  = 0U;
  memset((void *)USB_PMA_ADDR, 0, EP_BUF_ADDR);
  memset((void *)&usbd_state,  0, sizeof(usbd_state));
  memset((void *)ep,           0, sizeof(ep));

  BTABLE = 0x00;                        // set BTABLE Address

  DADDR = DADDR_EF | 0;                 // Enable USB Default Address
}


// USBD Driver functions

/**
  \fn          ARM_DRIVER_VERSION USBD_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USBD_GetVersion (void) { return usbd_driver_version; }

/**
  \fn          ARM_USBD_CAPABILITIES USBD_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBD_CAPABILITIES
*/
static ARM_USBD_CAPABILITIES USBD_GetCapabilities (void) { return usbd_driver_capabilities; }

/**
  \fn          int32_t USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                        ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
  \brief       Initialize USB Device Interface.
  \param[in]   cb_device_event    Pointer to \ref ARM_USBD_SignalDeviceEvent
  \param[in]   cb_endpoint_event  Pointer to \ref ARM_USBD_SignalEndpointEvent
  \return      \ref execution_status
*/
static int32_t USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                ARM_USBD_SignalEndpointEvent_t cb_endpoint_event) {

  if (hw_initialized == true) {
    return ARM_DRIVER_OK;
  }

  SignalDeviceEvent   = cb_device_event;
  SignalEndpointEvent = cb_endpoint_event;

#if (RTE_USB_DEVICE_CON_PIN)
  // Configure CON pin (controls pull-up on D+ line)
  GPIO_PortClock (RTE_USB_DEVICE_CON_PORT, true);
  GPIO_PinWrite (RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, !RTE_USB_DEVICE_CON_ACTIVE);
  if (!GPIO_PinConfigure(RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, GPIO_OUT_OPENDRAIN, GPIO_MODE_OUT10MHZ)) { return ARM_DRIVER_ERROR; }
#endif

  hw_initialized = true;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_Uninitialize (void)
  \brief       De-initialize USB Device Interface.
  \return      \ref execution_status
*/
static int32_t USBD_Uninitialize (void) {

#if (RTE_USB_DEVICE_CON_PIN)
  // Unconfigure CON pin (controls pull-up on D+ line)
  GPIO_PortClock (RTE_USB_DEVICE_CON_PORT, true);
  GPIO_PinWrite (RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, !RTE_USB_DEVICE_CON_ACTIVE);
  if (!GPIO_PinConfigure(RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, GPIO_IN_FLOATING, GPIO_MODE_INPUT))  { return ARM_DRIVER_ERROR; }
#endif

  hw_initialized = false;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Device Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t USBD_PowerControl (ARM_POWER_STATE state) {

  switch (state) {
    case ARM_POWER_OFF:
      RCC->APB1ENR  |=  RCC_APB1ENR_USBEN;              // Enable USB Device clock
      NVIC_DisableIRQ      (USB_LP_CAN1_RX0_IRQn);      // Disable interrupt
      NVIC_ClearPendingIRQ (USB_LP_CAN1_RX0_IRQn);      // Clear pending interrupt

      hw_powered     = false;                           // Clear powered flag
      CNTR = CNTR_FRES | CNTR_PDWN;                     // Switch off USB Device
#if (RTE_USB_DEVICE_CON_PIN)
      if (!GPIO_PinConfigure(RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, GPIO_IN_FLOATING, GPIO_MODE_INPUT)) { return ARM_DRIVER_ERROR; }
#endif
      RCC->APB1RSTR  |=  RCC_APB1RSTR_USBRST;           // Reset USB Device

                                                        // Reset variables
      setup_received =  0U;
      memset((void *)&usbd_state, 0, sizeof(usbd_state));
      memset((void *)ep,          0, sizeof(ep));

      RCC->APB1ENR   &= ~RCC_APB1ENR_USBEN;             // Disable USB Device clock

      break;

    case ARM_POWER_FULL:
      if (hw_initialized == false) {
        return ARM_DRIVER_ERROR;
      }
      if (hw_powered     == true) {
        return ARM_DRIVER_OK;
      }

      RCC->APB1ENR   |=  RCC_APB1ENR_USBEN;             // Enable USB Device clock
      RCC->APB1RSTR  |=  RCC_APB1RSTR_USBRST;           // Reset USB Device
      osDelay(1);                                       // Wait 1 ms
      RCC->APB1RSTR  &= ~RCC_APB1RSTR_USBRST;           // Reset USB Device

      USBD_Reset ();                                    // Reset variables and endpoint settings

      hw_powered     = true;                            // Set powered flag

      NVIC_EnableIRQ   (USB_LP_CAN1_RX0_IRQn);          // Enable interrupt
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_DeviceConnect (void)
  \brief       Connect USB Device.
  \return      \ref execution_status
*/
static int32_t USBD_DeviceConnect (void) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR; }

  // Soft connect
#if (RTE_USB_DEVICE_CON_PIN)
  GPIO_PinWrite (RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, RTE_USB_DEVICE_CON_ACTIVE);
#endif

  CNTR = CNTR_FRES;                             // Force USB Reset
  CNTR = 0;
  ISTR = 0;                                     // Clear Interrupt Status
  CNTR = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM; // USB Interrupt Mask

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_DeviceDisconnect (void)
  \brief       Disconnect USB Device.
  \return      \ref execution_status
*/
static int32_t USBD_DeviceDisconnect (void) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR; }

  CNTR = CNTR_FRES | CNTR_PDWN;                 // Switch Off USB Device

  // Soft disconnect
#if (RTE_USB_DEVICE_CON_PIN)
  GPIO_PinWrite (RTE_USB_DEVICE_CON_PORT, RTE_USB_DEVICE_CON_BIT, !RTE_USB_DEVICE_CON_ACTIVE);
#endif

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USBD_STATE USBD_DeviceGetState (void)
  \brief       Get current USB Device State.
  \return      Device State \ref ARM_USBD_STATE
*/
static ARM_USBD_STATE USBD_DeviceGetState (void) {
  return usbd_state;
}

/**
  \fn          int32_t USBD_DeviceRemoteWakeup (void)
  \brief       Trigger USB Remote Wakeup.
  \return      \ref execution_status
*/
static int32_t USBD_DeviceRemoteWakeup (void) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR; }

  CNTR |=   CNTR;       // Remote wakeup signalling
  osDelay(2U);
  CNTR &=  ~CNTR;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_DeviceSetAddress (uint8_t dev_addr)
  \brief       Set USB Device Address.
  \param[in]   dev_addr  Device Address
  \return      \ref execution_status
*/
static int32_t USBD_DeviceSetAddress (uint8_t dev_addr) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR; }

  DADDR = DADDR_EF | dev_addr;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_ReadSetupPacket (uint8_t *setup)
  \brief       Read setup packet received over Control Endpoint.
  \param[out]  setup  Pointer to buffer for setup packet
  \return      \ref execution_status
*/
static int32_t USBD_ReadSetupPacket (uint8_t *setup) {

  if (hw_powered == false)  { return ARM_DRIVER_ERROR; }
  if (setup_received == 0U) { return ARM_DRIVER_ERROR; }

  setup_received = 0U;
  memcpy(setup, setup_packet, 8);

  if (setup_received != 0U) {           // If new setup packet was received while this was being read
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointConfigure (uint8_t  ep_addr,
                                               uint8_t  ep_type,
                                               uint16_t ep_max_packet_size)
  \brief       Configure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type  Endpoint Type (ARM_USB_ENDPOINT_xxx)
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \return      \ref execution_status
*/
static int32_t USBD_EndpointConfigure (uint8_t  ep_addr,
                                       uint8_t  ep_type,
                                       uint16_t ep_max_packet_size) {
  volatile ENDPOINT_t *ptr_ep;
  uint8_t              ep_num;
  uint16_t             ep_mps;
  bool                 ep_dir;
  uint32_t             ep_reg;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) { return ARM_DRIVER_ERROR; }
  if (hw_powered == false)            { return ARM_DRIVER_ERROR; }

  ptr_ep = &ep[EP_ID(ep_addr)];
  if (ptr_ep->active != 0U)           { return ARM_DRIVER_ERROR_BUSY; }

  ep_mps =  ep_max_packet_size & ARM_USB_ENDPOINT_MAX_PACKET_SIZE_MASK;
  ep_dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) == ARM_USB_ENDPOINT_DIRECTION_MASK;

  // Check Endpoint buffer size configuration
  if ((ep_mps + EP_buff_offset[EP_ID(ep_addr)]) > EP_buff_offset[EP_ID(ep_addr) + 1]) {
    // Configured Endpoint buffer is too small
    return ARM_DRIVER_ERROR;
  }

  // Clear Endpoint transfer and configuration information
  memset((void *)(ptr_ep), 0, sizeof (ENDPOINT_t));

  // Set maximum packet size to requested
  ptr_ep->max_packet_size = ep_mps;

  if (ep_dir != 0U) {                                   // IN Endpoint
    (pBUF_DSCR + ep_num)->ADDR_TX = EP_buff_offset[EP_ID(ep_addr)];
  } else {                                              // OUT Endpoint
    (pBUF_DSCR + ep_num)->ADDR_RX = EP_buff_offset[EP_ID(ep_addr)];
    if (ep_mps > 62) {
      ep_mps = (ep_mps + 31) & ~31;
      (pBUF_DSCR + ep_num)->COUNT_RX = ((ep_mps << 5) - 1) | 0x8000;
    } else {
      ep_mps = (ep_mps + 1)  & ~1;
      (pBUF_DSCR + ep_num)->COUNT_RX =   ep_mps << 9;
    }
  }

  switch (ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
      ep_reg = EP_CONTROL;
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
      ep_reg = EP_ISOCHRONOUS;
      break;
    case ARM_USB_ENDPOINT_BULK:
      ep_reg = EP_BULK;
      break;
    case ARM_USB_ENDPOINT_INTERRUPT:
      ep_reg = EP_INTERRUPT;
      break;
  }

  if (ep_addr == 0U) {
    // Enable Endpoint 0 to receive Setup and OUT packets
    EPxREG(0) = EP_CONTROL | EP_RX_VALID;
  } else if (ep_addr != 0x80U){
    ep_reg |= ep_num;
    EPxREG(ep_num) = ep_reg;

    if (ep_dir != 0U) {                                   // IN Endpoint
      IN_EP_RESET(ep_num);
      IN_EP_Status(ep_num, EP_TX_NAK);
    } else {                                              // OUT Endpoint
      OUT_EP_RESET(ep_num);
      OUT_EP_Status(ep_num, EP_RX_NAK);
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointUnconfigure (uint8_t ep_addr)
  \brief       Unconfigure USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t USBD_EndpointUnconfigure (uint8_t ep_addr) {
  volatile ENDPOINT_t *ptr_ep;
  uint8_t              ep_num;
  bool                 ep_dir;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) { return ARM_DRIVER_ERROR; }
  if (hw_powered == false)            { return ARM_DRIVER_ERROR; }

  ptr_ep = &ep[EP_ID(ep_addr)];
  if (ptr_ep->active != 0U)           { return ARM_DRIVER_ERROR_BUSY; }

  ep_dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) == ARM_USB_ENDPOINT_DIRECTION_MASK;

  if (ep_dir != 0U) {                                   // IN Endpoint
    IN_EP_RESET(ep_num);
    IN_EP_Status(ep_num, EP_TX_DIS);
  } else {                                              // OUT Endpoint
    OUT_EP_RESET(ep_num);
    OUT_EP_Status(ep_num, EP_RX_DIS);
  }

  // Clear Endpoint transfer and configuration information
  memset((void *)(ptr_ep), 0, sizeof (ENDPOINT_t));

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointStall (uint8_t ep_addr, bool stall)
  \brief       Set/Clear Stall for USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
  \param[in]   stall  Operation
                - \b false Clear
                - \b true Set
  \return      \ref execution_status
*/
static int32_t USBD_EndpointStall (uint8_t ep_addr, bool stall) {
  volatile ENDPOINT_t *ptr_ep;
  uint8_t              ep_num;
  bool                 ep_dir;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) { return ARM_DRIVER_ERROR; }
  if (hw_powered == false)            { return ARM_DRIVER_ERROR; }

  ptr_ep = &ep[EP_ID(ep_addr)];
  if (ptr_ep->active != 0U)           { return ARM_DRIVER_ERROR_BUSY; }

  ep_dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) == ARM_USB_ENDPOINT_DIRECTION_MASK;

  if (stall == true) {
    if (ep_dir != 0U) {                                   // IN Endpoint
      IN_EP_Status(ep_num, EP_TX_STALL);                  // Stall IN Endpoint
    } else {                                              // OUT Endpoint
      OUT_EP_Status(ep_num, EP_RX_STALL);                 // Stall OUT Endpoint
    }
  } else {
    if (ep_dir != 0U) {                                   // IN Endpoint
      IN_EP_RESET(ep_num);                                // Reset DTog Bits
      IN_EP_Status(ep_num, EP_TX_NAK);                    // Clear STALL
    } else {                                              // OUT Endpoint
      OUT_EP_RESET(ep_num);                               // Reset DTog Bits
      OUT_EP_Status(ep_num, EP_RX_NAK);                   // Clear Stall
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num)
  \brief       Read data from or Write data to USB Endpoint.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
  \param[out]  data Pointer to buffer for data to read or with data to write
  \param[in]   num  Number of data bytes to transfer
  \return      \ref execution_status
*/
static int32_t USBD_EndpointTransfer (uint8_t ep_addr, uint8_t *data, uint32_t num) {
  volatile ENDPOINT_t *ptr_ep;
  uint8_t              ep_num;
  bool                 ep_dir;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) { return ARM_DRIVER_ERROR; }
  if (hw_powered == false)            { return ARM_DRIVER_ERROR; }

  ptr_ep = &ep[EP_ID(ep_addr)];
  if (ptr_ep->active != 0U)           { return ARM_DRIVER_ERROR_BUSY; }

  ep_dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) == ARM_USB_ENDPOINT_DIRECTION_MASK;

  ptr_ep->active = 1U;

  ptr_ep->data                  = data;
  ptr_ep->num                   = num;
  ptr_ep->num_transferred_total = 0U;
  ptr_ep->num_transferring      = 0U;

  if (ep_dir != 0U) {                                   // IN Endpoint
    USBD_EP_HW_Write (ep_addr);                         // Write data to Endpoint buffer
  } else {                                              // OUT Endpoint
    OUT_EP_Status(ep_num, EP_RX_VALID);                 // OUT EP able to receive data
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t USBD_EndpointTransferGetResult (uint8_t ep_addr)
  \brief       Get result of USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
  \return      number of successfully transferred data bytes
*/
static uint32_t USBD_EndpointTransferGetResult (uint8_t ep_addr) {

  if (EP_NUM(ep_addr) > USBD_MAX_ENDPOINT_NUM) { return 0U; }

  return (ep[EP_ID(ep_addr)].num_transferred_total);
}

/**
  \fn          int32_t USBD_EndpointTransferAbort (uint8_t ep_addr)
  \brief       Abort current USB Endpoint transfer.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..7: Address
                - ep_addr.7:    Direction
  \return      \ref execution_status
*/
static int32_t USBD_EndpointTransferAbort (uint8_t ep_addr) {
  volatile ENDPOINT_t *ptr_ep;
  uint8_t              ep_num;
  bool                 ep_dir;

  ep_num = EP_NUM(ep_addr);
  if (ep_num > USBD_MAX_ENDPOINT_NUM) { return ARM_DRIVER_ERROR; }
  if (hw_powered == false)            { return ARM_DRIVER_ERROR; }

  ep_dir = (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) == ARM_USB_ENDPOINT_DIRECTION_MASK;
  ptr_ep = &ep[EP_ID(ep_addr)];

  ptr_ep->num    = 0U;

  if (ep_dir != 0U) {                                   // IN Endpoint
    IN_EP_Status(ep_num, EP_TX_NAK);                    // Set NAK
  } else {                                              // OUT Endpoint
    OUT_EP_Status(ep_num, EP_RX_NAK);                   // Set NAK
  }

  ptr_ep->active = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          uint16_t USBD_GetFrameNumber (void)
  \brief       Get current USB Frame Number.
  \return      Frame Number
*/
static uint16_t USBD_GetFrameNumber (void) {

  if (hw_powered == false) { return 0U; }

  return (FNR & FNR_FN);
}

/**
  \fn          void USB_LP_CAN1_RX0_IRQHandler (void)
  \brief       USB Device Interrupt Routine (IRQ).
*/
void USB_LP_CAN1_RX0_IRQHandler (void)  {
  __packed uint16_t   *ptr_dest;
  volatile uint32_t   *ptr_src;
  volatile ENDPOINT_t *ptr_ep;
           uint32_t    istr, ep_num, val, i;

  istr = ISTR;

// Reset interrupt
  if (istr & ISTR_RESET) {
    USBD_Reset();
    SignalDeviceEvent(ARM_USBD_EVENT_RESET);
    ISTR = ~ISTR_RESET;
  }

// Suspend interrupt
  if (istr & ISTR_SUSP) {
    CNTR |= CNTR_SUSPM;
    usbd_state.active = 0U;
    SignalDeviceEvent(ARM_USBD_EVENT_SUSPEND);
    ISTR = ~ISTR_SUSP;
  }

// Resume interrupt
  if (istr & ISTR_WKUP) {
    usbd_state.active = 1U;
    CNTR &= ~CNTR_SUSPM;
    SignalDeviceEvent(ARM_USBD_EVENT_RESUME);
    ISTR = ~ISTR_WKUP;
  }

// PMA Over/underrun
  if (istr & ISTR_PMAOVR) {
    ISTR = ~ISTR_PMAOVR;
  }

// Error: No Answer, CRC Error, Bit Stuff Error, Frame Format Error
  if (istr & ISTR_ERR) {
    ISTR = ~ISTR_ERR;
  }

// Endpoint interrupts
  if ((istr = ISTR) & ISTR_CTR) {
//    ISTR = ~ISTR_CTR;

    ep_num = istr & ISTR_EP_ID;

    val = EPxREG(ep_num);
    if (val & EP_CTR_RX) {
      ptr_ep = &ep[EP_ID(ep_num)];
      EPxREG(ep_num) = val & ~EP_CTR_RX & EP_MASK;

// Setup Packet
      if (val & EP_SETUP) {
        // Read Setup Packet
        ptr_src  = (uint32_t *)(USB_PMA_ADDR + 2*((pBUF_DSCR)->ADDR_RX));
        ptr_dest = (__packed uint16_t *)(setup_packet);
        for (i = 0U; i < 4U; i++) {
          *ptr_dest++ = *ptr_src++;
        }
        setup_received = 1U;
        SignalEndpointEvent(ep_num, ARM_USBD_EVENT_SETUP);
      } else {
// OUT Packet
        USBD_EP_HW_Read(ep_num);
        if (ptr_ep->num != 0U) {
          OUT_EP_Status(ep_num, EP_RX_VALID);
        } else {
          ptr_ep->active = 0U;
          SignalEndpointEvent(ep_num, ARM_USBD_EVENT_OUT);
        }
      }
    }

// IN Packet
    if (val & EP_CTR_TX) {
      ptr_ep = &ep[EP_ID(ep_num | ARM_USB_ENDPOINT_DIRECTION_MASK)];
      EPxREG(ep_num) = val & ~EP_CTR_TX & EP_MASK;
      ptr_ep->num_transferred_total += ptr_ep->num_transferring;
      if (ptr_ep->num == 0U) {
        ptr_ep->data   = NULL;
        ptr_ep->active = 0U;
        SignalEndpointEvent(ep_num | ARM_USB_ENDPOINT_DIRECTION_MASK, ARM_USBD_EVENT_IN);
      } else {
        USBD_EP_HW_Write(ep_num | ARM_USB_ENDPOINT_DIRECTION_MASK);
      }
    }
  }
}

ARM_DRIVER_USBD Driver_USBD0 = {
  USBD_GetVersion,
  USBD_GetCapabilities,
  USBD_Initialize,
  USBD_Uninitialize,
  USBD_PowerControl,
  USBD_DeviceConnect,
  USBD_DeviceDisconnect,
  USBD_DeviceGetState,
  USBD_DeviceRemoteWakeup,
  USBD_DeviceSetAddress,
  USBD_ReadSetupPacket,
  USBD_EndpointConfigure,
  USBD_EndpointUnconfigure,
  USBD_EndpointStall,
  USBD_EndpointTransfer,
  USBD_EndpointTransferGetResult,
  USBD_EndpointTransferAbort,
  USBD_GetFrameNumber
};

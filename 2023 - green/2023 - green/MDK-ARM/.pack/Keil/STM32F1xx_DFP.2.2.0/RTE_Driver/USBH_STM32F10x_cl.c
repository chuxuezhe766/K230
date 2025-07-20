/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2016 ARM Ltd.
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
 * $Date:        4. January 2016
 * $Revision:    V2.3
 *
 * Driver:       Driver_USBH0
 * Configured:   via RTE_Device.h configuration file
 * Project:      USB Full/Low-Speed Host Driver for ST STM32F10x
 *               Connectivity Line
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                  Value
 *   ---------------------                  -----
 *   Connect to hardware via Driver_USBH# = 0
 *   USB Host controller interface        = Custom
 * --------------------------------------------------------------------------
 * Defines used for driver configuration (at compile time):
 *
 *   USBH_MAX_PIPE_NUM: defines maximum number of Pipes that driver will
 *                      support, this value impacts driver memory
 *                      requirements
 *     - default value: 8
 *     - maximum value: 8
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.3
 *    Removed interrupt priority handling
 *  Version 2.2
 *    Corrected multiple packet sending.
 *    Corrected PowerControl function for:
 *      - Unconditional Power Off
 *      - Conditional Power full (driver must be initialized)
 *  Version 2.1
 *    Update for USB Host CMSIS Driver API v2.01
 *  Version 2.0
 *    Initial release for USB Host CMSIS Driver API v2.0
 */

#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"
#include "stm32f10x.h"

#include "Driver_USBH.h"

#include "OTG_STM32F10x_cl.h"

#ifndef USBH_MAX_PIPE_NUM
#define USBH_MAX_PIPE_NUM               8U
#endif
#if    (USBH_MAX_PIPE_NUM > 8)
#error  Too many Pipes, maximum Pipes that this driver supports is 8 !!!
#endif

extern uint8_t otg_fs_role;

extern void OTG_FS_PinsConfigure   (uint8_t pins_mask);
extern void OTG_FS_PinsUnconfigure (uint8_t pins_mask);
extern void OTG_FS_PinVbusOnOff    (bool state);
extern bool OTG_FS_PinGetOC        (void);


// USBH Driver *****************************************************************

#define ARM_USBH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,3)

// Driver Version
static const ARM_DRIVER_VERSION usbh_driver_version = { ARM_USBH_API_VERSION, ARM_USBH_DRV_VERSION };

// Driver Capabilities
static const ARM_USBH_CAPABILITIES usbh_driver_capabilities = {
  0x0001U,      // Root HUB available Ports Mask
  0U,           // Automatic SPLIT packet handling
  1U,           // Signal Connect event
  1U,           // Signal Disconnect event
  0U            // Signal Overcurrent event
};

#define OTG                     OTG_FS

// FIFO sizes in bytes (total available memory for FIFOs is 1.25 kB)
#define RX_FIFO_SIZE            640U    // RxFIFO depth is half of max 1.25 kB
#define TX_FIFO_SIZE_NON_PERI   512U    // Non-periodic Tx FIFO size
#define TX_FIFO_SIZE_PERI       128U    // Periodic Tx FIFO size

static volatile uint32_t *OTG_DFIFO[] = { OTG_FS_DFIFO0,
                                          OTG_FS_DFIFO1,
                                          OTG_FS_DFIFO2,
                                          OTG_FS_DFIFO3,
                                          OTG_FS_DFIFO4,
                                          OTG_FS_DFIFO5,
                                          OTG_FS_DFIFO6,
                                          OTG_FS_DFIFO7
                                        };

typedef struct {                        // Pipe structure definition
  uint32_t  packet;
  uint8_t  *data;
  uint32_t  num;
  uint32_t  num_transferred_total;
  uint32_t  num_transferring;
  uint16_t  ep_max_packet_size;
  uint16_t  interval_reload;
  uint16_t  interval;
  uint8_t   ep_type;
  uint8_t   active;
  uint8_t   in_progress;
  uint8_t   event;
} PIPE_t;

static ARM_USBH_SignalPortEvent_t SignalPortEvent;
static ARM_USBH_SignalPipeEvent_t SignalPipeEvent;

static bool            hw_initialized = false;
static bool            hw_powered     = false;
static bool            port_reset;

// Pipes runtime information
static volatile PIPE_t pipe[USBH_MAX_PIPE_NUM];


// Auxiliary functions

/**
  \fn          uint32_t USBH_CH_GetIndexFromAddress (OTG_FS_HC *ptr_ch)
  \brief       Get the Index of Channel from it's Address.
  \param[in]   ptr_ch   Pointer to the Channel
  \return      Index of the Channel
*/
__INLINE static uint32_t USBH_CH_GetIndexFromAddress (OTG_FS_HC *ptr_ch) {
  return (ptr_ch - ((OTG_FS_HC *)(&OTG->HCCHAR0)));
}

/**
  \fn          OTG_FS_HC *USBH_CH_GetAddressFromIndex (uint32_t index)
  \brief       Get the Channel Address from it's Index.
  \param[in]   index    Index of the Channel
  \return      Address of the Channel
*/
__INLINE static OTG_FS_HC *USBH_CH_GetAddressFromIndex (uint32_t index) {
  return (((OTG_FS_HC *)(&OTG->HCCHAR0)) + index);
}

/**
  \fn          OTG_FS_HC *USBH_CH_FindFree (void)
  \brief       Find a free Channel.
  \return      Pointer to the first free Channel (NULL = no free Channel is available)
*/
__INLINE static OTG_FS_HC *USBH_CH_FindFree (void) {
  OTG_FS_HC *ptr_ch;
  uint32_t   i;

  ptr_ch = (OTG_FS_HC *)(&(OTG->HCCHAR0));

  for (i = 0U; i < USBH_MAX_PIPE_NUM; i++) {
    if ((ptr_ch->HCCHAR & 0x3FFFFFFFU) == 0U) { return ptr_ch; }
    ptr_ch++;
  }

  return NULL;
}

/**
  \fn          bool USBH_CH_Disable (OTG_FS_HC *ptr_ch)
  \brief       Disable the Channel.
  \param[in]   ptr_ch   Pointer to the Channel
  \return      true = success, false = fail
*/
__INLINE static bool USBH_CH_Disable (OTG_FS_HC *ptr_ch) {
  uint32_t i;

  if (ptr_ch == 0U) { return false; }

  if ((ptr_ch->HCINT & OTG_FS_HCINTx_CHH) != 0U) { return true; }

  if ((ptr_ch->HCCHAR & OTG_FS_HCCHARx_CHENA) != 0U) {
    ptr_ch->HCINTMSK = 0U;
    osDelay(1U);
    if ((ptr_ch->HCINT & OTG_FS_HCINTx_NAK) != 0U) {
      ptr_ch->HCINT  =  0x7BBU;
      return true;
    }
    ptr_ch->HCINT  =  0x7BBU;
    ptr_ch->HCCHAR =  ptr_ch->HCCHAR | OTG_FS_HCCHARx_CHENA | OTG_FS_HCCHARx_CHDIS;
    for (i = 0U; i < 1000U; i++) {
      if ((ptr_ch->HCINT & OTG_FS_HCINTx_CHH) != 0U) {
        ptr_ch->HCINT = 0x7BBU;
        return true;
      }
    }
    return false;
  }

  return true;
}

/**
  \fn          bool USBH_HW_StartTransfer (PIPE_t *ptr_pipe, OTG_FS_HC *ptr_ch)
  \brief       Start transfer on Pipe.
  \param[in]   ptr_pipe Pointer to Pipe
  \param[in]   ptr_ch   Pointer to Channel
  \return      true = success, false = fail
*/
static bool USBH_HW_StartTransfer (PIPE_t *ptr_pipe, OTG_FS_HC *ptr_ch) {
  uint32_t           hcchar;
  uint32_t           hctsiz;
  uint32_t           hcintmsk;
  uint32_t           txsts;
  uint32_t           pckt_num;
  uint32_t           data_num;
  uint32_t           max_pckt_size;
  uint32_t           max_data;
  uint32_t           max_num_pckt;
  uint32_t           num_to_transfer;
  uint8_t           *ptr_src;
  volatile uint32_t *ptr_dest;
  uint16_t           cnt;
  uint8_t            out;

  if (ptr_pipe == 0U)                        { return false; }
  if (ptr_ch   == 0U)                        { return false; }
  if ((OTG->HPRT & OTG_FS_HPRT_PCSTS) == 0U) { return false; }

  hcchar   = ptr_ch->HCCHAR;                    // Read channel characteristics
  hctsiz   = ptr_ch->HCTSIZ;                    // Read channel size info
  hcintmsk = 0U;
  cnt      = 0U;
  out      = 0U;

  // Prepare transfer
                                                // Prepare HCCHAR register
  hcchar       &= (OTG_FS_HCCHARx_ODDFRM    |   // Keep ODDFRM
                   OTG_FS_HCCHARx_DAD_MSK   |   // Keep DAD
                   OTG_FS_HCCHARx_MCNT_MSK  |   // Keep MCNT
                   OTG_FS_HCCHARx_EPTYP_MSK |   // Keep EPTYP
                   OTG_FS_HCCHARx_LSDEV     |   // Keep LSDEV
                   OTG_FS_HCCHARx_EPNUM_MSK |   // Keep EPNUM
                   OTG_FS_HCCHARx_MPSIZ_MSK);   // Keep MPSIZ
  hctsiz       &=  OTG_FS_HCTSIZx_DPID_MSK;     // Keep DPID
  switch (ptr_pipe->packet & ARM_USBH_PACKET_TOKEN_Msk) {
    case ARM_USBH_PACKET_IN:
      hcchar   |=  OTG_FS_HCCHARx_EPDIR;
      hcintmsk  = (OTG_FS_HCINTMSKx_DTERRM  |
                   OTG_FS_HCINTMSKx_BBERRM  |
                   OTG_FS_HCINTMSKx_TXERRM  |
                   OTG_FS_HCINTMSKx_ACKM    |
                   OTG_FS_HCINTMSKx_NAKM    |
                   OTG_FS_HCINTMSKx_STALLM  |
                   OTG_FS_HCINTMSKx_XFRCM)  ;
      break;
    case ARM_USBH_PACKET_OUT:
      hcchar   &= ~OTG_FS_HCCHARx_EPDIR;
      hcintmsk  = (OTG_FS_HCINTMSKx_TXERRM  |
//                 OTG_FS_HCINTMSKx_ACKM    |         // After ACK there must be other relevant interrupt so ACK is ignorred
                   OTG_FS_HCINTMSKx_NAKM    |
                   OTG_FS_HCINTMSKx_STALLM  |
                   OTG_FS_HCINTMSKx_XFRCM)  ;
      out       =  1U;
      break;
    case ARM_USBH_PACKET_SETUP:
      hcchar   &= ~OTG_FS_HCCHARx_EPDIR;
      hcintmsk  = (OTG_FS_HCINTMSKx_TXERRM  |
                   OTG_FS_HCINTMSKx_XFRCM)  ;
      hctsiz   &= ~OTG_FS_HCTSIZx_DPID_MSK;
      hctsiz   |=  OTG_FS_HCTSIZx_DPID_MDATA;
      out       =  1U;
      break;
    default:
      return false;
  }

  num_to_transfer = ptr_pipe->num - ptr_pipe->num_transferred_total;
  switch (ptr_pipe->ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
    case ARM_USB_ENDPOINT_BULK:
      if (out != 0U) {
        txsts = OTG->HNPTXSTS;          // Read non-periodic FIFO status
      }
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
    case ARM_USB_ENDPOINT_INTERRUPT:
      if (out != 0U) {
        txsts = OTG->HPTXSTS;           // Read non-periodic FIFO status
      }
      if ((OTG->HFNUM & 1U) != 0U) {
        hcchar &= ~OTG_FS_HCCHARx_ODDFRM;
      } else {
        hcchar |=  OTG_FS_HCCHARx_ODDFRM;
      }
      break;
  }
  if (out != 0U) {
    // For packet OUT/SETUP limit number of bytes to send to maximum FIFO size
    // and maximum number of packets
    max_pckt_size =  ptr_pipe->ep_max_packet_size;
    max_data      = (txsts & 0x0000FFFFU) <<  2;
    max_num_pckt  = (txsts & 0x00FF0000U) >> 16;
    if (num_to_transfer > max_data) {
      num_to_transfer = max_data;
    }
    data_num = num_to_transfer;
    for (pckt_num = 1; data_num > max_pckt_size; pckt_num++) {
      data_num -= max_pckt_size;
    }
    if (pckt_num > max_num_pckt) {
      pckt_num = max_num_pckt;
    }

    if (num_to_transfer > (pckt_num * max_pckt_size)) {
      num_to_transfer = pckt_num * max_pckt_size;
    }
    cnt = (num_to_transfer + 3U) / 4U;
  }

  hcchar &= ~OTG_FS_HCCHARx_CHDIS;
  hcchar |=  OTG_FS_HCCHARx_CHENA;

                                                  // Prepare HCTSIZ register
  switch (ptr_pipe->packet & ARM_USBH_PACKET_DATA_Msk) {
    case ARM_USBH_PACKET_DATA0:
      hctsiz   &= ~OTG_FS_HCTSIZx_DPID_MSK;
      hctsiz   |=  OTG_FS_HCTSIZx_DPID_DATA0;
      break;
    case ARM_USBH_PACKET_DATA1:
      hctsiz   &= ~OTG_FS_HCTSIZx_DPID_MSK;
      hctsiz   |=  OTG_FS_HCTSIZx_DPID_DATA1;
      break;
    default:
      break;
  }

                                                  // Prepare HCTSIZ register
  if (num_to_transfer != 0U) {                    // Normal packet
                                                  // Prepare PKTCNT field
    hctsiz |= ((num_to_transfer + ptr_pipe->ep_max_packet_size - 1U) / ptr_pipe->ep_max_packet_size) << 19;
    hctsiz |=   num_to_transfer;                  // Prepare XFRSIZ field
  } else {                                        // Zero length packet
    hctsiz |=   1U << 19;                         // Prepare PKTCNT field
    hctsiz |=   0U;                               // Prepare XFRSIZ field
  }

  if (cnt != 0U) {
    ptr_src  = ptr_pipe->data + ptr_pipe->num_transferred_total;
    ptr_dest = OTG_DFIFO[USBH_CH_GetIndexFromAddress (ptr_ch)];
  }
  if (out != 0U) {
    // For OUT/SETUP transfer num_transferring represents num of bytes to be sent
    ptr_pipe->num_transferring = num_to_transfer;
  } else {
    // For IN transfer num_transferring represents num of bytes received (handled in IRQ)
    ptr_pipe->num_transferring = 0U;
  }
  NVIC_DisableIRQ (OTG_FS_IRQn);                  // Disable OTG interrupt
  ptr_ch->HCINTMSK = hcintmsk;                    // Enable channel interrupts
  ptr_ch->HCTSIZ   = hctsiz;                      // Write ch transfer size
  ptr_ch->HCCHAR   = hcchar;                      // Write ch characteristics
  while (cnt != 0U) {                             // Load data
    *ptr_dest = *((__packed uint32_t *)ptr_src);
    ptr_src  += 4U;
    cnt--;
  }
  NVIC_EnableIRQ  (OTG_FS_IRQn);                  // Enable OTG interrupt

  return true;
}


// USBH Driver functions

/**
  \fn          ARM_DRIVER_VERSION USBH_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USBH_GetVersion (void) { return usbh_driver_version; }

/**
  \fn          ARM_USBH_CAPABILITIES USBH_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBH_CAPABILITIES
*/
static ARM_USBH_CAPABILITIES USBH_GetCapabilities (void) { return usbh_driver_capabilities; }

/**
  \fn          int32_t USBH_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event,
                                        ARM_USBH_SignalPipeEvent_t cb_pipe_event)
  \brief       Initialize USB Host Interface.
  \param[in]   cb_port_event  Pointer to \ref ARM_USBH_SignalPortEvent
  \param[in]   cb_pipe_event  Pointer to \ref ARM_USBH_SignalPipeEvent
  \return      \ref execution_status
*/
static int32_t USBH_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event,
                                ARM_USBH_SignalPipeEvent_t cb_pipe_event) {

  if (hw_initialized == true) { return ARM_DRIVER_OK; }

  SignalPortEvent = cb_port_event;
  SignalPipeEvent = cb_pipe_event;

  otg_fs_role = ARM_USB_ROLE_HOST;

  OTG_FS_PinsConfigure (ARM_USB_PIN_DP | ARM_USB_PIN_DM | ARM_USB_PIN_OC | ARM_USB_PIN_VBUS);

  hw_initialized = true;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_Uninitialize (void)
  \brief       De-initialize USB Host Interface.
  \return      \ref execution_status
*/
static int32_t USBH_Uninitialize (void) {

  OTG_FS_PinsUnconfigure (ARM_USB_PIN_DP | ARM_USB_PIN_DM | ARM_USB_PIN_OC | ARM_USB_PIN_VBUS);

  otg_fs_role = ARM_USB_ROLE_NONE;

  hw_initialized = false;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Host Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t USBH_PowerControl (ARM_POWER_STATE state) {

  switch (state) {
    case ARM_POWER_OFF:
      RCC->AHBENR   |=  RCC_AHBENR_OTGFSEN;             // OTG FS clock enable
      NVIC_DisableIRQ      (OTG_FS_IRQn);               // Disable interrupt
      NVIC_ClearPendingIRQ (OTG_FS_IRQn);               // Clear pending interrupt

      hw_powered     =  false;                          // Clear powered flag
      OTG->GAHBCFG  &= ~OTG_FS_GAHBCFG_GINTMSK;         // Disable USB interrupts
      RCC->AHBRSTR  |=  RCC_AHBRSTR_OTGFSRST;           // Reset OTG FS module
      port_reset     =  false;                          // Reset variables
      memset((void *)(pipe), 0, sizeof(pipe));

      OTG->GCCFG    &= ~OTG_FS_GCCFG_PWRDWN;            // Enable PHY power down
      OTG->PCGCCTL  |=  OTG_FS_PCGCCTL_STPPCLK;         // Stop PHY clock
      OTG->GCCFG     =  0U;                             // Reset core configuration

      RCC->AHBENR   &= ~RCC_AHBENR_OTGFSEN;             // Disable OTG FS clock
      break;

    case ARM_POWER_FULL:
      if (hw_initialized == false) { return ARM_DRIVER_ERROR; }
      if (hw_powered     == true)  { return ARM_DRIVER_OK;    }

      RCC->AHBENR   |=  RCC_AHBENR_OTGFSEN;             // OTG FS clock enable
      RCC->AHBRSTR  |=  RCC_AHBRSTR_OTGFSRST;           // Reset OTG FS module
      osDelay(1U);
      RCC->AHBRSTR  &= ~RCC_AHBRSTR_OTGFSRST;           // Clear reset of OTG FS module
      osDelay(1U);

      // On-chip Full-speed PHY
      OTG->PCGCCTL  &= ~OTG_FS_PCGCCTL_STPPCLK;         // Start PHY clock
      OTG->GCCFG    |=  OTG_FS_GCCFG_PWRDWN;            // Disable power down
      OTG->GUSBCFG  |=  OTG_FS_GUSBCFG_PHYSEL;          // Full-speed transceiver

      OTG->HCFG     |=  OTG_FS_HCFG_FSLSS;              // Support for FS/LS only

      // Wait until AHB Master state machine is in the idle condition
      while ((OTG->GRSTCTL & OTG_FS_GRSTCTL_AHBIDL) == 0U);

      OTG->GRSTCTL  |=  OTG_FS_GRSTCTL_CSRST;           // Core soft reset
      while ((OTG->GRSTCTL & OTG_FS_GRSTCTL_CSRST)  != 0U);
      osDelay (1U);

      // Wait until AHB Master state machine is in the idle condition
      while ((OTG->GRSTCTL & OTG_FS_GRSTCTL_AHBIDL) == 0U);

      port_reset     =  false;                          // Reset variables
      memset((void *)(pipe), 0, sizeof(pipe));

      OTG->GCCFG    &= ~OTG_FS_GCCFG_VBUSBSEN;          // Disable VBUS sensing device "B"
      OTG->GCCFG    &= ~OTG_FS_GCCFG_VBUSASEN;          // Disable VBUS sensing device "A"
      OTG->GCCFG    |=  OTG_FS_GCCFG_NOVBUSSENS;        // Disable VBUS sensing

      if (((OTG->GUSBCFG & OTG_FS_GUSBCFG_FHMOD) == 0U) || ((OTG->GUSBCFG & OTG_FS_GUSBCFG_FDMOD) != 0U)) {
        OTG->GUSBCFG &= ~OTG_FS_GUSBCFG_FDMOD;          // Clear force device mode
        OTG->GUSBCFG |=  OTG_FS_GUSBCFG_FHMOD;          // Force host mode
        osDelay (100U);
      }

      // Rx FIFO setting
      OTG->GRXFSIZ   = (RX_FIFO_SIZE/4U);
      // Non-periodic Tx FIFO setting
      OTG->HNPTXFSIZ = ((TX_FIFO_SIZE_NON_PERI/4U) << 16) |  (RX_FIFO_SIZE / 4U);
      // Periodic Tx FIFO setting
      OTG->HPTXFSIZ  = ((TX_FIFO_SIZE_PERI    /4U) << 16) | ((RX_FIFO_SIZE + TX_FIFO_SIZE_NON_PERI) / 4U);

      OTG->HAINTMSK  = (1U << USBH_MAX_PIPE_NUM) - 1U;  // Enable channel interrupts
      OTG->GINTMSK   = (OTG_FS_GINTMSK_DISCINT |        // Unmask interrupts
                        OTG_FS_GINTMSK_HCIM    |
                        OTG_FS_GINTMSK_PRTIM   |
                        OTG_FS_GINTMSK_RXFLVLM |
                        OTG_FS_GINTMSK_SOFM)   ;

      OTG->GAHBCFG  |=  OTG_FS_GAHBCFG_GINTMSK;         // Enable interrupts

      hw_powered     = true;                            // Set powered flag
      NVIC_EnableIRQ   (OTG_FS_IRQn);                   // Enable interrupt
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PortVbusOnOff (uint8_t port, bool vbus)
  \brief       Root HUB Port VBUS on/off.
  \param[in]   port  Root HUB Port Number
  \param[in]   vbus
                - \b false VBUS off
                - \b true  VBUS on
  \return      \ref execution_status
*/
static int32_t USBH_PortVbusOnOff (uint8_t port, bool vbus) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR;           }
  if (port != 0U)          { return ARM_DRIVER_ERROR_PARAMETER; }

  if (vbus != 0U) {                                     // VBUS power on
    OTG->HPRT    |=  OTG_FS_HPRT_PPWR;                  // Port power on
    OTG_FS_PinVbusOnOff (true);                         // VBUS pin on
  } else {                                              // VBUS power off
    OTG_FS_PinVbusOnOff (false);                        // VBUS pin off
    OTG->HPRT    &= ~OTG_FS_HPRT_PPWR;                  // Port power off
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PortReset (uint8_t port)
  \brief       Do Root HUB Port Reset.
  \param[in]   port  Root HUB Port Number
  \return      \ref execution_status
*/
static int32_t USBH_PortReset (uint8_t port) {
  uint32_t hprt;

  if (hw_powered == false) { return ARM_DRIVER_ERROR;           }
  if (port != 0U)          { return ARM_DRIVER_ERROR_PARAMETER; }

  port_reset = true;
  hprt  =  OTG->HPRT;
  hprt &= ~OTG_FS_HPRT_PENA;                            // Disable port
  hprt |=  OTG_FS_HPRT_PRST;                            // Port reset
  OTG->HPRT = hprt;
  osDelay (18U);
  hprt &= ~OTG_FS_HPRT_PRST;                            // Clear port reset
  OTG->HPRT = hprt;
  osDelay (50U);
  if (port_reset == true) {
    port_reset = false;
    return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PortSuspend (uint8_t port)
  \brief       Suspend Root HUB Port (stop generating SOFs).
  \param[in]   port  Root HUB Port Number
  \return      \ref execution_status
*/
static int32_t USBH_PortSuspend (uint8_t port) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR;           }
  if (port != 0U)          { return ARM_DRIVER_ERROR_PARAMETER; }

  OTG->HPRT |=  OTG_FS_HPRT_PSUSP;                      // Port suspend

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PortResume (uint8_t port)
  \brief       Resume Root HUB Port (start generating SOFs).
  \param[in]   port  Root HUB Port Number
  \return      \ref execution_status
*/
static int32_t USBH_PortResume (uint8_t port) {

  if (hw_powered == false) { return ARM_DRIVER_ERROR;           }
  if (port != 0U)          { return ARM_DRIVER_ERROR_PARAMETER; }

  OTG->HPRT |=  OTG_FS_HPRT_PRES;                       // Port resume

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USBH_PORT_STATE USBH_PortGetState (uint8_t port)
  \brief       Get current Root HUB Port State.
  \param[in]   port  Root HUB Port Number
  \return      Port State \ref ARM_USBH_PORT_STATE
*/
static ARM_USBH_PORT_STATE USBH_PortGetState (uint8_t port) {
  ARM_USBH_PORT_STATE port_state = { 0U, 0U, 0U };
  uint32_t hprt;

  if (hw_powered == false) { return port_state; }
  if (port != 0U)          { return port_state; }

  hprt = OTG->HPRT;

  port_state.connected   = (hprt & OTG_FS_HPRT_PCSTS) != 0U;
#ifdef MX_USB_OTG_FS_Overcurrent_Pin
  port_state.overcurrent = OTG_FS_PinGetOC ();
#else
  port_state.overcurrent = 0U;
#endif
  switch ((hprt & OTG_FS_HPRT_PSPD_MSK) >> OTG_FS_HPRT_PSPD_POS) {
    case 0:                                             // High speed
     break;
    case 1:                                             // Full speed
     port_state.speed = ARM_USB_SPEED_FULL;
     break;
    case 2:                                             // Low speed
     port_state.speed = ARM_USB_SPEED_LOW;
     break;
    default:
     break;
  }

  return port_state;
}

/**
  \fn          ARM_USBH_PIPE_HANDLE USBH_PipeCreate (uint8_t  dev_addr,
                                                     uint8_t  dev_speed,
                                                     uint8_t  hub_addr,
                                                     uint8_t  hub_port,
                                                     uint8_t  ep_addr,
                                                     uint8_t  ep_type,
                                                     uint16_t ep_max_packet_size,
                                                     uint8_t  ep_interval)
  \brief       Create Pipe in System.
  \param[in]   dev_addr   Device Address
  \param[in]   dev_speed  Device Speed
  \param[in]   hub_addr   Hub Address
  \param[in]   hub_port   Hub Port
  \param[in]   ep_addr    Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type    Endpoint Type (ARM_USB_ENDPOINT_xxx)
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \param[in]   ep_interval        Endpoint Polling Interval
  \return      Pipe Handle \ref ARM_USBH_PIPE_HANDLE
*/
static ARM_USBH_PIPE_HANDLE USBH_PipeCreate (uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_max_packet_size, uint8_t  ep_interval) {
  PIPE_t    *ptr_pipe;
  OTG_FS_HC *ptr_ch;

  if (hw_powered == false) { return 0U; }

  ptr_ch = USBH_CH_FindFree ();                 // Find free Channel
  if (ptr_ch == 0U) { return 0U; }              // If no free

  ptr_pipe = (PIPE_t *)(&pipe[USBH_CH_GetIndexFromAddress (ptr_ch)]);

  memset((void *)ptr_pipe, 0, sizeof(PIPE_t));  // Clear Pipes runtime information

  // Fill in all fields of Endpoint Descriptor
  ptr_ch->HCCHAR = (OTG_FS_HCCHARx_MPSIZ   (ep_max_packet_size)              ) |
                   (OTG_FS_HCCHARx_EPNUM   (ep_addr)                         ) |
                   (OTG_FS_HCCHARx_EPDIR * (((ep_addr >> 7) & 0x0001U) == 0U)) |
                   (OTG_FS_HCCHARx_LSDEV * (dev_speed == ARM_USB_SPEED_LOW)  ) |
                   (OTG_FS_HCCHARx_EPTYP   (ep_type)                         ) |
                   (OTG_FS_HCCHARx_DAD     (dev_addr)                        ) ;

  // Store Pipe settings
  ptr_pipe->ep_max_packet_size = ep_max_packet_size;
  ptr_pipe->ep_type            = ep_type;
  switch (ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
    case ARM_USB_ENDPOINT_BULK:
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
    case ARM_USB_ENDPOINT_INTERRUPT:
      if (dev_speed == ARM_USB_SPEED_HIGH) {
        if ((ep_interval > 0U) && (ep_interval <= 16U)) {
          ptr_pipe->interval_reload = (uint16_t)1U << (ep_interval - 1U);
        }
      } else if ((dev_speed == ARM_USB_SPEED_FULL) || (dev_speed == ARM_USB_SPEED_LOW)) {
        if (ep_interval > 0U) {
          ptr_pipe->interval_reload = ep_interval;
        }
      }
      ptr_pipe->interval = 0U;
      ptr_ch->HCCHAR |= OTG_FS_HCCHARx_MCNT((((ep_max_packet_size >> 11) + 1U) & 3U));
      break;
  }

  return ((ARM_USBH_EP_HANDLE)ptr_ch);
}

/**
  \fn          int32_t USBH_PipeModify (ARM_USBH_PIPE_HANDLE pipe_hndl,
                                        uint8_t              dev_addr,
                                        uint8_t              dev_speed,
                                        uint8_t              hub_addr,
                                        uint8_t              hub_port,
                                        uint16_t             ep_max_packet_size)
  \brief       Modify Pipe in System.
  \param[in]   pipe_hndl  Pipe Handle
  \param[in]   dev_addr   Device Address
  \param[in]   dev_speed  Device Speed
  \param[in]   hub_addr   Hub Address
  \param[in]   hub_port   Hub Port
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \return      \ref execution_status
*/
static int32_t USBH_PipeModify (ARM_USBH_PIPE_HANDLE pipe_hndl, uint8_t dev_addr, uint8_t dev_speed, uint8_t hub_addr, uint8_t hub_port, uint16_t ep_max_packet_size) {
  PIPE_t    *ptr_pipe;
  OTG_FS_HC *ptr_ch;
  uint32_t   hcchar;

  if (hw_powered == false)    { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U)       { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_ch   = (OTG_FS_HC *)(pipe_hndl);
  ptr_pipe = (PIPE_t    *)(&pipe[USBH_CH_GetIndexFromAddress (ptr_ch)]);
  if (ptr_pipe->active != 0U) { return ARM_DRIVER_ERROR_BUSY;      }

  // Fill in all fields of Endpoint Descriptor
  hcchar  =   ptr_ch->HCCHAR;
  hcchar &= ~(OTG_FS_HCCHARx_MPSIZ_MSK |                // Clear maximum packet size field
              OTG_FS_HCCHARx_LSDEV     |                // Clear device speed bit
              OTG_FS_HCCHARx_DAD_MSK)  ;                // Clear device address field
  hcchar |=   OTG_FS_HCCHARx_MPSIZ   (ep_max_packet_size)              |
             (OTG_FS_HCCHARx_LSDEV * (dev_speed == ARM_USB_SPEED_LOW)) |
             (OTG_FS_HCCHARx_DAD     (dev_addr))                       ;
  ptr_ch->HCCHAR = hcchar;                              // Update modified fields

  ptr_pipe->ep_max_packet_size = ep_max_packet_size;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PipeDelete (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Delete Pipe from System.
  \param[in]   pipe_hndl  Pipe Handle
  \return      \ref execution_status
*/
static int32_t USBH_PipeDelete (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  PIPE_t    *ptr_pipe;
  OTG_FS_HC *ptr_ch;

  if (hw_powered == false)    { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U)       { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_ch   = (OTG_FS_HC *)(pipe_hndl);
  ptr_pipe = (PIPE_t    *)(&pipe[USBH_CH_GetIndexFromAddress (ptr_ch)]);
  if (ptr_pipe->active != 0U) { return ARM_DRIVER_ERROR_BUSY;      }

  ptr_ch->HCCHAR   = 0U;
  ptr_ch->HCINT    = 0U;
  ptr_ch->HCINTMSK = 0U;
  ptr_ch->HCTSIZ   = 0U;

  // Clear all fields of Pipe structure
  memset((void *)ptr_pipe, 0, sizeof(PIPE_t));

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PipeReset (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Reset Pipe.
  \param[in]   pipe_hndl  Pipe Handle
  \return      \ref execution_status
*/
static int32_t USBH_PipeReset (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  PIPE_t    *ptr_pipe;
  OTG_FS_HC *ptr_ch;

  if (hw_powered == false)    { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U)       { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_ch   = (OTG_FS_HC *)(pipe_hndl);
  ptr_pipe = (PIPE_t    *)(&pipe[USBH_CH_GetIndexFromAddress (ptr_ch)]);
  if (ptr_pipe->active != 0U) { return ARM_DRIVER_ERROR_BUSY;      }

  ptr_ch->HCINT    = 0U;
  ptr_ch->HCINTMSK = 0U;
  ptr_ch->HCTSIZ   = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USBH_PipeTransfer (ARM_USBH_PIPE_HANDLE pipe_hndl,
                                          uint32_t             packet,
                                          uint8_t             *data,
                                          uint32_t             num)
  \brief       Transfer packets through USB Pipe.
  \param[in]   pipe_hndl  Pipe Handle
  \param[in]   packet     Packet information
  \param[in]   data       Pointer to buffer with data to send or for data to receive
  \param[in]   num        Number of data bytes to transfer
  \return      \ref execution_status
*/
static int32_t USBH_PipeTransfer (ARM_USBH_PIPE_HANDLE pipe_hndl, uint32_t packet, uint8_t *data, uint32_t num) {
  PIPE_t *ptr_pipe;

  if (hw_powered == false)                   { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U)                      { return ARM_DRIVER_ERROR_PARAMETER; }
  if ((OTG->HPRT & OTG_FS_HPRT_PCSTS) == 0U) { return ARM_DRIVER_ERROR;           }

  ptr_pipe = (PIPE_t *)(&pipe[USBH_CH_GetIndexFromAddress ((OTG_FS_HC *)(pipe_hndl))]);
  if (ptr_pipe->active != 0U)                         { return ARM_DRIVER_ERROR_BUSY;      }

  // Update current transfer information
  ptr_pipe->packet                = packet;
  ptr_pipe->data                  = data;
  ptr_pipe->num                   = num;
  ptr_pipe->num_transferred_total = 0U;
  ptr_pipe->num_transferring      = 0U;
  ptr_pipe->active                = 0U;
  ptr_pipe->in_progress           = 0U;
  ptr_pipe->event                 = 0U;
  if ((ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) && (ptr_pipe->interval != 0U)) {
    // Already active interrupt endpoint (it will restart in IRQ based on interval)
    ptr_pipe->active              = 1U;
  } else {
    ptr_pipe->in_progress         = 1U;
    ptr_pipe->active              = 1U;
    if (USBH_HW_StartTransfer (ptr_pipe, (OTG_FS_HC *)pipe_hndl) == 0U) {
      ptr_pipe->in_progress       = 0U;
      ptr_pipe->active            = 0U;
      return ARM_DRIVER_ERROR;
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t USBH_PipeTransferGetResult (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Get result of USB Pipe transfer.
  \param[in]   pipe_hndl  Pipe Handle
  \return      number of successfully transferred data bytes
*/
static uint32_t USBH_PipeTransferGetResult (ARM_USBH_PIPE_HANDLE pipe_hndl) {

  if (pipe_hndl == 0U) { return 0U; }

  return (pipe[USBH_CH_GetIndexFromAddress((OTG_FS_HC *)pipe_hndl)].num_transferred_total);
}

/**
  \fn          int32_t USBH_PipeTransferAbort (ARM_USBH_PIPE_HANDLE pipe_hndl)
  \brief       Abort current USB Pipe transfer.
  \param[in]   pipe_hndl  Pipe Handle
  \return      \ref execution_status
*/
static int32_t USBH_PipeTransferAbort (ARM_USBH_PIPE_HANDLE pipe_hndl) {
  PIPE_t *ptr_pipe;

  if (hw_powered == false) { return ARM_DRIVER_ERROR;           }
  if (pipe_hndl  == 0U)    { return ARM_DRIVER_ERROR_PARAMETER; }

  ptr_pipe = (PIPE_t *)(&pipe[USBH_CH_GetIndexFromAddress ((OTG_FS_HC *)(pipe_hndl))]);

  if (ptr_pipe->active != 0U) {
    ptr_pipe->active = 0U;
    if (USBH_CH_Disable((OTG_FS_HC *)(pipe_hndl)) == 0U) { return ARM_DRIVER_ERROR; }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint16_t USBH_GetFrameNumber (void)
  \brief       Get current USB Frame Number.
  \return      Frame Number
*/
static uint16_t USBH_GetFrameNumber (void) {

  if (hw_powered == false) { return 0U; }

  return ((OTG->HFNUM >> 3) & 0x7FFU);
}

/**
  \fn          void USBH_FS_IRQ (uint32_t gintsts)
  \brief       USB Host Interrupt Routine (IRQ).
*/
void USBH_FS_IRQ (uint32_t gintsts) {
  PIPE_t            *ptr_pipe;
  OTG_FS_HC         *ptr_ch;
  uint8_t           *ptr_data;
  volatile uint32_t *dfifo;
  uint32_t           hprt, haint, hcint, pktcnt, mpsiz;
  uint32_t           grxsts, bcnt, ch, dat, len, len_rest;
  uint8_t            hchalt;

  if ((gintsts & OTG_FS_GINTSTS_HPRTINT) != 0U) {       // If host port interrupt
    hprt = OTG->HPRT;
    OTG->HPRT = hprt & (~OTG_FS_HPRT_PENA);             // Leave PENA bit
    if ((hprt & OTG_FS_HPRT_PCDET) != 0U) {             // Port connect detected
      switch ((hprt >> 17) & 3U) {
        case 0:                                         // High-speed detected
          break;
        case 1:                                         // Full-speed detected
          OTG->HCFG = OTG_FS_HCFG_FSLSPCS(1U);
          break;
        case 2:                                         // Low-speed detected
          OTG->HCFG = OTG_FS_HCFG_FSLSPCS(2U);
          break;
        default:
          break;
      }
      if (port_reset == false) {                        // If port not under reset
        SignalPortEvent(0, ARM_USBH_EVENT_CONNECT);
      }
    }
    if ((hprt & OTG_FS_HPRT_PENCHNG) != 0U) {           // If port enable changed
      if ((hprt & OTG_FS_HPRT_PENA) != 0U) {            // If device connected
        if (port_reset == true) {
          port_reset = false;
          SignalPortEvent(0, ARM_USBH_EVENT_RESET);
        }
      }
    }
  }
  if ((gintsts & OTG_FS_GINTSTS_DISCINT) != 0U) {       // If device disconnected
    OTG->GINTSTS = OTG_FS_GINTSTS_DISCINT;              // Clear disconnect interrupt
    if (port_reset == false) {                          // Ignore disconnect under reset
      ptr_ch   = (OTG_FS_HC *)(&OTG->HCCHAR0);
      ptr_pipe = (PIPE_t    *)(pipe);
      for (ch = 0U; ch < USBH_MAX_PIPE_NUM; ch++) {
        if (ptr_pipe->active != 0U) {
          ptr_pipe->active = 0U;
          SignalPipeEvent((ARM_USBH_EP_HANDLE)ptr_ch, ARM_USBH_EVENT_BUS_ERROR);
        }
        ptr_ch++;
        ptr_pipe++;
      }
      SignalPortEvent(0, ARM_USBH_EVENT_DISCONNECT);
    }
  }
                                                        // Handle reception interrupt
  if ((gintsts & OTG_FS_GINTSTS_RXFLVL) != 0U) {        // If RXFIFO non-empty interrupt
    OTG->GINTMSK &= ~OTG_FS_GINTMSK_RXFLVLM;
    grxsts = OTG->GRXSTSR;
    if (((grxsts >> 17) & 0x0FU) == 0x02U) {            // If PKTSTS = 0x02
      grxsts     = (OTG->GRXSTSP);
      ch         = (grxsts     ) & 0x00FU;
      bcnt       = (grxsts >> 4) & 0x7FFU;
      dfifo      =  OTG_DFIFO[ch];
      ptr_data   =  pipe[ch].data + pipe[ch].num_transferred_total;
      len        =  bcnt / 4U;                          // Received number of 32-bit data
      len_rest   =  bcnt & 3U;                          // Number of bytes left
      while (len != 0U) {
        *((__packed uint32_t *)ptr_data) = *dfifo;
        ptr_data += 4U;
        len--;
      }
      if (len_rest != 0U) {
        dat = *((__packed uint32_t *)dfifo);
        while (len_rest != 0U) {
          *ptr_data++ = dat;
          dat >>= 8;
          len_rest--;
        }
      }
      pipe[ch].num_transferring      += bcnt;
      pipe[ch].num_transferred_total += bcnt;
    } else {                                            // If PKTSTS != 0x02
      grxsts      = OTG->GRXSTSP;
    }
    OTG->GINTMSK |= OTG_FS_GINTMSK_RXFLVLM;
  }
                                                        // Handle host ctrl interrupt
  if ((gintsts & OTG_FS_GINTSTS_HCINT) != 0U) {         // If host channel interrupt
    haint = OTG->HAINT;
    for (ch = 0U; ch < USBH_MAX_PIPE_NUM; ch++) {
      if (haint == 0U) { break; }
      if ((haint & (1U << ch)) != 0U) {                 // If channels interrupt active
        haint     &= ~(1U << ch);
        ptr_ch     =  (OTG_FS_HC *)(&OTG->HCCHAR0) + ch;
        ptr_pipe   =  (PIPE_t    *)(&pipe[ch]);
        hcint      =   ptr_ch->HCINT & ptr_ch->HCINTMSK;
        hchalt     =   0U;
        if ((hcint & OTG_FS_HCINTx_CHH) != 0U) {        // If channel halted
          ptr_ch->HCINT    = OTG_FS_HCINTx_CHH;         // Clear halt interrupt
          ptr_ch->HCINTMSK = 0U;                        // Disable all channel interrupts
          ptr_ch->HCINT    = 0x7BBU;                    // Clear all interrupts
          ptr_pipe->in_progress = 0U;                   // Transfer not in progress
        } else if ((hcint & OTG_FS_HCINTx_XFRC) != 0U) {// If data transfer finished
          ptr_ch->HCINT   = 0x7BBU;                     // Clear all interrupts
          if ((ptr_ch->HCCHAR & (1U << 15)) != 0U) {    // If endpoint IN
            ptr_pipe->event = ARM_USBH_EVENT_TRANSFER_COMPLETE;
          } else {                                      // If endpoint OUT
            ptr_pipe->num_transferred_total += ptr_pipe->num_transferring;
            ptr_pipe->num_transferring       = 0U;
            if (ptr_pipe->num_transferred_total == ptr_pipe->num) {
              ptr_pipe->event = ARM_USBH_EVENT_TRANSFER_COMPLETE;
            }
          }
          hchalt = 1U;
        } else {
          if ((hcint & OTG_FS_HCINTx_ACK) != 0U) {      // If ACK received
            ptr_ch->HCINT = OTG_FS_HCINTx_ACK;          // Clear ACK interrupt
            // On ACK, ACK is not an event that can be returned so if transfer
            // is completed another interrupt will happen otherwise for IN
            // endpoint transfer will be restarted for remaining data
            if ((ptr_ch->HCCHAR & (1U << 15)) != 0U) {  // If endpoint IN
              if ((ptr_pipe->num != ptr_pipe->num_transferred_total) &&                   // If all data was not transferred
                  (ptr_pipe->num_transferring != 0U)                 &&                   // If zero-length packet was not received
                 ((ptr_pipe->num_transferred_total%ptr_pipe->ep_max_packet_size) == 0U)){ // If short packet was not received
                ptr_ch->HCCHAR |= OTG_FS_HCCHARx_CHENA;
              }
            } else {                                    // If endpoint OUT
              hchalt = 1U;
            }
          } else if ((hcint & (OTG_FS_HCINTx_STALL |            // If STALL received
                               OTG_FS_HCINTx_NAK   |            // If NAK received
                               OTG_FS_HCINTx_ERR   )) != 0U) {  // If any error occurred
            if ((ptr_ch->HCCHAR & (1U << 15)) == 0U) {          // If endpoint OUT
              // Update transferred count
              pktcnt = (ptr_ch->HCTSIZ >> 19) & 0x000003FFU;
              mpsiz  = (ptr_ch->HCCHAR      ) & 0x000007FFU;
              if ((ptr_pipe->num_transferring >= mpsiz) && (pktcnt > 0U)) {
                ptr_pipe->num_transferred_total += ptr_pipe->num_transferring - mpsiz * pktcnt;
              }
              ptr_pipe->num_transferring       = 0U;
            }
            if ((hcint & OTG_FS_HCINTx_NAK)!=0U){       // If NAK received
              ptr_ch->HCINT = OTG_FS_HCINTx_NAK;        // Clear NAK interrupt
              // On NAK, NAK is not returned to middle layer but transfer is
              // restarted from driver for remaining data, unless it is interrupt
              // endpoint in which case transfer is restarted on SOF event
              // when period has expired
              if ((ptr_ch->HCCHAR & (1U << 15)) != 0U) {// If endpoint IN
                if (ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) {
                  hchalt = 1U;
                } else {
                  ptr_ch->HCCHAR |= OTG_FS_HCCHARx_CHENA;
                }
              } else {                                  // If endpoint OUT
                hchalt = 1U;
              }
            } else if ((hcint&OTG_FS_HCINTx_STALL)!=0U){// If STALL received
              ptr_ch->HCINT   = OTG_FS_HCINTx_STALL;    // Clear STALL interrupt
              ptr_pipe->event = ARM_USBH_EVENT_HANDSHAKE_STALL;
              hchalt = 1U;
            } else {
              ptr_ch->HCINT   = OTG_FS_HCINTx_ERR;      // Clear all error interrupts
              ptr_pipe->event = ARM_USBH_EVENT_BUS_ERROR;
              hchalt = 1U;
            }
          }
        }
        if (hchalt != 0U) {                             // If channel should be halted
          ptr_ch->HCINTMSK = OTG_FS_HCINTx_CHH;         // Enable halt interrupt
          ptr_ch->HCCHAR  |= OTG_FS_HCCHARx_CHENA | OTG_FS_HCCHARx_CHDIS;
        }
        if ((ptr_pipe->in_progress == 0U) && (ptr_pipe->event != 0U)) {
          ptr_pipe->active = 0U;
          SignalPipeEvent((ARM_USBH_EP_HANDLE)ptr_ch, ptr_pipe->event);
        }
      }
    }
  }

  // Handle periodic transfer timings
  if ((gintsts & OTG_FS_GINTSTS_SOF) != 0U) {           // If start of frame interrupt
    OTG->GINTSTS =  OTG_FS_GINTSTS_SOF;                 // Clear SOF interrupt
    ptr_pipe     = (PIPE_t *)(pipe);
    for (ch = 0U; ch < USBH_MAX_PIPE_NUM; ch++) {
      // If interrupt transfer is active handle period (interval)
      if ((ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) && (ptr_pipe->active != 0U) && (ptr_pipe->interval != 0U)) {
        ptr_pipe->interval--;
      }
      ptr_pipe++;
    }
  }

  // Handle restarts of unfinished transfers (due to NAK or ACK)
  ptr_pipe = (PIPE_t *)(pipe);
  for (ch = 0U; ch < USBH_MAX_PIPE_NUM; ch++) {
    if ((ptr_pipe->active != 0U) && (ptr_pipe->in_progress == 0U)) {
      // Restart periodic transfer if not in progress and interval expired
      if (ptr_pipe->ep_type == ARM_USB_ENDPOINT_INTERRUPT) {
        if (ptr_pipe->interval == 0U) {
          ptr_pipe->interval = ptr_pipe->interval_reload;
        } else {
          continue;
        }
      }

      ptr_pipe->in_progress = 1;
      if (USBH_HW_StartTransfer (ptr_pipe, USBH_CH_GetAddressFromIndex (ch)) == 0U) {
        ptr_pipe->in_progress = 0U;
        ptr_pipe->active      = 0U;
      }
    }
    ptr_pipe++;
  }
}

ARM_DRIVER_USBH Driver_USBH0 = {
  USBH_GetVersion,
  USBH_GetCapabilities,
  USBH_Initialize,
  USBH_Uninitialize,
  USBH_PowerControl,
  USBH_PortVbusOnOff,
  USBH_PortReset,
  USBH_PortSuspend,
  USBH_PortResume,
  USBH_PortGetState,
  USBH_PipeCreate,
  USBH_PipeModify,
  USBH_PipeDelete,
  USBH_PipeReset,
  USBH_PipeTransfer,
  USBH_PipeTransferGetResult,
  USBH_PipeTransferAbort,
  USBH_GetFrameNumber
};

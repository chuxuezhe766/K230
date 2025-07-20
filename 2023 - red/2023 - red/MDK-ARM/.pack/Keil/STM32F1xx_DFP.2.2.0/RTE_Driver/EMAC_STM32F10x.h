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
 * Project:      Ethernet Media Access (MAC) Definitions for STM32F10x
 * -------------------------------------------------------------------------- */

#ifndef __EMAC_STM32F10X_H
#define __EMAC_STM32F10X_H

#include <string.h>

#include "Driver_ETH_MAC.h"
#include "stm32f10x.h"

#include "RTE_Components.h"
#include "RTE_Device.h"

#if (defined(RTE_Drivers_ETH_MAC0) && !RTE_ETH)
  #error "Ethernet not configured in RTE_Device.h!"
#endif

#if (RTE_ETH_MII && RTE_ETH_RMII)
#error "Ethernet interface configuration in RTE_Device.h is invalid!"
#endif

/* MDC management clock prescaler */
#if   (RTE_HCLK >= 60000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div42
#elif (RTE_HCLK >= 35000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div26
#elif (RTE_HCLK >= 20000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div16
#error "HCLK too slow for Ethernet! Check settings in RTE_Device.h!"
#endif

#define ETH_MDC_GPIOx        RTE_ETH_MDI_MDC_PORT
#define ETH_MDC_GPIO_Pin     RTE_ETH_MDI_MDC_PIN
#define ETH_MDIO_GPIOx       RTE_ETH_MDI_MDIO_PORT
#define ETH_MDIO_GPIO_Pin    RTE_ETH_MDI_MDIO_PIN

#if (RTE_ETH_MII)
  #define ETH_MII                 1

  #define ETH_TXD0_GPIOx       RTE_ETH_MII_TXD0_PORT
  #define ETH_TXD0_GPIO_Pin    RTE_ETH_MII_TXD0_PIN
  #define ETH_TXD1_GPIOx       RTE_ETH_MII_TXD1_PORT
  #define ETH_TXD1_GPIO_Pin    RTE_ETH_MII_TXD1_PIN
  #define ETH_TXD2_GPIOx       RTE_ETH_MII_TXD2_PORT
  #define ETH_TXD2_GPIO_Pin    RTE_ETH_MII_TXD2_PIN
  #define ETH_TXD3_GPIOx       RTE_ETH_MII_TXD3_PORT
  #define ETH_TXD3_GPIO_Pin    RTE_ETH_MII_TXD3_PIN
  #define ETH_RXD0_GPIOx       RTE_ETH_MII_RXD0_PORT
  #define ETH_RXD0_GPIO_Pin    RTE_ETH_MII_RXD0_PIN
  #define ETH_RXD1_GPIOx       RTE_ETH_MII_RXD1_PORT
  #define ETH_RXD1_GPIO_Pin    RTE_ETH_MII_RXD1_PIN
  #define ETH_RXD2_GPIOx       RTE_ETH_MII_RXD2_PORT
  #define ETH_RXD2_GPIO_Pin    RTE_ETH_MII_RXD2_PIN
  #define ETH_RXD3_GPIOx       RTE_ETH_MII_RXD3_PORT
  #define ETH_RXD3_GPIO_Pin    RTE_ETH_MII_RXD3_PIN
  #define ETH_TX_EN_GPIOx      RTE_ETH_MII_TX_EN_PORT
  #define ETH_TX_EN_GPIO_Pin   RTE_ETH_MII_TX_EN_PIN
  #define ETH_TX_CLK_GPIOx     RTE_ETH_MII_TX_CLK_PORT
  #define ETH_TX_CLK_GPIO_Pin  RTE_ETH_MII_TX_CLK_PIN
  #define ETH_RX_CLK_GPIOx     RTE_ETH_MII_RX_CLK_PORT
  #define ETH_RX_CLK_GPIO_Pin  RTE_ETH_MII_RX_CLK_PIN
  #define ETH_CRS_GPIOx        RTE_ETH_MII_CRS_PORT
  #define ETH_CRS_GPIO_Pin     RTE_ETH_MII_CRS_PIN
  #define ETH_COL_GPIOx        RTE_ETH_MII_COL_PORT
  #define ETH_COL_GPIO_Pin     RTE_ETH_MII_COL_PIN
  #define ETH_RX_DV_GPIOx      RTE_ETH_MII_RX_DV_PORT
  #define ETH_RX_DV_GPIO_Pin   RTE_ETH_MII_RX_DV_PIN
  #define ETH_RX_ER_GPIOx      RTE_ETH_MII_RX_ER_PORT
  #define ETH_RX_ER_GPIO_Pin   RTE_ETH_MII_RX_ER_PIN

#else
  #define ETH_MII                 0

  #define ETH_TXD0_GPIOx       RTE_ETH_RMII_TXD0_PORT
  #define ETH_TXD0_GPIO_Pin    RTE_ETH_RMII_TXD0_PIN
  #define ETH_TXD1_GPIOx       RTE_ETH_RMII_TXD1_PORT
  #define ETH_TXD1_GPIO_Pin    RTE_ETH_RMII_TXD1_PIN
  #define ETH_RXD0_GPIOx       RTE_ETH_RMII_RXD0_PORT
  #define ETH_RXD0_GPIO_Pin    RTE_ETH_RMII_RXD0_PIN
  #define ETH_RXD1_GPIOx       RTE_ETH_RMII_RXD1_PORT
  #define ETH_RXD1_GPIO_Pin    RTE_ETH_RMII_RXD1_PIN
  #define ETH_TX_EN_GPIOx      RTE_ETH_RMII_TX_EN_PORT
  #define ETH_TX_EN_GPIO_Pin   RTE_ETH_RMII_TX_EN_PIN
  #define ETH_REF_CLK_GPIOx    RTE_ETH_RMII_REF_CLK_PORT
  #define ETH_REF_CLK_GPIO_Pin RTE_ETH_RMII_REF_CLK_PIN
  #define ETH_CRS_DV_GPIOx     RTE_ETH_RMII_CRS_DV_PORT
  #define ETH_CRS_DV_GPIO_Pin  RTE_ETH_RMII_CRS_DV_PIN

#endif /* RTE_ETH_RMII */


/* EMAC Driver state flags */
#define EMAC_FLAG_INIT      (1 << 0)    // Driver initialized
#define EMAC_FLAG_POWER     (1 << 1)    // Driver power on

/* PTP subsecond increment value */
#define PTPSSIR_Val(hclk)     ((0x7FFFFFFFU + (hclk)/2U) / (hclk))

/* TDES0 - DMA Descriptor TX Packet Control/Status */
#define DMA_TX_OWN      0x80000000U     // Own bit 1=DMA,0=CPU
#define DMA_TX_IC       0x40000000U     // Interrupt on completition
#define DMA_TX_LS       0x20000000U     // Last segment
#define DMA_TX_FS       0x10000000U     // First segment
#define DMA_TX_DC       0x08000000U     // Disable CRC
#define DMA_TX_DP       0x04000000U     // Disable pad
#define DMA_TX_TTSE     0x02000000U     // Transmit time stamp enable
#define DMA_TX_CIC      0x00C00000U     // Checksum insertion control
#define DMA_TX_TER      0x00200000U     // Transmit end of ring
#define DMA_TX_TCH      0x00100000U     // Second address chained
#define DMA_TX_TTSS     0x00020000U     // Transmit time stamp status
#define DMA_TX_IHE      0x00010000U     // IP header error status
#define DMA_TX_ES       0x00008000U     // Error summary
#define DMA_TX_JT       0x00004000U     // Jabber timeout
#define DMA_TX_FF       0x00002000U     // Frame flushed
#define DMA_TX_IPE      0x00001000U     // IP payload error
#define DMA_TX_LC       0x00000800U     // Loss of carrier
#define DMA_TX_NC       0x00000400U     // No carrier
#define DMA_TX_LCOL     0x00000200U     // Late collision
#define DMA_TX_EC       0x00000100U     // Excessive collision
#define DMA_TX_VF       0x00000080U     // VLAN frame
#define DMA_TX_CC       0x00000078U     // Collision count
#define DMA_TX_ED       0x00000004U     // Excessive deferral
#define DMA_TX_UF       0x00000002U     // Underflow error
#define DMA_TX_DB       0x00000001U     // Deferred bit

/* TDES1 - DMA Descriptor TX Packet Control */
#define DMA_RX_TBS2     0x1FFF0000U     // Transmit buffer 2 size
#define DMA_RX_TBS1     0x00001FFFU     // Transmit buffer 1 size

/* RDES0 - DMA Descriptor RX Packet Status */
#define DMA_RX_OWN      0x80000000U     // Own bit 1=DMA,0=CPU
#define DMA_RX_AFM      0x40000000U     // Destination address filter fail
#define DMA_RX_FL       0x3FFF0000U     // Frame length mask
#define DMA_RX_ES       0x00008000U     // Error summary
#define DMA_RX_DE       0x00004000U     // Descriptor error
#define DMA_RX_SAF      0x00002000U     // Source address filter fail
#define DMA_RX_LE       0x00001000U     // Length error
#define DMA_RX_OE       0x00000800U     // Overflow error
#define DMA_RX_VLAN     0x00000400U     // VLAN tag
#define DMA_RX_FS       0x00000200U     // First descriptor
#define DMA_RX_LS       0x00000100U     // Last descriptor
#define DMA_RX_IPHCE    0x00000080U     // IPv4 header checksum error
#define DMA_RX_LC       0x00000040U     // late collision
#define DMA_RX_FT       0x00000020U     // Frame type
#define DMA_RX_RWT      0x00000010U     // Receive watchdog timeout
#define DMA_RX_RE       0x00000008U     // Receive error
#define DMA_RX_DRE      0x00000004U     // Dribble bit error
#define DMA_RX_CE       0x00000002U     // CRC error
#define DMA_RX_RMAM     0x00000001U     // Rx MAC adr.match/payload cks.error

/* RDES1 - DMA Descriptor RX Packet Control */
#define DMA_RX_DIC      0x80000000U     // Disable interrupt on completion
#define DMA_RX_RBS2     0x1FFF0000U     // Receive buffer 2 size
#define DMA_RX_RER      0x00008000U     // Receive end of ring
#define DMA_RX_RCH      0x00004000U     // Second address chained
#define DMA_RX_RBS1     0x00001FFFU     // Receive buffer 1 size

/* EMAC DMA RX Descriptor */
typedef struct rx_desc {
  uint32_t volatile Stat;
  uint32_t          Ctrl;
  uint8_t  const   *Addr;
  struct rx_desc   *Next;
#if ((EMAC_CHECKSUM_OFFLOAD != 0) || (EMAC_TIME_STAMP != 0))
  uint32_t          ExtStat;
  uint32_t          Reserved[1];
  uint32_t          TimeLo;
  uint32_t          TimeHi;
#endif
} RX_Desc;

/* EMAC DMA TX Descriptor */
typedef struct tx_desc {
  uint32_t volatile CtrlStat;
  uint32_t          Size;
  uint8_t          *Addr;
  struct tx_desc   *Next;
#if ((EMAC_CHECKSUM_OFFLOAD != 0) || (EMAC_TIME_STAMP != 0))
  uint32_t          Reserved[2];
  uint32_t          TimeLo;
  uint32_t          TimeHi;
#endif
} TX_Desc;

/* EMAC Pin Descriptor */
typedef struct _ETH_PIN {
  GPIO_TypeDef *port;
  uint16_t      pin;
} ETH_PIN;

/* EMAC Driver Control Information */
typedef struct {
  ARM_ETH_MAC_SignalEvent_t cb_event;   // Event callback
  uint8_t       flags;                  // Control and state flags
  uint8_t       tx_index;               // Transmit descriptor index
  uint8_t       rx_index;               // Receive descriptor index
#if (EMAC_CHECKSUM_OFFLOAD)
  bool          tx_cks_offload;         // Checksum offload enabled/disabled
#endif
#if (EMAC_TIME_STAMP)
  uint8_t       tx_ts_index;            // Transmit Timestamp descriptor index
#endif
  uint8_t      *frame_end;              // End of assembled frame fragments
} EMAC_CTRL;

#endif /* __EMAC_STM32F10X_H */

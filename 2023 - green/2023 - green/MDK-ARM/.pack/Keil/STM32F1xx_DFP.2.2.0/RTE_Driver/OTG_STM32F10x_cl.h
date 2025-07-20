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
 * $Date:        14. January 2016
 * $Revision:    V1.2
 *
 * Project:      OTG Full/Low-Speed Driver Header for ST STM32F1xx
 * -------------------------------------------------------------------------- */

#include <stdint.h>

#include "RTE_Components.h"
#include "RTE_Device.h"

// Do basic RTE configuration check
#if    (RTE_USB_OTG_FS == 1)
#if   ((RTE_USB_OTG_FS_HOST == 1) && (RTE_OTG_FS_VBUS_PIN == 0))
#error  Configure VBUS Power On/Off pin for USB OTG Full-speed in RTE_Device.h!
#endif
#else
#error  Enable USB OTG Full-speed in RTE_Device.h!
#endif

// If Framework Classic is used create MX defines from RTE defines
#ifdef RTE_Drivers_USBD0
#define MX_USB_OTG_FS_DEVICE                    1
#endif
#ifdef RTE_Drivers_USBH0
#define MX_USB_OTG_FS_HOST                      1
#endif

#if    ((RTE_USB_OTG_FS_HOST == 1) && (RTE_OTG_FS_VBUS_PIN == 1))
// Pin USB_OTG_FS_VBUS_Power : GPIO pin
#define MX_USB_OTG_FS_VBUS_Power_Pin            1U
#define MX_USB_OTG_FS_VBUS_Power_GPIOx          RTE_OTG_FS_VBUS_PORT
#define MX_USB_OTG_FS_VBUS_Power_GPIO_Pin       RTE_OTG_FS_VBUS_BIT
#define MX_USB_OTG_FS_VBUS_Power_Pin_Active     RTE_OTG_FS_VBUS_ACTIVE
#endif

#if    ((RTE_USB_OTG_FS_HOST == 1) && (RTE_OTG_FS_OC_PIN == 1))
// Pin USB_OTG_FS_Overcurrent : GPIO pin
#define MX_USB_OTG_FS_Overcurrent_Pin           1
#define MX_USB_OTG_FS_Overcurrent_GPIOx         RTE_OTG_FS_OC_PORT
#define MX_USB_OTG_FS_Overcurrent_GPIO_Pin      RTE_OTG_FS_OC_BIT
#define MX_USB_OTG_FS_Overcurrent_Pin_Active    RTE_OTG_FS_OC_ACTIVE
#endif

#pragma push
#pragma anon_unions

// OTG_FS
typedef struct
{
  __IO uint32_t GOTGCTL;
  __IO uint32_t GOTGINT;
  __IO uint32_t GAHBCFG;
  __IO uint32_t GUSBCFG;
  __IO uint32_t GRSTCTL;
  __IO uint32_t GINTSTS;
  __IO uint32_t GINTMSK;
  __IO uint32_t GRXSTSR;
  __IO uint32_t GRXSTSP;
  __IO uint32_t GRXFSIZ;
  union {
    __IO uint32_t HNPTXFSIZ;
    __IO uint32_t DIEPTXF0;
  };
  __IO uint32_t HNPTXSTS;
       uint32_t RESERVED0[2];
  __IO uint32_t GCCFG;
  __IO uint32_t CID;
       uint32_t RESERVED1[48];
  __IO uint32_t HPTXFSIZ;
  __IO uint32_t DIEPTXF1;
  __IO uint32_t DIEPTXF2;
  __IO uint32_t DIEPTXF3;
  __IO uint32_t DIEPTXF4;
       uint32_t RESERVED2[187];
  __IO uint32_t HCFG;
  __IO uint32_t HFIR;
  __IO uint32_t HFNUM;
       uint32_t RESERVED3;
  __IO uint32_t HPTXSTS;
  __IO uint32_t HAINT;
  __IO uint32_t HAINTMSK;
       uint32_t RESERVED4[9];
  __IO uint32_t HPRT;
       uint32_t RESERVED5[47];
  __IO uint32_t HCCHAR0;
       uint32_t RESERVED6;
  __IO uint32_t HCINT0;
  __IO uint32_t HCINTMSK0;
  __IO uint32_t HCTSIZ0;
       uint32_t RESERVED7[3];
  __IO uint32_t HCCHAR1;
       uint32_t RESERVED8;
  __IO uint32_t HCINT1;
  __IO uint32_t HCINTMSK1;
  __IO uint32_t HCTSIZ1;
       uint32_t RESERVED9[3];
  __IO uint32_t HCCHAR2;
       uint32_t RESERVED10;
  __IO uint32_t HCINT2;
  __IO uint32_t HCINTMSK2;
  __IO uint32_t HCTSIZ2;
       uint32_t RESERVED11[3];
  __IO uint32_t HCCHR3;
       uint32_t RESERVED12;
  __IO uint32_t HCINT3;
  __IO uint32_t HCINTMSK3;
  __IO uint32_t HCTSIZ3;
       uint32_t RESERVED13[3];
  __IO uint32_t HCCHR4;
       uint32_t RESERVED14;
  __IO uint32_t HCINT4;
  __IO uint32_t HCINTMSK4;
  __IO uint32_t HCTSIZ4;
       uint32_t RESERVED15[3];
  __IO uint32_t HCCHR5;
       uint32_t RESERVED16;
  __IO uint32_t HCINT5;
  __IO uint32_t HCINTMSK5;
  __IO uint32_t HCTSIZ5;
       uint32_t RESERVED17[3];
  __IO uint32_t HCCHR6;
       uint32_t RESERVED18;
  __IO uint32_t HCINT6;
  __IO uint32_t HCINTMSK6;
  __IO uint32_t HCTSIZ6;
       uint32_t RESERVED19[3];
  __IO uint32_t HCCHR7;
       uint32_t RESERVED20;
  __IO uint32_t HCINT7;
  __IO uint32_t HCINTMSK7;
  __IO uint32_t HCTSIZ7;
       uint32_t RESERVED21[3];
  __IO uint32_t HCCHR8;
       uint32_t RESERVED22;
  __IO uint32_t HCINT8;
  __IO uint32_t HCINTMSK8;
  __IO uint32_t HCTSIZ8;
       uint32_t RESERVED23[3];
  __IO uint32_t HCCHR9;
       uint32_t RESERVED24;
  __IO uint32_t HCINT9;
  __IO uint32_t HCINTMSK9;
  __IO uint32_t HCTSIZ9;
       uint32_t RESERVED25[3];
  __IO uint32_t HCCHR10;
       uint32_t RESERVED26;
  __IO uint32_t HCINT10;
  __IO uint32_t HCINTMSK10;
  __IO uint32_t HCTSIZ10;
       uint32_t RESERVED27[3];
  __IO uint32_t HCCHR11;
       uint32_t RESERVED28;
  __IO uint32_t HCINT11;
  __IO uint32_t HCINTMSK11;
  __IO uint32_t HCTSIZ11;
       uint32_t RESERVED29[3];
  __IO uint32_t HCCHR12;
       uint32_t RESERVED30;
  __IO uint32_t HCINT12;
  __IO uint32_t HCINTMSK12;
  __IO uint32_t HCTSIZ12;
       uint32_t RESERVED31[3];
  __IO uint32_t HCCHR13;
       uint32_t RESERVED32;
  __IO uint32_t HCINT13;
  __IO uint32_t HCINTMSK13;
  __IO uint32_t HCTSIZ13;
       uint32_t RESERVED33[3];
  __IO uint32_t HCCHR14;
       uint32_t RESERVED34;
  __IO uint32_t HCINT14;
  __IO uint32_t HCINTMSK14;
  __IO uint32_t HCTSIZ14;
       uint32_t RESERVED35[3];
  __IO uint32_t HCCHR15;
       uint32_t RESERVED36;
  __IO uint32_t HCINT15;
  __IO uint32_t HCINTMSK15;
  __IO uint32_t HCTSIZ15;
       uint32_t RESERVED37[3];
       uint32_t RESERVED38[64];
  __IO uint32_t DCFG;
  __IO uint32_t DCTL;
  __IO uint32_t DSTS;
       uint32_t RESERVED39;
  __IO uint32_t DIEPMSK;
  __IO uint32_t DOEPMSK;
  __IO uint32_t DAINT;
  __IO uint32_t DAINTMSK;
       uint32_t RESERVED40[2];
  __IO uint32_t DVBUSDIS;
  __IO uint32_t DVBUSPULSE;
       uint32_t RESERVED41;
  __IO uint32_t DIEPEMPMSK;
       uint32_t RESERVED42[50];
  __IO uint32_t DIEPCTL0;
       uint32_t RESERVED43;
  __IO uint32_t DIEPINT0;
       uint32_t RESERVED44;
  __IO uint32_t DIEPTSIZ0;
       uint32_t RESERVED45;
  __IO uint32_t DTXFSTS0;
       uint32_t RESERVED46;
  __IO uint32_t DIEPCTL1;
       uint32_t RESERVED47;
  __IO uint32_t DIEPINT1;
       uint32_t RESERVED48;
  __IO uint32_t DIEPTSIZ1;
       uint32_t RESERVED49;
  __IO uint32_t DTXFSTS1;
       uint32_t RESERVED50;
  __IO uint32_t DIEPCTL2;
       uint32_t RESERVED51;
  __IO uint32_t DIEPINT2;
       uint32_t RESERVED52;
  __IO uint32_t DIEPTSIZ2;
       uint32_t RESERVED53;
  __IO uint32_t DTXFSTS2;
       uint32_t RESERVED54;
  __IO uint32_t DIEPCTL3;
       uint32_t RESERVED55;
  __IO uint32_t DIEPINT3;
       uint32_t RESERVED56;
  __IO uint32_t DIEPTSIZ3;
       uint32_t RESERVED57;
  __IO uint32_t DTXFSTS3;
       uint32_t RESERVED58;
  __IO uint32_t DIEPCTL4;
       uint32_t RESERVED59;
  __IO uint32_t DIEPINT4;
       uint32_t RESERVED60;
  __IO uint32_t DIEPTSIZ4;
       uint32_t RESERVED61;
  __IO uint32_t DTXFSTS4;
       uint32_t RESERVED62;
  __IO uint32_t DIEPCTL5;
       uint32_t RESERVED63;
  __IO uint32_t DIEPINT5;
       uint32_t RESERVED64;
  __IO uint32_t DIEPTSIZ5;
       uint32_t RESERVED65[3];
  __IO uint32_t DIEPCTL6;
       uint32_t RESERVED66;
  __IO uint32_t DIEPINT6;
       uint32_t RESERVED67;
  __IO uint32_t DIEPTSIZ6;
       uint32_t RESERVED68[3];
  __IO uint32_t DIEPCTL7;
       uint32_t RESERVED69;
  __IO uint32_t DIEPINT7;
       uint32_t RESERVED70;
  __IO uint32_t DIEPTSIZ7;
       uint32_t RESERVED71[3];
  __IO uint32_t DIEPCTL8;
       uint32_t RESERVED72;
  __IO uint32_t DIEPINT8;
       uint32_t RESERVED73;
  __IO uint32_t DIEPTSIZ8;
       uint32_t RESERVED74[3];
  __IO uint32_t DIEPCTL9;
       uint32_t RESERVED75;
  __IO uint32_t DIEPINT9;
       uint32_t RESERVED76;
  __IO uint32_t DIEPTSIZ9;
       uint32_t RESERVED77[3];
  __IO uint32_t DIEPCTL10;
       uint32_t RESERVED78;
  __IO uint32_t DIEPINT10;
       uint32_t RESERVED79;
  __IO uint32_t DIEPTSIZ10;
       uint32_t RESERVED80[3];
  __IO uint32_t DIEPCTL11;
       uint32_t RESERVED81;
  __IO uint32_t DIEPINT11;
       uint32_t RESERVED82;
  __IO uint32_t DIEPTSIZ11;
       uint32_t RESERVED83[3];
  __IO uint32_t DIEPCTL12;
       uint32_t RESERVED84;
  __IO uint32_t DIEPINT12;
       uint32_t RESERVED85;
  __IO uint32_t DIEPTSIZ86;
       uint32_t RESERVED86[3];
  __IO uint32_t DIEPCTL13;
       uint32_t RESERVED87;
  __IO uint32_t DIEPINT13;
       uint32_t RESERVED88;
  __IO uint32_t DIEPTSIZ13;
       uint32_t RESERVED89[3];
  __IO uint32_t DIEPCTL14;
       uint32_t RESERVED90;
  __IO uint32_t DIEPINT14;
       uint32_t RESERVED91;
  __IO uint32_t DIEPTSIZ14;
       uint32_t RESERVED92[3];
  __IO uint32_t DIEPCTL15;
       uint32_t RESERVED93;
  __IO uint32_t DIEPINT15;
       uint32_t RESERVED94;
  __IO uint32_t DIEPTSIZ15;
       uint32_t RESERVED95[3];
  __IO uint32_t DOEPCTL0;
       uint32_t RESERVED96;
  __IO uint32_t DOEPINT0;
       uint32_t RESERVED97;
  __IO uint32_t DOEPTSIZ0;
       uint32_t RESERVED98[3];
  __IO uint32_t DOEPCTL1;
       uint32_t RESERVED99;
  __IO uint32_t DOEPINT1;
       uint32_t RESERVED100;
  __IO uint32_t DOEPTSIZ1;
       uint32_t RESERVED101[3];
  __IO uint32_t DOEPCTL2;
       uint32_t RESERVED102;
  __IO uint32_t DOEPINT2;
       uint32_t RESERVED103;
  __IO uint32_t DOEPTSIZ2;
       uint32_t RESERVED104[3];
  __IO uint32_t DOEPCTL3;
       uint32_t RESERVED105;
  __IO uint32_t DOEPINT3;
       uint32_t RESERVED106;
  __IO uint32_t DOEPTSIZ3;
       uint32_t RESERVED107[3];
  __IO uint32_t DOEPCTL4;
       uint32_t RESERVED108;
  __IO uint32_t DOEPINT4;
       uint32_t RESERVED109;
  __IO uint32_t DOEPTSIZ4;
       uint32_t RESERVED110[3];
  __IO uint32_t DOEPCTL5;
       uint32_t RESERVED111;
  __IO uint32_t DOEPINT5;
       uint32_t RESERVED112;
  __IO uint32_t DOEPTSIZ5;
       uint32_t RESERVED113[3];
  __IO uint32_t DOEPCTL6;
       uint32_t RESERVED114;
  __IO uint32_t DOEPINT6;
       uint32_t RESERVED115;
  __IO uint32_t DOEPTSIZ6;
       uint32_t RESERVED116[3];
  __IO uint32_t DOEPCTL7;
       uint32_t RESERVED117;
  __IO uint32_t DOEPINT7;
       uint32_t RESERVED118;
  __IO uint32_t DOEPTSIZ7;
       uint32_t RESERVED119[3];
  __IO uint32_t DOEPCTL8;
       uint32_t RESERVED120;
  __IO uint32_t DOEPINT8;
       uint32_t RESERVED121;
  __IO uint32_t DOEPTSIZ8;
       uint32_t RESERVED122[3];
  __IO uint32_t DOEPCTL9;
       uint32_t RESERVED123;
  __IO uint32_t DOEPINT9;
       uint32_t RESERVED124;
  __IO uint32_t DOEPTSIZ9;
       uint32_t RESERVED125[3];
  __IO uint32_t DOEPCTL10;
       uint32_t RESERVED126;
  __IO uint32_t DOEPINT10;
       uint32_t RESERVED127;
  __IO uint32_t DOEPTSIZ10;
       uint32_t RESERVED128[3];
  __IO uint32_t DOEPCTL11;
       uint32_t RESERVED129;
  __IO uint32_t DOEPINT11;
       uint32_t RESERVED130;
  __IO uint32_t DOEPTSIZ11;
       uint32_t RESERVED131[3];
  __IO uint32_t DOEPCTL12;
       uint32_t RESERVED132;
  __IO uint32_t DOEPINT12;
       uint32_t RESERVED133;
  __IO uint32_t DOEPTSIZ12;
       uint32_t RESERVED134[3];
  __IO uint32_t DOEPCTL13;
       uint32_t RESERVED135;
  __IO uint32_t DOEPINT13;
       uint32_t RESERVED136;
  __IO uint32_t DOEPTSIZ13;
       uint32_t RESERVED137[3];
  __IO uint32_t DOEPCTL14;
       uint32_t RESERVED138;
  __IO uint32_t DOEPINT14;
       uint32_t RESERVED139;
  __IO uint32_t DOEPTSIZ14;
       uint32_t RESERVED140[3];
  __IO uint32_t DOEPCTL15;
       uint32_t RESERVED141;
  __IO uint32_t DOEPINT15;
       uint32_t RESERVED142;
  __IO uint32_t DOEPTSIZ15;
       uint32_t RESERVED143[3];
       uint32_t RESERVED144[64];
  __IO uint32_t PCGCCTL;
} OTG_FS_TypeDef;

#define OTG_FS_BASE           (APB1PERIPH_BASE + 0x10000000U)
#define OTG_FS_DFIFO0_BASE    (APB1PERIPH_BASE + 0x10001000U)
#define OTG_FS_DFIFO1_BASE    (APB1PERIPH_BASE + 0x10002000U)
#define OTG_FS_DFIFO2_BASE    (APB1PERIPH_BASE + 0x10003000U)
#define OTG_FS_DFIFO3_BASE    (APB1PERIPH_BASE + 0x10004000U)
#define OTG_FS_DFIFO4_BASE    (APB1PERIPH_BASE + 0x10005000U)
#define OTG_FS_DFIFO5_BASE    (APB1PERIPH_BASE + 0x10006000U)
#define OTG_FS_DFIFO6_BASE    (APB1PERIPH_BASE + 0x10007000U)
#define OTG_FS_DFIFO7_BASE    (APB1PERIPH_BASE + 0x10008000U)

#define OTG_FS                ((OTG_FS_TypeDef *) OTG_FS_BASE)
#define OTG_FS_DFIFO0         (((uint32_t *) OTG_FS_DFIFO0_BASE) )
#define OTG_FS_DFIFO1         (((uint32_t *) OTG_FS_DFIFO1_BASE) )
#define OTG_FS_DFIFO2         (((uint32_t *) OTG_FS_DFIFO2_BASE) )
#define OTG_FS_DFIFO3         (((uint32_t *) OTG_FS_DFIFO3_BASE) )
#define OTG_FS_DFIFO4         (((uint32_t *) OTG_FS_DFIFO4_BASE) )
#define OTG_FS_DFIFO5         (((uint32_t *) OTG_FS_DFIFO5_BASE) )
#define OTG_FS_DFIFO6         (((uint32_t *) OTG_FS_DFIFO6_BASE) )
#define OTG_FS_DFIFO7         (((uint32_t *) OTG_FS_DFIFO7_BASE) )

// OTG_FS Host Channel
typedef struct __attribute__((packed)) {// Host Channel typedef (HC)
  union __attribute__((packed)) {
    uint32_t HCCHAR;                    // Channel Characteristics
    struct __attribute__((packed)) {
      uint32_t MPSIZ     : 11;          // Endpoint Maximum Packet Size
      uint32_t EPNUM     :  4;          // Endpoint Number
      uint32_t EPDIR     :  1;          // Endpoint Direction
      uint32_t Reserved0 :  1;          // Reserved
      uint32_t LSDEV     :  1;          // Endpoint Low-speed
      uint32_t EPTYP     :  2;          // Endpoint Type
      uint32_t MCNT      :  2;          // Multicount
      uint32_t DAD       :  7;          // Device Address
      uint32_t ODDFRM    :  1;          // Periodic Transaction Odd Frame
      uint32_t CHDIS     :  1;          // Channel Disable
      uint32_t CHENA     :  1;          // Channel Enable
    };
  };
  uint32_t Reserved1;                   // Reserved
  union __attribute__((packed)) {
    uint32_t HCINT;                     // Channel Interrupt
    struct __attribute__((packed)) {
      uint32_t XFCR      :  1;          // Transfer Completed
      uint32_t CHH       :  1;          // Channel Halted
      uint32_t Reserved2 :  1;          // Reserved
      uint32_t STALL     :  1;          // STALL Response Received Interrupt
      uint32_t NAK       :  1;          // NAK Response Received Interrupt
      uint32_t ACK       :  1;          // ACK Response Rece/Transmit Int
      uint32_t Reserved3 :  1;          // Reserved
      uint32_t TXERR     :  1;          // Transaction Error
      uint32_t BBERR     :  1;          // Babble Error
      uint32_t FRMOR     :  1;          // Frame Overrun
      uint32_t DTERR     :  1;          // Data Toggle Error
    };
  };
  union __attribute__((packed)) {
    uint32_t HCINTMSK;                  // Channel Interrupt Mask
    struct __attribute__((packed)) {
      uint32_t XFCRM     :  1;          // Transfer Completed Mask
      uint32_t CHHM      :  1;          // Channel Halted Mask
      uint32_t Reserved4 :  1;          // Reserved
      uint32_t STALLM    :  1;          // STALL Response Rece Interrupt Mask
      uint32_t NAKM      :  1;          // NAK Response Rece Interrupt Mask
      uint32_t ACKM      :  1;          // ACK Response Rece/Transmit Int Mask
      uint32_t Reserved5 :  1;          // Reserved
      uint32_t TXERRM    :  1;          // Transaction Error Mask
      uint32_t BBERRM    :  1;          // Babble Error Mask
      uint32_t FRMORM    :  1;          // Frame Overrun Mask
      uint32_t DTERRM    :  1;          // Data Toggle Error Mask
    };
  };
  union __attribute__((packed)) {
    uint32_t HCTSIZ;                    // Channel Transfer Size
    struct __attribute__((packed)) {
      uint32_t XFRSIZ    : 19;          // Transfer Size
      uint32_t PKTCNT    : 10;          // Packet Count
      uint32_t DPID      :  2;          // Data PID
      uint32_t Reserved6 :  1;          // Reserved
    };
  };
  uint32_t Reserved7;                   // Reserved
  uint32_t Reserved8;                   // Reserved
  uint32_t Reserved9;                   // Reserved
} OTG_FS_HC;

#pragma pop

// OTG_FS control and status register (OTG_FS_GOTGCTL)
#define  OTG_FS_GOTGCTL_SRQSCS              ((uint32_t)    1U       )
#define  OTG_FS_GOTGCTL_SRQ                 ((uint32_t)    1U  <<  1)
#define  OTG_FS_GOTGCTL_HNGSCS              ((uint32_t)    1U  <<  8)
#define  OTG_FS_GOTGCTL_HNPRQ               ((uint32_t)    1U  <<  9)
#define  OTG_FS_GOTGCTL_HSHNPEN             ((uint32_t)    1U  << 10)
#define  OTG_FS_GOTGCTL_DHNPEN              ((uint32_t)    1U  << 11)
#define  OTG_FS_GOTGCTL_CIDSTS              ((uint32_t)    1U  << 16)
#define  OTG_FS_GOTGCTL_DBCT                ((uint32_t)    1U  << 17)
#define  OTG_FS_GOTGCTL_ASVLD               ((uint32_t)    1U  << 18)
#define  OTG_FS_GOTGCTL_BSVLD               ((uint32_t)    1U  << 19)

// OTG_FS interrupt register (OTG_FS_GOTGINT)
#define  OTG_FS_GOTGINT_SEDET               ((uint32_t)    1U  <<  2)
#define  OTG_FS_GOTGINT_SRSSCHG             ((uint32_t)    1U  <<  8)
#define  OTG_FS_GOTGINT_HNSSCHG             ((uint32_t)    1U  <<  9)
#define  OTG_FS_GOTGINT_HNGDET              ((uint32_t)    1U  << 17)
#define  OTG_FS_GOTGINT_ADTOCHG             ((uint32_t)    1U  << 18)
#define  OTG_FS_GOTGINT_DBCDNE              ((uint32_t)    1U  << 19)

// OTG_FS AHB configuration register (OTG_FS_GAHBCFG)
#define  OTG_FS_GAHBCFG_GINTMSK             ((uint32_t)    1U       )
#define  OTG_FS_GAHBCFG_TXFELVL             ((uint32_t)    1U  <<  7)
#define  OTG_FS_GAHBCFG_PTXFELVL            ((uint32_t)    1U  <<  8)

// OTG_FS USB configuration register (OTG_FS_GUSBCFG)
#define  OTG_FS_GUSBCFG_TOCAL_POS           ((uint32_t)           0U)
#define  OTG_FS_GUSBCFG_TOCAL_MSK           ((uint32_t)    7U       )
#define  OTG_FS_GUSBCFG_TOCAL(x)           (((uint32_t)    x        ) & OTG_FS_GUSBCFG_TOCAL_MSK)
#define  OTG_FS_GUSBCFG_PHYSEL              ((uint32_t)    1U  <<  6)
#define  OTG_FS_GUSBCFG_SRPCAP              ((uint32_t)    1U  <<  8)
#define  OTG_FS_GUSBCFG_HNPCAP              ((uint32_t)    1U  <<  9)
#define  OTG_FS_GUSBCFG_TRDT_POS            ((uint32_t)           10U)
#define  OTG_FS_GUSBCFG_TRDT_MSK            ((uint32_t) 0x0FU  << OTG_FS_GUSBCFG_TRDT_POS)
#define  OTG_FS_GUSBCFG_TRDT(x)            (((uint32_t)    x   << OTG_FS_GUSBCFG_TRDT_POS) & OTG_FS_GUSBCFG_TRDT_MSK)
#define  OTG_FS_GUSBCFG_FHMOD               ((uint32_t)    1U  << 29)
#define  OTG_FS_GUSBCFG_FDMOD               ((uint32_t)    1U  << 30)
#define  OTG_FS_GUSBCFG_CTXPKT              ((uint32_t)    1U  << 31)

// OTG_FS reset register (OTG_FS_GRSTCTL)
#define  OTG_FS_GRSTCTL_CSRST               ((uint32_t)    1U       )
#define  OTG_FS_GRSTCTL_HSRST               ((uint32_t)    1U  <<  1)
#define  OTG_FS_GRSTCTL_FCRST               ((uint32_t)    1U  <<  2)
#define  OTG_FS_GRSTCTL_RXFFLSH             ((uint32_t)    1U  <<  4)
#define  OTG_FS_GRSTCTL_TXFFLSH             ((uint32_t)    1U  <<  5)
#define  OTG_FS_GRSTCTL_TXFNUM_POS          ((uint32_t)           6U)
#define  OTG_FS_GRSTCTL_TXFNUM_MSK          ((uint32_t) 0x1FU  <<  OTG_FS_GRSTCTL_TXFNUM_POS)
#define  OTG_FS_GRSTCTL_TXFNUM(x)          (((uint32_t)    x   <<  OTG_FS_GRSTCTL_TXFNUM_POS) & OTG_FS_GRSTCTL_TXFNUM_MSK)
#define  OTG_FS_GRSTCTL_AHBIDL              ((uint32_t)    1U  << 31)

// OTG_FS core interrupt register (OTG_FS_GINTSTS)
#define  OTG_FS_GINTSTS_CMOD                ((uint32_t)    1U       )
#define  OTG_FS_GINTSTS_MMIS                ((uint32_t)    1U  <<  1)
#define  OTG_FS_GINTSTS_OTGINT              ((uint32_t)    1U  <<  2)
#define  OTG_FS_GINTSTS_SOF                 ((uint32_t)    1U  <<  3)
#define  OTG_FS_GINTSTS_RXFLVL              ((uint32_t)    1U  <<  4)
#define  OTG_FS_GINTSTS_NPTXFE              ((uint32_t)    1U  <<  5)
#define  OTG_FS_GINTSTS_GINAKEFF            ((uint32_t)    1U  <<  6)
#define  OTG_FS_GINTSTS_GONAKEFF            ((uint32_t)    1U  <<  7)
#define  OTG_FS_GINTSTS_ESUSP               ((uint32_t)    1U  << 10)
#define  OTG_FS_GINTSTS_USBSUSP             ((uint32_t)    1U  << 11)
#define  OTG_FS_GINTSTS_USBRST              ((uint32_t)    1U  << 12)
#define  OTG_FS_GINTSTS_ENUMDNE             ((uint32_t)    1U  << 13)
#define  OTG_FS_GINTSTS_ISOODRP             ((uint32_t)    1U  << 14)
#define  OTG_FS_GINTSTS_EOPF                ((uint32_t)    1U  << 15)
#define  OTG_FS_GINTSTS_IEPINT              ((uint32_t)    1U  << 18)
#define  OTG_FS_GINTSTS_OEPINT              ((uint32_t)    1U  << 19)
#define  OTG_FS_GINTSTS_IISOIXFR            ((uint32_t)    1U  << 20)
#define  OTG_FS_GINTSTS_IPXFR               ((uint32_t)    1U  << 21)
#define  OTG_FS_GINTSTS_INCOMPISOOUT        ((uint32_t)    1U  << 21)
#define  OTG_FS_GINTSTS_HPRTINT             ((uint32_t)    1U  << 24)
#define  OTG_FS_GINTSTS_HCINT               ((uint32_t)    1U  << 25)
#define  OTG_FS_GINTSTS_PTXFE               ((uint32_t)    1U  << 26)
#define  OTG_FS_GINTSTS_CIDSCHG             ((uint32_t)    1U  << 28)
#define  OTG_FS_GINTSTS_DISCINT             ((uint32_t)    1U  << 29)
#define  OTG_FS_GINTSTS_SRQINT              ((uint32_t)    1U  << 30)
#define  OTG_FS_GINTSTS_WKUPINT             ((uint32_t)    1U  << 31)

// OTG_FS interrupt mask register (OTG_FS_GINTMSK)
#define  OTG_FS_GINTMSK_MMISM               ((uint32_t)    1U  <<  1)
#define  OTG_FS_GINTMSK_OTGINT              ((uint32_t)    1U  <<  2)
#define  OTG_FS_GINTMSK_SOFM                ((uint32_t)    1U  <<  3)
#define  OTG_FS_GINTMSK_RXFLVLM             ((uint32_t)    1U  <<  4)
#define  OTG_FS_GINTMSK_NPTXFEM             ((uint32_t)    1U  <<  5)
#define  OTG_FS_GINTMSK_GINAKEFFM           ((uint32_t)    1U  <<  6)
#define  OTG_FS_GINTMSK_GONAKEFFM           ((uint32_t)    1U  <<  7)
#define  OTG_FS_GINTMSK_ESUSPM              ((uint32_t)    1U  << 10)
#define  OTG_FS_GINTMSK_USBSUSPM            ((uint32_t)    1U  << 11)
#define  OTG_FS_GINTMSK_USBRST              ((uint32_t)    1U  << 12)
#define  OTG_FS_GINTMSK_ENUMDNEM            ((uint32_t)    1U  << 13)
#define  OTG_FS_GINTMSK_ISOODRPM            ((uint32_t)    1U  << 14)
#define  OTG_FS_GINTMSK_EOPFM               ((uint32_t)    1U  << 15)
#define  OTG_FS_GINTMSK_EPMISM              ((uint32_t)    1U  << 17)
#define  OTG_FS_GINTMSK_IEPINT              ((uint32_t)    1U  << 18)
#define  OTG_FS_GINTMSK_OEPINT              ((uint32_t)    1U  << 19)
#define  OTG_FS_GINTMSK_IISOIXFRM           ((uint32_t)    1U  << 20)
#define  OTG_FS_GINTMSK_IPXFRM              ((uint32_t)    1U  << 21)
#define  OTG_FS_GINTMSK_IISOOXFRM           ((uint32_t)    1U  << 21)
#define  OTG_FS_GINTMSK_PRTIM               ((uint32_t)    1U  << 24)
#define  OTG_FS_GINTMSK_HCIM                ((uint32_t)    1U  << 25)
#define  OTG_FS_GINTMSK_PTXFEM              ((uint32_t)    1U  << 26)
#define  OTG_FS_GINTMSK_CIDSCHGM            ((uint32_t)    1U  << 28)
#define  OTG_FS_GINTMSK_DISCINT             ((uint32_t)    1U  << 29)
#define  OTG_FS_GINTMSK_SRQIM               ((uint32_t)    1U  << 30)
#define  OTG_FS_GINTMSK_WUIM                ((uint32_t)    1U  << 31)

// OTG_FS status debug read register (OTG_FS_GRXSTSR)
#define  OTG_FS_GRXSTSR_CHNUM_POS           ((uint32_t)           0U)
#define  OTG_FS_GRXSTSR_CHNUM_MSK           ((uint32_t) 0x0FU       )
#define  OTG_FS_GRXSTSR_CHNUM(x)           (((uint32_t)    x        ) & OTG_FS_GRXSTSR_CHNUM_MSK)
#define  OTG_FS_GRXSTSR_EPNUM_POS           ((uint32_t)           0U)
#define  OTG_FS_GRXSTSR_EPNUM_MSK           ((uint32_t) 0x0FU       )
#define  OTG_FS_GRXSTSR_EPNUM(x)           (((uint32_t)    x        ) & OTG_FS_GRXSTSR_EPNUM_MSK)
#define  OTG_FS_GRXSTSR_BCNT_POS            ((uint32_t)           4U)
#define  OTG_FS_GRXSTSR_BCNT_MSK            ((uint32_t)0x7FFU  << OTG_FS_GRXSTSR_BCNT_POS)
#define  OTG_FS_GRXSTSR_BCNT(x)            (((uint32_t)    x   << OTG_FS_GRXSTSR_BCNT_POS) & OTG_FS_GRXSTSR_BCNT_MSK)
#define  OTG_FS_GRXSTSR_DPID_POS            ((uint32_t)           15U)
#define  OTG_FS_GRXSTSR_DPID_MSK            ((uint32_t)    3U  << OTG_FS_GRXSTSR_DPID_POS)
#define  OTG_FS_GRXSTSR_DPID(x)            (((uint32_t)    x   << OTG_FS_GRXSTSR_DPID_POS) & OTG_FS_GRXSTSR_DPID_MSK)
#define  OTG_FS_GRXSTSR_PKTSTS_POS          ((uint32_t)           17U)
#define  OTG_FS_GRXSTSR_PKTSTS_MSK          ((uint32_t) 0x0FU  << OTG_FS_GRXSTSR_PKTSTS_POS)
#define  OTG_FS_GRXSTSR_PKTSTS(x)          (((uint32_t)    x   << OTG_FS_GRXSTSR_PKTSTS_POS) & OTG_FS_GRXSTSR_PKTSTS_MSK)
#define  OTG_FS_GRXSTSR_FRMNUM_POS          ((uint32_t)           21U)
#define  OTG_FS_GRXSTSR_FRMNUM_MSK          ((uint32_t) 0x0FU  << OTG_FS_GRXSTSR_FRMNUM_POS)
#define  OTG_FS_GRXSTSR_FRMNUM(x)          (((uint32_t)    x   << OTG_FS_GRXSTSR_FRMNUM_POS) & OTG_FS_GRXSTSR_FRMNUM_MSK)

// OTG_FS status read and pop register (OTG_FS_GRXSTSP)
#define  OTG_FS_GRXSTSP_CHNUM_POS           ((uint32_t)           0U)
#define  OTG_FS_GRXSTSP_CHNUM_MSK           ((uint32_t) 0x0FU       )
#define  OTG_FS_GRXSTSP_CHNUM(x)           (((uint32_t)    x        ) & OTG_FS_GRXSTSP_CHNUM_MSK)
#define  OTG_FS_GRXSTSP_EPNUM_POS           ((uint32_t)           0U)
#define  OTG_FS_GRXSTSP_EPNUM_MSK           ((uint32_t) 0x0FU       )
#define  OTG_FS_GRXSTSP_EPNUM(x)           (((uint32_t)    x        ) & OTG_FS_GRXSTSP_EPNUM_MSK)
#define  OTG_FS_GRXSTSP_BCNT_POS            ((uint32_t)           4U)
#define  OTG_FS_GRXSTSP_BCNT_MSK            ((uint32_t)0x7FFU  << OTG_FS_GRXSTSP_BCNT_POS)
#define  OTG_FS_GRXSTSP_BCNT(x)            (((uint32_t)    x   << OTG_FS_GRXSTSP_BCNT_POS) & OTG_FS_GRXSTSP_BCNT_MSK)
#define  OTG_FS_GRXSTSP_DPID_POS            ((uint32_t)           15U)
#define  OTG_FS_GRXSTSP_DPID_MSK            ((uint32_t)    3U  << OTG_FS_GRXSTSP_DPID_POS)
#define  OTG_FS_GRXSTSP_DPID(x)            (((uint32_t)    x   << OTG_FS_GRXSTSP_DPID_POS) & OTG_FS_GRXSTSP_DPID_MSK)
#define  OTG_FS_GRXSTSP_PKTSTS_POS          ((uint32_t)           17U)
#define  OTG_FS_GRXSTSP_PKTSTS_MSK          ((uint32_t) 0x0FU  << OTG_FS_GRXSTSP_PKTSTS_POS)
#define  OTG_FS_GRXSTSP_PKTSTS(x)          (((uint32_t)    x   << OTG_FS_GRXSTSP_PKTSTS_POS) & OTG_FS_GRXSTSP_PKTSTS_MSK)
#define  OTG_FS_GRXSTSP_FRMNUM_POS          ((uint32_t)           21U)
#define  OTG_FS_GRXSTSP_FRMNUM_MSK          ((uint32_t) 0x0FU  << OTG_FS_GRXSTSP_FRMNUM_POS)
#define  OTG_FS_GRXSTSP_FRMNUM(x)          (((uint32_t)    x   << OTG_FS_GRXSTSP_FRMNUM_POS) & OTG_FS_GRXSTSP_FRMNUM_MSK)

// OTG_FS Receive FIFO size register (OTG_FS_GRXFSIZ)
#define  OTG_FS_GRXFSIZ_RXFD_POS            ((uint32_t)            0U)
#define  OTG_FS_GRXFSIZ_RXFD_MSK            ((uint32_t)0xFFFFU      )
#define  OTG_FS_GRXFSIZ_RXFD(x)            (((uint32_t)    x        ) & OTG_FS_GRXFSIZ_RXFD_MSK)

// OTG_FS Host non-periodic transmit FIFO size register (OTG_FS_HNPTXFSIZ)
#define  OTG_FS_HNPTXFSIZ_NPTXFSA_POS       ((uint32_t)           0U)
#define  OTG_FS_HNPTXFSIZ_NPTXFSA_MSK       ((uint32_t)0xFFFFU      )
#define  OTG_FS_HNPTXFSIZ_NPTXFSA(x)       (((uint32_t)    x        ) & OTG_FS_HNPTXFSIZ_NPTXFSA_MSK)
#define  OTG_FS_HNPTXFSIZ_NPTXFD_POS        ((uint32_t)           16U)
#define  OTG_FS_HNPTXFSIZ_NPTXFD_MSK        ((uint32_t)0xFFFFU << OTG_FS_HNPTXFSIZ_NPTXFD_POS)
#define  OTG_FS_HNPTXFSIZ_NPTXFD(x)        (((uint32_t)    x   << OTG_FS_HNPTXFSIZ_NPTXFD_POS) & OTG_FS_HNPTXFSIZ_NPTXFD_MSK)

// OTG_FS Endpoint 0 transmit FIFO size register (OTG_FS_DIEPTXF0)
#define  OTG_FS_DIEPTXF0_TX0FSA_POS         ((uint32_t)           0U)
#define  OTG_FS_DIEPTXF0_TX0FSA_MSK         ((uint32_t)0xFFFFU      )
#define  OTG_FS_DIEPTXF0_TX0FSA(x)         (((uint32_t)    x        ) & OTG_FS_DIEPTXF0_TX0FSA_MSK)
#define  OTG_FS_DIEPTXF0_TX0FD_POS          ((uint32_t)           16U)
#define  OTG_FS_DIEPTXF0_TX0FD_MSK          ((uint32_t)0xFFFFU << OTG_FS_DIEPTXF0_TX0FD_POS)
#define  OTG_FS_DIEPTXF0_TX0FD(x)          (((uint32_t)    x   << OTG_FS_DIEPTXF0_TX0FD_POS) & OTG_FS_DIEPTXF0_TX0FD_MSK)

// OTG_FS Host non-periodic transmit FIFO/queue status register (OTG_FS_HNPTXSTS)
#define  OTG_FS_HNPTXSTS_NPTXFSAV_POS       ((uint32_t)           0U)
#define  OTG_FS_HNPTXSTS_NPTXFSAV_MSK       ((uint32_t)0xFFFFU      )
#define  OTG_FS_HNPTXSTS_NPTXFSAV(x)       (((uint32_t)    x        ) & OTG_FS_HNPTXSTS_NPTXFSAV_MSK)
#define  OTG_FS_HNPTXSTS_NPTQXSAV_POS       ((uint32_t)           16U)
#define  OTG_FS_HNPTXSTS_NPTQXSAV_MSK       ((uint32_t) 0xFFU  << OTG_FS_HNPTXSTS_NPTQXSAV_POS)
#define  OTG_FS_HNPTXSTS_NPTQXSAV(x)       (((uint32_t)    x   << OTG_FS_HNPTXSTS_NPTQXSAV_POS) & OTG_FS_HNPTXSTS_NPTQXSAV_MSK)
#define  OTG_FS_HNPTXSTS_NPTXQTOP_POS       ((uint32_t)           24U)
#define  OTG_FS_HNPTXSTS_NPTXQTOP_MSK       ((uint32_t) 0x7FU  << OTG_FS_HNPTXSTS_NPTXQTOP_POS)
#define  OTG_FS_HNPTXSTS_NPTXQTOP(x)       (((uint32_t)    x   << OTG_FS_HNPTXSTS_NPTXQTOP_POS) & OTG_FS_HNPTXSTS_NPTXQTOP_MSK)

// OTG_FS general core configuration register (OTG_FS_GCCFG)
#define  OTG_FS_GCCFG_PWRDWN                ((uint32_t)    1U  << 16)
#define  OTG_FS_GCCFG_VBUSASEN              ((uint32_t)    1U  << 18)
#define  OTG_FS_GCCFG_VBUSBSEN              ((uint32_t)    1U  << 19)
#define  OTG_FS_GCCFG_SOFOUTEN              ((uint32_t)    1U  << 20)
#define  OTG_FS_GCCFG_NOVBUSSENS            ((uint32_t)    1U  << 21)

// OTG_FS Host periodic transmit FIFO size register (OTG_FS_HPTXFSIZ)
#define  OTG_FS_HPTXFSIZ_PTXSA_POS          ((uint32_t)           0U)
#define  OTG_FS_HPTXFSIZ_PTXSA_MSK          ((uint32_t)0xFFFFU      )
#define  OTG_FS_HPTXFSIZ_PTXSA(x)          (((uint32_t)    x        ) & OTG_FS_HPTXFSIZ_PTXSA_MSK)
#define  OTG_FS_HPTXFSIZ_PTXFD_POS          ((uint32_t)           16U)
#define  OTG_FS_HPTXFSIZ_PTXFD_MSK          ((uint32_t)0xFFFFU << OTG_FS_HPTXFSIZ_PTXFD_POS)
#define  OTG_FS_HPTXFSIZ_PTXFD(x)          (((uint32_t)    x   << OTG_FS_HPTXFSIZ_PTXFD_POS) & OTG_FS_HPTXFSIZ_PTXFD_MSK)

// OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXFx)
#define  OTG_FS_DIEPTXFx_INEPTXSA_POS       ((uint32_t)           0U)
#define  OTG_FS_DIEPTXFx_INEPTXSA_MSK       ((uint32_t)0xFFFFU      )
#define  OTG_FS_DIEPTXFx_INEPTXSA(x)       (((uint32_t)    x        ) & OTG_FS_DIEPTXFx_INEPTXSA_MSK)
#define  OTG_FS_DIEPTXFx_INEPTXFD_POS       ((uint32_t)           16U)
#define  OTG_FS_DIEPTXFx_INEPTXFD_MSK       ((uint32_t)0xFFFFU << OTG_FS_DIEPTXFx_INEPTXFD_POS)
#define  OTG_FS_DIEPTXFx_INEPTXFD(x)       (((uint32_t)    x   << OTG_FS_DIEPTXFx_INEPTXFD_POS) & OTG_FS_DIEPTXFx_INEPTXFD_MSK)

// OTG_FS Host configuration register (OTG_FS_HCFG)
#define  OTG_FS_HCFG_FSLSPCS_POS            ((uint32_t)           0U)
#define  OTG_FS_HCFG_FSLSPCS_MSK            ((uint32_t)    3U       )
#define  OTG_FS_HCFG_FSLSPCS(x)            (((uint32_t)    x        ) & OTG_FS_HCFG_FSLSPCS_MSK)
#define  OTG_FS_HCFG_FSLSS                  ((uint32_t)    1U  <<  2)

// OTG_FS Host frame interval register (OTG_FS_HFIR)
#define  OTG_FS_HFIR_FRIVL_POS              ((uint32_t)           0U)
#define  OTG_FS_HFIR_FRIVL_MSK              ((uint32_t)0xFFFFU      )
#define  OTG_FS_HFIR_FRIVL(x)              (((uint32_t)    x        ) & OTG_FS_HFIR_FRIVL_MSK)

// OTG_FS Host frame number/frame time remaining register (OTG_FS_HFNUM)
#define  OTG_FS_HFNUM_FRNUM_POS             ((uint32_t)           0U)
#define  OTG_FS_HFNUM_FRNUM_MSK             ((uint32_t)0xFFFFU      )
#define  OTG_FS_HFNUM_FRNUM(x)             (((uint32_t)    x        ) & OTG_FS_HFNUM_FRNUM_MSK)
#define  OTG_FS_HFNUM_FTREM_POS             ((uint32_t)           16U)
#define  OTG_FS_HFNUM_FTREM_MSK             ((uint32_t)0xFFFFU << OTG_FS_HFNUM_FRNUM_POS)
#define  OTG_FS_HFNUM_FTREM(x)             (((uint32_t)    x   << OTG_FS_HFNUM_FRNUM_POS) & OTG_FS_HFNUM_FRNUM_MSK)

// OTG_FS Host periodic transmit FIFO/queue status register (OTG_FS_HPTXSTS)
#define  OTG_FS_HPTXSTS_PTXFSAVL_POS        ((uint32_t)           0U)
#define  OTG_FS_HPTXSTS_PTXFSAVL_MSK        ((uint32_t)0xFFFFU      )
#define  OTG_FS_HPTXSTS_PTXFSAVL(x)        (((uint32_t)    x        ) & OTG_FS_HPTXSTS_PTXFSAVL_MSK)
#define  OTG_FS_HPTXSTS_PTXQSAV_POS         ((uint32_t)           16U)
#define  OTG_FS_HPTXSTS_PTXQSAV_MSK         ((uint32_t) 0xFFU  << OTG_FS_HPTXSTS_PTXQSAV_POS)
#define  OTG_FS_HPTXSTS_PTXQSAV(x)         (((uint32_t)    x   << OTG_FS_HPTXSTS_PTXQSAV_POS) & OTG_FS_HPTXSTS_PTXQSAV_MSK)
#define  OTG_FS_HPTXSTS_PTXQTOP_POS         ((uint32_t)           24U)
#define  OTG_FS_HPTXSTS_PTXQTOP_MSK         ((uint32_t) 0xFFU  << OTG_FS_HPTXSTS_PTXQTOP_POS)
#define  OTG_FS_HPTXSTS_PTXQTOP(x)         (((uint32_t)    x   << OTG_FS_HPTXSTS_PTXQTOP_POS) & OTG_FS_HPTXSTS_PTXQTOP_MSK)

// OTG_FS Host all channels interrupt register (OTG_FS_HAINT)
#define  OTG_FS_HAINT_HAINT_POS             ((uint32_t)           0U)
#define  OTG_FS_HAINT_HAINT_MSK             ((uint32_t)0xFFFFU      )
#define  OTG_FS_HAINT_HAINT(x)             (((uint32_t)    x        ) & OTG_FS_HAINT_HAINT_MSK)

// OTG_FS Host all channels interrupt mask register (OTG_FS_HAINTMSK)
#define  OTG_FS_HAINTMSK_HAINTM_POS         ((uint32_t)           0U)
#define  OTG_FS_HAINTMSK_HAINTM_MSK         ((uint32_t)0xFFFFU      )
#define  OTG_FS_HAINTMSK_HAINTM(x)         (((uint32_t)    x        ) & OTG_FS_HAINTMSK_HAINTM_MSK)

// OTG_FS Host port control and status register (OTG_FS_HPRT)
#define  OTG_FS_HPRT_PCSTS                  ((uint32_t)    1U       )
#define  OTG_FS_HPRT_PCDET                  ((uint32_t)    1U  <<  1)
#define  OTG_FS_HPRT_PENA                   ((uint32_t)    1U  <<  2)
#define  OTG_FS_HPRT_PENCHNG                ((uint32_t)    1U  <<  3)
#define  OTG_FS_HPRT_POCA                   ((uint32_t)    1U  <<  4)
#define  OTG_FS_HPRT_POCCHNG                ((uint32_t)    1U  <<  5)
#define  OTG_FS_HPRT_PRES                   ((uint32_t)    1U  <<  6)
#define  OTG_FS_HPRT_PSUSP                  ((uint32_t)    1U  <<  7)
#define  OTG_FS_HPRT_PRST                   ((uint32_t)    1U  <<  8)
#define  OTG_FS_HPRT_PLSTS_POS              ((uint32_t)           10U)
#define  OTG_FS_HPRT_PLSTS_MSK              ((uint32_t)    3U  << OTG_FS_HPRT_PLSTS_POS)
#define  OTG_FS_HPRT_PLSTS(x)              (((uint32_t)    x   << OTG_FS_HPRT_PLSTS_POS) & OTG_FS_HPRT_PLSTS_MSK)
#define  OTG_FS_HPRT_PPWR                   ((uint32_t)    1U  << 12)
#define  OTG_FS_HPRT_PTCTL_POS              ((uint32_t)           13U)
#define  OTG_FS_HPRT_PTCTL_MSK              ((uint32_t) 0x0FU  << OTG_FS_HPRT_PTCTL_POS)
#define  OTG_FS_HPRT_PTCTL(x)              (((uint32_t)    x   << OTG_FS_HPRT_PTCTL_POS) & OTG_FS_HPRT_PTCTL_MSK)
#define  OTG_FS_HPRT_PSPD_POS               ((uint32_t)           17U)
#define  OTG_FS_HPRT_PSPD_MSK               ((uint32_t)    3U  << OTG_FS_HPRT_PSPD_POS)
#define  OTG_FS_HPRT_PSPD(x)               (((uint32_t)    x   << OTG_FS_HPRT_PSPD_POS) & OTG_FS_HPRT_PSPD_MSK)

// OTG_FS Host channel-x characteristics register (OTG_FS_HCCHARx)
#define  OTG_FS_HCCHARx_MPSIZ_POS           ((uint32_t)           0U)
#define  OTG_FS_HCCHARx_MPSIZ_MSK           ((uint32_t)0x7FFU       )
#define  OTG_FS_HCCHARx_MPSIZ(x)           (((uint32_t)    x        ) & OTG_FS_HCCHARx_MPSIZ_MSK)
#define  OTG_FS_HCCHARx_EPNUM_POS           ((uint32_t)           11U)
#define  OTG_FS_HCCHARx_EPNUM_MSK           ((uint32_t) 0x0FU  << OTG_FS_HCCHARx_EPNUM_POS)
#define  OTG_FS_HCCHARx_EPNUM(x)           (((uint32_t)    x   << OTG_FS_HCCHARx_EPNUM_POS) & OTG_FS_HCCHARx_EPNUM_MSK)
#define  OTG_FS_HCCHARx_EPDIR               ((uint32_t)    1U  << 15)
#define  OTG_FS_HCCHARx_LSDEV               ((uint32_t)    1U  << 17)
#define  OTG_FS_HCCHARx_EPTYP_POS           ((uint32_t)           18U)
#define  OTG_FS_HCCHARx_EPTYP_MSK           ((uint32_t)    3U  << OTG_FS_HCCHARx_EPTYP_POS)
#define  OTG_FS_HCCHARx_EPTYP(x)           (((uint32_t)    x   << OTG_FS_HCCHARx_EPTYP_POS) & OTG_FS_HCCHARx_EPTYP_MSK)
#define  OTG_FS_HCCHARx_MCNT_POS            ((uint32_t)           20U)
#define  OTG_FS_HCCHARx_MCNT_MSK            ((uint32_t)    3U  << OTG_FS_HCCHARx_MCNT_POS)
#define  OTG_FS_HCCHARx_MCNT(x)            (((uint32_t)    x   << OTG_FS_HCCHARx_MCNT_POS) & OTG_FS_HCCHARx_MCNT_MSK)
#define  OTG_FS_HCCHARx_DAD_POS             ((uint32_t)           22U)
#define  OTG_FS_HCCHARx_DAD_MSK             ((uint32_t) 0x7FU  << OTG_FS_HCCHARx_DAD_POS)
#define  OTG_FS_HCCHARx_DAD(x)             (((uint32_t)    x   << OTG_FS_HCCHARx_DAD_POS) & OTG_FS_HCCHARx_DAD_MSK)
#define  OTG_FS_HCCHARx_ODDFRM              ((uint32_t)    1U  << 29)
#define  OTG_FS_HCCHARx_CHDIS               ((uint32_t)    1U  << 30)
#define  OTG_FS_HCCHARx_CHENA               ((uint32_t)    1U  << 31)

// OTG_FS Host channel-x interrupt register (OTG_FS_HCINTx)
#define  OTG_FS_HCINTx_XFRC                 ((uint32_t)    1U       )
#define  OTG_FS_HCINTx_CHH                  ((uint32_t)    1U  <<  1)
#define  OTG_FS_HCINTx_STALL                ((uint32_t)    1U  <<  3)
#define  OTG_FS_HCINTx_NAK                  ((uint32_t)    1U  <<  4)
#define  OTG_FS_HCINTx_ACK                  ((uint32_t)    1U  <<  5)
#define  OTG_FS_HCINTx_TXERR                ((uint32_t)    1U  <<  7)
#define  OTG_FS_HCINTx_BBERR                ((uint32_t)    1U  <<  8)
#define  OTG_FS_HCINTx_FRMOR                ((uint32_t)    1U  <<  9)
#define  OTG_FS_HCINTx_DTERR                ((uint32_t)    1U  << 10)
#define  OTG_FS_HCINTx_ERR                  ((uint32_t) 0x0FU  <<  7)

// OTG_FS Host channel-x interrupt mask register (OTG_FS_HCINTMSKx)
#define  OTG_FS_HCINTMSKx_XFRCM             ((uint32_t)    1U       )
#define  OTG_FS_HCINTMSKx_CHHM              ((uint32_t)    1U  <<  1)
#define  OTG_FS_HCINTMSKx_STALLM            ((uint32_t)    1U  <<  3)
#define  OTG_FS_HCINTMSKx_NAKM              ((uint32_t)    1U  <<  4)
#define  OTG_FS_HCINTMSKx_ACKM              ((uint32_t)    1U  <<  5)
#define  OTG_FS_HCINTMSKx_TXERRM            ((uint32_t)    1U  <<  7)
#define  OTG_FS_HCINTMSKx_BBERRM            ((uint32_t)    1U  <<  8)
#define  OTG_FS_HCINTMSKx_FRMORM            ((uint32_t)    1U  <<  9)
#define  OTG_FS_HCINTMSKx_DTERRM            ((uint32_t)    1U  << 10)

// OTG_FS Host channel-x transfer size register (OTG_FS_HCTSIZx)
#define  OTG_FS_HCTSIZx_XFRSIZ_POS          ((uint32_t)           0U)
#define  OTG_FS_HCTSIZx_XFRSIZ_MSK          ((uint32_t)0x7FFFFU     )
#define  OTG_FS_HCTSIZx_XFRSIZ(x)          (((uint32_t)    x        ) & OTG_FS_HCTSIZx_XFRSIZ_MSK)
#define  OTG_FS_HCTSIZx_PKTCNT_POS          ((uint32_t)           19U)
#define  OTG_FS_HCTSIZx_PKTCNT_MSK          ((uint32_t)0x3FFU  << OTG_FS_HCTSIZx_PKTCNT_POS)
#define  OTG_FS_HCTSIZx_PKTCNT(x)          (((uint32_t)    x   << OTG_FS_HCTSIZx_PKTCNT_POS) & OTG_FS_HCTSIZx_PKTCNT_MSK)
#define  OTG_FS_HCTSIZx_DPID_POS            ((uint32_t)           29U)
#define  OTG_FS_HCTSIZx_DPID_MSK            ((uint32_t)    3U  << OTG_FS_HCTSIZx_DPID_POS)
#define  OTG_FS_HCTSIZx_DPID(x)            (((uint32_t)    x   << OTG_FS_HCTSIZx_DPID_POS) & OTG_FS_HCTSIZx_DPID_MSK)
#define  OTG_FS_HCTSIZx_DPID_DATA0          (OTG_FS_HCTSIZx_DPID(0U))
#define  OTG_FS_HCTSIZx_DPID_DATA2          (OTG_FS_HCTSIZx_DPID(1U))
#define  OTG_FS_HCTSIZx_DPID_DATA1          (OTG_FS_HCTSIZx_DPID(2U))
#define  OTG_FS_HCTSIZx_DPID_MDATA          (OTG_FS_HCTSIZx_DPID(3U))
#define  OTG_FS_HCTSIZx_DPID_SETUP          (OTG_FS_HCTSIZx_DPID(3U))

// OTG_FS device configuration register (OTG_FS_DCFG)
#define  OTG_FS_DCFG_DSPD_POS               ((uint32_t)           0U)
#define  OTG_FS_DCFG_DSPD_MSK               ((uint32_t)    3U       )
#define  OTG_FS_DCFG_DSPD(x)               (((uint32_t)    x        ) & OTG_FS_DCFG_DSPD_MSK)
#define  OTG_FS_DCFG_NZLSOHSK               ((uint32_t)    1U  <<  2)
#define  OTG_FS_DCFG_DAD_POS                ((uint32_t)           4U)
#define  OTG_FS_DCFG_DAD_MSK                ((uint32_t) 0x7FU  << OTG_FS_DCFG_DAD_POS)
#define  OTG_FS_DCFG_DAD(x)                (((uint32_t)    x   << OTG_FS_DCFG_DAD_POS) & OTG_FS_DCFG_DAD_MSK)
#define  OTG_FS_DCFG_PFIVL_POS              ((uint32_t)           11U)
#define  OTG_FS_DCFG_PFIVL_MSK              ((uint32_t)    3U  << OTG_FS_DCFG_PFIVL_POS)
#define  OTG_FS_DCFG_PFIVL(x)              (((uint32_t)    x   << OTG_FS_DCFG_PFIVL_POS) & OTG_FS_DCFG_PFIVL_MSK)

// OTG_FS device control register (OTG_FS_DCTL)
#define  OTG_FS_DCTL_RWUSIG                 ((uint32_t)    1U       )
#define  OTG_FS_DCTL_SDIS                   ((uint32_t)    1U  <<  1)
#define  OTG_FS_DCTL_GINSTS                 ((uint32_t)    1U  <<  2)
#define  OTG_FS_DCTL_GONSTS                 ((uint32_t)    1U  <<  3)
#define  OTG_FS_DCTL_TCTL_POS               ((uint32_t)           4U)
#define  OTG_FS_DCTL_TCTL_MSK               ((uint32_t)    7U  << OTG_FS_DCTL_TCTL_POS)
#define  OTG_FS_DCTL_TCTL(x)               (((uint32_t)    x   << OTG_FS_DCTL_TCTL_POS) & OTG_FS_DCTL_TCTL_MSK)
#define  OTG_FS_DCTL_SGINAK                 ((uint32_t)    1U  <<  7)
#define  OTG_FS_DCTL_CGINAK                 ((uint32_t)    1U  <<  8)
#define  OTG_FS_DCTL_SGONAK                 ((uint32_t)    1U  <<  9)
#define  OTG_FS_DCTL_CGONAK                 ((uint32_t)    1U  << 10)
#define  OTG_FS_DCTL_POPRGDNE               ((uint32_t)    1U  << 11)

// OTG_FS device status register (OTG_FS_DSTS)
#define  OTG_FS_DSTS_SUSPSTS                ((uint32_t)    1U       )
#define  OTG_FS_DSTS_ENUMSPD_POS            ((uint32_t)           1U)
#define  OTG_FS_DSTS_ENUMSPD_MSK            ((uint32_t)    3U  << OTG_FS_DSTS_ENUMSPD_POS)
#define  OTG_FS_DSTS_ENUMSPD(x)            (((uint32_t)    x   << OTG_FS_DSTS_ENUMSPD_POS) & OTG_FS_DSTS_ENUMSPD_MSK)
#define  OTG_FS_DSTS_EERR                   ((uint32_t)    1U  <<  3)
#define  OTG_FS_DSTS_FNSOF_POS              ((uint32_t)           8U)
#define  OTG_FS_DSTS_FNSOF_MSK              ((uint32_t)0x3FFFU << OTG_FS_DSTS_FNSOF_POS)
#define  OTG_FS_DSTS_FNSOF(x)              (((uint32_t)    x   << OTG_FS_DSTS_FNSOF_POS) & OTG_FS_DSTS_FNSOF_MSK)

// OTG_FS device IN endpoint common interrupt mask register (OTG_FS_DIEPMSK)
#define  OTG_FS_DIEPMSK_XFRCM               ((uint32_t)    1U       )
#define  OTG_FS_DIEPMSK_EPDM                ((uint32_t)    1U  <<  1)
#define  OTG_FS_DIEPMSK_TOM                 ((uint32_t)    1U  <<  3)
#define  OTG_FS_DIEPMSK_ITTXFEMSK           ((uint32_t)    1U  <<  4)
#define  OTG_FS_DIEPMSK_INEPNMM             ((uint32_t)    1U  <<  5)
#define  OTG_FS_DIEPMSK_INEPNEM             ((uint32_t)    1U  <<  6)

// OTG_FS device OUT endpoint common interrupt mask register (OTG_FS_DOEPMSK)
#define  OTG_FS_DOEPMSK_XFRCM               ((uint32_t)    1U       )
#define  OTG_FS_DOEPMSK_EPDM                ((uint32_t)    1U  <<  1)
#define  OTG_FS_DOEPMSK_STUPM               ((uint32_t)    1U  <<  3)
#define  OTG_FS_DOEPMSK_OTEPDM              ((uint32_t)    1U  <<  4)

// OTG_FS device all endpoints interrupt register (OTG_FS_DAINT)
#define  OTG_FS_DAINT_IEPINT(in_ep)         ((uint32_t)((1U <<         in_ep)  & 0x0000FFFFU))
#define  OTG_FS_DAINT_OEPINT(out_ep)        ((uint32_t)((1U << (16U + out_ep)) & 0xFFFF0000U))

// OTG_FS device all endpoints interrupt mask register (OTG_FS_DAINTMSK)
#define  OTG_FS_DAINTMSK_IEPM(in_ep)        ((uint32_t)((1U <<         in_ep)  & 0x0000FFFFU))
#define  OTG_FS_DAINTMSK_OEPM(out_ep)       ((uint32_t)((1U << (16U + out_ep)) & 0xFFFF0000U))

// OTG_FS device Vbus discharge time register (OTG_FS_DVBUSDIS)
#define  OTG_FS_DVBUSDIS_VBUSDT_POS         ((uint32_t)           0U)
#define  OTG_FS_DVBUSDIS_VBUSDT_MSK         ((uint32_t)0xFFFFU      )
#define  OTG_FS_DVBUSDIS_VBUSDT(x)         (((uint32_t)    x        ) & OTG_FS_DVBUSDIS_VBUSDT_MSK)

// OTG_FS device Vbus pulsing time register (OTG_FS_DVBUSPULSE)
#define  OTG_FS_DVBUSPULSE_DVBUSP_POS       ((uint32_t)           0U)
#define  OTG_FS_DVBUSPULSE_DVBUSP_MSK       ((uint32_t)0xFFFU       )
#define  OTG_FS_DVBUSPULSE_DVBUSP(x)       (((uint32_t)    x        ) & OTG_FS_DVBUSPULSE_DVBUSP)

// OTG_FS device IN endpoint FIFO emtpy interrupt mask register (OTG_FS_DIEPEMPMSK)
#define  OTG_FS_DIEPEMPMSK_INEPTXFEM_POS    ((uint32_t)           0U)
#define  OTG_FS_DIEPEMPMSK_INEPTXFEM_MSK    ((uint32_t)0xFFFFU      )
#define  OTG_FS_DIEPEMPMSK_INEPTXFEM(x)    (((uint32_t)    x        ) & OTG_FS_DIEPEMPMSK_INEPTXFEM_MSK)

// OTG_FS device control IN endpoint 0 control register (OTG_FS_DIEPCTL0)
#define  OTG_FS_DIEPCTL0_MPSIZ_POS          ((uint32_t)           0U)
#define  OTG_FS_DIEPCTL0_MPSIZ_MSK          ((uint32_t)    3U       )
#define  OTG_FS_DIEPCTL0_MPSIZ(x)          (((uint32_t)    x        ) & OTG_FS_DIEPCTL0_MPSIZ_MSK)
#define  OTG_FS_DIEPCTL0_USBAEP             ((uint32_t)    1U  << 15)
#define  OTG_FS_DIEPCTL0_NAKSTS             ((uint32_t)    1U  << 17)
#define  OTG_FS_DIEPCTL0_EPTYP_POS          ((uint32_t)           18U)
#define  OTG_FS_DIEPCTL0_EPTYP_MSK          ((uint32_t)    3U  << OTG_FS_DIEPCTL0_EPTYP_POS)
#define  OTG_FS_DIEPCTL0_EPTYP(x)          (((uint32_t)    x   << OTG_FS_DIEPCTL0_EPTYP_POS) & OTG_FS_DIEPCTL0_EPTYP_MSK)
#define  OTG_FS_DIEPCTL0_STALL              ((uint32_t)    1U  << 21)
#define  OTG_FS_DIEPCTL0_TXFNUM_POS         ((uint32_t)           22U)
#define  OTG_FS_DIEPCTL0_TXFNUM_MSK         ((uint32_t) 0x0FU  << OTG_FS_DIEPCTL0_TXFNUM_POS)
#define  OTG_FS_DIEPCTL0_TXFNUM(x)         (((uint32_t)    x   << OTG_FS_DIEPCTL0_TXFNUM_POS) & OTG_FS_DIEPCTL0_TXFNUM_MSK)
#define  OTG_FS_DIEPCTL0_CNAK               ((uint32_t)    1U  << 26)
#define  OTG_FS_DIEPCTL0_SNAK               ((uint32_t)    1U  << 27)
#define  OTG_FS_DIEPCTL0_EPDIS              ((uint32_t)    1U  << 30)
#define  OTG_FS_DIEPCTL0_EPENA              ((uint32_t)    1U  << 31)

// OTG_FS device endpoint-x control register (OTG_FS_DIEPCTLx)
#define  OTG_FS_DIEPCTLx_MPSIZ_POS          ((uint32_t)           0U)
#define  OTG_FS_DIEPCTLx_MPSIZ_MSK          ((uint32_t)0x7FFU       )
#define  OTG_FS_DIEPCTLx_MPSIZ(x)          (((uint32_t)    x        ) & OTG_FS_DIEPCTLx_MPSIZ_MSK)
#define  OTG_FS_DIEPCTLx_USBAEP             ((uint32_t)    1U  << 15)
#define  OTG_FS_DIEPCTLx_EONUM_POS          ((uint32_t)           16U)
#define  OTG_FS_DIEPCTLx_EONUM              ((uint32_t)    1U  << OTG_FS_DIEPCTLx_EONUM_POS)
#define  OTG_FS_DIEPCTLx_DPID               ((uint32_t)    1U  << 16)
#define  OTG_FS_DIEPCTLx_NAKSTS             ((uint32_t)    1U  << 17)
#define  OTG_FS_DIEPCTLx_EPTYP_POS          ((uint32_t)           18U)
#define  OTG_FS_DIEPCTLx_EPTYP_MSK          ((uint32_t)    3U  << OTG_FS_DIEPCTLx_EPTYP_POS)
#define  OTG_FS_DIEPCTLx_EPTYP(x)          (((uint32_t)    x   << OTG_FS_DIEPCTLx_EPTYP_POS) & OTG_FS_DIEPCTLx_EPTYP_MSK)
#define  OTG_FS_DIEPCTLx_STALL              ((uint32_t)    1U  << 21)
#define  OTG_FS_DIEPCTLx_TXFNUM_POS         ((uint32_t)           22U)
#define  OTG_FS_DIEPCTLx_TXFNUM_MSK         ((uint32_t) 0x0FU  << OTG_FS_DIEPCTLx_TXFNUM_POS)
#define  OTG_FS_DIEPCTLx_TXFNUM(x)         (((uint32_t)    x   << OTG_FS_DIEPCTLx_TXFNUM_POS) & OTG_FS_DIEPCTLx_TXFNUM_MSK)
#define  OTG_FS_DIEPCTLx_CNAK               ((uint32_t)    1U  << 26)
#define  OTG_FS_DIEPCTLx_SNAK               ((uint32_t)    1U  << 27)
#define  OTG_FS_DIEPCTLx_SD0PID             ((uint32_t)    1U  << 28)
#define  OTG_FS_DIEPCTLx_SEVNFRM            ((uint32_t)    1U  << 28)
#define  OTG_FS_DIEPCTLx_SODDFRM            ((uint32_t)    1U  << 29)
#define  OTG_FS_DIEPCTLx_EPDIS              ((uint32_t)    1U  << 30)
#define  OTG_FS_DIEPCTLx_EPENA              ((uint32_t)    1U  << 31)

// OTG_FS device control OUT endpoint 0 control register (OTG_FS_DOEPCTL0)
#define  OTG_FS_DOEPCTL0_MPSIZ_POS          ((uint32_t)           0U)
#define  OTG_FS_DOEPCTL0_MPSIZ_MSK          ((uint32_t)    3U       )
#define  OTG_FS_DOEPCTL0_MPSIZ(x)          (((uint32_t)    x        ) & OTG_FS_DOEPCTL0_MPSIZ_MSK)
#define  OTG_FS_DOEPCTL0_USBAEP             ((uint32_t)    1U  << 15)
#define  OTG_FS_DOEPCTL0_NAKSTS             ((uint32_t)    1U  << 17)
#define  OTG_FS_DOEPCTL0_EPTYP_POS          ((uint32_t)           18U)
#define  OTG_FS_DOEPCTL0_EPTYP_MSK          ((uint32_t)    3U  << OTG_FS_DOEPCTL0_EPTYP_POS)
#define  OTG_FS_DOEPCTL0_EPTYP(x)          (((uint32_t)    x   << OTG_FS_DOEPCTL0_EPTYP_POS) & OTG_FS_DOEPCTL0_EPTYP_MSK)
#define  OTG_FS_DOEPCTL0_SNPM               ((uint32_t)    1U  << 20)
#define  OTG_FS_DOEPCTL0_STALL              ((uint32_t)    1U  << 21)
#define  OTG_FS_DOEPCTL0_CNAK               ((uint32_t)    1U  << 26)
#define  OTG_FS_DOEPCTL0_SNAK               ((uint32_t)    1U  << 27)
#define  OTG_FS_DOEPCTL0_EPDIS              ((uint32_t)    1U  << 30)
#define  OTG_FS_DOEPCTL0_EPENA              ((uint32_t)    1U  << 31)

// OTG_FS device endpoint-x control register (OTG_FS_DOEPCTLx)
#define  OTG_FS_DOEPCTLx_MPSIZ_POS          ((uint32_t)           0U)
#define  OTG_FS_DOEPCTLx_MPSIZ_MSK          ((uint32_t)0x7FFU       )
#define  OTG_FS_DOEPCTLx_MPSIZ(x)          (((uint32_t)    x        ) & OTG_FS_DOEPCTLx_MPSIZ_MSK)
#define  OTG_FS_DOEPCTLx_USBAEP             ((uint32_t)    1U  << 15)
#define  OTG_FS_DOEPCTLx_EONUM_POS          ((uint32_t)           16U)
#define  OTG_FS_DOEPCTLx_EONUM              ((uint32_t)    1U  << OTG_FS_DOEPCTLx_EONUM_POS)
#define  OTG_FS_DOEPCTLx_DPID               ((uint32_t)    1U  << 16)
#define  OTG_FS_DOEPCTLx_NAKSTS             ((uint32_t)    1U  << 17)
#define  OTG_FS_DOEPCTLx_EPTYP_POS          ((uint32_t)           18U)
#define  OTG_FS_DOEPCTLx_EPTYP_MSK          ((uint32_t)    3U  << OTG_FS_DOEPCTLx_EPTYP_POS)
#define  OTG_FS_DOEPCTLx_EPTYP(x)          (((uint32_t)    x   << OTG_FS_DOEPCTLx_EPTYP_POS) & OTG_FS_DOEPCTLx_EPTYP_MSK)
#define  OTG_FS_DOEPCTLx_SNPM               ((uint32_t)    1U  << 20)
#define  OTG_FS_DOEPCTLx_STALL              ((uint32_t)    1U  << 21)
#define  OTG_FS_DOEPCTLx_CNAK               ((uint32_t)    1U  << 26)
#define  OTG_FS_DOEPCTLx_SNAK               ((uint32_t)    1U  << 27)
#define  OTG_FS_DOEPCTLx_SD0PID             ((uint32_t)    1U  << 28)
#define  OTG_FS_DOEPCTLx_SEVNFRM            ((uint32_t)    1U  << 28)
#define  OTG_FS_DOEPCTLx_SODDFRM            ((uint32_t)    1U  << 29)
#define  OTG_FS_DOEPCTLx_SD1PID             ((uint32_t)    1U  << 29)
#define  OTG_FS_DOEPCTLx_EPDIS              ((uint32_t)    1U  << 30)
#define  OTG_FS_DOEPCTLx_EPENA              ((uint32_t)    1U  << 31)

// OTG_FS device endpoint-x interrupt register (OTG_FS_DIEPINTx)
#define  OTG_FS_DIEPINTx_XFCR               ((uint32_t)    1U       )
#define  OTG_FS_DIEPINTx_EPDISD             ((uint32_t)    1U  <<  1)
#define  OTG_FS_DIEPINTx_TOC                ((uint32_t)    1U  <<  3)
#define  OTG_FS_DIEPINTx_ITTXFE             ((uint32_t)    1U  <<  4)
#define  OTG_FS_DIEPINTx_INEPNE             ((uint32_t)    1U  <<  6)
#define  OTG_FS_DIEPINTx_TXFE               ((uint32_t)    1U  <<  7)

// OTG_FS device endpoint-x interrupt register (OTG_FS_DOEPINTx)
#define  OTG_FS_DOEPINTx_XFCR               ((uint32_t)    1U       )
#define  OTG_FS_DOEPINTx_EPDISD             ((uint32_t)    1U  <<  1)
#define  OTG_FS_DOEPINTx_STUP               ((uint32_t)    1U  <<  3)
#define  OTG_FS_DOEPINTx_OTEPDIS            ((uint32_t)    1U  <<  4)
#define  OTG_FS_DOEPINTx_B2BSTUP            ((uint32_t)    1U  <<  6)

// OTG_FS device IN endpoint 0 transfer size register (OTG_FS_DIEPTSIZ0)
#define  OTG_FS_DIEPTSIZ0_XFRSIZ_POS        ((uint32_t)           0U)
#define  OTG_FS_DIEPTSIZ0_XFRSIZ_MSK        ((uint32_t) 0x7FU       )
#define  OTG_FS_DIEPTSIZ0_XFRSIZ(x)        (((uint32_t)    x        ) & OTG_FS_DIEPTSIZ0_XFRSIZ_MSK)
#define  OTG_FS_DIEPTSIZ0_PKTCNT_POS        ((uint32_t)           19U)
#define  OTG_FS_DIEPTSIZ0_PKTCNT_MSK        ((uint32_t)    3U  << OTG_FS_DIEPTSIZ0_PKTCNT_POS)
#define  OTG_FS_DIEPTSIZ0_PKTCNT(x)        (((uint32_t)    x   << OTG_FS_DIEPTSIZ0_PKTCNT_POS) & OTG_FS_DIEPTSIZ0_PKTCNT_MSK)

// OTG_FS device OUT endpoint 0 transfer size register (OTG_FS_DOEPTSIZ0)
#define  OTG_FS_DOEPTSIZ0_XFRSIZ_POS        ((uint32_t)           0U)
#define  OTG_FS_DOEPTSIZ0_XFRSIZ_MSK        ((uint32_t) 0x7FU       )
#define  OTG_FS_DOEPTSIZ0_XFRSIZ(x)        (((uint32_t)    x        ) & OTG_FS_DOEPTSIZ0_XFRSIZ_MSK)
#define  OTG_FS_DOEPTSIZ0_PKTCNT            ((uint32_t)    1U  << 19)
#define  OTG_FS_DOEPTSIZ0_STUPCNT_POS       ((uint32_t)           29U)
#define  OTG_FS_DOEPTSIZ0_STUPCNT_MSK       ((uint32_t)    3U  << OTG_FS_DOEPTSIZ0_STUPCNT_POS)
#define  OTG_FS_DOEPTSIZ0_STUPCNT(x)       (((uint32_t)    x   << OTG_FS_DOEPTSIZ0_STUPCNT_POS) & OTG_FS_DOEPTSIZ0_STUPCNT_MSK)

// OTG_FS device IN endpoints transver size register (OTG_FS_DIEPTSIZx)
#define  OTG_FS_DIEPTSIZx_XFRSIZ_POS        ((uint32_t)           0U)
#define  OTG_FS_DIEPTSIZx_XFRSIZ_MSK        ((uint32_t)0x7FFFFU     )
#define  OTG_FS_DIEPTSIZx_XFRSIZ(x)        (((uint32_t)    x        ) & OTG_FS_DIEPTSIZx_XFRSIZ_MSK)
#define  OTG_FS_DIEPTSIZx_PKTCNT_POS        ((uint32_t)           19U)
#define  OTG_FS_DIEPTSIZx_PKTCNT_MSK        ((uint32_t)0x3FFU  << OTG_FS_DIEPTSIZx_PKTCNT_POS)
#define  OTG_FS_DIEPTSIZx_PKTCNT(x)        (((uint32_t)    x   << OTG_FS_DIEPTSIZx_PKTCNT_POS) & OTG_FS_DIEPTSIZx_PKTCNT_MSK)
#define  OTG_FS_DIEPTSIZx_MCNT_POS          ((uint32_t)           29U)
#define  OTG_FS_DIEPTSIZx_MCNT_MSK          ((uint32_t)    3U  << OTG_FS_DIEPTSIZx_MCNT_POS)
#define  OTG_FS_DIEPTSIZx_MCNT(x)          (((uint32_t)    x   << OTG_FS_DIEPTSIZx_MCNT_POS) & OTG_FS_DIEPTSIZx_MCNT_MSK)

// OTG_FS device IN endpoint transmit FIFO status register (OTG_FS_DTXFSTSx)
#define  OTG_FS_DTXFSTSx_INEPTFSAV_POS      ((uint32_t)           0U)
#define  OTG_FS_DTXFSTSx_INEPTFSAV_MSK      ((uint32_t)0xFFFFU      )
#define  OTG_FS_DTXFSTSx_INEPTFSAV(x)      (((uint32_t)    x        ) & OTG_FS_DTXFSTSx_INEPTFSAV_MSK)

// OTG_FS device OUT endpoint-x transfer size register (OTG_FS_DOEPTSIZx)
#define  OTG_FS_DOEPTSIZx_XFRSIZ_POS        ((uint32_t)           0U)
#define  OTG_FS_DOEPTSIZx_XFRSIZ_MSK        ((uint32_t)0x7FFFFU     )
#define  OTG_FS_DOEPTSIZx_XFRSIZ(x)        (((uint32_t)    x        ) & OTG_FS_DOEPTSIZx_XFRSIZ_MSK)
#define  OTG_FS_DOEPTSIZx_PKTCNT_POS        ((uint32_t)           19U)
#define  OTG_FS_DOEPTSIZx_PKTCNT_MSK        ((uint32_t)0x3FFU  << OTG_FS_DOEPTSIZx_PKTCNT_POS)
#define  OTG_FS_DOEPTSIZx_PKTCNT(x)        (((uint32_t)    x   << OTG_FS_DOEPTSIZx_PKTCNT_POS) & OTG_FS_DOEPTSIZx_PKTCNT_MSK)
#define  OTG_FS_DOEPTSIZx_RXDPID_POS        ((uint32_t)           29U)
#define  OTG_FS_DOEPTSIZx_RXDPID_MSK        ((uint32_t)    3U  << OTG_FS_DOEPTSIZx_RXDPID_POS)
#define  OTG_FS_DOEPTSIZx_RXDPID(x)        (((uint32_t)    x   << OTG_FS_DOEPTSIZx_RXDPID_POS) & OTG_FS_DOEPTSIZx_RXDPID_MSK)
#define  OTG_FS_DOEPTSIZx_STUPCNT_POS       ((uint32_t)           29U)
#define  OTG_FS_DOEPTSIZx_STUPCNT_MSK       ((uint32_t)    3U  << OTG_FS_DOEPTSIZx_STUPCNT_POS)
#define  OTG_FS_DOEPTSIZx_STUPCNT(x)       (((uint32_t)    x   << OTG_FS_DOEPTSIZx_STUPCNT_POS) & OTG_FS_DOEPTSIZx_STUPCNT_MSK)

// OTG_FS power and gate clock gating control register (OTG_FS_PCGCCTL)
#define  OTG_FS_PCGCCTL_STPPCLK             ((uint32_t)    1U       )
#define  OTG_FS_PCGCCTL_GATEHCLK            ((uint32_t)    1U  <<  1)
#define  OTG_FS_PCGCCTL_PHYSUSP             ((uint32_t)    1U  <<  4)

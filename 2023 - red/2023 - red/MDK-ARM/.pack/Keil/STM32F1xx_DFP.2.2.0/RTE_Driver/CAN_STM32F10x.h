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
 * $Date:        24. December 2015
 * $Revision:    V1.1
 *
 * Driver:       Driver_CAN1/2
 * Configured:   via RTE_Device.h configuration file
 * Project:      CAN Driver Header for ST STM32F1xx
 * -------------------------------------------------------------------------- */

#ifndef __CAN_STM32F1XX_H
#define __CAN_STM32F1XX_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Driver_CAN.h"
#include "stm32f10x.h"
#include "GPIO_STM32F10x.h"

#include "RTE_Components.h"
#include "RTE_Device.h"

#if   ((defined(RTE_Drivers_CAN1) ||  defined(RTE_Drivers_CAN2)) && \
      ((RTE_CAN1 == 0)            && (RTE_CAN2 == 0)))
#error "No CAN configured in RTE_Device.h!"
#endif

#if    (RTE_CAN1 != 0)
#define MX_CAN1                         1

/* Pin CAN1_RX */
#define MX_CAN1_RX_Pin                  1
#define MX_CAN1_RX_GPIOx                RTE_CAN1_RX_PORT
#define MX_CAN1_RX_GPIO_Pin             RTE_CAN1_RX_BIT
#if    (RTE_CAN1_RX_PORT_ID == 0)
#define MX_CAN1_RX_GPIO_AF              AFIO_CAN_PA11_PA12
#elif  (RTE_CAN1_RX_PORT_ID == 1)
#define MX_CAN1_RX_GPIO_AF              AFIO_CAN_PB8_PB9
#elif  (RTE_CAN1_RX_PORT_ID == 2)
#define MX_CAN1_RX_GPIO_AF              AFIO_CAN_PD0_PD1
#else
#error "Unknown alternate function for CAN1_RX pin!"
#endif
#define MX_CAN1_RX_GPIO_Conf            GPIO_IN_FLOATING
#define MX_CAN1_RX_GPIO_Mode            GPIO_MODE_INPUT

/* Pin CAN1_TX */
#define MX_CAN1_TX_Pin                  1
#define MX_CAN1_TX_GPIOx                RTE_CAN1_TX_PORT
#define MX_CAN1_TX_GPIO_Pin             RTE_CAN1_TX_BIT
#if    (RTE_CAN1_TX_PORT_ID == 0)
#define MX_CAN1_TX_GPIO_AF              AFIO_CAN_PA11_PA12
#elif  (RTE_CAN1_TX_PORT_ID == 1)
#define MX_CAN1_TX_GPIO_AF              AFIO_CAN_PB8_PB9
#elif  (RTE_CAN1_TX_PORT_ID == 2)
#define MX_CAN1_TX_GPIO_AF              AFIO_CAN_PD0_PD1
#else
#error "Unknown alternate function for CAN1_TX pin!"
#endif
#define MX_CAN1_TX_GPIO_Conf            GPIO_AF_PUSHPULL
#define MX_CAN1_TX_GPIO_Mode            GPIO_MODE_OUT2MHZ
#endif

#if    (RTE_CAN2 != 0)
#define MX_CAN2                         1

/* Pin CAN2_RX */
#define MX_CAN2_RX_Pin                  1
#define MX_CAN2_RX_GPIOx                RTE_CAN2_RX_PORT
#define MX_CAN2_RX_GPIO_Pin             RTE_CAN2_RX_BIT
#if    (RTE_CAN2_RX_PORT_ID == 0)
#define MX_CAN2_RX_GPIO_AF              AFIO_CAN2_REMAP
#elif  (RTE_CAN2_RX_PORT_ID == 1)
#define MX_CAN2_RX_GPIO_AF              AFIO_CAN2_NO_REMAP
#else
#error "Unknown alternate function for CAN2_RX pin!"
#endif
#define MX_CAN2_RX_GPIO_Conf            GPIO_IN_FLOATING
#define MX_CAN2_RX_GPIO_Mode            GPIO_MODE_INPUT

/* Pin CAN2_TX */
#define MX_CAN2_TX_Pin                  1
#define MX_CAN2_TX_GPIOx                RTE_CAN2_TX_PORT
#define MX_CAN2_TX_GPIO_Pin             RTE_CAN2_TX_BIT
#if    (RTE_CAN2_TX_PORT_ID == 0)
#define MX_CAN2_TX_GPIO_AF              AFIO_CAN2_REMAP
#elif  (RTE_CAN2_TX_PORT_ID == 1)
#define MX_CAN2_TX_GPIO_AF              AFIO_CAN2_NO_REMAP
#else
#error "Unknown alternate function for CAN2_TX pin!"
#endif
#define MX_CAN2_TX_GPIO_Conf            GPIO_AF_PUSHPULL
#define MX_CAN2_TX_GPIO_Mode            GPIO_MODE_OUT2MHZ
#endif

#ifndef STM32F10X_CL
#define CAN1_TX_IRQn                    USB_HP_CAN1_TX_IRQn
#define CAN1_RX0_IRQn                   USB_LP_CAN1_RX0_IRQn
#endif

#if    !defined(MX_CAN1)
#define MX_CAN1                         (0U)
#endif
#if    !defined(MX_CAN2)
#define MX_CAN2                         (0U)
#endif

#if    (MX_CAN2 == 1U)
#define CAN_CTRL_NUM                    (2U)
#else
#define CAN_CTRL_NUM                    (1U)
#endif

#endif // __CAN_STM32F1XX_H

/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_Config_HID.h
 * Purpose: USB Host Human Interface Device class (HID) Configuration
 * Rev.:    V5.01
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>USB Host: Human Interface Device class (HID)
//   <o> Number of concurrent HID Devices in system <0-15>
#define USBH_HID_NUM                        1

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>HID Interrupt In Pipe handling Thread Stack Size <64-65536>
#define USBH_HID_INT_IN_THREAD_STACK_SIZE   384

//        HID Interrupt In Pipe handling Thread Priority
#define USBH_HID_INT_IN_THREAD_PRIORITY     osPriorityAboveNormal

//   </h>
// </h>

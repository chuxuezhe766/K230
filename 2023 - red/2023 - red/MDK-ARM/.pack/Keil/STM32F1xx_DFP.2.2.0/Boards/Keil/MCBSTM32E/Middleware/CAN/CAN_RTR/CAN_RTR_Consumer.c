/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::CAN
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    CAN_RTR_Consumer.c
 * Purpose: CAN transmit RTR/receive data Example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"

#include "stm32f10x.h"

#include "RTE_Components.h"

#include "Driver_CAN.h"

// CAN Driver Controller selector
#define  CAN_CONTROLLER         1       // CAN Controller number

// Enable/disable display usage for user interface output
#define  CAN_UI_GLCD            1       // 0 = no graphic LCD, 1 = use graphic LCD

#if     (CAN_UI_GLCD == 1)
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#endif

#define _CAN_Driver_(n)         Driver_CAN##n
#define  CAN_Driver_(n)        _CAN_Driver_(n)
extern   ARM_DRIVER_CAN         CAN_Driver_(CAN_CONTROLLER);
#define  ptrCAN               (&CAN_Driver_(CAN_CONTROLLER))

#if     (CAN_UI_GLCD == 1)
extern   GLCD_FONT              GLCD_Font_6x8;
extern   GLCD_FONT              GLCD_Font_16x24;
#endif

osThreadId                      ui_output_thread_id;
uint8_t                         rx_data_val            = 0U;
uint32_t                        rx_data_cnt            = 0U;
uint32_t                        rx_data_obj_idx        = 0xFFFFFFFFU;
uint8_t                         rx_data[8];
ARM_CAN_MSG_INFO                rx_data_msg_info;
uint32_t                        tx_rtr_rx_data_obj_idx = 0xFFFFFFFFU;
uint32_t                        tx_rtr_obj_idx         = 0xFFFFFFFFU;
ARM_CAN_MSG_INFO                tx_rtr_msg_info;

#ifdef RTE_CMSIS_RTOS_RTX
extern   uint32_t               os_time;
uint32_t HAL_GetTick (void) {
  return os_time;
}
#endif

/*------------------------------------------------------------------------------
 *        Error Handler
 *----------------------------------------------------------------------------*/
static void Error_Handler (void) {
  while (1);
}

/*------------------------------------------------------------------------------
 *        CAN Signal Unit Event Callback
 *----------------------------------------------------------------------------*/
void CAN_SignalUnitEvent (uint32_t event) {

  switch (event) {
    case ARM_CAN_EVENT_UNIT_ACTIVE:
      break;
    case ARM_CAN_EVENT_UNIT_WARNING:
      break;
    case ARM_CAN_EVENT_UNIT_PASSIVE:
      break;
    case ARM_CAN_EVENT_UNIT_BUS_OFF:
      break;
  }
}

/*------------------------------------------------------------------------------
 *        CAN Signal Object Event Callback
 *----------------------------------------------------------------------------*/
void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event) {

  if (event == ARM_CAN_EVENT_RECEIVE) {         // If receive event
    // Read received data
    if (ptrCAN->MessageRead(obj_idx, &rx_data_msg_info, rx_data, 8U) > 0U) {
      rx_data_val = rx_data[0];                 // Store received value for output to user interface
      rx_data_cnt++;                            // Increment receive counter
      osSignalSet(ui_output_thread_id, 1U);     // Send signal to user interface thread
    }
  }
}

/*------------------------------------------------------------------------------
 *        User Interface Output Thread
 *----------------------------------------------------------------------------*/
void UI_Output_Thread (void const *arg) {
  osEvent event;

#if (CAN_UI_GLCD == 1)
  char disp_str[7];

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, " CAN RTR - Consumer ");
  GLCD_DrawString         (0, 1*24, "      Example       ");
  GLCD_DrawString         (0, 2*24, "   Tx RTR/Rx data   ");
  GLCD_DrawString         (0, 5*24, "Data rx cnt:        ");
  GLCD_DrawString         (0, 6*24, "Data rx val:        ");
  GLCD_DrawString         (0, 8*24, "  Keil Tools by ARM ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");
#endif
  printf ("CAN RTR - Consumer Example\nTx RTR/Rx data\n");

  while (1) {
    event = osSignalWait(0, osWaitForever);
    if (event.status == osEventSignal) {
      if (event.value.signals & 1U) {           // If receive event happened
        printf ("Received value    = 0x%02X\n", rx_data_val);
#if (CAN_UI_GLCD == 1)
        sprintf(disp_str, "%-6i",        rx_data_cnt);
        GLCD_DrawString(13U*16U, 5U*24U, disp_str);
        sprintf(disp_str, "0x%02X",      rx_data_val);
        GLCD_DrawString(13U*16U, 6U*24U, disp_str);
#endif
      }
    }
  }
}
osThreadDef(UI_Output_Thread, osPriorityNormal, 1U, NULL);

/*------------------------------------------------------------------------------
 *        Application
 *----------------------------------------------------------------------------*/
int main (void) {
  ARM_CAN_CAPABILITIES     can_cap;
  ARM_CAN_OBJ_CAPABILITIES can_obj_cap;
  int32_t                  status;
  uint32_t                 i, num_objects, clock;

  ui_output_thread_id = osThreadCreate (osThread(UI_Output_Thread), NULL);

  can_cap = ptrCAN->GetCapabilities (); // Get CAN driver capabilities
  num_objects = can_cap.num_objects;    // Number of receive/transmit objects

  status = ptrCAN->Initialize    (CAN_SignalUnitEvent, CAN_SignalObjectEvent);  // Initialize CAN driver
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }

  status = ptrCAN->PowerControl  (ARM_POWER_FULL);                              // Power-up CAN controller
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }

  status = ptrCAN->SetMode       (ARM_CAN_MODE_INITIALIZATION);                 // Activate initialization mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }

  clock = ptrCAN->GetClock();                                                   // Get CAN bas clock
  if ((clock % (8U*125000U)) == 0U) {                                           // If CAN base clock is divisible by 8 * 125000 without remainder
    status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,                    // Set nominal bitrate
                                    125000U,                                    // Set bitrate to 125 kbit/s
                                    ARM_CAN_BIT_PROP_SEG(5U)   |                // Set propagation segment to 5 time quanta
                                    ARM_CAN_BIT_PHASE_SEG1(1U) |                // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                                    ARM_CAN_BIT_PHASE_SEG2(1U) |                // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                                    ARM_CAN_BIT_SJW(1U));                       // Resynchronization jump width is same as phase segment 2
  } else if ((clock % (10U*125000U)) == 0U) {                                   // If CAN base clock is divisible by 10 * 125000 without remainder
    status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,                    // Set nominal bitrate
                                    125000U,                                    // Set bitrate to 125 kbit/s
                                    ARM_CAN_BIT_PROP_SEG(7U)   |                // Set propagation segment to 7 time quanta
                                    ARM_CAN_BIT_PHASE_SEG1(1U) |                // Set phase segment 1 to 1 time quantum (sample point at 90% of bit time)
                                    ARM_CAN_BIT_PHASE_SEG2(1U) |                // Set phase segment 2 to 1 time quantum (total bit is 10 time quanta long)
                                    ARM_CAN_BIT_SJW(1U));                       // Resynchronization jump width is same as phase segment 2
  } else if ((clock % (12U*125000U)) == 0U) {                                   // If CAN base clock is divisible by 12 * 125000 without remainder
    status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,                    // Set nominal bitrate
                                    125000U,                                    // Set bitrate to 125 kbit/s
                                    ARM_CAN_BIT_PROP_SEG(7U)   |                // Set propagation segment to 7 time quanta
                                    ARM_CAN_BIT_PHASE_SEG1(2U) |                // Set phase segment 1 to 2 time quantum (sample point at 83.3% of bit time)
                                    ARM_CAN_BIT_PHASE_SEG2(2U) |                // Set phase segment 2 to 2 time quantum (total bit is 12 time quanta long)
                                    ARM_CAN_BIT_SJW(2U));                       // Resynchronization jump width is same as phase segment 2
  } else {
    Error_Handler();
  }
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }

  for (i = 0U; i < num_objects; i++) {                                          // Find first available object for receive and transmit
    can_obj_cap = ptrCAN->ObjectGetCapabilities (i);                            // Get object capabilities
    if      ((tx_rtr_rx_data_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.tx_rtr_rx_data == 1U)) { tx_rtr_rx_data_obj_idx = i; break; }
    else if ((rx_data_obj_idx        == 0xFFFFFFFFU) && (can_obj_cap.rx             == 1U)) { rx_data_obj_idx        = i;        }
    else if ((tx_rtr_obj_idx         == 0xFFFFFFFFU) && (can_obj_cap.tx             == 1U)) { tx_rtr_obj_idx         = i; break; }
  }
  if ((tx_rtr_rx_data_obj_idx == 0xFFFFFFFFU) && ((rx_data_obj_idx == 0xFFFFFFFFU) || (tx_rtr_obj_idx == 0xFFFFFFFFU))) { Error_Handler(); }

  if (tx_rtr_rx_data_obj_idx != 0xFFFFFFFFU) {                                  // If tx RTR rx data object is available
    // Set filter to receive messages with extended ID 0x11223344
    status = ptrCAN->ObjectSetFilter(tx_rtr_rx_data_obj_idx, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x11223344U), 0U);
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }

    // Configure tx RTR rx data object
    status = ptrCAN->ObjectConfigure(tx_rtr_rx_data_obj_idx, ARM_CAN_OBJ_TX_RTR_RX_DATA);
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  } else {
    // Set filter to receive messages with extended ID 0x11223344
    status = ptrCAN->ObjectSetFilter(rx_data_obj_idx,        ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x11223344U), 0U);
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }

    status = ptrCAN->ObjectConfigure(tx_rtr_obj_idx,  ARM_CAN_OBJ_TX);          // Configure standard transmit object
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }

    status = ptrCAN->ObjectConfigure(rx_data_obj_idx, ARM_CAN_OBJ_RX);          // Configure standard receive object
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  }

  status = ptrCAN->SetMode (ARM_CAN_MODE_NORMAL);                               // Activate normal operation mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }

  memset(&tx_rtr_msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));                       // Clear RTR message info structure
  tx_rtr_msg_info.id  = ARM_CAN_EXTENDED_ID(0x11223344U);                       // Set extended ID for transmit RTR message
  tx_rtr_msg_info.rtr = 1U;                                                     // Set RTR flag
  tx_rtr_msg_info.dlc = 1U;                                                     // Request 1 data byte
  while (1) {
    osDelay(1000U);
    if (tx_rtr_rx_data_obj_idx != 0xFFFFFFFFU) {                                // If tx RTR rx data object is used
      // Send RTR message
      status = ptrCAN->MessageSend(tx_rtr_rx_data_obj_idx, &tx_rtr_msg_info, 0U, 0U);
    } else {                                                                    // If standard tx object is used
      // Send RTR message
      status = ptrCAN->MessageSend(tx_rtr_obj_idx,         &tx_rtr_msg_info, 0U, 0U);
    }
    if ((status != ARM_DRIVER_OK) && (status != ARM_DRIVER_ERROR_BUSY)) { Error_Handler(); }
  }
}

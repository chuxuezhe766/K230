/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::CAN
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    CAN_RTR_Producer.c
 * Purpose: CAN receive RTR/transmit data Example
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
uint32_t                        rx_rtr_tx_data_obj_idx = 0xFFFFFFFFU;
uint32_t                        rx_rtr_obj_idx         = 0xFFFFFFFFU;
ARM_CAN_MSG_INFO                rx_rtr_msg_info;
uint8_t                         tx_data_val            = 0U;
uint32_t                        tx_data_cnt            = 0U;
uint32_t                        tx_data_obj_idx        = 0xFFFFFFFFU;
uint8_t                         tx_data[8];
ARM_CAN_MSG_INFO                tx_data_msg_info;

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
    if (obj_idx == rx_rtr_obj_idx) {            // If receive object event
      if (ptrCAN->MessageRead(rx_rtr_obj_idx, &rx_rtr_msg_info, 0U, 0U) == 0U) {
                                                // Check if received message is RTR message
        if ((rx_rtr_msg_info.rtr == 1U) && (rx_rtr_msg_info.id == ARM_CAN_EXTENDED_ID(0x11223344U))) {
          tx_data[0]++;                         // Increment transmit data
          // Send data message with number of data bytes requested by RTR message
          ptrCAN->MessageSend(tx_data_obj_idx, &tx_data_msg_info, tx_data, rx_rtr_msg_info.dlc);
        }
      }
    }
  }
  if (event == ARM_CAN_EVENT_SEND_COMPLETE) {   // If send completed event
    tx_data_val = tx_data[0];                   // Store transmitted value for output to user interface
    tx_data_cnt++;                              // Increment transmit counter
    osSignalSet(ui_output_thread_id, 1U);       // Send signal to user interface thread
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
  GLCD_DrawString         (0, 0*24, " CAN RTR - Producer ");
  GLCD_DrawString         (0, 1*24, "      Example       ");
  GLCD_DrawString         (0, 2*24, "   Rx RTR/Tx data   ");
  GLCD_DrawString         (0, 5*24, "Data tx cnt:        ");
  GLCD_DrawString         (0, 6*24, "Data tx val:        ");
  GLCD_DrawString         (0, 8*24, "  Keil Tools by ARM ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");
#endif
  printf ("CAN RTR - Producer Example\nRx RTR/Tx data\n");

  while (1) {
    event = osSignalWait(0, osWaitForever);
    if (event.status == osEventSignal) {
      if (event.value.signals & 1U) {           // If transmit event happened
        printf ("Transmitted value = 0x%02X\n", tx_data_val);
#if (CAN_UI_GLCD == 1)
        sprintf(disp_str, "%-6i",        tx_data_cnt);
        GLCD_DrawString(13U*16U, 5U*24U, disp_str);
        sprintf(disp_str, "0x%02X",      tx_data_val);
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
    if      ((rx_rtr_tx_data_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.rx_rtr_tx_data == 1U)) { rx_rtr_tx_data_obj_idx = i; break; }
    else if ((rx_rtr_obj_idx         == 0xFFFFFFFFU) && (can_obj_cap.rx             == 1U)) { rx_rtr_obj_idx         = i;        }
    else if ((tx_data_obj_idx        == 0xFFFFFFFFU) && (can_obj_cap.tx             == 1U)) { tx_data_obj_idx        = i; break; }
  }
  if ((rx_rtr_tx_data_obj_idx == 0xFFFFFFFFU) && ((rx_rtr_obj_idx == 0xFFFFFFFFU) || (tx_data_obj_idx == 0xFFFFFFFFU))) { Error_Handler(); }

  if (rx_rtr_tx_data_obj_idx != 0xFFFFFFFFU) {                                  // If rx RTR tx data object is available
    // Set filter to receive messages with extended ID 0x11223344
    status = ptrCAN->ObjectSetFilter(rx_rtr_tx_data_obj_idx, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x11223344U), 0U);
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }

    // Configure rx RTR tx data object
    status = ptrCAN->ObjectConfigure(rx_rtr_tx_data_obj_idx, ARM_CAN_OBJ_RX_RTR_TX_DATA);
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  } else {
    // Set filter to receive messages with extended ID 0x11223344
    status = ptrCAN->ObjectSetFilter(rx_rtr_obj_idx,         ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x11223344U), 0U);
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }

    status = ptrCAN->ObjectConfigure(tx_data_obj_idx, ARM_CAN_OBJ_TX);          // Configure standard transmit object
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }

    status = ptrCAN->ObjectConfigure(rx_rtr_obj_idx,  ARM_CAN_OBJ_RX);          // Configure standard receive object
    if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  }

  status = ptrCAN->SetMode (ARM_CAN_MODE_NORMAL);                               // Activate normal operation mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }

  memset(&tx_data_msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));                      // Clear data message info structure
  tx_data_msg_info.id = ARM_CAN_EXTENDED_ID(0x11223344U);                       // Set extended ID for transmit data message
  tx_data[0]          = 0xFFU;                                                  // Initialize transmit data
  while (1) {
    if (rx_rtr_tx_data_obj_idx != 0xFFFFFFFFU) {                                // If rx RTR tx data object is used
      tx_data[0]++;                                                             // Increment transmit data
      // Set data to be transmitted automatically upon RTR reception
      status = ptrCAN->MessageSend(rx_rtr_tx_data_obj_idx, &tx_data_msg_info, tx_data, 1U);
      if (status != 1U) { Error_Handler(); }
    }
    osDelay(1000U);
  }
}

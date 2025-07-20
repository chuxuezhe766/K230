/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SNMP_Agent_MIB.c
 * Purpose: SNMP Agent Management Information Base Module
 * Rev.:    V6.00
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "rl_net.h"
#include "Board_LED.h"

/* snmp_agent.c */
extern bool LCDupdate;
extern char lcd_text[2][20+1];
extern uint8_t get_button (void);

/* System */
extern uint32_t snmp_SysUpTime;

/* Local variables */
static uint8_t  LedOut;
static uint8_t  KeyIn;

/* MIB read only integer constants */
static const uint8_t sysServices = 79;

/* MIB event callback functions */
static void write_leds (int mode);
static void read_key (int mode);
static void upd_display (int mode);

/// MIB data table
const SNMP_MIB snmp_mib[] = {
// ---------- System MIB ----------- 
  /* SysDescr Entry */
  { MIB_OCTET_STR | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 1, 0},
    MIB_STR("Embedded System SNMP V1.0"),
    NULL },
  /* SysObjectID Entry */
  { MIB_OBJECT_ID | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 2, 0},
    MIB_STR("\x2b\x06\x01\x02\x01\x01\x02\x00"),
    NULL },
  /* SysUpTime Entry */
  { MIB_TIME_TICKS | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 3, 0},
    4, &snmp_SysUpTime,
    NULL },
  /* SysContact Entry */
  { MIB_OCTET_STR | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 4, 0},
    MIB_STR("test@keil.com"),
    NULL },
  /* SysName Entry */
  { MIB_OCTET_STR | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 5, 0},
    MIB_STR("Evaluation board"),
    NULL },
  /* SysLocation Entry */
  { MIB_OCTET_STR | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 6, 0},
    MIB_STR("Local"),
    NULL },
  /* SysServices Entry */
  { MIB_INTEGER | MIB_ATR_RO,
    8, {MIB_OID0(1,3), 6, 1, 2, 1, 1, 7, 0},
    MIB_INT(sysServices),
    NULL },
// ---------- Experimental MIB -----------
  /* LedOut Entry */
  { MIB_INTEGER,
    6, {MIB_OID0(1,3), 6, 1, 3, 1, 0},
    MIB_INT(LedOut),
    write_leds },
  /* KeyIn Entry */
  { MIB_INTEGER | MIB_ATR_RO,
    6, {MIB_OID0(1,3), 6, 1, 3, 2, 0},
    MIB_INT(KeyIn),
    read_key },
  /* LCD line1 Entry */
  { MIB_OCTET_STR,
    6, {MIB_OID0(1,3), 6, 1, 3, 3, 0},
    MIB_STR(lcd_text[0]),
    upd_display },
  /* LCD line2 Entry */
  { MIB_OCTET_STR,
    6, {MIB_OID0(1,3), 6, 1, 3, 4, 0},
    MIB_STR(lcd_text[1]),
    upd_display },
};
const int snmp_mib_size = SNMP_MIB_SIZE(snmp_mib);

/// MIB callback functions
static void write_leds (int mode) {
  /* Write onboard LED */
  if (mode == MIB_WRITE) {
    LED_SetOut (LedOut);
  }
}

static void read_key (int mode) {
  /* Read digital input */
  if (mode == MIB_READ) {
    KeyIn = get_button();
  }
}

static void upd_display (int mode) {
  /* Write text on LCD display */
  if (mode == MIB_WRITE) {
    LCDupdate = true;
  }
}

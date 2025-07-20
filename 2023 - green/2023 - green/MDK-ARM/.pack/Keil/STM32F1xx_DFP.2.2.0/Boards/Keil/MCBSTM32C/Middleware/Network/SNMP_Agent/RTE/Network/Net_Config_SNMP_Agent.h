/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_SNMP_Agent.h
 * Purpose: Network Configuration for SNMP Agent
 * Rev.:    V5.00
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>SNMP Agent
#define SNMP_AGENT_ENABLE       1

//   <s.15>Community Name
//   <i>Defines where an SNMP message is destined for.
//   <i>Default: "public"
#define SNMP_AGENT_COMMUNITY    "public"

//   <o>Port Number <1-65535>
//   <i>Listening port number.
//   <i>Default: 161
#define SNMP_AGENT_PORT_NUM     161

//   <o>Trap Port Number <1-65535>
//   <i>Port number for Trap operations.
//   <i>Default: 162
#define SNMP_AGENT_TRAP_PORT    162

//   <h>Trap Server
//   <i>Trap Server IP Address
//     <o>Address byte 1 <0-255>
//     <i>Default: 192
#define SNMP_AGENT_TRAP_IP1     192

//     <o>Address byte 2 <0-255>
//     <i>Default: 168
#define SNMP_AGENT_TRAP_IP2     168

//     <o>Address byte 3 <0-255>
//     <i>Default: 0
#define SNMP_AGENT_TRAP_IP3     0

//     <o>Address byte 4 <0-255>
//     <i>Default: 100
#define SNMP_AGENT_TRAP_IP4     100
//   </h>

// </h>

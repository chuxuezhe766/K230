/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_SMTP_Client.h
 * Purpose: Network Configuration for SMTP Client
 * Rev.:    V5.0.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>SMTP Client
#define SMTP_CLIENT_ENABLE      1

//   <o>Response Timeout in seconds <5-120>
//   <i>This is a time for SMTP Client to wait for a response from
//   <i>SMTP Server. If timeout expires, Client aborts operation.
//   <i>Default: 20
#define SMTP_CLIENT_TOUT        20

// </h>

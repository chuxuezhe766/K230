/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_FTP_Server.h
 * Purpose: Network Configuration for FTP Server
 * Rev.:    V5.00
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>FTP Server
#define FTP_SERVER_ENABLE       1

//   <o>Number of FTP Sessions <1-10>
//   <i>Number of simultaneously active FTP Sessions
//   <i>Default: 1
#define FTP_SERVER_NUM_SESSIONS 3

//   <o>Port Number <1-65535>
//   <i>Listening port number.
//   <i>Default: 21
#define FTP_SERVER_PORT_NUM     21

//   <s.50>Welcome Message
//   <i>This value is optional. If specified,
//   <i>it overrides the default welcome message.
//   <i>Default: ""
#define FTP_SERVER_MESSAGE      ""

//   <o>Idle Session Timeout in seconds <0-3600>
//   <i>When timeout expires, the connection is closed.
//   <i>A value of 0 disables disconnection on timeout.
//   <i>Default: 120
#define FTP_SERVER_TOUT         120

//   <e>Enable User Authentication
//   <i>When enabled, the user will have to authenticate
//   <i>himself by username and password before access
//   <i>to the system is allowed.
#define FTP_SERVER_AUTH_ENABLE  1

//     <s.15>Authentication Username
//     <i>Default: "admin"
#define FTP_SERVER_AUTH_USER    "admin"

//     <s.15>Authentication Password
//     <i>Default: ""
#define FTP_SERVER_AUTH_PASS    ""
//   </e>

// </h>

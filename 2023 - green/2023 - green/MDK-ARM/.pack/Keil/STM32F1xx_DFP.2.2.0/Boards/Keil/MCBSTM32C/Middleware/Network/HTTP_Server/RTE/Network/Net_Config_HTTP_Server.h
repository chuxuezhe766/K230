/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_HTTP_Server.h
 * Purpose: Network Configuration for HTTP Server
 * Rev.:    V5.0.1
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>HTTP Server
#define HTTP_SERVER_ENABLE          1

//   <o>Number of HTTP Sessions <1-10>
//   <i>Number of simultaneously active HTTP Sessions.
//   <i>Default: 6
#define HTTP_SERVER_NUM_SESSIONS    6

//   <o>Port Number <1-65535>
//   <i>Listening port number.
//   <i>Default: 80
#define HTTP_SERVER_PORT_NUM        80

//   <s.50>Server-Id header
//   <i>This value is optional. If specified, it overrides 
//   <i>the default HTTP Server header from the library.
//   <i>Default: ""
#define HTTP_SERVER_ID              ""

//   <e>Enable User Authentication
//   <i>When enabled, the user will have to authenticate
//   <i>himself by username and password before accessing
//   <i>any page on this Embedded WEB server.
#define HTTP_SERVER_AUTH_ENABLE     1

//     <s.20>Authentication Realm
//     <i>Default: "Embedded WEB Server"
#define HTTP_SERVER_AUTH_REALM      "Embedded WEB Server"

//     <s.15>Authentication Username
//     <i>Default: "admin"
#define HTTP_SERVER_AUTH_USER       "admin"

//     <s.15>Authentication Password
//     <i>Default: ""
#define HTTP_SERVER_AUTH_PASS       ""
//   </e>

// </h>

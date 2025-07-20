/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client_UIF.c
 * Purpose: SMTP Client User Interface
 * Rev.:    V6.00
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

// Email definitions
#define MAIL_FROM       "mcb@keil.com"
#define RCPT_TO         "somebody@keil.com"
#define SMTP_USER       "user"
#define SMTP_PASSWORD   "password"
#define MAIL_SUBJECT    "Reported measurements"

#define MSG_HEADER                                                    \
  "Hello!\r\n\r\n"                                                    \
  "This is measurement report sent from '%s' embedded system.\r\n"    \
  "Measurements are taken from analog inputs AIN0 - AIN7\r\n\r\n"     \
  "--------------------------------------------------------------\r\n"

#define MSG_FOOTER                                                    \
  "--------------------------------------------------------------\r\n"\
  "The end of measurements.\r\n\r\n"                                  \
  "Bye..."

// My structure of status variable pvar
typedef struct {
  uint8_t  id;
  uint16_t idx;
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

// Functions
extern uint16_t AD_in (uint32_t ch);
extern void smtp_inform (netSMTPc_Event event);

// Request parameters for SMTP client session.
uint32_t netSMTPc_Process (netSMTPc_Request request, char *buf, uint32_t buflen, uint32_t *pvar) {
  uint32_t i,len = 0;

  switch (request) {
    case netSMTPc_RequestUsername:
      // Username to login to SMTP server
      len = sprintf (buf, SMTP_USER);
      break;

    case netSMTPc_RequestPassword:
      // Password to login to SMTP server
      len = sprintf (buf, SMTP_PASSWORD);
      break;

    case netSMTPc_RequestSender:
      // Email address of the sender
      len = sprintf (buf, MAIL_FROM);
      break;

    case netSMTPc_RequestRecipient:
      // Email address of the recipient
      len = sprintf (buf, RCPT_TO);
      break;

    case netSMTPc_RequestSubject:
      // Subject of email
      len = sprintf (buf, MAIL_SUBJECT);
      break;

    case netSMTPc_RequestBody:
      // Email body in plain ascii formatl
      switch (MYBUF(pvar)->id) {
        case 0:
          // First call, enter an email header text
          //len = sprintf (buf, MSG_HEADER, lhost_name);
          len = sprintf (buf, MSG_HEADER, netSYS_GetHostName());
          MYBUF(pvar)->id  = 1;
          MYBUF(pvar)->idx = 1;
          goto rep;

        case 1:
          // Add email message body
          for (len = 0; len < buflen-150; ) {
            // Let's use as much of the buffer as possible
            len += sprintf (buf+len, "%d. ",MYBUF(pvar)->idx);
            for (i = 0; i < 8; i++) {
              len += sprintf (buf+len, "AD%d= %d  ", i, AD_in(i));
            }
            len += sprintf (buf+len, "\r\n");
            if (++MYBUF(pvar)->idx > 500) {
              MYBUF(pvar)->id = 2;
              break;
            }
          }
          // Set request for another callback
rep:      len |= (1u << 31);
          break;

        case 2:
          // Last one, add a footer text to this email
          len = sprintf (buf, MSG_FOOTER);
          break;
      }
  }
  return (len);
}

// Notify the user application when SMTP client operation ends.
void netSMTPc_Notify  (netSMTPc_Event event) {
  smtp_inform (event);
}

// Accept or deny authentication requested by SMTP server.
//bool smtp_client_accept_authentication (const uint8_t *ip_addr) {
bool netSMTPc_AcceptAuthentication (const NET_ADDR *addr) {
  return (true);
}

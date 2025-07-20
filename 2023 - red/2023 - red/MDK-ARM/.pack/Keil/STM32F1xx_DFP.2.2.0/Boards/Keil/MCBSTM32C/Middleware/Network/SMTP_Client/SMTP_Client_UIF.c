/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client_UIF.c
 * Purpose: SMTP Client User Interface
 * Rev.:    V6.00
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "rl_net.h"
#include "rl_net_lib.h"

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

// Net_Config.c 
extern struct sys_cfg   sys_config;
#define lhost_name      sys_config.HostName

// Functions
extern uint16_t AD_in (uint32_t ch);
extern void smtp_inform (smtpClientEvent event);

// Request parameters for SMTP client session.
uint32_t smtp_client_request (smtpClientRequest request, char *buf, uint32_t buflen, uint32_t *pvar) {
  uint32_t i,len = 0;

  switch (request) {
    case smtpClientUsername:
      // Username to login to SMTP server
      len = sprintf (buf, SMTP_USER);
      break;

    case smtpClientPassword:
      // Password to login to SMTP server
      len = sprintf (buf, SMTP_PASSWORD);
      break;

    case smtpClientSender:
      // Email address of the sender
      len = sprintf (buf, MAIL_FROM);
      break;

    case smtpClientRecipient:
      // Email address of the recipient
      len = sprintf (buf, RCPT_TO);
      break;

    case smtpClientSubject:
      // Subject of email
      len = sprintf (buf, MAIL_SUBJECT);
      break;

    case smtpClientBody:
      // Email body in plain ascii formatl
      switch (MYBUF(pvar)->id) {
        case 0:
          // First call, enter an email header text
          len = sprintf (buf, MSG_HEADER, lhost_name);
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
void smtp_client_notify (smtpClientEvent event) {
  smtp_inform (event);
}

// Accept or deny authentication requested by SMTP server.
bool smtp_client_accept_authentication (const uint8_t *ip_addr) {
  return (true);
}

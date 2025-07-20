/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server_UIF.c
 * Purpose: Telnet Server User Interface
 * Rev.:    V6.00
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "rl_net.h"
#include "rl_net_lib.h"

#include "Board_LED.h"

// Net_Config.c
extern struct tcp_cfg   tcp_config;
extern struct tnet_cfg  tnet_config;
#define tcp_NumSocks    tcp_config.NumSocks
#define tcp_socket      tcp_config.Scb
#define tnet_EnAuth     tnet_config.EnAuth
#define tnet_auth_passw tnet_config.Passw

// ANSI ESC Sequences for terminal control
#define CLS     "\033[2J"
#define TBLUE   "\033[37;44m"
#define TNORM   "\033[0m"

extern bool LEDrun;
extern bool LCDupdate;
extern char lcd_text[2][20+1];

// My structure of status variable pvar
typedef struct {
  uint8_t id;
  uint8_t nmax;
  uint8_t idx;
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

// Local variables
static const char telnet_help1[] = {
  "\r\n\r\n"
  "Commands:\r\n"
  "  led xx      - write hexval xx to LED port\r\n"
  "  led         - enable running lights\r\n"
  "  adin x      - read AD converter intput x\r\n"
  "  meas n      - display n measurements\r\n"
  "  tcpstat     - display a tcp status\r\n"
  "  rinfo       - display remote machine info\r\n"
  "  lcdN text   - write a text to LCD line N\r\n"
};
static const char telnet_help2[] = {
  "  passw [new] - change system password\r\n"
  "  passwd      - display current password\r\n"
};
static const char telnet_help3[] = {
  "  help, ?     - display this help\r\n"
  "  bye,\r\n"
  "  <ESC>,<^C>  - disconnect\r\n"
  "  <BS>        - delete Character left\r\n"
  "  <UP>,<DOWN> - recall Command History\r\n"
};

static const char meas_header[] = {
  "\r\n"
  " Nr.   ADIN0  ADIN1  ADIN2  ADIN3  ADIN4  ADIN5  ADIN6  ADIN7\r\n"
  "=============================================================="
};

static const char tcp_stat[] = {
  CLS "\r\n"
  " " TBLUE
  "==============================================================================\r\n" TNORM
  " " TBLUE
  " Sock  State        Port  Timer  Remote Address                Port           \r\n" TNORM
  " " TBLUE
  "==============================================================================\r\n" TNORM
};

// Functions
extern uint16_t AD_in (uint32_t ch);

// Request message for Telnet server session.
uint32_t telnet_server_message (telnetServerMessage msg, char *buf, uint32_t len) {
  uint32_t rlen = 0;

  switch (msg) {
    case telnetServerWelcome:
      // Initial welcome message
      rlen = sprintf (buf, "\r\n"
                           "\r\n"
                           "Telnet Server ready\r\n");
      break;
    case telnetServerPrompt:
      // Prompt message
      rlen = sprintf (buf, "\r\n"
                           "Cmd> ");
      break;
    case telnetServerLogin:
      // Login welcome message, if authentication is enabled
      rlen = sprintf (buf, "\r\n"
                           "Embedded Telnet Server\r\n"
                           "\r\n"
                           "Please login...");
      break;
    case telnetServerUsername:
      // Username request login message
      rlen = sprintf (buf, "\r\n"
                           "Username: ");
      break;
    case telnetServerPassword:
      // Password request login message
      rlen = sprintf (buf, "\r\n"
                           "Password: ");
      break;
    case telnetServerLoginFailed:
      // Incorrect login error message
      rlen = sprintf (buf, "\r\nLogin incorrect");
      break;
    case telnetServerLoginTimeout:
      // Login timeout error message
      rlen = sprintf (buf, "\r\nLogin timeout\r\n");
      break;
    case telnetServerUnsolicitedMessage:
      // Unsolicited message (ie. from basic interpreter)
      break;
  }
  return (rlen);
}

// Process a command and generate response.
uint32_t telnet_server_process (const char *cmd, char *buf, uint32_t buflen, uint32_t *pvar) {
  TCP_INFO *tsoc;
  uint8_t ip_addr[IP4_ADDR_LEN];
  uint8_t mac_addr[ETH_ADDR_LEN];
  uint32_t val,ch,temp;
  uint32_t len = 0;

  switch (MYBUF(pvar)->id) {
    case 0:
      // First call to this function
      break;

    case 1:
      // Command MEAS, repeated call
      while (len < buflen-80) {
        // Let's use as much of the buffer as possible
        len += sprintf (buf+len, "\r\n%4d", MYBUF(pvar)->idx);
        for (val = 0; val < 8; val++) {
          len += sprintf (buf+len, "%7d", AD_in(val));
        }
        if (++MYBUF(pvar)->idx >= MYBUF(pvar)->nmax) {
          // Requested number of measurements done
          return (len);
        }
      }
      // Set request for another callback
      return (len | (1u << 31));

    case 2:
      // Repeated call of TCP status display
      while (len < buflen-96) {
        // Let's use as much of the buffer as possible
        if (MYBUF(pvar)->idx == 0) {
          len += sprintf (buf, tcp_stat);
        }
        tsoc = &tcp_socket[MYBUF(pvar)->idx];
        MYBUF(pvar)->idx++;
        len += sprintf   (buf+len, "\r\n  %-6d%-13s", MYBUF(pvar)->idx,
                          tcp_ntoa(tsoc->State));
        if (tsoc->State <= tcpStateCLOSED) {
        }
        else if (tsoc->State == tcpStateLISTEN) {
          len += sprintf (buf+len, "%-5d", tsoc->LocPort);
        }
        else {
          len += sprintf (buf+len, "%-6d%-7d%-30s%-5d",
                          tsoc->LocPort, tsoc->AliveTimer,
                          ip4_ntoa (tsoc->RemAddr), tsoc->RemPort);
        }
        if (MYBUF(pvar)->idx == tcp_NumSocks) {
          // We are done, reset index counter for next callback
          len += sprintf (buf+len, "\r\n");
          MYBUF(pvar)->idx = 0;
          // Setup a callback delay 2 seconds (20 * 100ms)
          telnet_server_set_delay (20);
          break;
        }
      }
      // Set request for another callback
      return (len | (1u << 31));
  }

  // Simple command line parser
  len = strlen (cmd);
  if (telnet_check_command (cmd, "LED") == true) {
    // LED command given, control onboard LEDs
    if (len >= 5) {
      sscanf (cmd+4,"%x", &val);
      LED_SetOut (val);
      len = 0;
      if (LEDrun == true) {
        len = sprintf (buf," --> Running Lights OFF");
        LEDrun = false;
      }
      return (len);
    }
    len = 0;
    if (LEDrun == false) {
      len = sprintf (buf," --> Running Lights ON");
      LEDrun = true;
    }
    return (len);
  }
  if (telnet_check_command (cmd, "ADIN") == true) {
    // ADIN command given, print analog inputs
    if (len >= 6) {
      sscanf (cmd+5,"%d",&ch);
      val = AD_in (ch);
      len = sprintf (buf,"\r\n ADIN %d = %d",ch,val);
      return (len);
    }
  }
  if (telnet_check_command (cmd, "BYE") == true) {
    // BYE command given, disconnect the client
    len = sprintf (buf, "\r\nDisconnecting\r\n");
    // Bit-30 of return value is a disconnect flag
    return (len | (1u << 30));
  }

  if (telnet_check_command (cmd, "PASSW") == true && tnet_EnAuth) {
    // PASSW command given, change password
    if (len == 5) {
      // Disable password
      tnet_auth_passw[0] = 0;
    }
    else {
      // Change password
      memcpy (tnet_auth_passw, &cmd[6], 20);
    }
    len = sprintf (buf, "\r\n OK, New Password: \"%s\"", tnet_auth_passw);
    return (len);
  }

  if (telnet_check_command (cmd, "PASSWD") == true && tnet_EnAuth) {
    // PASSWD command given, print current password
    len = sprintf (buf, "\r\n System Password: \"%s\"", tnet_auth_passw);
    return (len);
  }

  if (telnet_check_command (cmd, "MEAS") == true) {
    // MEAS command given, monitor analog inputs
    MYBUF(pvar)->id = 1;
    if (len > 5) {
      // We must be careful here, because data is overlaid
      sscanf (&cmd[5], "%d", &temp);
      MYBUF(pvar)->nmax = temp;
    }
    len = sprintf (buf, meas_header);
    if (MYBUF(pvar)->nmax) {
      // Bit-31 is a repeat flag
      len |= (1u << 31);
    }
    return (len);
  }

  if (telnet_check_command (cmd, "TCPSTAT") == true) {
    // TCPSTAT command given, display TCP socket status
    MYBUF(pvar)->id = 2;
    len = sprintf (buf, CLS);
    return (len | (1u << 31));
  }

  if (telnet_check_command (cmd, "RINFO") == true) {
    // Print IP and MAC address of remote host
    telnet_server_get_client (ip_addr, mac_addr);
    len  = sprintf (buf,    "\r\n Remote IP4: %s",ip4_ntoa (ip_addr));
    len += sprintf (buf+len,"\r\n Remote MAC: %s",mac_ntoa (mac_addr));
    return (len);
  }

  if (telnet_check_command (cmd, "LCD1") == true) {
    // LCD1 command given, print text to LCD Module line 1
    lcd_text[0][0] = 0;
    if (len > 5) {
      memcpy (&lcd_text[0], &cmd[5], 20);
      LCDupdate = true;
    }
    return (0);
  }

  if (telnet_check_command (cmd, "LCD2") == true) {
    // LCD2 command given, print text to LCD Module line 2
    lcd_text[1][0] = 0;
    if (len > 5) {
      memcpy (&lcd_text[1], &cmd[5], 20);
      LCDupdate = true;
    }
    return (0);
  }

  if (telnet_check_command (cmd, "HELP") == true ||
      telnet_check_command (cmd, "?") == true) {
    // HELP command given, display help text
    len = sprintf (buf, telnet_help1);
    if (tnet_EnAuth) {
      len += sprintf (buf+len, telnet_help2);
    }
    len += sprintf (buf+len, telnet_help3);
    return (len);
  }
  // Unknown command given
  len = sprintf (buf, "\r\n==> Unknown Command: %s", cmd);
  return (len);
}

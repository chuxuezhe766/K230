/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Telnet_Server_UIF.c
 * Purpose: Telnet Server User Interface
 * Rev.:    V6.00
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "Board_LED.h"                  // ::Board Support:LED


// ANSI ESC Sequences for terminal control
#define CLS     "\033[2J"
#define TBLUE   "\033[37;44m"
#define TNORM   "\033[0m"

// External references
extern bool LEDrun;
extern char lcd_text[2][20+1];
extern osThreadId TID_Display;
extern uint16_t AD_in (uint32_t ch);
extern const char *net_tcp_ntoa (netTCP_State state);

// My structure of 32-bit status variable pvar
typedef struct {
  uint8_t id;
  uint8_t nmax;
  uint8_t idx;
  uint8_t unused;
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

// Local variables
static const char telnet_help1[] = {
  "\r\n\r\n"
  "Commands:\r\n"
  "  led xx      - write hexval xx to LED port\r\n"
  "  led         - enable running lights\r\n"
  "  adin x      - read AD converter input x\r\n"
  "  meas n      - print n measurements\r\n"
  "  tcpstat     - print TCP socket status\r\n"
  "  rinfo       - print remote client info\r\n"
  "  lcdN text   - write a text to LCD line N\r\n"
};
static const char telnet_help2[] = {
  "  passw [new] - change system password\r\n"
  "  passwd      - display current password\r\n"
};
static const char telnet_help3[] = {
  "  help, ?     - display this help\r\n"
  "  <BS>        - delete Character left\r\n"
  "  <UP>,<DOWN> - recall Command History\r\n"
  "  bye,<ESC>,^C- disconnect\r\n"
};

static const char meas_header[] = {
  "\r\n"
  " Nr.   ADIN0  ADIN1  ADIN2  ADIN3  ADIN4  ADIN5  ADIN6  ADIN7\r\n"
  "=============================================================="
};

static const char tcp_stat_header[] = {
  CLS "\r\n"
  " " TBLUE
  "==============================================================================\r\n" TNORM
  " " TBLUE
  " Sock  State        Port  Timer  Remote Address                Port           \r\n" TNORM
  " " TBLUE
  "==============================================================================\r\n" TNORM
};


// Request message for Telnet server session.
uint32_t netTELNETs_ProcessMessage (netTELNETs_Message msg, char *buf, uint32_t buf_len) {
  uint32_t len = 0;

  switch (msg) {
    case netTELNETs_MessageWelcome:
      // Initial welcome message
      len = sprintf (buf, "\r\n"
                          "Embedded Telnet Server\r\n");
      break;
    case netTELNETs_MessagePrompt:
      // Prompt message
      len = sprintf (buf, "\r\n"
                          "Cmd> ");
      break;
    case netTELNETs_MessageLogin:
     // Login message, if authentication is enabled
      len = sprintf (buf, "\r\n"
                          "Please login...");
      break;
    case netTELNETs_MessageUsername:
      // Username request login message
      len = sprintf (buf, "\r\n"
                          "Username: ");
      break;
    case netTELNETs_MessagePassword:
      // Password request login message
      len = sprintf (buf, "\r\n"
                          "Password: ");
      break;
    case netTELNETs_MessageLoginFailed:
      // Incorrect login error message
      len = sprintf (buf, "\r\n"
                          "Login incorrect");
      break;
    case netTELNETs_MessageLoginTimeout:
      // Login timeout error message
      len = sprintf (buf, "\r\n"
                          "Login timeout\r\n");
      break;
    case netTELNETs_MessageUnsolicited:
      // Unsolicited message (ie. from basic interpreter)
      break;
  }
  return (len);
}

// Process a command and generate response.
uint32_t netTELNETs_ProcessCommand (const char *cmd, char *buf, uint32_t buf_len, uint32_t *pvar) {
  int32_t socket;
  netTCP_State state;
  NET_ADDR client;
  char ip_ascii[40];
  const char *passwd;
  uint32_t val, ch, nmax;
  uint32_t len = 0;

  switch (MYBUF(pvar)->id) {
    case 0:
      // First call to this function
      break;

    case 1:
      // Command MEAS, repeated call
      // Let's use as much of the buffer as possible
      while (len < buf_len-80) {
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
      // Command TCPSTAT, repeated call
      if (MYBUF(pvar)->idx == 0) {
        len = sprintf (buf, tcp_stat_header);
      }
      // Let's use as much of the buffer as possible
      while (len < buf_len-96) {
        socket = ++MYBUF(pvar)->idx;
        state  = netTCP_GetState (socket);

        if (state == netTCP_StateINVALID) {
          // Invalid socket, we are done
          len += sprintf (buf+len, "\r\n");
          // Reset index counter for next callback
          MYBUF(pvar)->idx = 0;
          // Repeat a command after 20 ticks (2 seconds)
          netTELNETs_RepeatCommand (20);
          break;
        }

        len += sprintf (buf+len, "\r\n  %-6d%-13s", MYBUF(pvar)->idx, net_tcp_ntoa(state));

        if (state == netTCP_StateLISTEN) {
          len += sprintf (buf+len, "%-5d", netTCP_GetLocalPort (socket));
        }
        else if (state > netTCP_StateLISTEN) {
          // Retrieve remote client IP address and port number
          netTCP_GetPeer (socket, &client, sizeof(client));
          // Convert client IP address to ASCII string
          netIP_ntoa (client.addr_type, client.addr, ip_ascii, sizeof(ip_ascii));

          len += sprintf (buf+len, "%-6d%-7d%-30s%-5d",
                          netTCP_GetLocalPort (socket), netTCP_GetTimer (socket),
                          ip_ascii, client.port);
        }
      }
      // Set request for another callback
      return (len | (1u << 31));
  }

  // Simple command line parser
  len = strlen (cmd);
  if (netTELNETs_CheckCommand (cmd, "LED") == true) {
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
  if (netTELNETs_CheckCommand (cmd, "ADIN") == true) {
    // ADIN command given, print analog inputs
    if (len >= 6) {
      sscanf (cmd+5,"%d",&ch);
      val = AD_in (ch);
      len = sprintf (buf,"\r\n ADIN %d = %d",ch,val);
      return (len);
    }
  }
  if (netTELNETs_CheckCommand (cmd, "BYE") == true) {
    // BYE command given, disconnect the client
    len = sprintf (buf, "\r\nDisconnecting\r\n");
    // Bit-30 of return value is a disconnect flag
    return (len | (1u << 30));
  }

  if (netTELNETs_CheckCommand (cmd, "PASSW") == true && netTELNETs_LoginActive()) {
    // PASSW command given, change password
    if (len > 6) {
      // Change password
      passwd = &cmd[6];
    }
    else { passwd = NULL; }
    if (netTELNETs_SetPassword (passwd) == netOK) {
      len = sprintf (buf, "\r\n OK, New Password: \"%s\"", netTELNETs_GetPassword ());
    }
    else {
      len = sprintf (buf, "\r\nFailed to change password!");
    }
    return (len);
  }

  if (netTELNETs_CheckCommand (cmd, "PASSWD") == true && netTELNETs_LoginActive()) {
    // PASSWD command given, print current password
    len = sprintf (buf, "\r\n System Password: \"%s\"", netTELNETs_GetPassword());
    return (len);
  }

  if (netTELNETs_CheckCommand (cmd, "MEAS") == true) {
    // MEAS command given, monitor analog inputs
    MYBUF(pvar)->id = 1;
    if (len > 5) {
      sscanf (&cmd[5], "%u", &nmax);
      if (nmax > 255) { nmax = 255; }
      MYBUF(pvar)->nmax = nmax; 
    }
    len = sprintf (buf, meas_header);
    if (MYBUF(pvar)->nmax) {
      // Bit-31 is a repeat flag
      len |= (1u << 31);
    }
    return (len);
  }

  if (netTELNETs_CheckCommand (cmd, "TCPSTAT") == true) {
    // TCPSTAT command given, display TCP socket status
    MYBUF(pvar)->id = 2;
    len = sprintf (buf, CLS);
    return (len | (1u << 31));
  }

  if (netTELNETs_CheckCommand (cmd, "RINFO") == true) {
    // Print IP address and port number of connected telnet client
    if (netTELNETs_GetClient (&client, sizeof(client)) == netOK) {
      // Convert client IP address to ASCII string
      netIP_ntoa (client.addr_type, client.addr, ip_ascii, sizeof(ip_ascii));

      len  = sprintf (buf    ,"\r\n IP addr : %s", ip_ascii);
      len += sprintf (buf+len,"\r\n TCP port: %d", client.port);
      return (len);
    }
    len = sprintf (buf, "\r\n Error!");
    return (len);
  }

  if (netTELNETs_CheckCommand (cmd, "LCD1") == true) {
    // LCD1 command given, print text to LCD Module line 1
    lcd_text[0][0] = 0;
    if (len > 5) {
      memcpy (&lcd_text[0], &cmd[5], 20);
      osSignalSet (TID_Display, 0x01);
    }
    return (0);
  }

  if (netTELNETs_CheckCommand (cmd, "LCD2") == true) {
    // LCD2 command given, print text to LCD Module line 2
    lcd_text[1][0] = 0;
    if (len > 5) {
      memcpy (&lcd_text[1], &cmd[5], 20);
      osSignalSet (TID_Display, 0x01);
    }
    return (0);
  }

  if (netTELNETs_CheckCommand (cmd, "HELP") == true ||
      netTELNETs_CheckCommand (cmd, "?") == true) {
    // HELP command given, display help text
    len = sprintf (buf, telnet_help1);
    if (netTELNETs_LoginActive()) {
      len += sprintf (buf+len, telnet_help2);
    }
    len += sprintf (buf+len, telnet_help3);
    return (len);
  }
  // Unknown command given
  len = sprintf (buf, "\r\n==> Unknown Command: %s", cmd);
  return (len);
}

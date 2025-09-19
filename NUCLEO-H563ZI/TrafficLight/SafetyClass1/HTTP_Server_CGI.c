/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2024 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V6.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "Communication.h"

// Process query string received by GET request.
void netCGI_ProcessQuery (const char *qstr) {
  (void)qstr;
}

// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {

  if ((len >= 4U) && (code == 0U)) {
    /* Only code 0 is supported */
    if        (memcmp(data, "Btn0", 4) == 0) {  /* Inject a watchdog fault in Safety Class 0 (Operation Verification) */
      FaultInject(FAULT_INJECT_SC0_WDOG); 
    } else if (memcmp(data, "Btn1", 4) == 0) {  /* Inject a watchdog fault in Safety Class 2 (Safe-mode operation) */
      FaultInject(FAULT_INJECT_SC2_WDOG); 
    } else if (memcmp(data, "Btn2", 4) == 0) {  /* Inject a memory access fault in Safety Class 0 */
      FaultInject(FAULT_INJECT_SC0_MEM_ACCESS); 
    } else if (memcmp(data, "Btn3", 4) == 0) {  /* Inject a memory access fault in Safety Class 1 */
      FaultInject(FAULT_INJECT_SC1_MEM_ACCESS); 
    } else if (memcmp(data, "Btn5", 4) == 0) {  /* Inject a fatal error */
      (void)osKernelSuspend();
    }
  }
}

// Generate dynamic web data from a script line.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  uint32_t len;

  (void)pcgi;

  len = 0U;

  switch (env[0]) {
    case 'i':
      // Script line starting with "c i " in refresh.cgx script
      switch (env[2]) {
        case '1':                       // Script line starting with "c i 1 in refresh.cgx "
          len = (uint32_t)snprintf (buf, buflen, &env[4], DisplayGetOperatingMode());
          break;
        case '2':                       // Script line starting with "c i 2 in refresh.cgx "
          len = (uint32_t)snprintf (buf, buflen, &env[4], (DisplayGetLightsStatus() & 1U));
          break;
        case '3':                       // Script line starting with "c i 3 in refresh.cgx "
          len = (uint32_t)snprintf (buf, buflen, &env[4], ((DisplayGetLightsStatus() >> 1) & 1U));
          break;
        case '4':                       // Script line starting with "c i 4 in refresh.cgx "
          len = (uint32_t)snprintf (buf, buflen, &env[4], ((DisplayGetLightsStatus() >> 2) & 1U));
          break;
        case '5':                       // Script line starting with "c i 5 in refresh.cgx "
          len = (uint32_t)snprintf (buf, buflen, &env[4], DisplayGetSensorValue());
          break;
        case '6':                       // Script line starting with "c i 6 in refresh.cgx"
          len = (uint32_t)snprintf (buf, buflen, &env[4], DisplayGetFaultInfo());
          break;
        default:
          // Unknown script line number
          break;
      }
      break;
    default:
      // Unknown script line 
      break;
  }

  return len;
}

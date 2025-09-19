/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2025 Arm Limited (or its affiliates).
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

#include "SafeModeOperation.h"

#include "system_defs.h"
#include "io_defs.h"
#include "faults.h"
#include "zones.h"
#include "hw_watchdog.h"
#include "cmsis_vio.h"

#include "Communication.h"

/* Local variable located in shared RAM */
static osEventFlagsId_t safe_mode_operation_event_id    __attribute__((section("ram_shared"))) = NULL;

/* Local variable located in in RAM_SAFE_OP */
static osThreadId_t     safe_mode_operation_thread_id = NULL;

/* Thread stack located in RAM_SAFE_OP */
static uint64_t safe_mode_operation_thread_stack[512/8];

/* safe_mode_operation_event event flags attributes */
static const osEventFlagsAttr_t safe_mode_operation_event_attr = {
  .name       = "safe_mode_operation_event",
  .attr_bits  = osSafetyClass(0U), 
  .cb_mem     = NULL,                                   /* System allocated control block */
  .cb_size    = 0U
};

/* SafeModeOperationThread thread attributes */
static const osThreadAttr_t safe_mode_operation_thread_attr = {
  .name       = "SafeModeOperationThread",
  .attr_bits  = osThreadUnprivileged                           | 
                osThreadZone(ZONE_SAFE_OP)                     | 
                osSafetyClass(SAFETY_CLASS_SAFE_MODE_OPERATION), 
  .cb_mem     = NULL,                                   /* System allocated control block */
  .cb_size    = 0U,
  .stack_mem  = safe_mode_operation_thread_stack,       /* User provided stack */
  .stack_size = sizeof(safe_mode_operation_thread_stack),
  .priority   = osPriorityRealtime,
  .tz_module  = 0U,                                     /* Not used */
  .affinity_mask = 0U                                   /* Not used */
};

/* SafeModeOperationThread thread:
   Safe-Mode Operation is dormant until it is activated by a call to 
   SafeModeOperationActivate function.
   After Safe-Mode Operation is activated it flashes 
   yellow light with 700 ms on and 300 ms off period.
   Software watchdog is set to 500 ms timeout, and 
   hardware watchdog is set to 1000 ms timeout.
   If software watchdog expires it is handled in the 
   osWatchdogAlarm_Handler function.
   If hardware watchdog expires it will reset the system.
 */
static __NO_RETURN void SafeModeOperationThread (void *argument) {
  uint32_t flags;
  uint32_t time_ms;

  (void)argument;

  /* Initialize local variables */
  time_ms = 0U;

  /* Safe-Mode Operation is dormant and waiting to be activated with a call to 
     SafeModeOperationActivate function.
     In this state hardware and software watchdogs are fed every 250 ms. */
  do {
    flags = osEventFlagsWait(safe_mode_operation_event_id, SAFE_MODE_OPERATION, osFlagsWaitAny, 250U);
    HwWatchdogFeed();                   /* Feed the hw watchdog (timeout 1000 ms) */
    (void)osThreadFeedWatchdog(500U);   /* Feed the sw watchdog (timeout 500 ms) */
    if (FaultWasInjected(FAULT_INJECT_SC2_WDOG) != 0U) {
      for (;;) {}                       /* Provoke watchdog alarm */
    }
  } while (flags == osFlagsErrorTimeout);

  if ((flags & 0x80000000U) != 0U) {
    /* If there was an error while Safe-Mode Operation was dormant 
       the SafeModeDormantError_Handler function is called. 
       It activates Safe-Mode Operation and suspends all other functionality. */
    SafeModeDormantError_Handler();
  }

  /* Safe-Mode Operation beginning:
     turn on yellow light and activate statuses for display on a web page */
  vioSetSignal(LIGHT_RED,    vioLEDoff);
  vioSetSignal(LIGHT_YELLOW, vioLEDon);
  vioSetSignal(LIGHT_GREEN,  vioLEDoff);
  DisplaySetOperatingMode(MODE_SAFE);
  DisplaySetLightsStatus(LIGHT_YELLOW_VAL);

  for (;;) {
    HwWatchdogFeed();                   /* Feed the hw watchdog (timeout 1000 ms) */
    (void)osThreadFeedWatchdog(500U);   /* Feed the sw watchdog (timeout 500 ms) */

    if (time_ms >= 1000U) {
      /* Yellow off */
      time_ms = 0U;
      vioSetSignal(LIGHT_YELLOW, vioLEDoff);
      DisplaySetLightsStatus(LIGHT_ALL_OFF_VAL);
    } else if (time_ms >= 700U) {
      /* Yellow on */
      vioSetSignal(LIGHT_YELLOW, vioLEDon);
      DisplaySetLightsStatus(LIGHT_YELLOW_VAL);
    }

    (void)osDelay(100U);
    time_ms += 100U;
  
    if (FaultWasInjected(FAULT_INJECT_SC2_WDOG) != 0U) {
      /* If injection of a watchdog fault was activated from the web interface */
      for (;;) {}                       /* Provoke watchdog alarm */
    }
  }
}

/* Initialize Safe-Mode Operation */
void SafeModeOperationInit (void) {

  safe_mode_operation_event_id  = osEventFlagsNew(&safe_mode_operation_event_attr);
  safe_mode_operation_thread_id = osThreadNew(SafeModeOperationThread, NULL, &safe_mode_operation_thread_attr);
}

/* Activate Safe-Mode Operation: 
   This function activates Safe-Mode Operation from dormant state.
   This function can be called from any other threads of any Zone, so 
   safe_mode_operation_event event flag has to be located in shared RAM. */
void SafeModeOperationActivate (void) {
  (void)osEventFlagsSet(safe_mode_operation_event_id, SAFE_MODE_OPERATION);
}

/* Get Thread Id of Safe-Mode Operation thread */
osThreadId_t SafeModeOperationThreadGetId (void) {
  return safe_mode_operation_thread_id;
}

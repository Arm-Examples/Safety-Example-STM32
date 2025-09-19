/*---------------------------------------------------------------------------
 * Copyright (c) 2025 Arm Limited (or its affiliates).
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

#include "RTE_Components.h"             // Component selection
#include CMSIS_device_header            // Device header
#include "system_defs.h"
#include "io_defs.h"
#include "zones.h"
#include "faults.h"

#include "cmsis_vio.h"
#include "hw_watchdog.h"

#include "NormalOperation.h"
#include "OperationVerification.h"
#include "Communication.h"
#include "SafeModeOperation.h"


/*
  Application initialization.
*/
int app_main (void) {

  SystemCoreClockUpdate();

  /* Turn on red light until normal operation starts */
  vioSetSignal(LIGHT_RED,    vioLEDon);
  vioSetSignal(LIGHT_YELLOW, vioLEDoff);
  vioSetSignal(LIGHT_GREEN,  vioLEDoff);

#ifndef DISABLE_HW_WATCHDOG
  /* Initialize and start hw watchdog with 1 second timeout */
  HwWatchdogInit(1000U);
#endif

  /* Initialize CMSIS-RTOS2 */
  (void)osKernelInitialize();

  /* Initialize all functionality */
  NormalOperationInit();
  SafeModeOperationInit();
  OperationVerificationInit();
  CommunicationInit();

  ZonesInitialize();                    /* Initialize Zones (MPU) */

  /* Prevent further creation of privileged threads */
  (void)osThreadProtectPrivileged();

  /* Protect the RTOS Kernel scheduler access from safety class lower than Communication.
     We simulate fatal system error by calling osKernelSuspend from Communication thread. */
  (void)osKernelProtect(SAFETY_CLASS_COMMUNICATION);

  /* Start thread execution */
  (void)osKernelStart();

  return 0;
}

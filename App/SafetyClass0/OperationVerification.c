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

#include "OperationVerification.h"

#include "system_defs.h"
#include "faults.h"
#include "zones.h"
#include "cmsis_os2.h"

#include "NormalOperation.h"
#include "Communication.h"

/* Thread stacks located in RAM_VERIFY_OP */
static uint64_t operation_verification_thread_stack[512/8];

/* OperationVerificationThread thread attributes */
static const osThreadAttr_t operation_verification_thread_attr = {
  .name       = "OperationVerificationThread",
  .attr_bits  = osThreadUnprivileged                              | 
                osThreadZone(ZONE_VERIFY_OP)                      | 
                osSafetyClass(SAFETY_CLASS_OPERATION_VERIFICATION), 
  .cb_mem     = NULL,                                   /* System allocated control block */
  .cb_size    = 0U,
  .stack_mem  = operation_verification_thread_stack,    /* User provided stack */
  .stack_size = sizeof(operation_verification_thread_stack),
  .priority   = osPriorityNormal,
  .tz_module  = 0U,                                     /* Not used */
  .affinity_mask = 0U                                   /* Not used */
};

/* OperationVerificationThread thread:
   Check if input data is plausible (less than max) and if it is not activate Safe-Mode Operation. */
static __NO_RETURN void OperationVerificationThread (void *argument) {
  uint32_t sensor_val;

  (void)argument;

  for (;;) {
    (void)osThreadFeedWatchdog(100U);   /* Timeout for software watchdog is 100 ms */

    sensor_val = SensorGetData(80U);    /* Get sensor value from message queue with 80 ms timeout */
    if (sensor_val > 4000U) {
      /* If input data is not plausible (larger than 4000 or timed-out) 
         then call NormalOperationError_Handler function located in faults.c module. */ 
      NormalOperationError_Handler();
    }
    DisplaySetSensorValue(sensor_val);

    if (FaultWasInjected(FAULT_INJECT_SC0_WDOG) != 0U) {
      /* If injection of a watchdog fault was activated from the web interface */
      for (;;) {}                       /* Provoke watchdog alarm */
    }
  }
}

/* Initialize Operation Verification */
void OperationVerificationInit (void) {

  (void)osThreadNew(OperationVerificationThread, NULL, &operation_verification_thread_attr);
}

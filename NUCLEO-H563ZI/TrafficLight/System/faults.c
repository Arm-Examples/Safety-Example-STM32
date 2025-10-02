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

#include "faults.h"

#include <stdint.h>
#include <stdio.h>

#ifdef   _RTE_
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

#include "system_defs.h"
#include "io_defs.h"
#include "zones.h"
#include "rtx_os.h"
#include "cmsis_vio.h"

#include "SafeModeOperation.h"
#include "Communication.h"
#include "NormalOperation.h"

/* Basic Frame structure type */
typedef struct {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R12;
  uint32_t LR;
  uint32_t PC;
  uint32_t xPSR;
} bf_t; 

/* Local variables located in shared RAM */
static char temp_str[128] __attribute__((section("ram_shared")));

/* Local functions */
static void FatalError_Handler (void);

/* Fatal error handler:
   This handler is executed when there is no safe way of recovery from an error. */
static __NO_RETURN void FatalError_Handler (void) {
  /* For fatal error turn on red light until system is reset and Normal Operation restarts. 
     All lights will be reset to off state, so this should be insured by hardware design. */
  vioSetSignal(LIGHT_RED,    vioLEDon);
  vioSetSignal(LIGHT_YELLOW, vioLEDoff);
  vioSetSignal(LIGHT_GREEN,  vioLEDoff);
  NVIC_SystemReset();                   /* Reset the system */
}

/* System Startup error handler:
   This handler is executed when an error was detected during System Startup. */
__NO_RETURN void SystemStartupError_Handler (void) {
  FatalError_Handler();
}

/* Normal Operation error handler:
   This handler is executed when an error was detected in Normal Operation 
   by the Operation Verification. */
void NormalOperationError_Handler (void) {
  SafeModeOperationActivate();
  DisplaySetFaultInfo("Operation Verification has detected an error in input data \r\n\r\n");
  (void)osThreadSuspendClass(SAFETY_CLASS_NORMAL_OPERATION, osSafetyWithSameClass);
}

/* Safe-Mode Dormant error handler:
   This handler is executed when an error was detected while Safe-Mode Operation was dormant. */
void SafeModeDormantError_Handler (void) {
  SafeModeOperationActivate();
  DisplaySetFaultInfo("Safe-Mode Operation has detected an error while dormant \r\n\r\n");
  (void)osThreadSuspendClass(SAFETY_CLASS_SAFE_MODE_OPERATION, osSafetyWithLowerClass);
}

/* Zone error handler:
   This handler is executed when undefined zone is requested for activation. */
__NO_RETURN void ZoneError_Handler (void) {
  FatalError_Handler();
}

/* RTX OS error handler:
   This handler is called when RTX detects a fault. */
uint32_t osRtxErrorNotify (uint32_t code, void *object_id) {
  uint32_t safety_class;

  switch (code) {
    case osRtxErrorStackOverflow:
      // Stack overflow detected for thread (thread_id=object_id)
      if (object_id != NULL) {
        if (SafeModeOperationThreadGetId() == (osThreadId_t)object_id) {
          /* If overflow error occurred in Safe-Mode Operation thread then it is a fatal error */
          FatalError_Handler();
        } else {
          /* If overflow error occurred in any thread other than Safe-Mode Operation thread.
             Activate Safe-Mode Operation and suspend all threads of same or lower Safety Class. */
          safety_class = osThreadGetClass(object_id);
          if (safety_class > SAFETY_CLASS_SAFE_MODE_OPERATION) {
            /* If safety class is higher than safe-mode operation class then it is a fatal error */
            FatalError_Handler();
          }

          SafeModeOperationActivate();
          (void)osThreadSuspendClass(safety_class, osSafetyWithSameClass | osSafetyWithLowerClass);

          /* Set Fault Info (for web display) */
          (void)snprintf(temp_str, sizeof(temp_str), "Stack overflow happened in thread %s \r\n\r\n", osThreadGetName(object_id));
          DisplaySetFaultInfo(temp_str);
        }
      } else {
        /* If object_id is invalid then this is a fatal error */
        FatalError_Handler();
      }
      break;
    case osRtxErrorISRQueueOverflow:
      // ISR Queue overflow detected when inserting object (object_id)
      FatalError_Handler();
      break;
    case osRtxErrorTimerQueueOverflow:
      // User Timer Callback Queue overflow detected for timer (timer_id=object_id)
      SafeModeOperationActivate();
      (void)osKernelDestroyClass(SAFETY_CLASS_SAFE_MODE_OPERATION, osSafetyWithLowerClass);
      break;
    case osRtxErrorClibSpace:
      // Standard C/C++ library libspace not available: increase OS_THREAD_LIBSPACE_NUM
    case osRtxErrorClibMutex:
      // Standard C/C++ library mutex initialization failed
    case osRtxErrorSVC:
      // Invalid SVC function called (function=object_id)
    default:
      // Reserved
      FatalError_Handler();
      break;
  }

  return 0U;
}

/* Software watchdog error handler:
   This handler is executed when a software watchdog expires.
   - If Safe-Mode Operation software watchdog expired, all threads below its Safety Class 
     are suspended (web interface does not work anymore)
     Safe-mode Operation has another 500 ms before hardware watchdog resets the system
   - If software watchdog expired in any other thread, activate Safe-mode Operation 
     and suspend all threads with same or lower safety class (web interface might still work
     if software watchdog expired in Safety Class below it) */
uint32_t osWatchdogAlarm_Handler (osThreadId_t thread_id) {
  uint32_t safety_class;
  uint32_t next_interval;

  if (thread_id == NULL) {
    /* Thread id should never be NULL, so it is a fatal error */
    FatalError_Handler();
  }

  safety_class = osThreadGetClass(thread_id);
  if (safety_class > SAFETY_CLASS_SAFE_MODE_OPERATION) {
    /* If safety class is higher than safe-mode operation class then it is a fatal error */
    FatalError_Handler();
  }

  if (SafeModeOperationThreadGetId() == thread_id) {
    /* If software watchdog expired in Safe-Mode Operation thread.
       Suspend all threads with lower class than Safe-Mode Operation thread, 
       allow 510 ms before another watchdog alarm.
       However, if the execution does not continue after 500 ms a hardware 
       watchdog will reset the system. */
    (void)osThreadSuspendClass(SAFETY_CLASS_SAFE_MODE_OPERATION, osSafetyWithLowerClass);
    next_interval = 510U;
  } else {
    /* If software watchdog expired in any thread other than Safe-Mode Operation thread.
       Activate Safe-Mode Operation and suspend all threads of same or lower Safety Class. */
    SafeModeOperationActivate();
    (void)osThreadSuspendClass(safety_class, osSafetyWithSameClass | osSafetyWithLowerClass);

    /* Set Fault Info (for web display) */
    (void)snprintf(temp_str, sizeof(temp_str), "Thread watchdog alarm was triggered for thread %s \r\n\r\n", osThreadGetName(thread_id));
    DisplaySetFaultInfo(temp_str);
    next_interval = 0U;
  }

  return next_interval;
}

/* Memory management error handler (in C):
   This handler is executed if a thread tries to access memory out of its Zone.
   - If memory fault occurred in Safe-Mode Operation thread call FatalError_Handler
   - If memory fault occurred in Normal Operation or Operation Verification threads 
     then activate Safe-Mode Operation and suspend all threads of Safety Class 0.
   - If memory fault occurred in any other thread activate Safe-Mode Operation and 
     suspend all threads of same and lower Security Class.
*/
void MemManage_Handler_in_C (void) {
  osThreadId_t thread_id;
  uint32_t     zone;
  uint32_t     cfsr, icsr, mmfar;
  const bf_t * ptr_bf;

  cfsr  = SCB->CFSR;
  icsr  = SCB->ICSR;
  mmfar = SCB->MMFAR;
  SCB->CFSR = SCB_CFSR_IACCVIOL_Msk  |
              SCB_CFSR_DACCVIOL_Msk  |
              SCB_CFSR_MUNSTKERR_Msk |
              SCB_CFSR_MSTKERR_Msk   |
              SCB_CFSR_MLSPERR_Msk   |
              SCB_CFSR_MMARVALID_Msk;

  if ((cfsr & SCB_CFSR_MSTKERR_Msk) != 0U) {
    /* If a fault occurred on exception entry then clear pending SVC */
    SCB->SHCSR &= ~SCB_SHCSR_SVCALLPENDED_Msk;
  }

  if ((icsr & SCB_ICSR_RETTOBASE_Msk) == 0U) {
    /* If fault was not caused by a thread then this is a fatal error */
    FatalError_Handler();
  }

  thread_id = osThreadGetId ();
  if (thread_id == NULL) {
    /* If thread_id is invalid then this is a fatal error */
    FatalError_Handler();
  }

  /* Set Fault Info (for web display), before terminating the zone */
  if ((cfsr & (SCB_CFSR_MMARVALID_Msk | SCB_CFSR_DACCVIOL_Msk)) == (SCB_CFSR_MMARVALID_Msk | SCB_CFSR_DACCVIOL_Msk)) {
    /* If it is data access violation */
    (void)snprintf(temp_str, sizeof(temp_str), "Memory fault caused by thread %s trying to access data at address 0x%x \r\n\r\n", osThreadGetName(thread_id), mmfar);
  } else if ((cfsr & SCB_CFSR_MUNSTKERR_Msk) != 0U) {
    /* If it is unstack error */
    (void)snprintf(temp_str, sizeof(temp_str), "Memory fault caused by unstacking to stack of thread %s \r\n\r\n", osThreadGetName(thread_id));
  } else if ((cfsr & SCB_CFSR_IACCVIOL_Msk) != 0U) {
    /* If it is instruction access violation */
    ptr_bf = (const bf_t *)__get_PSP();
    (void)snprintf(temp_str, sizeof(temp_str), "Memory fault caused by thread %s trying to access instruction at address 0x%x \r\n\r\n", osThreadGetName(thread_id), ptr_bf->PC);
  } else {
    (void)snprintf(temp_str, sizeof(temp_str), "Memory fault cause unknown \r\n\r\n");
  }
  DisplaySetFaultInfo(temp_str);

  if (SafeModeOperationThreadGetId() == thread_id) {
    /* If a Safe-Mode Operation thread caused the fault then this is a fatal error */
    FatalError_Handler();
  }

  zone = osThreadGetZone(thread_id);
  if (zone >= ZONES_NUM) {
    /* If zone is invalid (id higher than maximum) then this is a fatal error */
    FatalError_Handler();
  }

  /* If it is a valid thread ID then terminate its zone */
  (void)osThreadTerminateZone(zone);
}

/* Memory management exception handler:
   This is an exception handler that calls MemManage_Handler_in_C 
   which is C function for handling the memory management fault. 
   If a MemManage_Handler_in_C returns back to this handler then it 
   continues normal RTOS operation by calling osFaultResume. */
__attribute__((naked)) void MemManage_Handler (void) {
  __ASM volatile (
    ".syntax unified\n\t"
    "push {r12,lr}\n\t"
    "ldr  r0,=MemManage_Handler_in_C\n\t"
    "blx  r0\n\t"
    "pop  {r12,lr}\n\t"
    "ldr  r0,=osFaultResume\n\t"
    "bx   r0\n\t"
  );
}

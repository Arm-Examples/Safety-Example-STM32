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

#include "NormalOperation.h"

#include "system_defs.h"
#include "io_defs.h"
#include "zones.h"
#include "cmsis_os2.h"
#include "cmsis_vio.h"

#include "Communication.h"

/* Local variable located in shared RAM */
static osMessageQueueId_t sensor_data_mq __attribute__((section("ram_shared"))) = NULL;

/* Local variables located in RAM_NORMAL_OP */
static uint32_t sensor_val = 0U;

/* Thread stacks located in RAM_NORMAL_OP */
static uint64_t sensor_thread_stack          [512/8];
static uint64_t normal_operation_thread_stack[512/8];

/* sensor_data_mq message queue attributes */
static const osMessageQueueAttr_t sensor_data_mq_attr = {
  .name       = "sensor_data_mq",
  .attr_bits  = osSafetyClass(SAFETY_CLASS_NORMAL_OPERATION), 
  .cb_mem     = NULL,                           /* System allocated control block */
  .cb_size    = 0U,
  .mq_mem     = NULL,                           /* System allocated data memory */
  .mq_size    = 0U
};

/* SensorThread thread attributes */
static const osThreadAttr_t sensor_thread_attr = {
  .name       = "SensorThread",
  .attr_bits  = osThreadUnprivileged                        | 
                osThreadZone(ZONE_NORMAL_OP)                | 
                osSafetyClass(SAFETY_CLASS_NORMAL_OPERATION), 
  .cb_mem     = NULL,                           /* System allocated control block */
  .cb_size    = 0U,
  .stack_mem  = sensor_thread_stack,            /* User provided stack */
  .stack_size = sizeof(sensor_thread_stack),
  .priority   = osPriorityNormal,
  .tz_module  = 0U,                             /* Not used */
  .affinity_mask = 0U                           /* Not used */
};

/* NormalOperationThread thread attributes */
static const osThreadAttr_t normal_operation_thread_attr = {
  .name       = "NormalOperationThread",
  .attr_bits  = osThreadUnprivileged                        | 
                osThreadZone(ZONE_NORMAL_OP)                | 
                osSafetyClass(SAFETY_CLASS_NORMAL_OPERATION), 
  .cb_mem     = NULL,                           /* System allocated control block */
  .cb_size    = 0U,
  .stack_mem  = normal_operation_thread_stack,  /* User provided stack */
  .stack_size = sizeof(normal_operation_thread_stack),
  .priority   = osPriorityNormal,
  .tz_module  = 0U,                             /* Not used */
  .affinity_mask = 0U                           /* Not used */
};

/* SensorThread thread:
   Get traffic sensor input data. 
   Value changes while USER button is pressed. 
   Double-click is used to force implausible value. */
static __NO_RETURN void SensorThread (void *argument) {
   int32_t sensor_dir  = -1;
  uint32_t sensor_step = 32U;
  uint32_t button_curr = 0U;
  uint32_t button_prev = 0U;
  uint32_t button_cnt  = 0U;

  (void)argument;

  for (;;) {
    if (button_cnt != 0U) {
      button_cnt--;
    }
    button_curr = vioGetSignal(BUTTON_SENSOR);
    if ((button_curr ^ button_prev) != 0U) {    /* If button state has changed */
      button_prev = button_curr;
      if (button_curr == 1U) {                  /* If button is pressed */
        sensor_dir = -sensor_dir;               /* Change sensor value change direction */
        if (button_cnt > 0U) {                  /* If double-click set sensor value to max */
          sensor_val = 4096U;
        } else {                                /* If 1st click */
          button_cnt = 10U;
        }
      }
    }
    if (button_curr == 1U) {                    /* If button is pressed */
      if (sensor_val != 4096U) {
        if (sensor_dir > 0) {                   /* If incrementing value */
          if (sensor_val < 4000U) {
            if ((sensor_val + sensor_step) < 4000U) {
              sensor_val += sensor_step;
            } else {
              sensor_val  = 4000U;
            }
          }
        } else {                                /* If decrementing value */
          if (sensor_val != 0U) {
            if (sensor_val > sensor_step) {
              sensor_val -= sensor_step;
            } else {
              sensor_val  = 0U;
            }
          }
        }
      }
    }

    /* Put value into a message queue for the Operation Verification */
    (void)osMessageQueuePut(sensor_data_mq, (const uint32_t *)&sensor_val, 0U, 0U);

    (void)osDelay(40U);
  }
}

/* NormalOperationThread thread:
   Normal operation of traffic light with green light duration proportional to the sensor input value.
   Software watchdog is set to 200 ms period.
   If software watchdog expires it is handled by the osWatchdogAlarm_Handler function.
   Handle also injection of memory fault in Normal Operation thread. */
static __NO_RETURN void NormalOperationThread (void *argument) {
  volatile uint32_t val;
           uint32_t time_ms, sensor_curr;

  (void)argument;

  /* Initialize local variables */
  time_ms     = 0U;
  sensor_curr = 0U;

  /* Normal Operation beginning:
     turn on red light and activate statuses for display on a web page */
  vioSetSignal(LIGHT_RED,    vioLEDon);
  vioSetSignal(LIGHT_YELLOW, vioLEDoff);
  vioSetSignal(LIGHT_GREEN,  vioLEDoff);
  DisplaySetOperatingMode(MODE_NORMAL);
  DisplaySetLightsStatus(LIGHT_RED_VAL);

  for (;;) {
    (void)osThreadFeedWatchdog(200U);   /* Timeout for software watchdog is 200 ms */

    if (time_ms >= (8000U + sensor_curr)) {
      /* Yellow -> red */
      time_ms = 0U;

      vioSetSignal(LIGHT_RED,    vioLEDon);
      vioSetSignal(LIGHT_YELLOW, vioLEDoff);
      vioSetSignal(LIGHT_GREEN,  vioLEDoff);
      DisplaySetLightsStatus(LIGHT_RED_VAL);
    } else if (time_ms >= (7000U + sensor_curr)) {
      /* Green -> yellow */
      vioSetSignal(LIGHT_RED,    vioLEDoff);
      vioSetSignal(LIGHT_YELLOW, vioLEDon);
      vioSetSignal(LIGHT_GREEN,  vioLEDoff);
      DisplaySetLightsStatus(LIGHT_YELLOW_VAL);
    } else if (time_ms >= 6000U) {
      sensor_curr = sensor_val & 0xFFFU;

      /* Red + yellow -> green */
      vioSetSignal(LIGHT_RED,    vioLEDoff);
      vioSetSignal(LIGHT_YELLOW, vioLEDoff);
      vioSetSignal(LIGHT_GREEN,  vioLEDon);
      DisplaySetLightsStatus(LIGHT_GREEN_VAL);
    } else if (time_ms >= 5000U) {
      /* Red -> red + yellow */
      vioSetSignal(LIGHT_RED,    vioLEDon);
      vioSetSignal(LIGHT_YELLOW, vioLEDon);
      vioSetSignal(LIGHT_GREEN,  vioLEDoff);
      DisplaySetLightsStatus(LIGHT_RED_VAL | LIGHT_YELLOW_VAL);
    }

    (void)osDelay(100U);
    time_ms += 100U;

    if (FaultWasInjected(FAULT_INJECT_SC0_MEM_ACCESS) != 0U) {
      /* If injection of a memory fault was activated from the web interface */
      val = *((uint32_t *)0x20000000);  /* Access memory out of Zone */
    }
  }
}

/* Initialize Normal Operation */
void NormalOperationInit (void) {

  sensor_data_mq = osMessageQueueNew(8U, 4U, &sensor_data_mq_attr);

  (void)osThreadNew(SensorThread,          NULL, &sensor_thread_attr);
  (void)osThreadNew(NormalOperationThread, NULL, &normal_operation_thread_attr);
}

/* Get sensor data for verification:
   If no data is available in timeout then return value is SENSOR_ERROR. */
uint32_t SensorGetData (uint32_t timeout) {
  uint32_t val;

  if (osMessageQueueGet(sensor_data_mq, &val, NULL, timeout) != osOK) {
    val = SENSOR_ERROR;
  }

  return val;
}

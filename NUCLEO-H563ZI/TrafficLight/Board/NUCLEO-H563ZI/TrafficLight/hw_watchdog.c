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

#include "hw_watchdog.h"

#ifdef   _RTE_
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

/* Global variables defined in the main.c module */
extern IWDG_HandleTypeDef hiwdg;

/* Initialize hardware watchdog with specified timeout in milliseconds */
void HwWatchdogInit (uint32_t timeout) {

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Window = timeout;
  hiwdg.Init.Reload = timeout;
  (void)HAL_IWDG_Init(&hiwdg);
}

/* Feed hardware watchdog */
void HwWatchdogFeed (void) {

  if (hiwdg.Instance != NULL) {
    (void)HAL_IWDG_Refresh(&hiwdg);
  }
}

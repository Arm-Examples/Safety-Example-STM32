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

#include "reset_reason.h"

#ifdef   _RTE_
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

/* Get reset reason */
uint32_t GetResetReason (void) {
  uint32_t reset_reason = 0U;

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
    /* If system was reset by a software reset */
    reset_reason |= RESET_REASON_SOFTWARE;
  } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) {
    /* If system was reset by a hardware watchdog reset */
    reset_reason |= RESET_REASON_HARDWARE;
  }

  __HAL_RCC_CLEAR_RESET_FLAGS();

  return reset_reason;
}

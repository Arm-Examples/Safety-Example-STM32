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

#ifndef RESET_REASON_H_
#define RESET_REASON_H_

#include <stdint.h>

/* Reset reason definitions */
#define RESET_REASON_SOFTWARE   ((uint32_t)1U)
#define RESET_REASON_HARDWARE   ((uint32_t)2U)

/* Get reset reason function prototype */
extern uint32_t GetResetReason (void);

#endif

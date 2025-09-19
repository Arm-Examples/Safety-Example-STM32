/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2021 Arm Limited (or its affiliates).
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

#ifndef SAFE_MODE_OPERATION_H_
#define SAFE_MODE_OPERATION_H_

#include <stdint.h>

#include "cmsis_os2.h"

/* Definitions */
#define SAFE_MODE_OPERATION             (1U)

/* Global functions defined by SafeModeOperation.c module */
extern void         SafeModeOperationInit        (void);
extern void         SafeModeOperationActivate    (void);
extern osThreadId_t SafeModeOperationThreadGetId (void);

#endif

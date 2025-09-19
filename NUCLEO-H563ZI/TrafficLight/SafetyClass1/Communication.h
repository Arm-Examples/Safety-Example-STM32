/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2024 Arm Limited (or its affiliates).
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

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdint.h>

/* Definitions */
/* Mode definitions */
#define MODE_NORMAL                     (0U)
#define MODE_SAFE                       (1U)

/* Lights status definitions */
#define LIGHT_ALL_OFF_VAL               (0U)
#define LIGHT_RED_VAL                   (1U)
#define LIGHT_YELLOW_VAL                (2U)
#define LIGHT_GREEN_VAL                 (4U)

/* Fault injection definitions */
#define FAULT_INJECT_SC2_FATAL          ((uint32_t)1U)
#define FAULT_INJECT_SC2_WDOG           ((uint32_t)1U << 1)
#define FAULT_INJECT_SC1_MEM_ACCESS     ((uint32_t)1U << 3)
#define FAULT_INJECT_SC0_WDOG           ((uint32_t)1U << 4)
#define FAULT_INJECT_SC0_MEM_ACCESS     ((uint32_t)1U << 5)

/* Global functions defined by Communication.c module */
extern void        CommunicationInit        (void);
extern void        DisplaySetOperatingMode  (uint32_t mode);
extern uint32_t    DisplayGetOperatingMode  (void);
extern void        DisplaySetLightsStatus   (uint32_t status);
extern uint32_t    DisplayGetLightsStatus   (void);
extern void        DisplaySetSensorValue    (uint32_t value);
extern uint32_t    DisplayGetSensorValue    (void);
extern void        DisplaySetFaultInfo      (const char *str);
extern const char *DisplayGetFaultInfo      (void);
extern void        FaultInject              (uint32_t fault_flag);
extern uint32_t    FaultWasInjected         (uint32_t fault_flag);

#endif

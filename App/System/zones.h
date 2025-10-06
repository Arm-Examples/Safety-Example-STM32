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

#ifndef ZONES_H_
#define ZONES_H_

/* Zone definitions */
#define ZONE_NORMAL_OP                  ((uint32_t)0U)
#define ZONE_VERIFY_OP                  ((uint32_t)1U)
#define ZONE_COM                        ((uint32_t)2U)
#define ZONE_SAFE_OP                    ((uint32_t)3U)
#define ZONE_TIMER                      ((uint32_t)4U)
#define ZONE_IDLE                       ((uint32_t)5U)

/* Number of zones */
#define ZONES_NUM                       6U

/* Zone initialization function prototype */
extern void ZonesInitialize (void);

#endif

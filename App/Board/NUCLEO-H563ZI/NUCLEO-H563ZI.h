/*---------------------------------------------------------------------------
 * Copyright (c) 2024 Arm Limited (or its affiliates).
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

#ifndef NUCLEO_H563ZI_H_
#define NUCLEO_H563ZI_H_

#include "GPIO_STM32.h"
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"

// CMSIS Driver instances of Board peripherals
#define CMSIS_DRIVER_ETH    0           // Driver_ETH_MAC0

// CMSIS Drivers
extern ARM_DRIVER_ETH_MAC   ARM_Driver_ETH_MAC_(CMSIS_DRIVER_ETH);      // ETH MAC
extern ARM_DRIVER_ETH_PHY   ARM_Driver_ETH_PHY_(CMSIS_DRIVER_ETH);      // ETH PHY

#ifdef   CMSIS_shield_header
#include CMSIS_shield_header
#endif

#endif // NUCLEO_H563ZI_H_

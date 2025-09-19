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

// Vendor:    STMicroelectronics
// Device:    STM32H563ZIT6
// Processor: Cortex-M33
// Project:   TrafficLight

#include "zones.h"

#ifdef   _RTE_
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

#include "mem_layout.h"                 // Memory layout header

#include "faults.h"
#include "cmsis_os2.h"

#define MPU_REGIONS                     8U

static const ARM_MPU_Region_t mpu_table[ZONES_NUM][MPU_REGIONS];

/* Initialize Zones (MPU) settings */
void ZonesInitialize (void) {

  ARM_MPU_Disable();

  /* Memory attributes for Flash (index 0) = Outer/Inner: Normal cacheable memory, Non-Transient, no Write-Back, Read Allocate, no Write Allocate */
  ARM_MPU_SetMemAttr(0UL, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL), ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL)));

  /* Memory attributes for RAM (index 1) = Outer/Inner: Normal non-cacheable memory */
  ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

  /* Memory attributes for Peripherals (index 2) = Device memory: nG (non-Gathering), nR (non-Reordering), nE (no Early Write Acknowledgment) */
  ARM_MPU_SetMemAttr(2UL, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE));

  /* Enable Privileged Default */
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

/* Update MPU settings for newly activating Zone */
void osZoneSetup_Callback (uint32_t zone) {

  if (zone >= ZONES_NUM) {
    ZoneError_Handler();
  }

  ARM_MPU_Disable();
  ARM_MPU_Load(0U, mpu_table[zone], MPU_REGIONS);
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

/* MPU table specifying MPU regions for all Zones */
static const ARM_MPU_Region_t mpu_table[ZONES_NUM][MPU_REGIONS] = {
  /* Zone 'Zone_Normal_OP' */ 
  {
    //                    (BASE                       , Shareability  , RO , NP , XN ),                     (LIMIT                                                                                   , ATTR IDX) // Comment  
    { .RBAR = ARM_MPU_RBAR(REGION_FLASH_START         , ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(REGION_FLASH_START          + REGION_FLASH_SIZE                                    - 1UL, 0UL) },   // FLASH
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SHARED_START    , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SHARED_START     + REGION_RAM_NO_INIT_EVR_SIZE + REGION_RAM_SHARED_SIZE - 1UL, 1UL) },   // RAM_SHARED + RAM_EVR
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_NORMAL_OP_START , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_NORMAL_OP_START  + REGION_RAM_NORMAL_OP_SIZE                            - 1UL, 1UL) },   // RAM_NORMAL_OP
    { .RBAR = ARM_MPU_RBAR(GPIOB_BASE_NS              , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(GPIOB_BASE_NS               + (2 * 0x400UL)                                        - 1UL, 2UL) },   // LIGHT_GREEN (GPIOB) + BUTTON_USER (GPIOC)
    { .RBAR = ARM_MPU_RBAR(GPIOF_BASE_NS              , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(GPIOF_BASE_NS               + (2 * 0x400UL)                                        - 1UL, 2UL) },   // LIGHT_YELLOW (GPIOF) + LIGHT_RED (GPIOG)
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL }                                                                                                                                                                                 // Not used
  },
  /* Zone 'Zone_Verify_OP' */ 
  {
    //                    (BASE                       , Shareability  , RO , NP , XN ),                     (LIMIT                                                                                   , ATTR IDX) // Comment  
    { .RBAR = ARM_MPU_RBAR(REGION_FLASH_START         , ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(REGION_FLASH_START          + REGION_FLASH_SIZE                                    - 1UL, 0UL) },   // FLASH
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SHARED_START    , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SHARED_START     + REGION_RAM_NO_INIT_EVR_SIZE + REGION_RAM_SHARED_SIZE - 1UL, 1UL) },   // RAM_SHARED + RAM_EVR
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_VERIFY_OP_START , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_VERIFY_OP_START  + REGION_RAM_VERIFY_OP_SIZE                            - 1UL, 1UL) },   // RAM_VERIFY_OP
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL }                                                                                                                                                                                 // Not used
  },
  /* Zone 'Zone_Com' */ 
  {
    //                    (BASE                       , Shareability  , RO , NP , XN ),                     (LIMIT                                                                                   , ATTR IDX) // Comment  
    { .RBAR = ARM_MPU_RBAR(REGION_FLASH_START         , ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(REGION_FLASH_START          + REGION_FLASH_SIZE                                    - 1UL, 0UL) },   // FLASH
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SHARED_START    , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SHARED_START     + REGION_RAM_NO_INIT_EVR_SIZE + REGION_RAM_SHARED_SIZE - 1UL, 1UL) },   // RAM_SHARED + RAM_EVR
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_COM_START       , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_COM_START        + REGION_RAM_COM_SIZE                                  - 1UL, 1UL) },   // RAM_COM
    { .RBAR = ARM_MPU_RBAR(GPIOA_BASE_NS              , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(GPIOA_BASE_NS               + (3 * 0x400UL)                                        - 1UL, 2UL) },   // ETH pins (GPIOA + GPIOB + GPIOC)
    { .RBAR = ARM_MPU_RBAR(GPIOG_BASE_NS              , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(GPIOG_BASE_NS               + 0x400UL                                              - 1UL, 2UL) },   // ETH pins (GPIOG)
    { .RBAR = ARM_MPU_RBAR(RCC_BASE_NS                , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(RCC_BASE_NS                 + 0x400UL                                              - 1UL, 2UL) },   // RCC
    { .RBAR = ARM_MPU_RBAR(SBS_BASE_NS                , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(SBS_BASE_NS                 + 0x400UL                                              - 1UL, 2UL) },   // SBS
    { .RBAR = ARM_MPU_RBAR(ETH_BASE_NS                , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(ETH_BASE_NS                 + 0x1400UL                                             - 1UL, 2UL) }    // ETH
  },
  /* Zone 'Zone_Safe_OP' */ 
  {
    //                    (BASE                       , Shareability  , RO , NP , XN ),                     (LIMIT                                                                                   , ATTR IDX) // Comment  
    { .RBAR = ARM_MPU_RBAR(REGION_FLASH_START         , ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(REGION_FLASH_START          + REGION_FLASH_SIZE                                    - 1UL, 0UL) },   // FLASH
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SHARED_START    , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SHARED_START     + REGION_RAM_NO_INIT_EVR_SIZE + REGION_RAM_SHARED_SIZE - 1UL, 1UL) },   // RAM_SHARED + RAM_EVR
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SAFE_OP_START   , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SAFE_OP_START    + REGION_RAM_SAFE_OP_SIZE                              - 1UL, 1UL) },   // RAM_SAFE_OP
    { .RBAR = ARM_MPU_RBAR(IWDG_BASE_NS               , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(IWDG_BASE_NS                + 0x400UL                                              - 1UL, 2UL) },   // IWDG
    { .RBAR = ARM_MPU_RBAR(GPIOB_BASE_NS              , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(GPIOB_BASE_NS               + 0x400UL                                              - 1UL, 2UL) },   // LIGHT_GREEN (GPIOB)
    { .RBAR = ARM_MPU_RBAR(GPIOF_BASE_NS              , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(GPIOF_BASE_NS               + (2 * 0x400UL)                                        - 1UL, 2UL) },   // LIGHT_YELLOW (GPIOF) + LIGHT_RED (GPIOG)
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL }                                                                                                                                                                                 // Not used
  },
  /* Zone 'Zone_Timer' */ 
  {
    //                    (BASE                       , Shareability  , RO , NP , XN ),                     (LIMIT                                                                                   , ATTR IDX) // Comment  
    { .RBAR = ARM_MPU_RBAR(REGION_FLASH_START         , ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(REGION_FLASH_START          + REGION_FLASH_SIZE                                    - 1UL, 0UL) },   // FLASH
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SHARED_START    , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SHARED_START     + REGION_RAM_NO_INIT_EVR_SIZE + REGION_RAM_SHARED_SIZE - 1UL, 1UL) },   // RAM_SHARED + RAM_EVR
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_COM_START       , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_COM_START        + REGION_RAM_COM_SIZE                                  - 1UL, 1UL) },   // RAM_COM
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_TIMER_START     , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_TIMER_START      + REGION_RAM_TIMER_SIZE                                - 1UL, 1UL) },   // RAM_TIMER
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL }                                                                                                                                                                                 // Not used
  },
  /* Zone 'Zone_Idle' */ 
  {
    //                    (BASE                       , Shareability  , RO , NP , XN ),                     (LIMIT                                                                                   , ATTR IDX) // Comment  
    { .RBAR = ARM_MPU_RBAR(REGION_FLASH_START         , ARM_MPU_SH_NON, 1UL, 1UL, 0UL), .RLAR = ARM_MPU_RLAR(REGION_FLASH_START          + REGION_FLASH_SIZE                                    - 1UL, 0UL) },   // FLASH
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_SHARED_START    , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_SHARED_START     + REGION_RAM_NO_INIT_EVR_SIZE + REGION_RAM_SHARED_SIZE - 1UL, 1UL) },   // RAM_SHARED + RAM_EVR
    { .RBAR = ARM_MPU_RBAR(REGION_RAM_IDLE_START      , ARM_MPU_SH_NON, 0UL, 1UL, 1UL), .RLAR = ARM_MPU_RLAR(REGION_RAM_IDLE_START       + REGION_RAM_IDLE_SIZE                                 - 1UL, 1UL) },   // RAM_IDLE
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL },                                                                                                                                                                                // Not used
    { .RBAR = 0UL, .RLAR = 0UL }                                                                                                                                                                                 // Not used
  }
};

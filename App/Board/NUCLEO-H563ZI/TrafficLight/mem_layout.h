// Vendor:    STMicroelectronics
// Device:    STM32H563ZIT6
// Processor: Cortex-M33
// Project:   TrafficLight

#ifndef MEM_LAYOUT_H
#define MEM_LAYOUT_H

#define REGION_FLASH_START                      0x08000000
#define REGION_FLASH_SIZE                       0x00200000

#define REGION_RAM_PRIVILEGED_START             0x20010000
#define REGION_RAM_PRIVILEGED_SIZE              0x00008000

#define REGION_RAM_COM_START                    0x20018000
#define REGION_RAM_COM_SIZE                     0x00008000

#define REGION_RAM_SHARED_START                 0x20020000
#define REGION_RAM_SHARED_SIZE                  0x00003000

#define REGION_RAM_NO_INIT_EVR_START            0x20023000
#define REGION_RAM_NO_INIT_EVR_SIZE             0x00001000

#define REGION_RAM_NORMAL_OP_START              0x20024000
#define REGION_RAM_NORMAL_OP_SIZE               0x00001000

#define REGION_RAM_VERIFY_OP_START              0x20025000
#define REGION_RAM_VERIFY_OP_SIZE               0x00001000

#define REGION_RAM_SAFE_OP_START                0x20026000
#define REGION_RAM_SAFE_OP_SIZE                 0x00001000

#define REGION_RAM_TIMER_START                  0x20027000
#define REGION_RAM_TIMER_SIZE                   0x00001000

#define REGION_RAM_IDLE_START                   0x20028000
#define REGION_RAM_IDLE_SIZE                    0x00001000

#define REGION_ARM_LIB_STACK_START              0x2009FC00
#define REGION_ARM_LIB_STACK_SIZE               0x00000400      // This size is ignored, stack size is defined in statup_stm32h563xx.s

#endif

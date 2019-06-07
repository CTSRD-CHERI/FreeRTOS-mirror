/*****************************************************************************/
/**
*
* @file bsp.h
* @addtogroup bsp
* @{
*
* Test
* ## This is markdown example
*
* **More** markdown
*
* @note
*
* Lorem impsum
*
*
******************************************************************************/
#ifndef RISCV_GENERIC_BSP_H
#define RISCV_GENERIC_BSP_H

#include "stdint.h"
#include "plic_driver.h"

/**
 * PLIC defines
 */
#define PLIC_BASE_ADDR (0xC000000ULL)

#define PLIC_NUM_SOURCES 16
#define PLIC_NUM_PRIORITIES 16

extern plic_instance_t Plic;

void prvSetupHardware(void);
void external_interrupt_handler(uint32_t cause);

/**
 * Xilinx Drivers defines
 * Some xillinx drivers require to sleep for given number of seconds
 */
#include "FreeRTOS.h"
#include "task.h"

#define sleep(_SECS) vTaskDelay(pdMS_TO_TICKS(_SECS * 1000));
#define msleep(_MSECS) vTaskDelay(pdMS_TO_TICKS(_MSECS));

#endif /* RISCV_GENERIC_BSP_H */

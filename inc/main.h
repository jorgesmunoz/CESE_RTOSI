/*=============================================================================
 * Copyright (c) 2020, jorge <jorgesmunoz@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/06/21
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __RTOS_UART_LED_TEC_H__
#define __RTOS_UART_LED_TEC_H__

/*=====[Inclusions of public function dependencies]==========================*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "sapi.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOSConfig.h"
#include "irq_button_handler.h"
#include "uart_driver.h"
#include "userTasks.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define CANT_LEDS 4
#define ANTIREBOTE_MS 20

#define UP      1
#define FALLING 2
#define DOWN	3
#define RISING  4
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __RTOS_UART_LED_TEC_H__ */

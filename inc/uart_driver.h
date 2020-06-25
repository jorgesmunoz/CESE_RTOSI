/*
 * uart_driver.h
 *
 *  Created on: 21 jun. 2020
 *      Author: jorge
 */

#ifndef PROGRAMS_PCSE_TP_INC_UART_DRIVER_H_
#define PROGRAMS_PCSE_TP_INC_UART_DRIVER_H_

#include "FreeRTOS.h"
#include "task.h"

#include "sapi.h"
#include <stdint.h>
#include <stddef.h>
#include "FreeRTOSConfig.h"
#include "main.h"
#include "userTasks.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uartMap_t uart_value;
	uint32_t baud_rate;
	SemaphoreHandle_t uart_mutex;
	SemaphoreHandle_t button_activation;
	char byte_ring;
}uart_drive_t;

bool_t ag9_test( int32_t uart );
bool_t ag9_attach( int32_t uart );
bool_t ag9_syntax( int32_t uart );
bool_t uart_cofig( uart_drive_t *uart_drive );
void functionRX( void * );

uart_drive_t uart_struct;

typedef struct{
	char state_rec;
}char_rec;

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* PROGRAMS_RTOS_UART_RX_INTERRUPT_INC_UART_DRIVE_H_ */

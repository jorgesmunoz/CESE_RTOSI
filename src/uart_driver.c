/*
 * uart_driver.c
 *
 *  Created on: 21 jun. 2020
 *      Author: jorge
 */


#include "uart_driver.h"

#include "FreeRTOSConfig.h"
#include "main.h"
#include "userTasks.h"
#include "FreeRTOS.h"
#include "task.h"

#include "sapi.h"
#include "uart_driver.h"

bool_t uart_cofig( uart_drive_t *uart_drive ){

	uartConfig(uart_drive->uart_value, uart_drive->baud_rate);

	uartCallbackSet(uart_drive->uart_value, UART_RECEIVE, functionRX, NULL);

	uartInterrupt(uart_drive->uart_value, TRUE);

	return TRUE;

}

void functionRX( void *param )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* Se abre seccion critica para lectura de bytes recibidos */
	xSemaphoreTakeFromISR( uart_struct.uart_mutex, &xHigherPriorityTaskWoken );

	char c = uartRxRead( uart_struct.uart_value );

	/* Verifica que hay un llamado entrante */
	if(c == uart_struct.byte_ring){
		if(c == 'R')
			uart_struct.byte_ring = 'I';
		if(c == 'I')
			uart_struct.byte_ring = 'N';
		if(c == 'N')
			uart_struct.byte_ring = 'G';
		if(c == 'G'){
			uart_struct.byte_ring = '\0';
			tec_save.str_button = Tecla1;
			printf( "Sistema activado\r\n" );
			//xQueueSendFromISR(lcd_queue, &tec_save, &xHigherPriorityTaskWoken);
			xSemaphoreGiveFromISR( system_access.sem_system, &xHigherPriorityTaskWoken);
		}
	}

	xSemaphoreGiveFromISR( uart_struct.uart_mutex, &xHigherPriorityTaskWoken);
	/* Se cierra seccion critica para lectura de bytes recibidos */

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}



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


	/* Envia comando AT al modulo para verificar si funciona */
	if( ag9_test( uart_drive->uart_value ) ){
		printf("Modulo funciona correctamente\r\n");
	}
	else{
		printf("Modulo ag9 no funciona\r\n");
	}

	/* Configuración de modo texto para recibir SMS */
	if( ag9_syntax( uart_drive->uart_value ) ){
		printf("Modulo texto activado correctamente\r\n");
	}
	else{
		printf("Modo texto no activado\r\n");
	}

	/* Verifica si está attachado a la red */
	if( ag9_attach( uart_drive->uart_value ) ){
		printf("Modulo atachado a la red\r\n");
	}
	else{
		printf("Modulo af9 no esta atachado a la red\r\n");
	}


	return TRUE;

}

bool_t ag9_test( int32_t uart )
{
   uartWriteString( uart, "AT\r\n" );
   return waitForReceiveStringOrTimeoutBlocking( uart, "OK\r\n", strlen("OK\r\n"), 1000 );
}

bool_t ag9_syntax( int32_t uart )
{
   uartWriteString( uart, "AT+CMGF=1\r\n" );
   return waitForReceiveStringOrTimeoutBlocking( uart, "OK\r\n", strlen("OK\r\n"), 1000 );
}

bool_t ag9_attach( int32_t uart )
{
   uartWriteString( uart, "AT+CGATT?\r\n" );
   return waitForReceiveStringOrTimeoutBlocking( uart, "OK\r\n", strlen("OK\r\n"), 1000 );
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



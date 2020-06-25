/*=============================================================================
 * Copyright (c) 2020, jorge <jorgesmunoz@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/06/21
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include <stdint.h>
#include <stddef.h>
#include "FreeRTOSConfig.h"
#include "uart_driver.h"
#include "userTasks.h"

/*=====[Definition macros of private constants]==============================*/
#define MY_ASSERT(CONDICION) my_assert_debug(CONDICION)
DEBUG_PRINT_ENABLE;
extern pinInitGpioLpc4337_t gpioPinsInit[];
/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
	boardInit();

	uart_struct.uart_value = UART_232;
	uart_struct.baud_rate = 115200;
	uart_struct.byte_ring = 'R';

	uart_cofig( &uart_struct );

	/* Configuracion de interrupciones */
	IRQ_handler_Init();

	_task_init( tec_config );

	vTaskStartScheduler(); // Initialize scheduler

	while( true ); // If reach heare it means that the scheduler could not start

	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}

/*=============================================================================
 * Copyright (c) 2020, jorge <jorgesmunoz@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/06/21
 * Version: 1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/
static BaseType_t xErrorHandler = pdFAIL;
char lcd_text_t[] = "Sistema de";
char init_text_t[] = "iniciado";
char rele_state_t[4][10] = {"abierto", "cerrado", ": error", "iniciado"};
char rele_code_t[3][10] = {"correcto", "correcto", "erroneo"};
/* Flag para habilitar modo teclado */
uint8_t sem_ok = 0;
xTaskHandle TaskHandle_1;
/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/
   // Create a task in freeRTOS with dynamic memory
void _task_init( button_str_t * tec_str_config ){

	/* GPIO para rele */
	gpioInit(GPIO8, GPIO_OUTPUT);

	/* Se crean queue de capturas de teclas */
	int8_t i;
	for (i = CANT_TECLAS ; i-- ; i >= 0) {
		tec_str_config[i].str_button = i;
		if (NULL == (tec_str_config[i].queue_button = xQueueCreate(5,sizeof(tec_str_config)))){
			printf("No se crearon queue para teclas\r\n");
		}
	}

	/* Creacion de tarea para Tecla1, Tecla2, Tecla3 */
	for(uint8_t i = 0; i < CANT_TECLAS; i++){
		xErrorHandler = xTaskCreate(tecla_task,
				(const char *)"tecla_task",
				configMINIMAL_STACK_SIZE*2,
				&tec_str_config[i],
				tskIDLE_PRIORITY+1,
				0
		);

		if(xErrorHandler == pdPASS){
			printf("Tarea TEC%d  creada\r\n", i+1);
		}
	}

	xErrorHandler = pdFAIL;

	/* Creacion tarea led */
	xErrorHandler = xTaskCreate(led_task,
			(const char *)"led_task",
			configMINIMAL_STACK_SIZE*2,
			0,
			tskIDLE_PRIORITY+1,
			0
	);

	if(xErrorHandler == pdPASS){
		printf("Tarea led creada\r\n");
	}

	xErrorHandler = pdFAIL;

	/* Creacion tarea system */
	xErrorHandler = xTaskCreate(system_task,
			(const char *)"system_task",
			configMINIMAL_STACK_SIZE*2,
			0,
			tskIDLE_PRIORITY+1,
			0
	);

	if(xErrorHandler == pdPASS){
		printf("Tarea system creada\r\n");
	}

	xErrorHandler = pdFAIL;

	/* Creacion tarea LCD */
	xErrorHandler = xTaskCreate(lcd_task,
			(const char *)"lcd_task",
			configMINIMAL_STACK_SIZE*2,
			0,
			tskIDLE_PRIORITY+1,
			0
	);

	if(xErrorHandler == pdPASS){
		printf("Tarea lcd creada\r\n");
	}


	/* Queue para imprimir en LCD */
	if (NULL == (lcd_queue = xQueueCreate(3,sizeof(read_tecla)))){
		printf("Error al crear queue LCD\r\n");
	}

	/* Queue para Leds */
	if (NULL == (led_queue = xQueueCreate(3,sizeof(read_tecla)))){
		printf("Error al crear queue led\r\n");
	}

	/* Semaforo binario para control de rele (sistema de acceso) */
	if (NULL == (system_access.sem_system = xSemaphoreCreateBinary())){
		printf("Error al crear semaforo led\r\n");
	}

	/* Mutex para LCD y UART */
	if (NULL == (uart_struct.uart_mutex = xSemaphoreCreateMutex())){
		printf("Error al crear mutex\r\n");
	}

	first_init();

}

/* Tarea de inicialización del sistema */
void first_init(void){

	read_tecla _init_state;
	_init_state.str_button = Tecla2;

	/* El sistema se inicializa cerrado */
	gpioWrite(LED1, ON);

	/* Inicializa relay */
	gpioWrite(GPIO8, ON);

	xQueueSend(lcd_queue, &_init_state, portMAX_DELAY);
	printf("Sistema iniciado\r\n");

}


void tecla_task(void* taskParmPt){

	button_str_t* tecla_select; //Me preparo para recibir la direcciÃ³n de la estructura y copiarla en una varibale local
	tecla_select = (button_str_t*) taskParmPt;
	tecla_select->state_button = UP;

	read_tecla tec_saved;
	tec_saved.str_button = tecla_select->str_button;

	button_control button_pressed;

	// ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE) {

		/* Espera semaforo si se activo el modo manual */

		if(system_access.system_flag == TRUE){

			if (xQueueReceive(tecla_select->queue_button, &button_pressed, portMAX_DELAY)){

				switch (tecla_select->state_button){

				case UP:
					if(button_pressed.flank == FALLING){ //AcÃ¡ adentro estÃ¡ el pseudo estado Falling

						if (pdFALSE == (xQueueReceive(tecla_select->queue_button, &button_pressed, (ANTIREBOTE_MS / portTICK_RATE_MS)))){
							tecla_select->state_button = DOWN;

							if (pdTRUE == xSemaphoreTake( uart_struct.uart_mutex, portMAX_DELAY)){

								printf("Se capturo una pulsacion\r\n");

								xSemaphoreGive( uart_struct.uart_mutex );

							}
						}
					}
					break;

				case DOWN:
					if(button_pressed.flank == RISING){ //AcÃ¡ adentro estÃ¡ el pseudo estado Rising

						if (pdFALSE == (xQueueReceive(tecla_select->queue_button, &button_pressed, (ANTIREBOTE_MS / portTICK_RATE_MS)))){
							tecla_select->state_button = UP;

							if (pdTRUE == xSemaphoreTake( uart_struct.uart_mutex, portMAX_DELAY)){

								printf("Lectura completada en la tecla %d\r\n", tec_saved.str_button+1);

								xSemaphoreGive( uart_struct.uart_mutex );

							}

							//Queue para led
							xQueueSend(led_queue, &tec_saved, portMAX_DELAY);
							//Queue para lcd
							xQueueSend(lcd_queue, &tec_saved, portMAX_DELAY);
						}
					}
					break;

				default:
					tecla_select->state_button = UP;
					break;

				}

			}
		}
	}

}

void led_task(void* taskParmPt){

	read_tecla read_led;

	gpioMap_t Led_Map[CANT_LEDS] = {LEDR,LED1,LED2,LED3};

	while (TRUE){
		//Espero evento de Lectura completada
		if (xQueueReceive(led_queue, &read_led, portMAX_DELAY)){

			/* Seccion critica para imprimir en consola */
			if (pdTRUE == xSemaphoreTake( uart_struct.uart_mutex , portMAX_DELAY)){

				printf("El codigo de acceso es %s. Sistema %s\r\n", rele_code_t[read_led.str_button], rele_state_t[read_led.str_button]);
				xSemaphoreGive( uart_struct.uart_mutex );
			}
			/* Fin de seccion critica para imprimir en consola */

			switch(read_led.str_button){
			case Tecla1:
				system_access.system_status = OPEN;
				system_status(&system_access);
				break;
			case Tecla2:
				system_access.system_status = CLOSED;
				system_status(&system_access);
				break;
			case Tecla3:
				system_access.system_status = WRONG;
				system_status(&system_access);
				break;
			default:
				break;
			}
		}
	}
}

void system_task(void* taskParmPt){

	while(TRUE){

		xSemaphoreTake( system_access.sem_system, portMAX_DELAY );

		read_tecla tec_init;
		tec_init.str_button = 3;

		xQueueSend(lcd_queue, &tec_init, portMAX_DELAY);

		/* Se habilita sistema */
		system_access.system_flag = TRUE;
		system_access.system_status = ACTIVATED;

		system_status(&system_access);

		vTaskDelay(1000/portTICK_RATE_MS);
	}

}

// Implementacion de funcion de la tarea LCD 16x2
void lcd_task( void* taskParmPtr ){

	i2cInit( I2C0, 100000 );

	// Inicializar LCD de 16x2 (caracteres x lineas) con cada caracter de 5x8 pixeles
	lcdInit( 16, 2, 5, 8 );

	lcdCursorSet( LCD_CURSOR_OFF );
	lcdClear();

	gpioMap_t Led_Map[CANT_LEDS] = {LEDR,LED1,LED2,LED3};

	while (TRUE){

		//Espero evento de Lectura completada
		if (xQueueReceive(lcd_queue, &tec_save, portMAX_DELAY)){

			/* Seccion critica para imprimir en LCD */
			if (pdTRUE == xSemaphoreTake( uart_struct.uart_mutex, portMAX_DELAY)){

				char lcd2[] = "acceso ";

				strcat(lcd2, rele_state_t[tec_save.str_button]);

				lcdClear();
				lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
				lcdSendStringRaw( lcd_text_t );
				lcdGoToXY( 0, 1 );
				lcdSendStringRaw( lcd2 );

				xSemaphoreGive( uart_struct.uart_mutex );
			}
			/* Fin seccion critica para imprimir en LCD */
		}
	}
}

void system_status(system_control_t *system_access){

	switch(system_access->system_status){
	case ACTIVATED:
		gpioWrite(LED3, ON);
		/* Habilita interrupciones de teclas */
		enable_buttons_interrupt();
		break;
	case OPEN:
		gpioWrite(LEDB, ON);
		gpioWrite(LED1, OFF);
		/* Enciende relay */
		gpioWrite(GPIO8, OFF);
		break;
	case CLOSED:
		gpioWrite(LEDB, OFF);
		gpioWrite(LED1, ON);
		gpioWrite(LED3, OFF);
		/* Apaga relay */
		gpioWrite(GPIO8, ON);
		/* Se apaga el sistema y queda a la espera de un nuevo llamado */
		system_access->system_flag = FALSE;
		uart_struct.byte_ring = 'R';
		/* Se deshabilitan las interrupciones de teclas */
		disable_buttons_interrupt();
		break;
	case WRONG:
		gpioWrite(LED1, OFF);
		gpioWrite(LEDB, OFF);
		gpioWrite(LED2, ON);
		gpioWrite(LED3, OFF);
		/* Apaga relay */
		gpioWrite(GPIO8, ON);
		/* Se apaga el sistema y queda a la espera de un nuevo llamado */
		system_access->system_flag = FALSE;
		uart_struct.byte_ring = 'R';
		/* Se deshabilitan las interrupciones de teclas */
		disable_buttons_interrupt();
		break;
	default:
		break;
	}
}
/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/


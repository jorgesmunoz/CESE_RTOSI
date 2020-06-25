/*=============================================================================
 * Copyright (c) 2020, jorge <jorgesmunoz@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/06/21
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __USER_TASKS_H__
#define __USER_TASKS_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include "main.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/
enum Teclas_t {Tecla1, Tecla2, Tecla3}; //indices de teclas para el vector de estructuras
#define CANT_TECLAS 3
#define CANT_LEDS 4
#define ANTIREBOTE_MS 20

typedef enum {ACTIVATED, OPEN, CLOSED, WRONG} system_state_t;

/* Definicion de estructura de sistema de acceso */
typedef struct{
	SemaphoreHandle_t sem_system;
	uint8_t system_flag;
	system_state_t system_status;
}system_control_t;

/* Estructura para sistema de acceso */
system_control_t system_access;

/* Definicion de estructura de maquina de estados de teclas */
typedef struct {
	uint8_t str_button;
	uint8_t state_button;
	xQueueHandle queue_button;
}button_str_t;

/* Definicion de estructura que guarda la tecla presionada */
typedef struct {
	uint8_t str_button;
}read_tecla;

/* Definicion para los flancos de las teclas */
typedef struct { //estructura de control de datos capturados por la interrupciÃ³n
	uint8_t flank;
}button_control;

/* Estructura para guardar tecla presionada */
read_tecla tec_save;
/* Estructura para teclas */
button_str_t tec_config[CANT_TECLAS];
/* Estructura para flancos antirebote */
button_control flank_capture;

/* Queue para lcd y led */
xQueueHandle lcd_queue;
xQueueHandle led_queue;

/* Funcion de estado del sistema */
void system_status(system_control_t *system_access);

/* Declaracion de tareas */
void led_task( void* taskParmPtr );  // Task declaration
void uart_task( void* taskParmPtr );
void lcd_task( void* taskParmPtr );
void system_task( void* taskParmPtr );
void tecla_task( void* taskParmPtr );
void first_init(void);
void _task_init( button_str_t * tec_str_config );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __USER_TASKS_H__ */

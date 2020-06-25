/*
 * irq_button_handler.c
 *
 *  Created on: 21 jun. 2020
 *      Author: jorge
 */

#include "main.h"

void IRQ_handler_Init (){
	//Inicializamos las interrupciones (LPCopen)
	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	//Inicializamos de cada evento de interrupcion (LPCopen)
	// TEC1 FALL
	Chip_SCU_GPIOIntPinSel(0, 0, 4); 	//(Canal 0 a 7, Puerto GPIO, Pin GPIO)
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);//Se configura el canal para que se active por flanco
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);//Se configura para que el flanco sea el de bajada

	// TEC1 RISE
	Chip_SCU_GPIOIntPinSel(1, 0, 4);	//(Canal 0 a 7, Puerto GPIO, Pin GPIO)
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH1);//Se configura el canal para que se active por flanco
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH1);//En este caso el flanco es de subida

	// TEC2 FALL
	Chip_SCU_GPIOIntPinSel(2, 0, 8);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH2);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH2);

	// TEC1 RISE
	Chip_SCU_GPIOIntPinSel(3, 0, 8);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH3);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH3);

	// TEC3 FALL
	Chip_SCU_GPIOIntPinSel(4, 0, 9);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH4);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH4);

	// TEC1 RISE
	Chip_SCU_GPIOIntPinSel(5, 0, 9);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH5);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH5);


}

void enable_buttons_interrupt(void){

	//Una vez que se han configurado los eventos para cada canal de interrupcion
	//Se activan las interrupciones para que comiencen a llamar al handler
	NVIC_SetPriority(PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_SetPriority(PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_SetPriority(PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_SetPriority(PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(PIN_INT3_IRQn);
	NVIC_SetPriority(PIN_INT4_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(PIN_INT4_IRQn);
	NVIC_SetPriority(PIN_INT5_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(PIN_INT5_IRQn);

}

void disable_buttons_interrupt(void){
	//Una vez que se han configurado los eventos para cada canal de interrupcion
	//Se activan las interrupciones para que comiencen a llamar al handler
	NVIC_DisableIRQ(PIN_INT0_IRQn);
	NVIC_DisableIRQ(PIN_INT1_IRQn);
	NVIC_DisableIRQ(PIN_INT2_IRQn);
	NVIC_DisableIRQ(PIN_INT3_IRQn);
	NVIC_DisableIRQ(PIN_INT4_IRQn);
	NVIC_DisableIRQ(PIN_INT5_IRQn);
}

//Handler de interrupciones
void GPIO0_IRQHandler(void){


	BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable

	if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH0){ //Verificamos que la interrupciÃ³n es la esperada
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0); //Borramos el flag de interrupciÃ³n
		//codigo a ejecutar si ocurriÃ³ la interrupciÃ³n
		button_control button_capture;
		button_capture.flank = FALLING;
		xQueueSendFromISR( tec_config[0].queue_button, &button_capture, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIO1_IRQHandler(void){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH1){
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
		//codigo a ejecutar si ocurriÃ³ la interrupciÃ³n
		button_control button_capture;
		button_capture.flank = RISING;
		xQueueSendFromISR( tec_config[0].queue_button, &button_capture, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void GPIO2_IRQHandler(void){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable


	if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH2){ //Verificamos que la interrupciÃ³n es la esperada
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2); //Borramos el flag de interrupciÃ³n
		//codigo a ejecutar si ocurriÃ³ la interrupciÃ³n
		button_control button_capture;
		button_capture.flank = FALLING;
		xQueueSendFromISR( tec_config[Tecla2].queue_button, &button_capture, &xHigherPriorityTaskWoken );
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIO3_IRQHandler(void){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH3){
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
		//codigo a ejecutar si ocurriÃ³ la interrupciÃ³n
		button_control button_capture;
		button_capture.flank = RISING;
		xQueueSendFromISR( tec_config[Tecla2].queue_button, &button_capture, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIO4_IRQHandler(void){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable


	if (Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT) & PININTCH4){ //Verificamos que la interrupciÃ³n es la esperada
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH4); //Borramos el flag de interrupciÃ³n
		//codigo a ejecutar si ocurriÃ³ la interrupciÃ³n
		button_control button_capture;
		button_capture.flank = FALLING;
		xQueueSendFromISR( tec_config[Tecla3].queue_button, &button_capture, &xHigherPriorityTaskWoken );
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIO5_IRQHandler(void){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH5){
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH5);
		//codigo a ejecutar si ocurriÃ³ la interrupciÃ³n
		button_control button_capture;
		button_capture.flank = RISING;
		xQueueSendFromISR( tec_config[Tecla3].queue_button, &button_capture, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}





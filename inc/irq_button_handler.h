/*
 * irq_button_handler.h
 *
 *  Created on: 21 jun. 2020
 *      Author: jorge
 */

#ifndef PROGRAMS_PCSE_TP_INC_IRQ_BUTTON_HANDLER_H_
#define PROGRAMS_PCSE_TP_INC_IRQ_BUTTON_HANDLER_H_


void IRQ_handler_Init (void);
void enable_buttons_interrupt(void);
void disable_buttons_interrupt(void);

#endif /* PROGRAMS_PCSE_TP_INC_IRQ_BUTTON_HANDLER_H_ */

/*
 * interrupt.h
 *
 *  Created on: Oct 4, 2018
 *      Author: lstefana
 */

#ifndef SRC_INTERRUPT_H_
#define SRC_INTERRUPT_H_

#include "xintc.h"

#define XINTC_INT_SRC_TIMER 0

int intc_init(u16 device_id);
void intc_enable(u8 id);
void intc_disable(u8 id);

// Default isr for vio source.
void timer_isr(void);

// Delay routines.
void tdelay_ms(uint32_t t);
void tdelay_s(uint32_t t);

#endif /* SRC_INTERRUPT_H_ */

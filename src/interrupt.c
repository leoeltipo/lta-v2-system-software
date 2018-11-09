/*
 * interrupt.c
 *
 *  Created on: Oct 4, 2018
 *      Author: lstefana
 */

#include "interrupt.h"

// Interrupt controller instance.
XIntc intc_i;

// Global delay variable.
uint32_t interrupt_counter;

int intc_init(u16 device_id)
{
	int status;

	// Initialize interrupt controller.
	status = XIntc_Initialize(&intc_i, device_id);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Perform self-test.
	status = XIntc_SelfTest(&intc_i);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Connect handler for interrupt 0 (timer).
	status = XIntc_Connect(	&intc_i,
							XINTC_INT_SRC_TIMER,
							(XInterruptHandler)timer_isr,
							(void *)0);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Start interrupt controller.
	status = XIntc_Start(&intc_i, XIN_REAL_MODE);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Initialize the exception table.
	Xil_ExceptionInit();

	// Register the interrupt controller handler with the exception table.
	Xil_ExceptionRegisterHandler(	XIL_EXCEPTION_ID_INT,
									(Xil_ExceptionHandler)XIntc_InterruptHandler,
									&intc_i);

	// Enable exceptions.
	Xil_ExceptionEnable();

	// Disable interrupts.
	intc_disable(XINTC_INT_SRC_TIMER);

	return XST_SUCCESS;
}

void intc_enable(u8 id)
{
	XIntc_Enable(&intc_i, id);
}

void intc_disable(u8 id)
{
	XIntc_Disable(&intc_i, id);
}

void timer_isr(void)
{
	interrupt_counter--;
}

void tdelay_ms(uint32_t t)
{
	// Load global variable.
	interrupt_counter = t;

	// Enable timer interrupt.
	intc_enable(XINTC_INT_SRC_TIMER);

	// Wait for counter to finish.
	while (interrupt_counter);

	// Disable timer interrupt.
	intc_disable(XINTC_INT_SRC_TIMER);
}

void tdelay_s(uint32_t t)
{
	// Load global variable.
	interrupt_counter = 1000*t;

	// Enable timer interrupt.
	intc_enable(XINTC_INT_SRC_TIMER);

	// Wait for counter to finish.
	while (interrupt_counter);

	// Disable timer interrupt.
	intc_disable(XINTC_INT_SRC_TIMER);
}

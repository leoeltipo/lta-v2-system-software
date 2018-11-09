#include "uart.h"
#include "xuartlite.h"
#include "packer.h"
#include "adc.h"
#include <stdio.h>

XUartLite Uart;

int uart_init(uint16_t device_id)
{
	return XUartLite_Initialize(&Uart, device_id);
}


unsigned int uart_rcv(uint8_t *buf)
{
	/*
	 * Receive the buffer from the UartLite waiting til there's data by (block),
	 * if the specified number of bytes was not received successfully, then
	 * an error occurred.
	 */

	return XUartLite_Recv(&Uart, buf, 1);
}

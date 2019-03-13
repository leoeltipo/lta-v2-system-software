/*
 * gpio_root.c
 *
 *  Created on: Mar 12, 2019
 *      Author: lstefana
 */

#include <xgpio.h>

#include "gpio_root.h"
#include "uart.h"

// XGpio device driver variables.
XGpio gpio_root_i;

// Structure for password control
gpio_root_t gpio_root;

int gpio_root_init(uint32_t gpio_device_id)
{
	int ret;

	// Init gpio_leds_i structure.
	ret = XGpio_Initialize(&gpio_root_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be inputs.
	XGpio_SetDataDirection(&gpio_root_i, 1, 0xFFFFFFFF);

	// Initialize password structure.
	gpio_root.waitPass = 1;
	gpio_root.passOk = 0;

	return ret;
}

int gpio_root_sw(void)
{
	volatile u32 data = XGpio_DiscreteRead(&gpio_root_i, 1);
	return (data & GPIO_ROOT_SWITCH_MASK);
}

uint8_t gpio_root_get_waitPass()
{
	return gpio_root.waitPass;
}

void gpio_root_check_pass(char *pass)
{
	if (strcmp(pass,GPIO_ROOT_PASS) == 0)
	{
		gpio_root.waitPass = 0;
		gpio_root.passOk = 1;

		// Print menu.
		uart_printMenu();
	}
	else
	{
		print("Enter password: ");
	}
}

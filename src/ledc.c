/*
 * ledc.c
 *
 *  Created on: Aug 1, 2018
 *      Author: lstefana
 */

#include <stdint.h>
#include <xil_printf.h>
#include <xgpio.h>
//#include "sleep.h"
#include "interrupt.h"

#include "leds.h"

// XGpio device driver variables.
XGpio gpio_leds_i;

int gpio_leds_init(uint32_t gpio_device_id, leds_t *leds)
{
	int ret;

	// Init gpio_leds_i structure.
	ret = XGpio_Initialize(&gpio_leds_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * 0 LED0
	 * 1 LED1
	 * 2 LED2
	 * 3 LED3
	 * 4 LED4
	 *
	 * After Init function all LEDS off.
	 */

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_leds_i, 1, 0x0);

	// Initialize structure.
	strcpy(leds->leds_group.led0.name, "led0");
	leds->leds_group.led0.status = GPIO_LEDS_LED_OFF;
	leds->leds_group.led0.bit_position = GPIO_LEDS_LED0_POSITION;
	leds->leds_group.led0.min = GPIO_LEDS_LED_OFF;
	leds->leds_group.led0.max = GPIO_LEDS_LED_ON;

	strcpy(leds->leds_group.led1.name, "led1");
	leds->leds_group.led1.status = GPIO_LEDS_LED_OFF;
	leds->leds_group.led1.bit_position = GPIO_LEDS_LED1_POSITION;
	leds->leds_group.led1.min = GPIO_LEDS_LED_OFF;
	leds->leds_group.led1.max = GPIO_LEDS_LED_ON;

	strcpy(leds->leds_group.led2.name, "led2");
	leds->leds_group.led2.status = GPIO_LEDS_LED_OFF;
	leds->leds_group.led2.bit_position = GPIO_LEDS_LED2_POSITION;
	leds->leds_group.led2.min = GPIO_LEDS_LED_OFF;
	leds->leds_group.led2.max = GPIO_LEDS_LED_ON;

	strcpy(leds->leds_group.led3.name, "led3");
	leds->leds_group.led3.status = GPIO_LEDS_LED_OFF;
	leds->leds_group.led3.bit_position = GPIO_LEDS_LED3_POSITION;
	leds->leds_group.led3.min = GPIO_LEDS_LED_OFF;
	leds->leds_group.led3.max = GPIO_LEDS_LED_ON;

	strcpy(leds->leds_group.led4.name, "led4");
	leds->leds_group.led4.status = GPIO_LEDS_LED_OFF;
	leds->leds_group.led4.bit_position = GPIO_LEDS_LED4_POSITION;
	leds->leds_group.led4.min = GPIO_LEDS_LED_OFF;
	leds->leds_group.led4.max = GPIO_LEDS_LED_ON;

	strcpy(leds->leds_group.led5.name, "led5");
	leds->leds_group.led5.status = GPIO_LEDS_LED_OFF;
	leds->leds_group.led5.bit_position = GPIO_LEDS_LED5_POSITION;
	leds->leds_group.led5.min = GPIO_LEDS_LED_OFF;
	leds->leds_group.led5.max = GPIO_LEDS_LED_ON;

	// Blink leds.
	for (int i=0; i<3; i++)
	{
		gpio_leds_change_state(&(leds->leds_group.led0), NULL, 1);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led1), NULL, 1);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led2), NULL, 1);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led3), NULL, 1);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led4), NULL, 1);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led5), NULL, 1);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led0), NULL, 0);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led1), NULL, 0);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led2), NULL, 0);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led3), NULL, 0);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led4), NULL, 0);
		tdelay_ms(50);
		gpio_leds_change_state(&(leds->leds_group.led5), NULL, 0);
		tdelay_ms(50);
	}

	// Write values into hardware.
	leds->state = 0;

	// Channel A.
	gpio_leds_change_state(&(leds->leds_group.led0)	, &(leds->state), leds->leds_group.led0.status);
	gpio_leds_change_state(&(leds->leds_group.led1)	, &(leds->state), leds->leds_group.led1.status);
	gpio_leds_change_state(&(leds->leds_group.led2)	, &(leds->state), leds->leds_group.led2.status);
	gpio_leds_change_state(&(leds->leds_group.led3)	, &(leds->state), leds->leds_group.led3.status);
	gpio_leds_change_state(&(leds->leds_group.led4)	, &(leds->state), leds->leds_group.led4.status);
	gpio_leds_change_state(&(leds->leds_group.led5)	, &(leds->state), leds->leds_group.led5.status);

	return ret;
}

int gpio_leds_change_state(led_status_t *led, uint16_t *state, uint8_t new_status)
{
	// Update variable.
	led->status = new_status;

	if ( led->status == 0 )
	{
		*state &= ~(1 << led->bit_position);
	} else {

		*state |= (1 << led->bit_position);
	}

	// Write gpio register.
	XGpio_DiscreteWrite(&gpio_leds_i, 1, *state);

	return 0;
}


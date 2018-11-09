/*
 * leds.h
 *
 *  Created on: Aug 1, 2018
 *      Author: lstefana
 */

#ifndef SRC_LEDS_H_
#define SRC_LEDS_H_

#define GPIO_LEDS_LED0_POSITION		0
#define GPIO_LEDS_LED1_POSITION		1
#define GPIO_LEDS_LED2_POSITION		2
#define GPIO_LEDS_LED3_POSITION		3
#define GPIO_LEDS_LED4_POSITION		4
#define GPIO_LEDS_LED5_POSITION		5

#define GPIO_LEDS_LED_OFF	0
#define GPIO_LEDS_LED_ON	1

typedef struct {
	uint8_t status;
	uint8_t min;
	uint8_t max;
	uint8_t bit_position;
	char name[10];
} led_status_t;

typedef struct {
	led_status_t led0;
	led_status_t led1;
	led_status_t led2;
	led_status_t led3;
	led_status_t led4;
	led_status_t led5;
} led_group_status_t;

typedef struct {
	led_group_status_t leds_group;
	uint16_t state;
} leds_t;

int gpio_leds_init(uint32_t gpio_device_id, leds_t *leds);
int gpio_leds_change_state(led_status_t *led, uint16_t *state, uint8_t new_status);

#endif /* SRC_LEDS_H_ */

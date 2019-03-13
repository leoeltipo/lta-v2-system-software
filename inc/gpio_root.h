/*
 * gpio_root.h
 *
 *  Created on: Mar 12, 2019
 *      Author: lstefana
 */

#ifndef INC_GPIO_ROOT_H_
#define INC_GPIO_ROOT_H_

#include <stdint.h>

#define GPIO_ROOT_SWITCH_MASK	(1 << 0)

#define GPIO_ROOT_PASS	"lta-root"

typedef struct {
	uint8_t waitPass;
	uint8_t passOk;
} gpio_root_t;

int gpio_root_init(uint32_t gpio_device_id);
int gpio_root_sw(void);
uint8_t gpio_root_get_waitPass();
void gpio_root_check_pass(char *pass);


#endif /* INC_GPIO_ROOT_H_ */

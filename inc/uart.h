#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "defines.h"

int uart_init(uint16_t device_id);
unsigned int uart_rcv(uint8_t *buf);

#endif

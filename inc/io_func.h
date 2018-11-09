/*
 * stdio_func.h
 *
 *  Created on: Sep 11, 2018
 *      Author: lstefana
 *
 *      Support functions for printing.
 */

#ifndef SRC_IO_FUNC_H_
#define SRC_IO_FUNC_H_

#include "defines.h"

void io_init(system_state_t *sys);

void io_sprintf(char *str, char *fmt, ...);
void io_int2str(int32_t n, char *str);
void io_uint2str(uint32_t n, char *str);
void io_float2str(float n, char *str);

void mprint(const char *str);

#endif /* SRC_IO_FUNC_H_ */

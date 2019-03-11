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

#define IO_SPRINTF_BUFFER_LENGTH	256
#define IO_INT2STR_BUFFER_LENGTH	12
#define IO_UINT2STR_BUFFER_LENGTH	12
#define IO_FLOAT2STR_BUFFER_LENGTH	10

void io_init(system_state_t *sys);

void io_sprintf(char *str, char *fmt, ...);
void io_int2str(int32_t n, char *str);
void io_uint2str(uint32_t n, char *str);
void io_uint2hex(uint32_t n, char *str);
void io_float2str(float n, char *str);

void mprint(const char *str);

#endif /* SRC_IO_FUNC_H_ */

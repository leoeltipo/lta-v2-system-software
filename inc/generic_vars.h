/*
 * generic_vars.h
 *
 *  Created on: Aug 1, 2018
 *      Author: lstefana
 */

#include <stdint.h>

#ifndef SRC_GENERIC_VARS_H_
#define SRC_GENERIC_VARS_H_

#define GENERIC_VARS_ECHO_MIN	0
#define GENERIC_VARS_ECHO_MAX	1
#define GENERIC_VARS_ECHO_ON	1
#define GENERIC_VARS_ECHO_OFF	0

#define GENERIC_VARS_OUTETH_MIN	0
#define GENERIC_VARS_OUTETH_MAX	1
#define GENERIC_VARS_OUTETH_ON	1
#define GENERIC_VARS_OUTETH_OFF	0

// Generic variable structure.
typedef struct {
	float value;
	float min;
	float max;
	char name[10];
} generic_var_t;

typedef struct {
	generic_var_t echo;
	generic_var_t outeth;
} generic_vars_t;

int generic_vars_init(generic_vars_t *vars);
int generic_vars_change_value(generic_var_t *var, uint8_t value);

#endif /* SRC_GENERIC_VARS_H_ */

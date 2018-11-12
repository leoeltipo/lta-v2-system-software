/*
 * generic_vars.c
 *
 *  Created on: Aug 1, 2018
 *      Author: lstefana
 */
#include <string.h>

#include "generic_vars.h"
#include "io_func.h"

int generic_vars_init(generic_vars_t *vars)
{
	strcpy(vars->echo.name,"echo");
	vars->echo.min = GENERIC_VARS_ECHO_MIN;
	vars->echo.max = GENERIC_VARS_ECHO_MAX;
	vars->echo.value = GENERIC_VARS_ECHO_OFF;

	strcpy(vars->outeth.name, "outeth");
	vars->outeth.min = GENERIC_VARS_OUTETH_MIN;
	vars->outeth.max = GENERIC_VARS_OUTETH_MAX;
	vars->outeth.value = GENERIC_VARS_OUTETH_OFF;

	return 0;
}

int generic_vars_change_value(generic_var_t *var, uint8_t value)
{
	if (value >= var->min && value <= var->max)
	{
		var->value = (float)value;
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
 * exec.c
 *
 *  Created on: Aug 1, 2018
 *      Author: lstefana
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <xgpio.h>
#include <xil_printf.h>

#include "defines.h"
#include "interrupt.h"
#include "exec.h"
#include "ad5293.h"
#include "telemetry.h"
#include "smart_buffer.h"
#include "eth.h"
#include "io_func.h"

// For restoring clock values after inversion.
clk_group_status_t clocks_temp;

/* EXEC Functions
 *
 * These functions must be placed before exec_init as they are referenced but
 * no definition is included in the .h file. This is to avoid making these
 * functions accessible from other parts of the code.
 */

/*
 * Inversion routine for eliminating dark current on Skipper CCD.
 */
int ccd_erase(void *arg)
{
	mprint("######################################\r\n");
	mprint("### Entering in ccd_erase function ###\r\n");
	mprint("######################################\r\n");
	int status;
	system_state_t *sys = (system_state_t*)arg;

	/*
	 * Save clock voltage in clocks_temp structure.
	 * Set all clock voltages to 9.
	 */
	clk_status_t *clk_temp = (clk_status_t *) &clocks_temp;
	clk_status_t *clk = (clk_status_t *) &(sys->clks);
	int nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		(clk_temp+i)->value = (clk+i)->value;
		status = ad5371_set_voltage(clk+i, 9);
		if (status)
		{
			char str[60];
			io_sprintf(str, "ERROR: ccd_erase could not set %s to 9 V.\r\n",(clk+i)->name);
			mprint(str);
		}
	}

	/*
	 * Decrease VSUB
	 *
	 * MIN 7
	 * MAX Read from actual VSUB value.
	 * slope = 0.5/45ms = 11V/s
	 */

	float min = 7;
	float max = sys->biases.vsub.value;
	float vsub_temp = sys->biases.vsub.value;
	float step = 0.5;
	int n = (max-min)/step;

	float value = max;
	for (int i=0; i<n; i++)
	{
		status = ad5293_set_voltage(&(sys->biases.vsub), value);
		if (status)
		{
			char str[60];
			io_sprintf(str, "ERROR: ccd_erase could not set %s to %f.\r\n", (sys->biases.vsub.name), value);
			mprint(str);
		}
		value -= step;
		tdelay_ms(45);
	}

	// Disable vsub ldo.
	ad5293_sw_en(GPIO_LDO_VSUB, 0);

	// Wait 1s.
	tdelay_s(2);

	/*
	 * Restore clock voltages from clock_temp structure.
	 */
	clk_temp = (clk_status_t *) &clocks_temp;
	clk = (clk_status_t *) &(sys->clks);
	nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		(clk+i)->value = (clk_temp+i)->value;
	}

	// Set vsub to 7.
	status = ad5293_set_voltage(&(sys->biases.vsub), 7);

	// Enable vsub ldo.
	ad5293_sw_en(GPIO_LDO_VSUB, 1);

	/*
	 * Increase VSUB
	 *
	 * MIN 7
	 * MAX Old VSUB value (as set previous to running the inversion).
	 * slope = 0.5V/6666us = 75V/s.
	 */

	min = 7;
	max = vsub_temp;
	step = 0.5;
	n = (max-min)/step;
	uint8_t flag = 0;

	value = min;
	for (int i=0; i<n; i++)
	{
		status = ad5293_set_voltage(&(sys->biases.vsub), value);
		if (status)
		{
			char str[60];
			io_sprintf(str, "ERROR: ccd_erase could not set %s to %f.\r\n", (sys->biases.vsub.name), value);
			mprint(str);
		}
		value += step;
		tdelay_ms(7);

		// Check if voltage reached 10V.
		if (!flag)
		{
			if (sys->biases.vsub.value > 10)
			{
				flag = 1;
				// Enable clocks.
				for(int i = 0; i < nClocks; i++)
				{
					status = ad5371_set_voltage(clk+i, (clk+i)->value);
					if (status)
					{
						char str[60];
						io_sprintf(str, "ERROR: ccd_erase could not set %s to %f\r\n.", (clk+i)->name, (clk+i)->value);
						mprint(str);
					}
				}
			}
		}
	}

	return 0;
}

/*
 * All clocks go to -9. Used in conjunction with ccd_erase
 * to minimize dark current.
 * This function does not modify VSUB.
 */
int cdd_epurge(void *arg)
{
	mprint("#######################################\r\n");
	mprint("### Entering in cdd_epurge function ###\r\n");
	mprint("#######################################\r\n");
	int status;
	system_state_t *sys = (system_state_t*)arg;

	/*
	 * Save clock voltage in clocks_temp structure.
	 * Set all clock voltages to -9.
	 */
	clk_status_t *clk_temp = (clk_status_t *) &clocks_temp;
	clk_status_t *clk = (clk_status_t *) &(sys->clks);
	int nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		(clk_temp+i)->value = (clk+i)->value;
		status = ad5371_set_voltage(clk+i, -9);
		if (status)
		{
			char str[60];
			io_sprintf(str, "ERROR: ccd_epurge could not set %s to -9 V.\r\n",(clk+i)->name);
			mprint(str);
		}
	}

	tdelay_s(1);

	/*
	 * Restore clock voltages from clock_temp structure.
	 */
	clk_temp = (clk_status_t *) &clocks_temp;
	clk = (clk_status_t *) &(sys->clks);
	nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		status = ad5371_set_voltage(clk+i,(clk_temp+i)->value);
		if (status)
		{
			char str[60];
			io_sprintf(str, "ERROR: ccd_epurge could not set %s to %f.\r\n",(clk+i)->name, (clk_temp+i)->value);
			mprint(str);
		}
	}

	return 0;
}

/*
 * Function to disable VSUB LDO.
 * VSUB voltage should descend at a speed given by the external
 * RC time constant.
 */
int vsub_down(void *arg)
{
	mprint("######################################\r\n");
	mprint("### Entering in vsub_down function ###\r\n");
	mprint("######################################\r\n");
	int status;
	system_state_t *sys = (system_state_t*)arg;

	/*
	 * Decrease VSUB
	 *
	 * MIN 7
	 * MAX Read from actual VSUB value.
	 * slope = 0.5/45ms = 11V/s
	 */

	float min = 7;
	float max = sys->biases.vsub.value;
	float step = 0.5;
	int n = (max-min)/step;

	float value = max;
	for (int i=0; i<n; i++)
	{
		status = ad5293_set_voltage(&(sys->biases.vsub), max);
		if (status)
		{
			char str[60];
			io_sprintf(str, "ERROR: vsub_down could not set %s to %f.\r\n",sys->biases.vsub.name, max);
			mprint(str);
		}
		value -= step;
		tdelay_ms(45);
	}

	// Disable vsub ldo.
	ad5293_sw_en(GPIO_LDO_VSUB, 0);

	return 0;
}

// Init function.
int exec_init(exec_t *exec)
{
	// Initialize functions structure.

	strcpy(exec->ccd_erase.name,"ccd_erase");
	strcpy(exec->ccd_erase.description,"CCD erase routine for dark current minimization.");
	exec->ccd_erase.func = &ccd_erase;

	strcpy(exec->cdd_epurge.name,"ccd_epurge");
	strcpy(exec->cdd_epurge.description,"CCD epurge routine for dark current minimization.");
	exec->cdd_epurge.func = &cdd_epurge;

	strcpy(exec->vsub_down.name,"vsub_down");
	strcpy(exec->vsub_down.description,"Disables VSUB LDO regulator.");
	exec->vsub_down.func = &vsub_down;

	return 0;
}

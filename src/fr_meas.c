/*
 * fr_meas.c
 *
 *  Created on: Mar 28, 2019
 *      Author: lstefana
 */

#include "fr_meas.h"

void fr_meas_init(fr_meas_t *fr_meas)
{
	// Init register structure.
	fr_meas->fclk.value				= FR_MEAS_FCLK_DEFAULT;
	fr_meas->fclk.min				= FR_MEAS_FCLK_MIN;
	fr_meas->fclk.max				= FR_MEAS_FCLK_MAX;
	fr_meas->fclk.reg_offset		= FR_MEAS_FCLK_REG_OFFSET;
	fr_meas->fclk.reg_mask			= FR_MEAS_FCLK_REG_MASK;
	strcpy(fr_meas->fclk.name,"frFclk");

	fr_meas->fmeas.value			= FR_MEAS_FMEAS_DEFAULT;
	fr_meas->fmeas.min				= FR_MEAS_FMEAS_MIN;
	fr_meas->fmeas.max				= FR_MEAS_FMEAS_MAX;
	fr_meas->fmeas.reg_offset		= FR_MEAS_FMEAS_REG_OFFSET;
	fr_meas->fmeas.reg_mask			= FR_MEAS_FMEAS_REG_MASK;
	strcpy(fr_meas->fmeas.name,"frFmeas");

	// Write values to hardware.
	fr_meas_change_status(&(fr_meas->fclk), fr_meas->fclk.value);

	// Update value from hardware.
	fr_meas_update_reg(&(fr_meas->fmeas));
}

int fr_meas_change_status(fr_meas_status_t *reg, uint32_t value)
{
	if (strcmp(reg->name,"frMeas") == 0)
	{
		return -1;
	}
	else
	{
		if (value >= reg->min && value <= reg->max)
		{
			reg->value = value;
		} else {
			return -1;
		}

		FR_MEAS_mWriteReg(XPAR_FR_MEAS_0_BASEADDR, reg->reg_offset,(uint32_t) reg->value);

		return 0;
	}

}

int fr_meas_update_reg(fr_meas_status_t *reg)
{
	volatile uint32_t value = FR_MEAS_mReadReg(XPAR_FR_MEAS_0_BASEADDR, reg->reg_offset);
	reg->value = value & reg->reg_mask;

	return 0;
}


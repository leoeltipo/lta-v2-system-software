/*
 * sync_gen.c
 *
 *  Created on: Feb 27, 2019
 *      Author: lstefana
 */

#include "sync_gen.h"

void sync_gen_init(sync_gen_t *sync_gen)
{
	// Init register structure.
	sync_gen->stop.value 			= SYNC_GEN_STOP;
	sync_gen->stop.min 				= SYNC_GEN_STOP;
	sync_gen->stop.max 				= SYNC_GEN_START;
	sync_gen->stop.reg_offset 		= SYNC_GEN_STOP_REG_OFFSET;
	sync_gen->stop.reg_mask 		= SYNC_GEN_STOP_REG_MASK;
	strcpy(sync_gen->stop.name,"syncStop");

	sync_gen->delay.value 			= SYNC_GEN_DELAY_MIN;
	sync_gen->delay.min 			= SYNC_GEN_DELAY_MIN;
	sync_gen->delay.max 			= SYNC_GEN_DELAY_MAX;
	sync_gen->delay.reg_offset 		= SYNC_GEN_DELAY_REG_OFFSET;
	sync_gen->delay.reg_mask 		= SYNC_GEN_DELAY_REG_MASK;
	strcpy(sync_gen->delay.name,"syncDelay");

	// Write values to hardware.
	sync_gen_change_status(&(sync_gen->stop), sync_gen->stop.value);
	sync_gen_change_status(&(sync_gen->delay), sync_gen->delay.value);
}

int sync_gen_change_status(sync_gen_status_t *reg, uint16_t value)
{
	if (value >= reg->min && value <= reg->max)
		{
			reg->value = value;
		} else {
			return -1;
		}

	SYNC_GEN_mWriteReg(XPAR_SYNC_GEN_0_BASEADDR, reg->reg_offset,(uint32_t) reg->value);

	return 0;
}


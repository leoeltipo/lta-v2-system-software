/*
 * master_sel.c
 *
 *  Created on: Feb 25, 2019
 *      Author: lstefana
 */

#include "master_sel.h"

void master_sel_init(master_sel_t *master_sel)
{
	// Init register structure.
	master_sel->sel.value 				= 0;
	master_sel->sel.min 				= MASTER_SEL_IS_SLAVE;
	master_sel->sel.max 				= MASTER_SEL_IS_MASTER;
	master_sel->sel.reg_offset 			= MASTER_SEL_MST_SEL_REG_OFFSET;
	master_sel->sel.reg_mask 			= MASTER_SEL_MST_SEL_REG_MASK;
	strcpy(master_sel->sel.name,"isSlave");

	// Update value from hardware.
	master_sel_update_reg(&(master_sel->sel));
}

int master_sel_update_reg(master_sel_status_t *reg)
{
	volatile uint32_t value = MASTER_SEL_mReadReg(XPAR_MASTER_SEL_0_BASEADDR, reg->reg_offset);
	reg->value = value & reg->reg_mask;

	return 0;
}



/*
 * packer.c
 * @file packer.c
 * @brief Packer 1.01 driver.
 * @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <xil_printf.h>
#include "xil_io.h"
#include "packer.h"
#include "io_func.h"

// Variable for keeping track of registers.
//packer_regs_t packer_regs;

void packer_init(packer_sw_group_status_t *packer_sw)
{
	/*
	 * Packer reset state:
	 *
	 * SOURCE_REG	= 9	=> CDS_SEQ.
	 * START_REG	= 0 => Packer stopped.
	 */
	packer_sw->source.status = PACKER_TRSRC_CDS_SEQ;
	packer_sw->source.min = PACKER_TRSRC_RAW_CHA;
	packer_sw->source.max = PACKER_TRSRC_CDS_SEQ;
	packer_sw->source.reg_offset = PACKER_SOURCE_REG_OFFSET;
	packer_sw->source.reg_mask = PACKER_SOURCE_REG_MASK;
	strcpy(packer_sw->source.name,"packSource");

	packer_sw->start.status = PACKER_START_OFF;
	packer_sw->start.min = PACKER_START_OFF;
	packer_sw->start.max = PACKER_START_ON;
	packer_sw->start.reg_offset = PACKER_START_REG_OFFSET;
	packer_sw->start.reg_mask = PACKER_START_REG_MASK;
	strcpy(packer_sw->start.name,"packStart");

	PACKER_mWriteReg(XPAR_PACKER_BASEADDR, packer_sw->source.reg_offset, 		packer_sw->source.status);
	PACKER_mWriteReg(XPAR_PACKER_BASEADDR, packer_sw->start.reg_offset, 		packer_sw->start.status);
}

int packer_change_sw_status(packer_sw_status_t *packer_sw_status, uint8_t value)
{
	if (value >= packer_sw_status->min && value <= packer_sw_status->max)
	{
		packer_sw_status->status = value;
	}
	else
	{
		return -1;
	}

	PACKER_mWriteReg(XPAR_PACKER_BASEADDR, packer_sw_status->reg_offset,(uint32_t) packer_sw_status->status);

	return 0;
}

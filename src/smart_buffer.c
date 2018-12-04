/*
 * smart_buffer.c
 *
 *  Created on: Aug 8, 2018
 *      Author: lstefana
 */
#include <stdint.h>
#include <xil_printf.h>
#include "xil_io.h"

#include "smart_buffer.h"
#include "io_func.h"

void smart_buffer_init(smart_buffer_group_status_t *smart_buffer)
{

	smart_buffer->cha_sel.value 				= SMART_BUFFER_CHX_SEL_CHA;
	smart_buffer->cha_sel.min 					= SMART_BUFFER_CHX_SEL_CHA;
	smart_buffer->cha_sel.max 					= SMART_BUFFER_CHX_SEL_CHD;
	smart_buffer->cha_sel.reg_offset 			= SMART_BUFFER_CHA_SEL_REG_OFFSET;
	smart_buffer->cha_sel.reg_mask 				= SMART_BUFFER_CHA_SEL_REG_MASK;
	strcpy(smart_buffer->cha_sel.name,"bufASel");

	smart_buffer->chb_sel.value 				= SMART_BUFFER_CHX_SEL_CHB;
	smart_buffer->chb_sel.min 					= SMART_BUFFER_CHX_SEL_CHA;
	smart_buffer->chb_sel.max 					= SMART_BUFFER_CHX_SEL_CHD;
	smart_buffer->chb_sel.reg_offset 			= SMART_BUFFER_CHB_SEL_REG_OFFSET;
	smart_buffer->chb_sel.reg_mask 				= SMART_BUFFER_CHB_SEL_REG_MASK;
	strcpy(smart_buffer->chb_sel.name,"bufBSel");

	smart_buffer->chc_sel.value 				= SMART_BUFFER_CHX_SEL_CHC;
	smart_buffer->chc_sel.min 					= SMART_BUFFER_CHX_SEL_CHA;
	smart_buffer->chc_sel.max 					= SMART_BUFFER_CHX_SEL_CHD;
	smart_buffer->chc_sel.reg_offset 			= SMART_BUFFER_CHC_SEL_REG_OFFSET;
	smart_buffer->chc_sel.reg_mask 				= SMART_BUFFER_CHC_SEL_REG_MASK;
	strcpy(smart_buffer->chc_sel.name,"bufCSel");

	smart_buffer->chd_sel.value 				= SMART_BUFFER_CHX_SEL_CHD;
	smart_buffer->chd_sel.min 					= SMART_BUFFER_CHX_SEL_CHA;
	smart_buffer->chd_sel.max 					= SMART_BUFFER_CHX_SEL_CHD;
	smart_buffer->chd_sel.reg_offset 			= SMART_BUFFER_CHD_SEL_REG_OFFSET;
	smart_buffer->chd_sel.reg_mask 				= SMART_BUFFER_CHD_SEL_REG_MASK;
	strcpy(smart_buffer->chd_sel.name,"bufDSel");

	smart_buffer->cha_nsamp.value 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->cha_nsamp.min 				= SMART_BUFFER_NSAMP_MIN;
	smart_buffer->cha_nsamp.max 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->cha_nsamp.reg_offset 			= SMART_BUFFER_CHA_NSAMP_REG_OFFSET;
	smart_buffer->cha_nsamp.reg_mask 			= SMART_BUFFER_CHA_NSAMP_REG_MASK;
	strcpy(smart_buffer->cha_nsamp.name,"bufASamp");

	smart_buffer->chb_nsamp.value 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->chb_nsamp.min 				= SMART_BUFFER_NSAMP_MIN;
	smart_buffer->chb_nsamp.max 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->chb_nsamp.reg_offset 			= SMART_BUFFER_CHB_NSAMP_REG_OFFSET;
	smart_buffer->chb_nsamp.reg_mask 			= SMART_BUFFER_CHB_NSAMP_REG_MASK;
	strcpy(smart_buffer->chb_nsamp.name,"bufBSamp");

	smart_buffer->chc_nsamp.value 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->chc_nsamp.min 				= SMART_BUFFER_NSAMP_MIN;
	smart_buffer->chc_nsamp.max 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->chc_nsamp.reg_offset 			= SMART_BUFFER_CHC_NSAMP_REG_OFFSET;
	smart_buffer->chc_nsamp.reg_mask 			= SMART_BUFFER_CHC_NSAMP_REG_MASK;
	strcpy(smart_buffer->chc_nsamp.name,"bufCSamp");

	smart_buffer->chd_nsamp.value 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->chd_nsamp.min 				= SMART_BUFFER_NSAMP_MIN;
	smart_buffer->chd_nsamp.max 				= SMART_BUFFER_NSAMP_MAX;
	smart_buffer->chd_nsamp.reg_offset 			= SMART_BUFFER_CHD_NSAMP_REG_OFFSET;
	smart_buffer->chd_nsamp.reg_mask 			= SMART_BUFFER_CHD_NSAMP_REG_MASK;
	strcpy(smart_buffer->chd_nsamp.name,"bufDSamp");

	smart_buffer->ch_mode.value 				= SMART_BUFFER_CH_MODE_SINGLE;
	smart_buffer->ch_mode.min 					= SMART_BUFFER_CH_MODE_SINGLE;
	smart_buffer->ch_mode.max 					= SMART_BUFFER_CH_MODE_QUAD;
	smart_buffer->ch_mode.reg_offset 			= SMART_BUFFER_CH_MODE_REG_OFFSET;
	smart_buffer->ch_mode.reg_mask 				= SMART_BUFFER_CH_MODE_REG_MASK;
	strcpy(smart_buffer->ch_mode.name,"bufChMode");

	smart_buffer->dataa_mode.value 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->dataa_mode.min 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->dataa_mode.max 				= SMART_BUFFER_DATAX_MODE_NSAMP;
	smart_buffer->dataa_mode.reg_offset 		= SMART_BUFFER_DATAA_MODE_REG_OFFSET;
	smart_buffer->dataa_mode.reg_mask 			= SMART_BUFFER_DATAA_MODE_REG_MASK;
	strcpy(smart_buffer->dataa_mode.name,"bufAMode");

	smart_buffer->datab_mode.value 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->datab_mode.min 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->datab_mode.max 				= SMART_BUFFER_DATAX_MODE_NSAMP;
	smart_buffer->datab_mode.reg_offset 		= SMART_BUFFER_DATAB_MODE_REG_OFFSET;
	smart_buffer->datab_mode.reg_mask 			= SMART_BUFFER_DATAB_MODE_REG_MASK;
	strcpy(smart_buffer->datab_mode.name,"bufBMode");

	smart_buffer->datac_mode.value 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->datac_mode.min 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->datac_mode.max 				= SMART_BUFFER_DATAX_MODE_NSAMP;
	smart_buffer->datac_mode.reg_offset 		= SMART_BUFFER_DATAC_MODE_REG_OFFSET;
	smart_buffer->datac_mode.reg_mask 			= SMART_BUFFER_DATAC_MODE_REG_MASK;
	strcpy(smart_buffer->datac_mode.name,"bufCMode");

	smart_buffer->datad_mode.value 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->datad_mode.min 				= SMART_BUFFER_DATAX_MODE_FULL;
	smart_buffer->datad_mode.max 				= SMART_BUFFER_DATAX_MODE_NSAMP;
	smart_buffer->datad_mode.reg_offset 		= SMART_BUFFER_DATAD_MODE_REG_OFFSET;
	smart_buffer->datad_mode.reg_mask 			= SMART_BUFFER_DATAD_MODE_REG_MASK;
	strcpy(smart_buffer->datad_mode.name,"bufDMode");

	smart_buffer->capture_mode.value 			= SMART_BUFFER_CAPTURE_MODE_SINGLE;
	smart_buffer->capture_mode.min 				= SMART_BUFFER_CAPTURE_MODE_SINGLE;
	smart_buffer->capture_mode.max 				= SMART_BUFFER_CAPTURE_MODE_CONTINUOUS;
	smart_buffer->capture_mode.reg_offset 		= SMART_BUFFER_CAPTURE_MODE_REG_OFFSET;
	smart_buffer->capture_mode.reg_mask 		= SMART_BUFFER_CAPTURE_MODE_REG_MASK;
	strcpy(smart_buffer->capture_mode.name,"bufCapMode");

	smart_buffer->capture_en_src.value 			= SMART_BUFFER_CAPTURE_EN_SRC_INTERNAL;
	smart_buffer->capture_en_src.min 			= SMART_BUFFER_CAPTURE_EN_SRC_EXTERNAL;
	smart_buffer->capture_en_src.max 			= SMART_BUFFER_CAPTURE_EN_SRC_INTERNAL;
	smart_buffer->capture_en_src.reg_offset 	= SMART_BUFFER_CAPTURE_EN_SRC_REG_OFFSET;
	smart_buffer->capture_en_src.reg_mask 		= SMART_BUFFER_CAPTURE_EN_SRC_REG_MASK;
	strcpy(smart_buffer->capture_en_src.name,"bufCapSrc");

	smart_buffer->capture_start.value 			= SMART_BUFFER_CAPTURE_STOP;
	smart_buffer->capture_start.min 			= SMART_BUFFER_CAPTURE_STOP;
	smart_buffer->capture_start.max 			= SMART_BUFFER_CAPTURE_START;
	smart_buffer->capture_start.reg_offset 		= SMART_BUFFER_CAPTURE_START_REG_OFFSET;
	smart_buffer->capture_start.reg_mask 		= SMART_BUFFER_CAPTURE_START_REG_MASK;
	strcpy(smart_buffer->capture_start.name,"bufCapStart");

	smart_buffer->capture_end.value 			= 0;
	smart_buffer->capture_end.min 				= SMART_BUFFER_CAPTURE_RUNNING;
	smart_buffer->capture_end.max 				= SMART_BUFFER_CAPTURE_FINISHED;
	smart_buffer->capture_end.reg_offset 		= SMART_BUFFER_CAPTURE_END_REG_OFFSET;
	smart_buffer->capture_end.reg_mask 			= SMART_BUFFER_CAPTURE_END_REG_MASK;
	strcpy(smart_buffer->capture_end.name,"bufCapEnd");

	smart_buffer->speed_ctrl.value 				= 1000;
	smart_buffer->speed_ctrl.min 				= SMART_BUFFER_SPEED_CTRL_MIN;
	smart_buffer->speed_ctrl.max 				= SMART_BUFFER_SPEED_CTRL_MAX;
	smart_buffer->speed_ctrl.reg_offset 		= SMART_BUFFER_SPEED_CTRL_REG_OFFSET;
	smart_buffer->speed_ctrl.reg_mask 			= SMART_BUFFER_SPEED_CTRL_REG_MASK;
	strcpy(smart_buffer->speed_ctrl.name,"bufSpeed");

	smart_buffer->transfer_start.value 			= SMART_BUFFER_TRNASFER_STOP;
	smart_buffer->transfer_start.min 			= SMART_BUFFER_TRNASFER_STOP;
	smart_buffer->transfer_start.max 			= SMART_BUFFER_TRANSFER_START;
	smart_buffer->transfer_start.reg_offset 	= SMART_BUFFER_TRANSFER_START_REG_OFFSET;
	smart_buffer->transfer_start.reg_mask 		= SMART_BUFFER_TRANSFER_START_REG_MASK;
	strcpy(smart_buffer->transfer_start.name,"bufTraStart");

	smart_buffer->transfer_end.value 			= 0;
	smart_buffer->transfer_end.min 				= SMART_BUFFER_TRANSFER_RUNNING;
	smart_buffer->transfer_end.max 				= SMART_BUFFER_TRANSFER_FINISHED;
	smart_buffer->transfer_end.reg_offset 		= SMART_BUFFER_TRANSFER_END_REG_OFFSET;
	smart_buffer->transfer_end.reg_mask 		= SMART_BUFFER_TRANSFER_END_REG_MASK;
	strcpy(smart_buffer->transfer_end.name,"bufTraEnd");

	// TODO: RESET_REG not implemented yet in hardware.
	smart_buffer->reset.value 					= SMART_BUFFER_RESET_OFF;
	smart_buffer->reset.min 					= SMART_BUFFER_RESET_OFF;
	smart_buffer->reset.max 					= SMART_BUFFER_RESET_ON;
	smart_buffer->reset.reg_offset 				= SMART_BUFFER_RESET_REG_OFFSET;
	smart_buffer->reset.reg_mask 				= SMART_BUFFER_RESET_REG_MASK;
	strcpy(smart_buffer->reset.name,"bufReset");

	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->cha_sel.reg_offset, 		smart_buffer->cha_sel.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->chb_sel.reg_offset, 		smart_buffer->chb_sel.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->chc_sel.reg_offset, 		smart_buffer->chc_sel.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->chd_sel.reg_offset, 		smart_buffer->chd_sel.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->cha_nsamp.reg_offset, 		smart_buffer->cha_nsamp.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->chb_nsamp.reg_offset, 		smart_buffer->chb_nsamp.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->chc_nsamp.reg_offset, 		smart_buffer->chc_nsamp.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->chd_nsamp.reg_offset, 		smart_buffer->chd_nsamp.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->ch_mode.reg_offset, 		smart_buffer->ch_mode.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->dataa_mode.reg_offset, 	smart_buffer->dataa_mode.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->datab_mode.reg_offset, 	smart_buffer->datab_mode.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->datac_mode.reg_offset,		smart_buffer->datac_mode.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->datad_mode.reg_offset, 	smart_buffer->datad_mode.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->capture_mode.reg_offset, 	smart_buffer->capture_mode.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->capture_en_src.reg_offset, smart_buffer->capture_en_src.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->capture_start.reg_offset, 	smart_buffer->capture_start.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->speed_ctrl.reg_offset, 	smart_buffer->speed_ctrl.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->transfer_start.reg_offset, smart_buffer->transfer_start.value);
	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->reset.reg_offset, 			smart_buffer->reset.value);

	// Read actual value of capture_end and transfer_end registers.
	volatile uint32_t value;
	value = SMART_BUFFER_mReadReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->capture_end.reg_offset);
	smart_buffer->capture_end.value = (uint16_t)value;
	value = SMART_BUFFER_mReadReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->transfer_end.reg_offset);
	smart_buffer->transfer_end.value = (uint16_t)value;

}

int smart_buffer_change_status(smart_buffer_status_t *reg, uint16_t value)
{
	if (value >= reg->min && value <= reg->max)
	{
		reg->value = value;
	} else {
		return -1;
	}

	SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, reg->reg_offset,(uint32_t) reg->value);

	return 0;
}

int smart_buffer_eoc(smart_buffer_group_status_t *smart_buffer)
{
	volatile uint32_t end = SMART_BUFFER_mReadReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->capture_end.reg_offset);

	// Check if the capture has finished.
	if(end)
	{
		// If it finished, STOP capture unit.
		smart_buffer->capture_start.value = SMART_BUFFER_CAPTURE_STOP;
		SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->capture_start.reg_offset, smart_buffer->capture_start.value);
		return 1;
	}
	else
	{
		// Not finished yet.
		return 0;
	}
}

int smart_buffer_eot(smart_buffer_group_status_t *smart_buffer)
{
	volatile uint32_t end = SMART_BUFFER_mReadReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->transfer_end.reg_offset);

	// Check if the transfer has finished.
	if(end)
	{
		// If it finished, STOP transfer unit.
		smart_buffer->transfer_start.value = SMART_BUFFER_TRNASFER_STOP;
		SMART_BUFFER_mWriteReg(XPAR_SMART_BUFFER_BASEADDR, smart_buffer->transfer_start.reg_offset, smart_buffer->transfer_start.value);
		return 1;
	}
	else
	{
		// Not finished yet.
		return 0;
	}

}

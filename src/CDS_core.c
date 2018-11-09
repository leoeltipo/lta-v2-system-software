

/***************************** Include Files *******************************/
#include <stdint.h>

#include "xil_io.h"
#include "CDS_core.h"
#include "io_func.h"

/************************** Function Definitions ***************************/
int cds_core_init(cds_var_group_status_t *cds_var_group)
{
	cds_var_group->delay_p.value = CDS_CORE_DELAY_P_DEFAULT;
	cds_var_group->delay_p.min = CDS_CORE_DELAY_MIN;
	cds_var_group->delay_p.max = CDS_CORE_DELAY_MAX;
	cds_var_group->delay_p.reg_offset = CDS_CORE_DELAY_P_OFFSET;
	cds_var_group->delay_p.reg_mask = CDS_CORE_DELAY_P_MASK;
	cds_var_group->delay_p.nbits = CDS_CORE_DELAY_P_NUM_BITS;
	strcpy(cds_var_group->delay_p.name,"pinit");

	cds_var_group->delay_s.value = CDS_CORE_DELAY_S_DEFAULT;
	cds_var_group->delay_s.min = CDS_CORE_DELAY_MIN;
	cds_var_group->delay_s.max = CDS_CORE_DELAY_MAX;
	cds_var_group->delay_s.reg_offset = CDS_CORE_DELAY_S_OFFSET;
	cds_var_group->delay_s.reg_mask = CDS_CORE_DELAY_S_MASK;
	cds_var_group->delay_s.nbits = CDS_CORE_DELAY_S_NUM_BITS;
	strcpy(cds_var_group->delay_s.name,"sinit");

	cds_var_group->sample_p.value = CDS_CORE_SAMPLES_P_DEFAULT;
	cds_var_group->sample_p.min = CDS_CORE_SAMPLE_MIN;
	cds_var_group->sample_p.max = CDS_CORE_SAMPLE_MAX;
	cds_var_group->sample_p.reg_offset = CDS_CORE_SAMPLE_P_OFFSET;
	cds_var_group->sample_p.reg_mask = CDS_CORE_SAMPLE_P_MASK;
	cds_var_group->sample_p.nbits = CDS_CORE_SAMPLE_P_NUM_BITS;
	strcpy(cds_var_group->sample_p.name,"psamp");

	cds_var_group->sample_s.value = CDS_CORE_SAMPLES_S_DEFAULT;
	cds_var_group->sample_s.min = CDS_CORE_SAMPLE_MIN;
	cds_var_group->sample_s.max = CDS_CORE_SAMPLE_MAX;
	cds_var_group->sample_s.reg_offset = CDS_CORE_SAMPLE_S_OFFSET;
	cds_var_group->sample_s.reg_mask = CDS_CORE_SAMPLE_S_MASK;
	cds_var_group->sample_s.nbits = CDS_CORE_SAMPLE_S_NUM_BITS;
	strcpy(cds_var_group->sample_s.name,"ssamp");

	cds_var_group->outsel.value = CDS_CORE_OUTSEL_DEFAULT;
	cds_var_group->outsel.min = CDS_CORE_OUTSEL_PED;
	cds_var_group->outsel.max = CDS_CORE_OUTSEL_PED_m_SIG;
	cds_var_group->outsel.reg_offset = CDS_CORE_OUTSEL_OFFSET;
	cds_var_group->outsel.reg_mask = CDS_CORE_OUTSEL_MASK;
	cds_var_group->outsel.nbits = CDS_CORE_OUTSEL_NUM_BITS;
	strcpy(cds_var_group->outsel.name,"cdsout");

	//put default values in hardware
	cds_core_change_var_value(&(cds_var_group->delay_p),1);
	cds_core_change_var_value(&(cds_var_group->delay_s),1);
	cds_core_change_var_value(&(cds_var_group->sample_p),1);
	cds_core_change_var_value(&(cds_var_group->sample_s),1);
	cds_core_change_var_value(&(cds_var_group->outsel),2);

	return 0;
}

int cds_core_change_var_value(cds_var_status_t *cds_var, const uint16_t value)
{


	if (value >= cds_var->min && value <= cds_var->max)
	{
		cds_var->value = value;
	}
	else
	{
		return -1;
	}

	CDS_CORE_mWriteReg(XPAR_CDS_CORE_A_BASEADDR, cds_var->reg_offset,(uint32_t) cds_var->value);
	CDS_CORE_mWriteReg(XPAR_CDS_CORE_B_BASEADDR, cds_var->reg_offset,(uint32_t) cds_var->value);
	CDS_CORE_mWriteReg(XPAR_CDS_CORE_C_BASEADDR, cds_var->reg_offset,(uint32_t) cds_var->value);
	CDS_CORE_mWriteReg(XPAR_CDS_CORE_D_BASEADDR, cds_var->reg_offset,(uint32_t) cds_var->value);
	return 0;
}

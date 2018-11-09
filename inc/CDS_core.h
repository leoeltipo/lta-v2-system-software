/*
 * CDS_core.h
 * @file CDS_core.h
 * @brief CDS driver
 * @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 *
 * CDS core for computing Correlated Double Sampling algorithm.
 * The core is non-causal, which means that during the pedestal input samples
 * are stored on a internal buffer. Buffer length is 4096, 18-bit.
 *
 * After pedestal finishes, samples in the buffer are used to accumulate the
 * pedestal part.
 *
 * Signal samples are accumulated while they arrive and are not stored in any
 * buffer.
 *
 * Before accumulating the samples, a programmed number of samples can be
 * discarded. For the pedestal, these samples correspond to the newest samples
 * in time, i.e., the samples that arrived last into the buffer. For the signal,
 * the samples that are discarded are the samples that arrive first.
 *
 * Example:
 * DELAY_P = 1
 * SAMPLE_P = 3
 * DELAY_S = 2
 * SAMPLE_S = 4
 *
 * p[0] p[1] p[2] p[3] p[4] p[5] p[6] p[7] p[8] p[9] p[10]
 * s[0] s[1] s[2] s[3] s[4] s[5] s[6] s[7] s[8] ...........
 *
 * In this case, the oldest sample is p[0], while the newest is p[10]. The result
 * of the accumulator for the pedestal will be:
 *
 * acc_p = p[7] + p[8] + p[9]
 *
 * For the signal, the first sample is s[0], while the last or newest is s[8]. The
 * result of the accumulator for the signal will be:
 *
 * acc_s = s[2] + s[3] + s[4] + s[5]
 *
 * The result of the core is:
 *
 * cds_out = acc_s - acc_p
 *
 * NOTE: if the number of samples acquired during the pedestal are not enough to accumulate
 * the required number, less samples are accumulated. If N is the number of samples acquired,
 * Dp the delay and Np the number of samples to be accumulated, the actual number of accumulated
 * samples is:
 *
 * Np_actual = N - Dp
 *
 * This number can even be zero, in which case no samples are accumulated and the core does not
 * generate an output packet. In the other hand, if the number of acquired samples exceed the
 * buffer length, older samples are lost. This means that for very long acquisition time, the
 * buffer always will contain the newest 4096 samples.
 *
 * For the signal samples are accumulated as they arrive into the block. if the signal state
 * terminates, no more samples are accumulated. This can cause in less samples being accumulated
 * than expected. In the other hand, if the number of accumulated samples is reached and the
 * signal state continues, all extra samples are discarded.
 *
 * #################
 * ### Registers ###
 * #################
 * The are four registers to control the operation of the core:
 *
 * DELAY_P 	: number of samples discarded for the pedestal.
 * DELAY_S 	: number of samples discarded for the signal.
 * SAMPLE_P : number of samples accumulated for the pedestal.
 * SAMPLE_S : number of samples accumulated for the signal.
 * OUTSEL	: operation output selector.
 *
 * For output operation selection:
 * 00 : pedestal.
 * 01 : signal.
 * 10 : signal - pedestal.
 * 11 : pedestal - signal.
 */
#ifndef CDS_CORE_H
#define CDS_CORE_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"

#define CDS_CORE_DELAY_P_OFFSET			 0  // Register offset of pedestal samples discarded
#define CDS_CORE_DELAY_S_OFFSET			 4  // Register offset of signal samples discarded
#define CDS_CORE_SAMPLE_P_OFFSET		 8  // Register offset of pedestal used samples
#define CDS_CORE_SAMPLE_S_OFFSET		 12 // Register offset of signal used samples
#define CDS_CORE_OUTSEL_OFFSET			 16 // Register offset of output operation selection

#define CDS_CORE_DELAY_P_NUM_BITS        13
#define CDS_CORE_DELAY_S_NUM_BITS		 13
#define CDS_CORE_SAMPLE_P_NUM_BITS		 16
#define CDS_CORE_SAMPLE_S_NUM_BITS		 16
#define CDS_CORE_OUTSEL_NUM_BITS		 2

#define CDS_CORE_DELAY_P_MASK			 ((1<<CDS_CORE_DELAY_P_NUM_BITS)-1)  // Mask of pedestal samples discarded
#define CDS_CORE_DELAY_S_MASK			 ((1<<CDS_CORE_DELAY_S_NUM_BITS)-1)  // Mask of signal samples discarded
#define CDS_CORE_SAMPLE_P_MASK		 	 ((1<<CDS_CORE_SAMPLE_P_NUM_BITS)-1) // Mask of pedestal used samples
#define CDS_CORE_SAMPLE_S_MASK		 	 ((1<<CDS_CORE_SAMPLE_S_NUM_BITS)-1) // Mask of signal used samples
#define CDS_CORE_OUTSEL_MASK			 ((1<<CDS_CORE_OUTSEL_NUM_BITS)-1)

#define CDS_CORE_DELAY_MIN				0
#define CDS_CORE_DELAY_MAX				0x1FFF
#define CDS_CORE_SAMPLE_MIN				0
#define CDS_CORE_SAMPLE_MAX				0xFFFF

#define CDS_CORE_DELAY_P_DEFAULT        1
#define CDS_CORE_DELAY_S_DEFAULT		1
#define CDS_CORE_SAMPLES_P_DEFAULT		1
#define CDS_CORE_SAMPLES_S_DEFAULT		1
#define CDS_CORE_OUTSEL_DEFAULT			2

#define CDS_CORE_OUTSEL_PED				0
#define CDS_CORE_OUTSEL_SIG				1
#define CDS_CORE_OUTSEL_SIG_m_PED		2
#define CDS_CORE_OUTSEL_PED_m_SIG		3


typedef struct {
	uint16_t value;
	uint16_t min;
	uint16_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	uint32_t nbits;
	char name[10];
}cds_var_status_t;

typedef struct {
	cds_var_status_t delay_p;
	cds_var_status_t delay_s;
	cds_var_status_t sample_p;
	cds_var_status_t sample_s;
	cds_var_status_t outsel;
}cds_var_group_status_t;

#define CDS_CORE_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define CDS_CORE_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

int cds_core_init(cds_var_group_status_t *cds_var_group);
int cds_core_change_var_value(cds_var_status_t *cds_var, const uint16_t value);


#endif // CDS_CORE_H

/*
 * fr_meas.h
 *
 *  Created on: Mar 28, 2019
 *      Author: lstefana
 */

#ifndef INC_FR_MEAS_H_
#define INC_FR_MEAS_H_

#include <xil_io.h>
#include <stdint.h>

#define FR_MEAS_FCLK_REG			0
#define FR_MEAS_FCLK_REG_OFFSET		0
#define FR_MEAS_FCLK_REG_MASK		0xFFFFFFFF
#define FR_MEAS_FMEAS_REG			1
#define FR_MEAS_FMEAS_REG_OFFSET	4
#define FR_MEAS_FMEAS_REG_MASK		0xFFFFFFFF

#define FR_MEAS_FCLK_MIN			0
#define FR_MEAS_FCLK_MAX			0xFFFFFFFF
#define FR_MEAS_FCLK_DEFAULT		100000

#define FR_MEAS_FMEAS_MIN			0
#define FR_MEAS_FMEAS_MAX			0xFFFFFFFF
#define FR_MEAS_FMEAS_DEFAULT		0

typedef struct {
	uint32_t value;
	uint32_t min;
	uint32_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	char name[15];
} fr_meas_status_t;

typedef struct {
	fr_meas_status_t fclk;
	fr_meas_status_t fmeas;
} fr_meas_t;

// Register read and write functions.
#define FR_MEAS_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define FR_MEAS_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

void fr_meas_init(fr_meas_t *fr_meas);
int fr_meas_change_status(fr_meas_status_t *reg, uint32_t value);
int fr_meas_update_reg(fr_meas_status_t *reg);


#endif /* INC_FR_MEAS_H_ */

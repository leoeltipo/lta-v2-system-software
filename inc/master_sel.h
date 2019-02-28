/*
 * master_sel.h
 *
 *  Created on: Feb 25, 2019
 *      Author: lstefana
 */

#ifndef INC_MASTER_SEL_H_
#define INC_MASTER_SEL_H_

#include <xil_io.h>

// Registers.
#define MASTER_SEL_MST_SEL_REG 				0
#define MASTER_SEL_MST_SEL_REG_OFFSET 		0
#define MASTER_SEL_MST_SEL_REG_MASK 		0x00000001

#define MASTER_SEL_IS_SLAVE		1
#define MASTER_SEL_IS_MASTER 	0

typedef struct {
	uint16_t value;
	uint16_t min;
	uint16_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	char name[15];
} master_sel_status_t;

typedef struct {
	master_sel_status_t sel;
} master_sel_t;

// Register read and write functions.
#define MASTER_SEL_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define MASTER_SEL_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))


void master_sel_init(master_sel_t *master_sel);
int master_sel_update_reg(master_sel_status_t *reg);

#endif /* INC_MASTER_SEL_H_ */

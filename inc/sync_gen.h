/*
 * sync_gen.h
 *
 *  Created on: Feb 27, 2019
 *      Author: lstefana
 */

#ifndef INC_SYNC_GEN_H_
#define INC_SYNC_GEN_H_

#include <xil_io.h>

// Registers.
#define SYNC_GEN_STOP_REG 			0
#define SYNC_GEN_STOP_REG_OFFSET 	0
#define SYNC_GEN_STOP_REG_MASK 		0x00000001

#define SYNC_GEN_DELAY_REG 			1
#define SYNC_GEN_DELAY_REG_OFFSET 	4
#define SYNC_GEN_DELAY_REG_MASK		0x000000FF

#define SYNC_GEN_STOP	0
#define SYNC_GEN_START	1

#define SYNC_GEN_DELAY_MIN	0
#define SYNC_GEN_DELAY_MAX	255

typedef struct {
	uint16_t value;
	uint16_t min;
	uint16_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	char name[15];
} sync_gen_status_t;

typedef struct {
	sync_gen_status_t stop;
	sync_gen_status_t delay;
} sync_gen_t;

// Register read and write functions.
#define SYNC_GEN_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define SYNC_GEN_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

void sync_gen_init(sync_gen_t *sync_gen);
int sync_gen_change_status(sync_gen_status_t *reg, uint16_t value);

#endif /* INC_SYNC_GEN_H_ */

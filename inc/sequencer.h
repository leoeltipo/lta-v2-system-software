/*
 * sequencer.h
 * @file sequencer.h
 * @brief Sequencer driver
 * @author Angel Soto (angel.soto@uns.edu.ar)
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#define SEQUENCER_STOP_SEQUENCE_OFFSET 	0
#define SEQUENCER_EOS_OFFSET 			4
#define SEQUENCER_WEA_OFFSET 			8
#define SEQUENCER_ADDR_OFFSET 			12
#define SEQUENCER_DATA_OFFSET 			16

#define SEQUENCER_STOP					0
#define SEQUENCER_START					1

#define SEQUENCER_MEMORY_SIZE			256
#define END_OF_SEQUENCER_INSTRUCTION    2147483648
#define DEFAULT_SEQUENCER 0

typedef struct {
	uint8_t status;
	uint8_t min;
	uint8_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	char name[10];
}seq_sw_status_t;

typedef struct {
	seq_sw_status_t start;
}seq_sw_group_status_t;

typedef struct {
	uint32_t program[SEQUENCER_MEMORY_SIZE];
	uint32_t size;
	char name[10];
}sequencer_t;

typedef struct {
	seq_sw_group_status_t sw_group;
	sequencer_t sequencer;
}seq_t;



/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a SEQUENCER register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the SEQUENCERdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void SEQUENCER_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define SEQUENCER_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a SEQUENCER register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the SEQUENCER device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 SEQUENCER_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define SEQUENCER_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))


int sequencer_init(seq_t *seq);
int sequencer_clear_program(seq_t *seq);
int sequencer_reset_program(seq_t *seq);
int sequencer_load_program(sequencer_t *sequencer);
int sequencer_change_program(seq_t *seq, unsigned int position, uint32_t value);
int seq_change_sw_status(seq_sw_status_t *seq_sw, uint8_t value);
void sequencer_reset();
void sequencer_start();
void sequencer_stop();
//int sequencer_eos();
int sequencer_eos(seq_t *seq);

#endif /* SEQUENCER_H_ */

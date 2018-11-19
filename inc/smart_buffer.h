/*
 * smart_buffer.h
 *
 *  Created on: Aug 8, 2018
 *      Author: lstefana
 *
 * Smart Buffer block for storing raw data coming from the 4 A/D video channels.
 *
 * The buffer has two major modes of operation: single capture or continuous mode. In
 * single capture mode, the number of samples to be captured for each channel can be
 * configured. In continuous mode, the buffer will store incoming samples in the
 * corresponding memory region while the CAPTURE_START_REG = 1. When this register is
 * set back to '0', capture ends and the stored data is kept until it is transfered.
 *
 * There are 4 independent capture channels, which means that 1, 2 or the 4 A/D video
 * signals can be captured simultaneously. The total buffer is divided in 2 or 4 for
 * these modes. Internal buffers are named A, B, C and D. When capturing a single
 * channel, buffer A will be used and can extend to the whole memory space (256K x 18).
 * When capturing two channels, buffers A and B will be used, and each buffer can store
 * half the memory space (128K x 18). When the all four channels are captured, buffers
 * A, B, C and D are used, each with a maximum of 1/4 the memory space (64K x 18).
 *
 * There are two registers that can be used as flags for detecting the end of capture
 * and the end of transfer stages. These can be accessed in the uBlaze address space.
 *
 * Once a capture is started, even if the transfer is also started, it will take place
 * after the current capture finishes. Both start signals can be set to 1 at the same
 * time. If data wants to be captured and sent later, it capture mode can be started
 * alone. Later on, transfer can be started to send the buffered data. It is important
 * to mention that a capture-transfer cycle must be completed. It means that the user
 * cannot launch multiple captures with different configuration without transferring
 * them. If the capture-transfer cycle is not completed, the register RESET_REG can be
 * used to reset the internal state machines and discard any captured data.
 *
 * *****************
 * *** Registers ***
 * *****************
 * There is a big list of registers that allow configuring the buffer:
 *
 * CHA_SEL_REG : 2 bits. Selects the A/D channel to be captured by buffer A.
 * 	-> 00 : A/D channel A.
 * 	-> 01 : A/D channel B.
 * 	-> 10 : A/D channel C.
 * 	-> 11 : A/D channel D.
 *
 * CHB_SEL_REG : 2 bits. Selects the A/D channel to be captured by buffer B.
 * 	-> 00 : A/D channel A.
 * 	-> 01 : A/D channel B.
 * 	-> 10 : A/D channel C.
 * 	-> 11 : A/D channel D.
 *
 * CHC_SEL_REG : 2 bits. Selects the A/D channel to be captured by buffer C.
 * 	-> 00 : A/D channel A.
 * 	-> 01 : A/D channel B.
 * 	-> 10 : A/D channel C.
 * 	-> 11 : A/D channel D.
 *
 * CHD_SEL_REG : 2 bits. Selects the A/D channel to be captured by buffer D.
 * 	-> 00 : A/D channel A.
 * 	-> 01 : A/D channel B.
 * 	-> 10 : A/D channel C.
 * 	-> 11 : A/D channel D.
 *
 * CHA_NSAMP_REG : 16 bits. Number of samples to be captured by buffer A.
 * CHB_NSAMP_REG : 16 bits. Number of samples to be captured by buffer B.
 * CHC_NSAMP_REG : 16 bits. Number of samples to be captured by buffer C.
 * CHD_NSAMP_REG : 16 bits. Number of samples to be captured by buffer D.
 *
 * CH_MODE_REG : 2 bits. Selects single, double or quad channel mode.
 * 	-> 00 : Single Channel.
 * 	-> 01 : Double Channel.
 * 	-> 10 : Quad Channel.
 * 	-> 00 : Not used (defaults to Single Channel).
 *
 * DATAA_MODE_REG : 1 bit. Select Full- or NSAMP-based capture for buffer A.
 * 	-> 0 : Full buffer capture.
 * 	-> 1 : NSAMP buffer capture.
 *
 * DATAB_MODE_REG : 1 bit. Select Full- or NSAMP-based capture for buffer B.
 * 	-> 0 : Full buffer capture.
 * 	-> 1 : NSAMP buffer capture.
 *
 * DATAC_MODE_REG : 1 bit. Select Full- or NSAMP-based capture for buffer C.
 * 	-> 0 : Full buffer capture.
 * 	-> 1 : NSAMP buffer capture.
 *
 * DATAD_MODE_REG : 1 bit. Select Full- or NSAMP-based capture for buffer D.
 * 	-> 0 : Full buffer capture.
 * 	-> 1 : NSAMP buffer capture.
 *
 * CAPTURE_MODE_REG : 1 bit. Selects single or continuous capture mode.
 * 	-> 0 : Single Capture.
 * 	-> 1 : Continuous Capture.
 *
 * CAPTURE_EN_SRC_REG : 1 bit.  External enable or always enabled capture.
 * 	-> 0 : external capture_en_chx pin enables/disables capture.
 * 	-> 1 : always capture data.
 *
 * CAPTURE_START_REG : 1 bit. Start/end the capture.
 * 	-> 0 : Capture doesn't start. End Capture in Continuous Capture mode.
 * 	-> 1 : Start the capture.
 *
 * CAPTURE_END_REG : 1 bit. Flag to indicate capture has finished.
 * 	-> 0 : Capture not finished yet.
 * 	-> 1 : Capture finished.
 *
 * SPEED_CTRL_REG : 16 bits. Divide main clock to slow down transfer.
 *
 * TRANSFER_START_REG : 1 bit. Start transfer.
 *	-> 0 : Transfer doesn't start.
 *	-> 1 : Start transfer (after capture finishes).
 *
 * TRANSFER_END_REG : 1 bit. Flag to indicate transfer has finished.
 *	-> 0 : Transfer not finished yet.
 *	-> 1 : Transfer finished.
 *
 * RESET_REG : 1 bit. Reset internal state.
 * 	-> 0 : Not in reset state.
 *	-> 1 : Reset block.
 *
 */

#ifndef SRC_SMART_BUFFER_H_
#define SRC_SMART_BUFFER_H_

// Registers.
#define SMART_BUFFER_CHA_SEL_REG 				0
#define SMART_BUFFER_CHA_SEL_REG_OFFSET 		0
#define SMART_BUFFER_CHA_SEL_REG_MASK 			0x00000003

#define SMART_BUFFER_CHB_SEL_REG 				1
#define SMART_BUFFER_CHB_SEL_REG_OFFSET 		4
#define SMART_BUFFER_CHB_SEL_REG_MASK 			0x00000003

#define SMART_BUFFER_CHC_SEL_REG 				2
#define SMART_BUFFER_CHC_SEL_REG_OFFSET 		8
#define SMART_BUFFER_CHC_SEL_REG_MASK 			0x00000003

#define SMART_BUFFER_CHD_SEL_REG 				3
#define SMART_BUFFER_CHD_SEL_REG_OFFSET 		12
#define SMART_BUFFER_CHD_SEL_REG_MASK 			0x00000003

#define SMART_BUFFER_CHA_NSAMP_REG				4
#define SMART_BUFFER_CHA_NSAMP_REG_OFFSET		16
#define SMART_BUFFER_CHA_NSAMP_REG_MASK			0x0000FFFF

#define SMART_BUFFER_CHB_NSAMP_REG				5
#define SMART_BUFFER_CHB_NSAMP_REG_OFFSET		20
#define SMART_BUFFER_CHB_NSAMP_REG_MASK			0x0000FFFF

#define SMART_BUFFER_CHC_NSAMP_REG				6
#define SMART_BUFFER_CHC_NSAMP_REG_OFFSET		24
#define SMART_BUFFER_CHC_NSAMP_REG_MASK			0x0000FFFF

#define SMART_BUFFER_CHD_NSAMP_REG				7
#define SMART_BUFFER_CHD_NSAMP_REG_OFFSET		28
#define SMART_BUFFER_CHD_NSAMP_REG_MASK			0x0000FFFF

#define SMART_BUFFER_CH_MODE_REG				8
#define SMART_BUFFER_CH_MODE_REG_OFFSET			32
#define SMART_BUFFER_CH_MODE_REG_MASK			0x00000003

#define SMART_BUFFER_DATAA_MODE_REG				9
#define SMART_BUFFER_DATAA_MODE_REG_OFFSET		36
#define SMART_BUFFER_DATAA_MODE_REG_MASK		0x00000001

#define SMART_BUFFER_DATAB_MODE_REG				10
#define SMART_BUFFER_DATAB_MODE_REG_OFFSET		40
#define SMART_BUFFER_DATAB_MODE_REG_MASK		0x00000001

#define SMART_BUFFER_DATAC_MODE_REG				11
#define SMART_BUFFER_DATAC_MODE_REG_OFFSET		44
#define SMART_BUFFER_DATAC_MODE_REG_MASK		0x00000001

#define SMART_BUFFER_DATAD_MODE_REG				12
#define SMART_BUFFER_DATAD_MODE_REG_OFFSET		48
#define SMART_BUFFER_DATAD_MODE_REG_MASK		0x00000001

#define SMART_BUFFER_CAPTURE_MODE_REG			13
#define SMART_BUFFER_CAPTURE_MODE_REG_OFFSET	52
#define SMART_BUFFER_CAPTURE_MODE_REG_MASK		0x00000001

#define SMART_BUFFER_CAPTURE_EN_SRC_REG			14
#define SMART_BUFFER_CAPTURE_EN_SRC_REG_OFFSET	56
#define SMART_BUFFER_CAPTURE_EN_SRC_REG_MASK	0x00000001

#define SMART_BUFFER_CAPTURE_START_REG			15
#define SMART_BUFFER_CAPTURE_START_REG_OFFSET	60
#define SMART_BUFFER_CAPTURE_START_REG_MASK		0x00000001

#define SMART_BUFFER_CAPTURE_END_REG			16
#define SMART_BUFFER_CAPTURE_END_REG_OFFSET		64
#define SMART_BUFFER_CAPTURE_END_REG_MASK		0x00000001

#define SMART_BUFFER_SPEED_CTRL_REG				17
#define SMART_BUFFER_SPEED_CTRL_REG_OFFSET		68
#define SMART_BUFFER_SPEED_CTRL_REG_MASK		0x0000FFFF

#define SMART_BUFFER_TRANSFER_START_REG			18
#define SMART_BUFFER_TRANSFER_START_REG_OFFSET	72
#define SMART_BUFFER_TRANSFER_START_REG_MASK	0x00000001

#define SMART_BUFFER_TRANSFER_END_REG			19
#define SMART_BUFFER_TRANSFER_END_REG_OFFSET	76
#define SMART_BUFFER_TRANSFER_END_REG_MASK		0x00000001

#define SMART_BUFFER_RESET_REG					20
#define SMART_BUFFER_RESET_REG_OFFSET			80
#define SMART_BUFFER_RESET_REG_MASK				0x00000001

// Defines for modes.
#define SMART_BUFFER_CHX_SEL_CHA 				0
#define SMART_BUFFER_CHX_SEL_CHB 				1
#define SMART_BUFFER_CHX_SEL_CHC 				2
#define SMART_BUFFER_CHX_SEL_CHD 				3

#define SMART_BUFFER_CH_MODE_SINGLE 			0
#define SMART_BUFFER_CH_MODE_DUAL 				1
#define SMART_BUFFER_CH_MODE_QUAD 				2

#define SMART_BUFFER_DATAX_MODE_FULL 			0
#define SMART_BUFFER_DATAX_MODE_NSAMP 			1

#define SMART_BUFFER_CAPTURE_MODE_SINGLE 		0
#define SMART_BUFFER_CAPTURE_MODE_CONTINUOUS	1

#define SMART_BUFFER_CAPTURE_EN_SRC_EXTERNAL 	0
#define SMART_BUFFER_CAPTURE_EN_SRC_INTERNAL 	1

#define SMART_BUFFER_CAPTURE_START 				1
#define SMART_BUFFER_CAPTURE_STOP 				0

#define SMART_BUFFER_CAPTURE_RUNNING 			0
#define SMART_BUFFER_CAPTURE_FINISHED 			1

#define SMART_BUFFER_TRANSFER_START 			1
#define SMART_BUFFER_TRNASFER_STOP 				0

#define SMART_BUFFER_TRANSFER_RUNNING 			0
#define SMART_BUFFER_TRANSFER_FINISHED 			1

#define SMART_BUFFER_NSAMP_MIN					0
#define SMART_BUFFER_NSAMP_MAX					65535

#define SMART_BUFFER_SPEED_CTRL_MIN				0
#define SMART_BUFFER_SPEED_CTRL_MAX				65535

#define SMART_BUFFER_RESET_OFF					0
#define SMART_BUFFER_RESET_ON					1

typedef struct {
	uint16_t value;
	uint16_t min;
	uint16_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	char name[15];
} smart_buffer_status_t;

typedef struct {
	smart_buffer_status_t cha_sel;
	smart_buffer_status_t chb_sel;
	smart_buffer_status_t chc_sel;
	smart_buffer_status_t chd_sel;
	smart_buffer_status_t cha_nsamp;
	smart_buffer_status_t chb_nsamp;
	smart_buffer_status_t chc_nsamp;
	smart_buffer_status_t chd_nsamp;
	smart_buffer_status_t ch_mode;
	smart_buffer_status_t dataa_mode;
	smart_buffer_status_t datab_mode;
	smart_buffer_status_t datac_mode;
	smart_buffer_status_t datad_mode;
	smart_buffer_status_t capture_mode;
	smart_buffer_status_t capture_en_src;
	smart_buffer_status_t capture_start;
	smart_buffer_status_t capture_end;
	smart_buffer_status_t speed_ctrl;
	smart_buffer_status_t transfer_start;
	smart_buffer_status_t transfer_end;
	smart_buffer_status_t reset;
} smart_buffer_group_status_t;

// Register read and write functions.
#define SMART_BUFFER_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define SMART_BUFFER_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

void smart_buffer_init(smart_buffer_group_status_t *smart_buffer);
int smart_buffer_change_status(smart_buffer_status_t *reg, uint16_t value);
int smart_buffer_eoc(smart_buffer_group_status_t *smart_buffer);
int smart_buffer_eot(smart_buffer_group_status_t *smart_buffer);

#endif /* SRC_SMART_BUFFER_H_ */

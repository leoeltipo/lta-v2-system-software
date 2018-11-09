/*
 * packer.h
 * @file packer.h
 * @brief Sequencer driver
 * @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 *
 * Packer Driver with 9 sources:
 *
 * Source 0 : RAW data channel A.
 * Source 1 : RAW data channel B.
 * Source 2 : RAW data channel C.
 * Source 3 : RAW data channel D.
 * Source 4 : RAW data from Smart Buffer.
 * Source 5 : CDS channel A.
 * Source 6 : CDS channel B.
 * Source 7 : CDS channel C.
 * Source 8 : CDS channel D.
 * Source 9 : CDS Sequential: A, B, C and D.
 *
 * ################
 * ### RAW Data ###
 * ################
 * In this mode, three consecutive samples of the selected channel are sent in
 * one 64-bit packet. Packet format is as follows:
 *
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 54 | 53 -- 0 |
 * Description:    | counter  | ch  id   | header   | 3x data |
 *
 * Channel ID:
 *	-> 0000 : A
 *	-> 0001 : B
 *	-> 0010 : C
 *	-> 0011 : D
 *
 * For the samples, the oldest sample is at the rightmost position of the packet.
 * Each sample is 18-bit. 
 *
 * ##################################	
 * ### RAW DATA FROM SMART BUFFER ###
 * ##################################
 * In this mode, RAW data from the Smart Buffer is sent in the 64-bit packet. The smart
 * buffer will automatically switch to the captured channels. Channel ID indicates the
 * origin of the RAW data.
 *
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 54 | 53 -- 18 | 17 -- 0 |
 * Description:    | counter  | ch  id   |  header  | 00000000 |  data   |
 *
 * Channel ID:
 * 	-> 0100 : A
 *	-> 0101 : B
 *	-> 0110 : C
 *	-> 0111 : D
 *
 * ###########
 * ### CDS ###
 * ###########
 * In this mode, one CDS output is sent per 64-bit packet. The packet format is
 * as follows:
 *
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 32 | 31 -- 0 |
 * Description:    | counter  | ch  id   | 00000000 |  data   |
 *
 * Channel ID:
 *	-> 1000 : A
 *	-> 1001 : B
 *	-> 1010 : C
 *	-> 1011 : D
 * 
 * ######################
 * ### CDS Sequential ###
 * ######################
 * In this mode, four 64-bit output packet are generated, each containing only
 * one CDS channel. Packet format is as follows:
 *
 * Packet 1
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 32 | 31 -- 0  |
 * Description:    | counter  | cha  id  | 00000000 | cha data |
 *
 * Packet 2
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 32 | 31 -- 0  |
 * Description:    | counter  | chb  id  | 00000000 | chb data |
 *
 * Packet 3
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 32 | 31 -- 0  |
 * Description:    | counter  | chc  id  | 00000000 | chc data |
 *
 * Packet 4
 * BIT # :         | 63 -- 60 | 59 -- 56 | 55 -- 32 | 31 -- 0  |
 * Description:    | counter  | chd  id  | 00000000 | chd data |
 *
 * Channel ID:
 * 	-> 1100 : A
 * 	-> 1101 : B
 *	-> 1110 : C
 *	-> 1111 : D
 *
 * ###############
 * ### Counter ###
 * ###############
 * For all modes of operation, the 4 most significant bits include a pakcet counter.
 * The purpose of this counter is to keep track of the 64-bit packet generated at
 * the packer side to check if all packet have been received. The counter increments
 * on each new created packet.
 *
 * #################
 * ### Registers ###
 * #################
 * SOURCE_REG  		: Selects the source.
 * START_REG		: Start/stop the packer. 
 *
 */

#ifndef PACKER_H_
#define PACKER_H_

#include <stdint.h>

#define PACKER_SOURCE_REG_OFFSET	0
#define PACKER_START_REG_OFFSET		4

#define PACKER_SOURCE_REG			0
#define PACKER_SOURCE_REG_MASK		0x0000000F

#define PACKER_START_REG			1
#define PACKER_START_REG_MASK		0x00000001


// Raw channels.
#define PACKER_TRSRC_RAW_CHA	0
#define PACKER_TRSRC_RAW_CHB	1
#define PACKER_TRSRC_RAW_CHC	2
#define PACKER_TRSRC_RAW_CHD	3
#define PACKER_TRSRC_RAW_SB		4

// CDS pixel data.
#define PACKER_TRSRC_CDS_CHA	5
#define PACKER_TRSRC_CDS_CHB	6
#define PACKER_TRSRC_CDS_CHC	7
#define PACKER_TRSRC_CDS_CHD	8
#define PACKER_TRSRC_CDS_SEQ	9

#define PACKER_START_ON			1
#define PACKER_START_OFF		0

typedef struct {
	uint8_t status;
	uint8_t min;
	uint8_t max;
	uint32_t reg_offset;
	uint32_t reg_mask;
	char name[12];
}packer_sw_status_t;

typedef struct {
	packer_sw_status_t source;
	packer_sw_status_t start;
}packer_sw_group_status_t;

/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a PACKER register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PACKER device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void PACKER_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define PACKER_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a PACKER register. A 32 bit read is performed.
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
 * 	u32 PACKER_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define PACKER_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

void packer_init(packer_sw_group_status_t *packer_sw);
int packer_change_sw_status(packer_sw_status_t *packer_sw_status, uint8_t value);

#endif // PACKER_H_

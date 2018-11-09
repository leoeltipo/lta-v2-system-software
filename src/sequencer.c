/*
 * sequencer.c
 * @file sequencer.c
 * @brief Sequencer 1.01 driver.
 * @author Angel Soto (angel.soto@uns.edu.ar)
 */


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <xil_printf.h>
#include "xil_io.h"
#include "sequencer.h"
#include "sequencer_samples.h"



int sequencer_init(seq_t *seq)
{
	//initialize sequencer status
	seq->sw_group.start.status = 0;
	seq->sw_group.start.min = 0;
	seq->sw_group.start.max = 1;
	seq->sw_group.start.reg_offset = SEQUENCER_STOP_SEQUENCE_OFFSET;
	strcpy(seq->sw_group.start.name,"seqStart");

	//initialize sequencer
	seq->sequencer.size = SEQUENCER_MEMORY_SIZE;
	strcpy(seq->sequencer.name,"sequencer");

	//bring default sequencer to RAM
	int status = 0;
	status = sequencer_reset_program(seq);

	// Load sequencer to hardware
	status = sequencer_load_program(&(seq->sequencer));

	return status;
}


int sequencer_clear_program(seq_t *seq)
{
	// Clean sequencer program.
	for (int i=0; i<SEQUENCER_MEMORY_SIZE;i++)
	{
		seq->sequencer.program[i] = END_OF_SEQUENCER_INSTRUCTION;
	}

	return 0;
}

int sequencer_reset_program(seq_t *seq)
{
	int status = 0;

	//clear program variable
	status = sequencer_clear_program(seq);

	//load sequencer to variable
	unsigned int seqInd = DEFAULT_SEQUENCER;
	strcpy(seq->sequencer.name,seqComment[seqInd]);

	uint32_t *program = (uint32_t *) seqPointer[seqInd];
	for (int i=0; i<seqSize[seqInd];i++)
	{
		seq->sequencer.program[i] = program[i];
	}

	return status;
}

int sequencer_load_program(sequencer_t *sequencer)
{
	//sequencer assignment to sequencer state machine
	uint32_t n = sequencer->size;

	if (n > SEQUENCER_MEMORY_SIZE) {
		return -1;
	}
	else
	{
		// Stop the sequencer
		SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_STOP_SEQUENCE_OFFSET, 0x0);

		// Load program.
		for (int i = 0; i < n; ++i) {
			SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_ADDR_OFFSET, (uint32_t)i);
			SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_DATA_OFFSET, sequencer->program[i]);
			SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_WEA_OFFSET, 0x00000001);
			SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_WEA_OFFSET, 0x00000000);
		}

		return 0;
	}
}

int sequencer_change_program(seq_t *seq, unsigned int position, uint32_t value)
{
	if (position >=0 && position <=SEQUENCER_MEMORY_SIZE)
	{
		seq->sequencer.program[position] = value;
	}
	else
	{
		return -1;
	}

	return 0;
}


int seq_change_sw_status(seq_sw_status_t *seq_sw, uint8_t value)
{
	if (value >= seq_sw->min && value <= seq_sw->max)
	{
		seq_sw->status = value;
	}
	else
	{
		return -1;
	}

	SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR, seq_sw->reg_offset,(uint32_t) seq_sw->status);

	return 0;
}

void sequencer_reset()
{
	SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_STOP_SEQUENCE_OFFSET, 0x0);
}

void sequencer_start()
{
	SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_STOP_SEQUENCE_OFFSET, 0x1);
}

void sequencer_stop()
{
	SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_STOP_SEQUENCE_OFFSET, 0x0);
}

int sequencer_eos(seq_t *seq)
{
	volatile uint32_t end = SEQUENCER_mReadReg(XPAR_SEQUENCER_BASEADDR, SEQUENCER_EOS_OFFSET);

	// Check if the sequence has finished.
	if(end)
	{
		// If it finished, stop the sequencer.
		SEQUENCER_mWriteReg(XPAR_SEQUENCER_BASEADDR,SEQUENCER_STOP_SEQUENCE_OFFSET, 0x0);
		//move sequencer start switch to 0
		seq_change_sw_status(&(seq->sw_group.start), 0);
		return 1;
	}
	else
	{
		// Not finished yet.
		return 0;
	}

}

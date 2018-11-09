/*
 * sequencer_samples.h
 * @file sequencer_samples.h
 * @brief Sequencer samples
 * @author Guillermo Fernandez Moroni (fmoroni.guillermo@gmail.com)
 */

#ifndef SEQUENCER_SAMPLES_H
#define SEQUENCER_SAMPLES_H

#include <stdint.h>
#include <xil_printf.h>
#include "xil_io.h"


#define MAX_NUMB_SEQUENCER 1

//variables for different sequencers
extern const uint32_t * seqPointer[];
extern const int seqSize[];
extern const char *seqComment[];


extern const uint32_t seq_both_amps_iw_1500[];
extern char comment_both_amps_iw_1500[];

extern const uint32_t seq_clean[];
extern char comment_clean[];

#endif /* SEQUENCER_SAMPLES_H_ */

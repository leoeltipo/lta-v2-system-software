/*
 * defines.h
 *
 *  Created on: Nov 15, 2017
 *      Author: ccds
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include "xparameters.h"
#include "packer.h"
#include "ad5293.h"
#include "ad5371.h"
#include "telemetry.h"
#include "max14802.h"
#include "sequencer.h"
#include "packer.h"
#include "adc.h"
#include "CDS_core.h"
#include "exec.h"
#include "generic_vars.h"
#include "leds.h"
#include "smart_buffer.h"
#include "eth.h"

#define SPI_LDO_VDRAIN_SLAVE_SELECT AD5293_SLAVE_SELECT_0
#define SPI_LDO_VDD_SLAVE_SELECT	AD5293_SLAVE_SELECT_1
#define SPI_LDO_VR_SLAVE_SELECT		AD5293_SLAVE_SELECT_2
#define SPI_LDO_VSUB_SLAVE_SELECT	AD5293_SLAVE_SELECT_3

// ASCII Characters.
#define ASCII_CHAR_BS 	8	// Back space.
#define ASCII_CHAR_CR 	13 	// Carriage return.
#define ASCII_CHAR_CAN 	24 	// Cancel.
#define ASCII_CHAR_EM 	25 	// End of Medium.
#define ASCII_CHAR_SUB 	26 	// Substitute.
#define ASCII_CHAR_ESC 	27 	// Escape.
#define ASCII_CHAR_FS 	28 	// File Separator.
#define ASCII_CHAR_DEL 	127 // Delete.

typedef struct {
	seq_t 						seq;
	packer_sw_group_status_t 	packer_sw;
	gpio_adc_t 					gpio_adc;
	gpio_sw_t 					gpio_sw;
	bias_sw_t 					bias_sw;
	clk_sw_t 					clk_sw;
	clk_group_status_t 			clks;
	bias_group_status_t 		biases;
	cds_var_group_status_t 		cds;
	telemetry_group_t 			telemetry;
	uint8_t 					go;
	exec_t 						exec;
	generic_vars_t 				generic_vars;
	leds_t						leds;
	smart_buffer_group_status_t	smart_buffer;
	eth_t						eth;
} system_state_t;


#endif /* DEFINES_H_ */

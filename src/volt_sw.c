/*
* @file volt_switch.c
* @brief Implementation of VOLT_SW Driver.
* @author Angel Soto (angel.soto@uns.edu.ar)
 */


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <xspi.h>
#include <xgpio.h>
#include "interrupt.h"
#include "volt_sw.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
// XSpi device driver variables.
XSpi_Config	*spi_volt_sw_cfg;
XSpi		spi_volt_sw_i;

// XGpio device driver variables.
XGpio gpio_volt_sw_i;

// Voltage Switch State variable.
//volt_sw_state_t volt_sw_state;

// Gpio State variable.
//gpio_volt_sw_state_t gpio_volt_sw;

/********************
* @brief volt_sw_init
*********************/
int volt_sw_init(uint32_t spi_device_id, uint32_t gpio_device_id, bias_sw_t *bias_sw, gpio_sw_t *gpio_sw)
{
	int ret;
	uint32_t base_addr		= 0;
	uint32_t control_val 	= 0;

	// Init spi_volt_sw_i structure.
	ret = XSpi_Initialize(&spi_volt_sw_i, spi_device_id);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	ret = XSpi_Stop(&spi_volt_sw_i);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Get device configuration from generated structure.
	spi_volt_sw_cfg = XSpi_LookupConfig(spi_device_id);

	// Get BaseAddress from config.
	base_addr = spi_volt_sw_cfg->BaseAddress;

	// Initialize hardware device.
	ret = XSpi_CfgInitialize(&spi_volt_sw_i, spi_volt_sw_cfg, base_addr);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Configure SPI options.
	control_val = 	XSP_MASTER_OPTION 			|
			XSP_CLK_ACTIVE_LOW_OPTION	|
			XSP_CLK_PHASE_1_OPTION		|
			XSP_MANUAL_SSELECT_OPTION;

	// Write options into hardware spi device.
	ret = XSpi_SetOptions(&spi_volt_sw_i, control_val);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Start the device.
	ret = XSpi_Start(&spi_volt_sw_i);

	// Disable interrupts for this device.
	XSpi_IntrGlobalDisable(&spi_volt_sw_i);

	// Init gpio_dac_i structure.
	ret = XGpio_Initialize(&gpio_volt_sw_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * 0 VOLT_SW_CLR
	 * 1 VOLT_SW_LE_N
	 *
	 * After Init function:
	 * 	VOLT_SW_CLR 	= 0	: clear the contents of latch and set al switches to open state.
	 * 	VOLT_SW_LE_N	= 1 : maintains the state of the switches. Pull low for updating.
	 *
	 */

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_volt_sw_i, 1, 0x0);

	// Set the GPIO outputs to high
	XGpio_DiscreteWrite(&gpio_volt_sw_i, 1, 0x3);

	// Set VOLT_SW_CLR = 0, VOLT_SW_LE_N = 1.
	XGpio_DiscreteWrite(&gpio_volt_sw_i, 1, 0x2);

	// Init gpio state structure.
	gpio_sw->sw_group.clr.status = 0;
	gpio_sw->sw_group.clr.bit_position = GPIO_VOLT_SW_CLR;
	strcpy(gpio_sw->sw_group.clr.name,"clr");

	gpio_sw->sw_group.le_n.status = 1;
	gpio_sw->sw_group.le_n.bit_position = GPIO_VOLT_SW_LE_N;
	strcpy(gpio_sw->sw_group.le_n.name,"le_n");

	gpio_sw->state = 0x2;

	// Set values to hardware.
	//VOLT_SW_volt_sw_state_set(&(gpio_sw->sw_group.clr)	, &(gpio_sw->state), gpio_sw->sw_group.clr.status);
	//VOLT_SW_volt_sw_state_set(&(gpio_sw->sw_group.le_n), &(gpio_sw->state), gpio_sw->sw_group.le_n.status);

	// Init switch state structure.
	bias_sw->sw_group.ccd_vdd_sw.status = 0;
	bias_sw->sw_group.ccd_vdd_sw.bit_position = VOLT_SW_CCD_VDD_SWITCH;
	strcpy(bias_sw->sw_group.ccd_vdd_sw.name,"vdd_sw");

	bias_sw->sw_group.ccd_vdrain_sw.status = 0;
	bias_sw->sw_group.ccd_vdrain_sw.bit_position = VOLT_SW_CCD_VDRAIN_SWITCH;
	strcpy(bias_sw->sw_group.ccd_vdrain_sw.name,"vdrain_sw");

	bias_sw->sw_group.ccd_vsub_sw.status = 0;
	bias_sw->sw_group.ccd_vsub_sw.bit_position = VOLT_SW_CCD_VSUB_SWITCH;
	strcpy(bias_sw->sw_group.ccd_vsub_sw.name,"vsub_sw");

	bias_sw->sw_group.vsub_load_sw.status = 0;
	bias_sw->sw_group.vsub_load_sw.bit_position = VOLT_SW_VSUB_LOAD_SWITCH;
	strcpy(bias_sw->sw_group.vsub_load_sw.name,"vsub_load_sw");

	bias_sw->sw_group.vsub_rdiv_sw.status = 0;
	bias_sw->sw_group.vsub_rdiv_sw.bit_position = VOLT_SW_VSUB_RDIV_SWITCH;
	strcpy(bias_sw->sw_group.vsub_rdiv_sw.name,"vsub_rdiv_sw");

	bias_sw->sw_group.ccd_vr_sw.status = 0;
	bias_sw->sw_group.ccd_vr_sw.bit_position = VOLT_SW_CCD_VR_SWITCH;
	strcpy(bias_sw->sw_group.ccd_vr_sw.name,"vr_sw");

	bias_sw->sw_group.p15v_sw.status = 0;
	bias_sw->sw_group.p15v_sw.bit_position = VOLT_SW_P15V_SWITCH;
	strcpy(bias_sw->sw_group.p15v_sw.name,"p15v_sw");

	bias_sw->sw_group.m15v_sw.status = 0;
	bias_sw->sw_group.m15v_sw.bit_position = VOLT_SW_M15V_SWITCH;
	strcpy(bias_sw->sw_group.m15v_sw.name,"m15v_sw");

	// Set values to hardware.
	volt_sw_state_set(&(bias_sw->sw_group.ccd_vdd_sw)		, &(bias_sw->state), bias_sw->sw_group.ccd_vdd_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.ccd_vdrain_sw)	, &(bias_sw->state), bias_sw->sw_group.ccd_vdrain_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.ccd_vsub_sw)		, &(bias_sw->state), bias_sw->sw_group.ccd_vsub_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.vsub_load_sw)	, &(bias_sw->state), bias_sw->sw_group.vsub_load_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.vsub_rdiv_sw)	, &(bias_sw->state), bias_sw->sw_group.vsub_rdiv_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.ccd_vr_sw)		, &(bias_sw->state), bias_sw->sw_group.ccd_vr_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.p15v_sw)			, &(bias_sw->state), bias_sw->sw_group.p15v_sw.status);
	volt_sw_state_set(&(bias_sw->sw_group.m15v_sw)			, &(bias_sw->state), bias_sw->sw_group.m15v_sw.status);

	return ret;
}


/**********************
 * brief VOLT_SW_clear
 **********************/
void volt_sw_clear(bias_sw_status_t *clr, uint16_t *state)
{
	// Assert clr.
	clr->status = 1;
	*state |= (1 << clr->bit_position);
	/*gpio_volt_sw.clr = 1;
	gpio_volt_sw.state |= (1 << GPIO_VOLT_SW_CLR);*/

	XGpio_DiscreteWrite(&gpio_volt_sw_i, 1, *state);

	// De-assert clr : LE_N pulse required to reaload contents of shift register into latch.
	clr->status = 0;
	*state &= ~(1 << clr->bit_position);
	/*gpio_volt_sw.clr = 0;
	gpio_volt_sw.state &= ~(1 << GPIO_VOLT_SW_CLR);*/
	XGpio_DiscreteWrite(&gpio_volt_sw_i, 1, *state);
}

/***********************************
 * @brief VOLT_SW_volt_sw_state_set
 ***********************************/
int volt_sw_state_set(bias_sw_status_t *bias_sw_status, uint16_t *state, uint8_t val)
{

	if (val==1)
	{
		bias_sw_status->status = val;
		*state |= (1 << bias_sw_status->bit_position);
	}
	else
	{
		bias_sw_status->status = val;
		*state &= ~(1 << bias_sw_status->bit_position);
	}

	volt_sw_wr(state);

	return 0;

}

/***********************************
 * @brief VOLT_SW_volt_sw_state_get
 ***********************************/
uint8_t volt_sw_state_get(bias_sw_status_t *bias_sw_status)
{
	return bias_sw_status->status;
}

/**********************************
 * @brief VOLT_SW_volt_sw_transfer
 **********************************/
int volt_sw_wr(uint16_t *state)
{

	int ret;
	uint8_t buf[2];

	// Packet format:
	// BIT # || 15       | 14 | 13        | 12         | 11        | 10 | 9  | 8  | 7  | 6     | 5 | 4     | 3  | 2          | 1  | 0      ||
	//       ||          |    |           |            |           |    |    |    |    |       |   |       |    |            |    |        ||
	// 	 	 || CCD_VDD  |  X | VSUB_RDIV | VSUB_LOAD  | CCD_VSUB  | X  | X  | X  | X  | M15V  | X | P15V  | X  | CCD_VDRAIN | X  | CCD_VR ||
	buf[0] = (*state & VOLT_SW_DATA_HIGH_MASK) >> 8;
	buf[1] = (*state & VOLT_SW_DATA_LOW_MASK);

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_volt_sw_i, 1);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Send data.
	ret = XSpi_Transfer(&spi_volt_sw_i, buf, NULL, 2);

	// Latch data to copy shift register into internal latch.
	XGpio_DiscreteWrite(&gpio_volt_sw_i, 1, 0x0);

	// Set LE_N back to logic one.
	XGpio_DiscreteWrite(&gpio_volt_sw_i, 1, 0x2);

	return ret;

}


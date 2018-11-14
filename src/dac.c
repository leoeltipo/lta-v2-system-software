/***************************************************************************//**
* @file DAC.c
* @brief Implementation of DAC Driver.
* @author Angel Soto (angel.soto@uns.edu.ar or angeljsoto@gmail.com)
* @based on AD driver
********************************************************************************
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <xil_printf.h>
#include <xspi.h>
#include <xgpio.h>

#include "dac.h"
#include "interrupt.h"
#include "io_func.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
// XSpi device driver variables.
XSpi_Config		*spi_dac_cfg;
XSpi			spi_dac_i;

// XGpio device driver variables.
XGpio gpio_dac_i;

// Variable for maintaining the sate of the gpios.
//gpio_dac_bits_state_t gpio_dac_bits_state;


/*******************
* @brief DAC_init
********************/
int dac_init(clk_sw_t * clk_sw, clk_group_status_t *clks, uint32_t spi_device_id, uint32_t gpio_device_id)
{
	int ret;
	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;

	// Init spi_dac_i structure.
	ret = XSpi_Initialize(&spi_dac_i, spi_device_id);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	ret = XSpi_Stop(&spi_dac_i);
	if (ret != XST_SUCCESS) {
			return ret;
		}

	// Get device configuration from generated structure.
	spi_dac_cfg = XSpi_LookupConfig(spi_device_id);

	// Get BaseAddress from config.
	base_addr = spi_dac_cfg->BaseAddress;

	// Initialize hardware device.
	ret =  XSpi_CfgInitialize(&spi_dac_i, spi_dac_cfg, base_addr);
	if (ret != XST_SUCCESS) {
			return ret;
		}

	// Configure SPI options.
	control_val = 	XSP_MASTER_OPTION 			|
					//XSP_CLK_ACTIVE_LOW_OPTION	|
					XSP_CLK_PHASE_1_OPTION		|
					XSP_MANUAL_SSELECT_OPTION;


	// Write options into hardware spi device.
	ret = XSpi_SetOptions(&spi_dac_i, control_val);
	if (ret != XST_SUCCESS) {
			return ret;
		}

	// Start the device.
	ret = XSpi_Start(&spi_dac_i);

	// Disable interrupts for this device.
	XSpi_IntrGlobalDisable(&spi_dac_i);

	// Init gpio_dac_i structure.
	ret = XGpio_Initialize(&gpio_dac_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_dac_i, 1, 0x0);

	/*
	 * 0 DAC_LDAC_N
	 * 1 DAC_CLR_N
	 * 2 DAC_RESET_T
	 * 3 DAC_SW_EN
	 *
	 * After Init function:
	 * 	DAC_LDAC_N 	= 0
	 * 	DAC_CLR_N	= 1
	 * 	DAC_RESET_N	= 1
	 * 	DAC_SW_EN	= 0
	 *
	 * 	NOTE: DAC_SW_EN must be set to 0 to allow signals to go through the switch!!!
	 * 	If DAC_SW_EN is set to 1, output clocks go to switch's VSS (-15 V).
	 */

	// Initialize structure for gpio control.
	//status of ldac_n switch
	clk_sw->sw_group.ldac_n.status = 0;
	clk_sw->sw_group.ldac_n.bit_position = GPIO_DAC_DAC_LDAC_N_POSITION;
	strcpy(clk_sw->sw_group.ldac_n.name, "ldac_n");

	//status of clr_n switch
	clk_sw->sw_group.clr_n.status = 0;
	clk_sw->sw_group.clr_n.bit_position = GPIO_DAC_DAC_CLR_N_POSITION;
	strcpy(clk_sw->sw_group.clr_n.name, "clr_n");

	//status of reset_n switch
	clk_sw->sw_group.reset_n.status = 0;
	clk_sw->sw_group.reset_n.bit_position = GPIO_DAC_DAC_RESET_N_POSITION;
	strcpy(clk_sw->sw_group.reset_n.name, "reset_n");

	//status of sw_en switch
	clk_sw->sw_group.sw_en.status = 0;
	clk_sw->sw_group.sw_en.bit_position = GPIO_DAC_DAC_SW_EN_POSITION;
	strcpy(clk_sw->sw_group.sw_en.name, "sw_en");

	/*
	 * Reset sequence of DAC DAC.
	 *
	 * Step 1:
	 * LDAC_N 	= 0
	 * CLR_N 	= 0
	 * RESET_N 	= 0
	 *
	 * Step 2:
	 * LDAC_N	= 0
	 * CLR_N 	= 1
	 * RESET_N	= 0
	 *
	 * Step 3:
	 * LDAC_N	= 0
	 * CLR_N	= 1
	 * RESET_N	= 1
	 *
	 * For the whole reset sequence, DAC_SW_EN is kept enabled.
	 */

	// Step 1.
	clk_sw->sw_group.ldac_n.status 	= GPIO_DAC_LDAC_ENABLE;
	clk_sw->sw_group.clr_n.status 	= GPIO_DAC_CLR_ENABLE;
	clk_sw->sw_group.reset_n.status = GPIO_DAC_RESET_ENABLE;
	clk_sw->sw_group.sw_en.status 	= GPIO_DAC_SW_EN_ENABLE;

	dac_change_switch_status(&(clk_sw->sw_group.ldac_n)	, &(clk_sw->state), clk_sw->sw_group.ldac_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.clr_n)	, &(clk_sw->state), clk_sw->sw_group.clr_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.reset_n)	, &(clk_sw->state), clk_sw->sw_group.reset_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.sw_en)	, &(clk_sw->state), clk_sw->sw_group.sw_en.status);

	// Step 2.
	clk_sw->sw_group.ldac_n.status 	= GPIO_DAC_LDAC_ENABLE;
	clk_sw->sw_group.clr_n.status 	= GPIO_DAC_CLR_DISABLE;
	clk_sw->sw_group.reset_n.status = GPIO_DAC_RESET_ENABLE;
	clk_sw->sw_group.sw_en.status 	= GPIO_DAC_SW_EN_ENABLE;

	dac_change_switch_status(&(clk_sw->sw_group.ldac_n)	, &(clk_sw->state), clk_sw->sw_group.ldac_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.clr_n)	, &(clk_sw->state), clk_sw->sw_group.clr_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.reset_n)	, &(clk_sw->state), clk_sw->sw_group.reset_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.sw_en)	, &(clk_sw->state), clk_sw->sw_group.sw_en.status);

	// Step 3.
	clk_sw->sw_group.ldac_n.status 	= GPIO_DAC_LDAC_ENABLE;
	clk_sw->sw_group.clr_n.status 	= GPIO_DAC_CLR_DISABLE;
	clk_sw->sw_group.reset_n.status = GPIO_DAC_RESET_DISABLE;
	clk_sw->sw_group.sw_en.status 	= GPIO_DAC_SW_EN_ENABLE;

	dac_change_switch_status(&(clk_sw->sw_group.ldac_n)	, &(clk_sw->state), clk_sw->sw_group.ldac_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.clr_n)	, &(clk_sw->state), clk_sw->sw_group.clr_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.reset_n)	, &(clk_sw->state), clk_sw->sw_group.reset_n.status);
	dac_change_switch_status(&(clk_sw->sw_group.sw_en)	, &(clk_sw->state), clk_sw->sw_group.sw_en.status);

	tdelay_s(1);

	// Initialize clocks to default value.
	/*
	 * DAC Channel 0.
	 */
	clks->v1ah.value = 0;
	clks->v1ah.reg = DAC_G0_Ch0;
	clks->v1ah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v1ah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v1ah.vref = (float)DAC_VREF;
	clks->v1ah.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->v1ah.gain = (float)DAC_VXC_GAIN;
	clks->v1ah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v1ah.name, "v1ah");

	clks->v1al.value = 0;
	clks->v1al.reg = DAC_G0_Ch1;
	clks->v1al.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v1al.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v1al.vref = (float)DAC_VREF;
	clks->v1al.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->v1al.gain = (float)DAC_VXC_GAIN;
	clks->v1al.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v1al.name, "v1al");

	clks->v2ch.value = 0;
	clks->v2ch.reg = DAC_G0_Ch2;
	clks->v2ch.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v2ch.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v2ch.vref = (float)DAC_VREF;
	clks->v2ch.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->v2ch.gain = (float)DAC_VXC_GAIN;
	clks->v2ch.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v2ch.name, "v2ch");

	clks->v2cl.value = 0;
	clks->v2cl.reg = DAC_G0_Ch3;
	clks->v2cl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v2cl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v2cl.vref = (float)DAC_VREF;
	clks->v2cl.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->v2cl.gain = (float)DAC_VXC_GAIN;
	clks->v2cl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v2cl.name, "v2cl");

	clks->v3ah.value = 0;
	clks->v3ah.reg = DAC_G0_Ch4;
	clks->v3ah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v3ah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v3ah.vref = (float)DAC_VREF;
	clks->v3ah.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->v3ah.gain = (float)DAC_VXC_GAIN;
	clks->v3ah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v3ah.name, "v3ah");

	clks->v3al.value = 0;
	clks->v3al.reg = DAC_G0_Ch5;
	clks->v3al.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v3al.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v3al.vref = (float)DAC_VREF;
	clks->v3al.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->v3al.gain = (float)DAC_VXC_GAIN;
	clks->v3al.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v3al.name, "v3al");

	clks->h1ah.value = 0;
	clks->h1ah.reg = DAC_G0_Ch6;
	clks->h1ah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h1ah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h1ah.vref = (float)DAC_VREF;
	clks->h1ah.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->h1ah.gain = (float)DAC_ALL_GAIN;
	clks->h1ah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h1ah.name, "h1ah");

	clks->h1al.value = 0;
	clks->h1al.reg = DAC_G0_Ch7;
	clks->h1al.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h1al.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h1al.vref = (float)DAC_VREF;
	clks->h1al.offset = (float)DAC_CH0_OFFSET_DEFAULT;
	clks->h1al.gain = (float)DAC_ALL_GAIN;
	clks->h1al.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h1al.name, "h1al");

	/*
	 * DAC Channel 1.
	 */

	clks->h1bh.value = 0;
	clks->h1bh.reg = DAC_G1_Ch0;
	clks->h1bh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h1bh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h1bh.vref = (float)DAC_VREF;
	clks->h1bh.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->h1bh.gain = (float)DAC_ALL_GAIN;
	clks->h1bh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h1bh.name, "h1bh");

	clks->h1bl.value = 0;
	clks->h1bl.reg = DAC_G1_Ch1;
	clks->h1bl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h1bl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h1bl.vref = (float)DAC_VREF;
	clks->h1bl.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->h1bl.gain = (float)DAC_ALL_GAIN;
	clks->h1bl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h1bl.name, "h1bl");

	clks->h2ch.value = 0;
	clks->h2ch.reg = DAC_G1_Ch2;
	clks->h2ch.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h2ch.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h2ch.vref = (float)DAC_VREF;
	clks->h2ch.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->h2ch.gain = (float)DAC_ALL_GAIN;
	clks->h2ch.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h2ch.name, "h2ch");

	clks->h2cl.value = 0;
	clks->h2cl.reg = DAC_G1_Ch3;
	clks->h2cl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h2cl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h2cl.vref = (float)DAC_VREF;
	clks->h2cl.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->h2cl.gain = (float)DAC_ALL_GAIN;
	clks->h2cl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h2cl.name, "h2cl");

	clks->v1bh.value = 0;
	clks->v1bh.reg = DAC_G1_Ch4;
	clks->v1bh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v1bh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v1bh.vref = (float)DAC_VREF;
	clks->v1bh.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->v1bh.gain = (float)DAC_ALL_GAIN;
	clks->v1bh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v1bh.name, "v1bh");

	clks->v1bl.value = -2;
	clks->v1bl.reg = DAC_G1_Ch5;
	clks->v1bl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v1bl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v1bl.vref = (float)DAC_VREF;
	clks->v1bl.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->v1bl.gain = (float)DAC_ALL_GAIN;
	clks->v1bl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v1bl.name, "v1bl");

	clks->h3ah.value = 0;
	clks->h3ah.reg = DAC_G1_Ch6;
	clks->h3ah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h3ah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h3ah.vref = (float)DAC_VREF;
	clks->h3ah.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->h3ah.gain = (float)DAC_ALL_GAIN;
	clks->h3ah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h3ah.name, "h3ah");

	clks->h3al.value = 0;
	clks->h3al.reg = DAC_G1_Ch7;
	clks->h3al.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h3al.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h3al.vref = (float)DAC_VREF;
	clks->h3al.offset = (float)DAC_CH1_OFFSET_DEFAULT;
	clks->h3al.gain = (float)DAC_ALL_GAIN;
	clks->h3al.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h3al.name, "h3al");

	/*
	 * DAC Channel 2.
	 */

	clks->h3bh.value = 0;
	clks->h3bh.reg = DAC_G2_Ch0;
	clks->h3bh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h3bh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h3bh.vref = (float)DAC_VREF;
	clks->h3bh.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->h3bh.gain = (float)DAC_ALL_GAIN;
	clks->h3bh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h3bh.name, "h3bh");

	clks->h3bl.value = 0;
	clks->h3bl.reg = DAC_G2_Ch1;
	clks->h3bl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->h3bl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->h3bl.vref = (float)DAC_VREF;
	clks->h3bl.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->h3bl.gain = (float)DAC_ALL_GAIN;
	clks->h3bl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->h3bl.name, "h3bl");

	clks->swah.value = 0;
	clks->swah.reg = DAC_G2_Ch2;
	clks->swah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->swah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->swah.vref = (float)DAC_VREF;
	clks->swah.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->swah.gain = (float)DAC_ALL_GAIN;
	clks->swah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->swah.name, "swah");

	clks->swal.value = 0;
	clks->swal.reg = DAC_G2_Ch3;
	clks->swal.vmin = (float)DAC_VOLTAGE_MIN;
	clks->swal.vmax = (float)DAC_VOLTAGE_MAX;
	clks->swal.vref = (float)DAC_VREF;
	clks->swal.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->swal.gain = (float)DAC_ALL_GAIN;
	clks->swal.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->swal.name, "swal");

	clks->swbh.value = 0;
	clks->swbh.reg = DAC_G2_Ch4;
	clks->swbh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->swbh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->swbh.vref = (float)DAC_VREF;
	clks->swbh.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->swbh.gain = (float)DAC_ALL_GAIN;
	clks->swbh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->swbh.name, "swbh");

	clks->swbl.value = 0;
	clks->swbl.reg = DAC_G2_Ch5;
	clks->swbl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->swbl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->swbl.vref = (float)DAC_VREF;
	clks->swbl.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->swbl.gain = (float)DAC_ALL_GAIN;
	clks->swbl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->swbl.name, "swbl");

	clks->rgah.value = 0;
	clks->rgah.reg = DAC_G2_Ch6;
	clks->rgah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->rgah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->rgah.vref = (float)DAC_VREF;
	clks->rgah.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->rgah.gain = (float)DAC_ALL_GAIN;
	clks->rgah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->rgah.name, "rgah");

	clks->rgal.value = 0;
	clks->rgal.reg = DAC_G2_Ch7;
	clks->rgal.vmin = (float)DAC_VOLTAGE_MIN;
	clks->rgal.vmax = (float)DAC_VOLTAGE_MAX;
	clks->rgal.vref = (float)DAC_VREF;
	clks->rgal.offset = (float)DAC_CH2_OFFSET_DEFAULT;
	clks->rgal.gain = (float)DAC_ALL_GAIN;
	clks->rgal.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->rgal.name, "rgal");

	/*
	 * DAC Channel 3.
	 */
	clks->rgbh.value = 0;
	clks->rgbh.reg = DAC_G3_Ch0;
	clks->rgbh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->rgbh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->rgbh.vref = (float)DAC_VREF;
	clks->rgbh.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->rgbh.gain = (float)DAC_ALL_GAIN;
	clks->rgbh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->rgbh.name, "rgbh");

	clks->rgbl.value = 0;
	clks->rgbl.reg = DAC_G3_Ch1;
	clks->rgbl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->rgbl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->rgbl.vref = (float)DAC_VREF;
	clks->rgbl.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->rgbl.gain = (float)DAC_ALL_GAIN;
	clks->rgbl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->rgbl.name, "rgbl");

	clks->ogah.value = 0;
	clks->ogah.reg = DAC_G3_Ch2;
	clks->ogah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->ogah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->ogah.vref = (float)DAC_VREF;
	clks->ogah.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->ogah.gain = (float)DAC_ALL_GAIN;
	clks->ogah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->ogah.name, "ogah");

	clks->ogal.value = 0;
	clks->ogal.reg = DAC_G3_Ch3;
	clks->ogal.vmin = (float)DAC_VOLTAGE_MIN;
	clks->ogal.vmax = (float)DAC_VOLTAGE_MAX;
	clks->ogal.vref = (float)DAC_VREF;
	clks->ogal.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->ogal.gain = (float)DAC_ALL_GAIN;
	clks->ogal.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->ogal.name, "ogal");

	clks->ogbh.value = 0;
	clks->ogbh.reg = DAC_G3_Ch4;
	clks->ogbh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->ogbh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->ogbh.vref = (float)DAC_VREF;
	clks->ogbh.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->ogbh.gain = (float)DAC_ALL_GAIN;
	clks->ogbh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->ogbh.name, "ogbh");

	clks->ogbl.value = 0;
	clks->ogbl.reg = DAC_G3_Ch5;
	clks->ogbl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->ogbl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->ogbl.vref = (float)DAC_VREF;
	clks->ogbl.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->ogbl.gain = (float)DAC_ALL_GAIN;
	clks->ogbl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->ogbl.name, "ogbl");

	clks->dgah.value = 0;
	clks->dgah.reg = DAC_G3_Ch6;
	clks->dgah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->dgah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->dgah.vref = (float)DAC_VREF;
	clks->dgah.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->dgah.gain = (float)DAC_ALL_GAIN;
	clks->dgah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->dgah.name, "dgah");

	clks->dgal.value = 0;
	clks->dgal.reg = DAC_G3_Ch7;
	clks->dgal.vmin = (float)DAC_VOLTAGE_MIN;
	clks->dgal.vmax = (float)DAC_VOLTAGE_MAX;
	clks->dgal.vref = (float)DAC_VREF;
	clks->dgal.offset = (float)DAC_CH3_OFFSET_DEFAULT;
	clks->dgal.gain = (float)DAC_ALL_GAIN;
	clks->dgal.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->dgal.name, "dgal");

	/*
	 * DAC Channel 4.
	 */

	clks->tgah.value = 0;
	clks->tgah.reg = DAC_G4_Ch0;
	clks->tgah.vmin = (float)DAC_VOLTAGE_MIN;
	clks->tgah.vmax = (float)DAC_VOLTAGE_MAX;
	clks->tgah.vref = (float)DAC_VREF;
	clks->tgah.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->tgah.gain = (float)DAC_ALL_GAIN;
	clks->tgah.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->tgah.name, "tgah");

	clks->tgal.value = 0;
	clks->tgal.reg = DAC_G4_Ch1;
	clks->tgal.vmin = (float)DAC_VOLTAGE_MIN;
	clks->tgal.vmax = (float)DAC_VOLTAGE_MAX;
	clks->tgal.vref = (float)DAC_VREF;
	clks->tgal.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->tgal.gain = (float)DAC_ALL_GAIN;
	clks->tgal.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->tgal.name, "tgal");

	clks->tgbh.value = 0;
	clks->tgbh.reg = DAC_G4_Ch2;
	clks->tgbh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->tgbh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->tgbh.vref = (float)DAC_VREF;
	clks->tgbh.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->tgbh.gain = (float)DAC_ALL_GAIN;
	clks->tgbh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->tgbh.name, "tgbh");

	clks->tgbl.value = 0;
	clks->tgbl.reg = DAC_G4_Ch3;
	clks->tgbl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->tgbl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->tgbl.vref = (float)DAC_VREF;
	clks->tgbl.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->tgbl.gain = (float)DAC_ALL_GAIN;
	clks->tgbl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->tgbl.name, "tgbl");

	clks->v3bh.value = 0;
	clks->v3bh.reg = DAC_G4_Ch4;
	clks->v3bh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v3bh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v3bh.vref = (float)DAC_VREF;
	clks->v3bh.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->v3bh.gain = (float)DAC_ALL_GAIN;
	clks->v3bh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v3bh.name, "v3bh");

	clks->v3bl.value = 0;
	clks->v3bl.reg = DAC_G4_Ch5;
	clks->v3bl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->v3bl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->v3bl.vref = (float)DAC_VREF;
	clks->v3bl.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->v3bl.gain = (float)DAC_ALL_GAIN;
	clks->v3bl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->v3bl.name, "v3bl");


	clks->dgbh.value = 0;
	clks->dgbh.reg = DAC_G4_Ch6;
	clks->dgbh.vmin = (float)DAC_VOLTAGE_MIN;
	clks->dgbh.vmax = (float)DAC_VOLTAGE_MAX;
	clks->dgbh.vref = (float)DAC_VREF;
	clks->dgbh.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->dgbh.gain = (float)DAC_ALL_GAIN;
	clks->dgbh.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->dgbh.name, "dgbh");

	clks->dgbl.value = 0;
	clks->dgbl.reg = DAC_G4_Ch7;
	clks->dgbl.vmin = (float)DAC_VOLTAGE_MIN;
	clks->dgbl.vmax = (float)DAC_VOLTAGE_MAX;
	clks->dgbl.vref = (float)DAC_VREF;
	clks->dgbl.offset = (float)DAC_CH4_OFFSET_DEFAULT;
	clks->dgbl.gain = (float)DAC_ALL_GAIN;
	clks->dgbl.max_code = (float)(1 << DAC_BITS);
	strcpy(clks->dgbl.name, "dgbl");


	//initialize offset groups
	int status = 0;
	// Channel offset.
	status = dac_write_sf(DAC_cmd_OFS0, &(clks->v1ah));
	if (status != XST_SUCCESS)
	{
		mprint("Error setting DAC CH0 Offset\r\n");
		return status;
	}
	status = dac_write_sf(DAC_cmd_OFS1, &(clks->h1bh));
 	if (status != XST_SUCCESS)
	{
 		mprint("Error setting DAC CH1 Offset\r\n");
		return status;
	}
	status = dac_write_sf(DAC_cmd_OFS2, &(clks->h3bh));
	if (status != XST_SUCCESS)
	{
		mprint("Error setting DAC CH2 Offset\r\n");
		return status;
	}

	//set default values to hardware
	clk_status_t *clk = (clk_status_t *) clks;
	int nClocks = sizeof(clk_group_status_t)/sizeof(clk_status_t);
	for(int i = 0; i < nClocks; i++)
	{
		status = dac_set_voltage(clk, clk->value);

		if (status != XST_SUCCESS)
		{
			char str[30];
			io_sprintf(str, "Error setting %s to %f\r\n", clk->name, clk->value);
			mprint(str);

			return status;
		}
		clk++;
	}

	return 0;
}

/*******************
* @brief DAC_read
********************/
int dac_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t n)
{
	int ret;
	uint8_t buf[20];

	// Set address.
	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_dac_i, 1);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Send/Receive data,
	return XSpi_Transfer(&spi_dac_i, buf, reg_data, n);
}

int dac_read_xcm(uint16_t addr_base, uint16_t channel, uint16_t *data)
{
	uint8_t buf[3];
	uint16_t addr;
	int ret;

	// Set slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_dac_i, 1);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Address of the register.
	switch (addr_base)
	{
	case DAC_RB_X1AREG_BASE:
	case DAC_RB_X1BREG_BASE:
	case DAC_RB_CREG_BASE:
	case DAC_RB_MREG_BASE:
		addr = addr_base + channel;
		break;

	default:
		addr = addr_base;

	}

	buf[0] = DAC_cmd_Reg2read;
	buf[1] = ( (addr & DAC_DATA_HIGH_MASK) >> 8 );
	buf[2] = (addr & DAC_DATA_LOW_MASK);

	// Send command.
	ret = XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

	// If Transfer was successfully completed, go ahead.
	if (ret != XST_SUCCESS ) {
		return ret;
	}

	// Retrieve data by sending nop command.
	buf[0] = DAC_cmd_nop;
	ret = XSpi_Transfer(&spi_dac_i, buf, buf, 3);

	*data = (buf[1] << 8) + buf[2];

	// Decode data.
	switch (addr_base)
	{
	case DAC_RB_X1AREG_BASE:
	case DAC_RB_X1BREG_BASE:
	case DAC_RB_CREG_BASE:
	case DAC_RB_MREG_BASE:
		*data = *data >> 2;
		break;

	default:
		break;
	}

	return ret;
}

/*******************
* @brief DAC_init
********************/
int dac_write(uint8_t reg_mode, uint8_t reg_addr , uint16_t reg_data)
{
	int ret;
	uint8_t buf[3];
	uint16_t data;


	if (reg_mode !=((uint8_t)0))
	{

		if ((reg_addr!=(uint8_t)7) && (reg_addr!=(uint8_t)6))
		{
			data = 0x3FFF & reg_data;
			data = data << 2 ;
			buf[0] = (reg_mode << 6) & 0xC0;
			buf[0] = buf[0] | (reg_addr & 0x3F);
			//buf[1] = data >> 8;
			buf[1] = (uint8_t)((data>>8) & 0xFF);
			//buf[2] = (uint8_t)data;
			buf[2] = (uint8_t)(data & 0xFF);

			ret = XSpi_SetSlaveSelect(&spi_dac_i, 1);
			if (ret != XST_SUCCESS) {
				return ret;
			}

			return XSpi_Transfer(&spi_dac_i, buf, NULL, 3);
		}
	}

	return -1;
}

/***********************
* @brief DAC_write_sf
************************/
//int DAC_write_sf(uint8_t cmd , uint16_t *reg_data)
int dac_write_sf(uint8_t cmd , clk_status_t *clk)
{
	uint8_t buf[3];
	uint16_t data;
	int ret;


	// Set slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_dac_i, 1);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	if (cmd <((uint8_t)12))
	{
		switch(cmd){
		case DAC_cmd_nop:
			buf[0] = 0x00;
			buf[1] = 0x00;
			buf[2] = 0x00;
			return XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

		case DAC_cmd_Reg2read:
			break;

		case DAC_cmd_WCR:
			data = (uint16_t) clk->offset;
			buf[0] = DAC_cmd_WCR;
			buf[1] = 0xFF;
			buf[2] = (uint8_t)data & 0xE7;
			return XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

		case DAC_cmd_OFS0:
		case DAC_cmd_OFS1:
		case DAC_cmd_OFS2:
		case DAC_cmd_ABSelReg0:
		case DAC_cmd_ABSelReg1:
		case DAC_cmd_ABSelReg2:
		case DAC_cmd_ABSelReg3:
		case DAC_cmd_ABSelReg4:
		case DAC_cmd_Block_ABSelReg:
			data = (uint16_t) clk->offset;
			buf[0] = cmd;
			buf[1] = (data >> 8);
			buf[2] = (uint8_t)data;
			return XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

		default:
			xil_printf("Not Valid Command \n\r");
			break;

		}
	}

	return -1;
}

int dac_change_switch_status(clk_sw_status_t * clk_sw_status, uint16_t *state, const uint8_t new_status)
{
	// Update variable.
	clk_sw_status->status = new_status;
	if (clk_sw_status->status==0)
	{
		*state &= ~(1 <<clk_sw_status->bit_position);
	} else {

		*state |= (1 <<clk_sw_status->bit_position);
	}

	// Write gpio register.
	XGpio_DiscreteWrite(&gpio_dac_i, 1, *state);

	return 0;
}

int dac_set_voltage(clk_status_t *clk, float value)
{
	uint16_t reg_val;

	dac_write_sf(DAC_cmd_OFS1, clk);

	// Check the range.
	if (value < clk->vmin || value > clk->vmax)
	{
		return -1;
	}
	else
	{
		// Update value.
		clk->value = value;

		// Compute DAC code.
		reg_val = (uint16_t)(clk->value*clk->max_code/(4*clk->vref*clk->gain) + clk->offset);
	}

	// Write value to register.
	return dac_write(DAC_REG_X, clk->reg, reg_val);
}


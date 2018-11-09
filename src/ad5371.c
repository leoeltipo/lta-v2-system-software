/***************************************************************************//**
* @file ad5371.c
* @brief Implementation of AD5371 Driver.
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
#include "interrupt.h"
#include "ad5371.h"
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
* @brief ad5371_init
********************/
int ad5371_init(clk_sw_t * clk_sw, clk_group_status_t *clks, uint32_t spi_device_id, uint32_t gpio_device_id)
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
	 * Reset sequence of AD5371 DAC.
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

	ad5371_change_switch_status(&(clk_sw->sw_group.ldac_n)	, &(clk_sw->state), clk_sw->sw_group.ldac_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.clr_n)	, &(clk_sw->state), clk_sw->sw_group.clr_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.reset_n)	, &(clk_sw->state), clk_sw->sw_group.reset_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.sw_en)	, &(clk_sw->state), clk_sw->sw_group.sw_en.status);

	// Step 2.
	clk_sw->sw_group.ldac_n.status 	= GPIO_DAC_LDAC_ENABLE;
	clk_sw->sw_group.clr_n.status 	= GPIO_DAC_CLR_DISABLE;
	clk_sw->sw_group.reset_n.status = GPIO_DAC_RESET_ENABLE;
	clk_sw->sw_group.sw_en.status 	= GPIO_DAC_SW_EN_ENABLE;

	ad5371_change_switch_status(&(clk_sw->sw_group.ldac_n)	, &(clk_sw->state), clk_sw->sw_group.ldac_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.clr_n)	, &(clk_sw->state), clk_sw->sw_group.clr_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.reset_n)	, &(clk_sw->state), clk_sw->sw_group.reset_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.sw_en)	, &(clk_sw->state), clk_sw->sw_group.sw_en.status);

	// Step 3.
	clk_sw->sw_group.ldac_n.status 	= GPIO_DAC_LDAC_ENABLE;
	clk_sw->sw_group.clr_n.status 	= GPIO_DAC_CLR_DISABLE;
	clk_sw->sw_group.reset_n.status = GPIO_DAC_RESET_DISABLE;
	clk_sw->sw_group.sw_en.status 	= GPIO_DAC_SW_EN_ENABLE;

	ad5371_change_switch_status(&(clk_sw->sw_group.ldac_n)	, &(clk_sw->state), clk_sw->sw_group.ldac_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.clr_n)	, &(clk_sw->state), clk_sw->sw_group.clr_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.reset_n)	, &(clk_sw->state), clk_sw->sw_group.reset_n.status);
	ad5371_change_switch_status(&(clk_sw->sw_group.sw_en)	, &(clk_sw->state), clk_sw->sw_group.sw_en.status);

	tdelay_s(1);

	// Initialize clocks to default value.
	/*
	 * DAC Channel 0.
	 */
	clks->v1ch.value = 0;
	clks->v1ch.reg = AD5371_CHANNEL_V1C_H;
	clks->v1ch.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->v1ch.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->v1ch.vref = (float)AD5371_VREF;
	clks->v1ch.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->v1ch.gain = (float)AD5371_VXC_GAIN;
	clks->v1ch.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->v1ch.name, "v1ch");

	clks->v1cl.value = 0;
	clks->v1cl.reg = AD5371_CHANNEL_V1C_L;
	clks->v1cl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->v1cl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->v1cl.vref = (float)AD5371_VREF;
	clks->v1cl.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->v1cl.gain = (float)AD5371_VXC_GAIN;
	clks->v1cl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->v1cl.name, "v1cl");

	clks->v2ch.value = 0;
	clks->v2ch.reg = AD5371_CHANNEL_V2C_H;
	clks->v2ch.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->v2ch.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->v2ch.vref = (float)AD5371_VREF;
	clks->v2ch.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->v2ch.gain = (float)AD5371_VXC_GAIN;
	clks->v2ch.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->v2ch.name, "v2ch");

	clks->v2cl.value = 0;
	clks->v2cl.reg = AD5371_CHANNEL_V2C_L;
	clks->v2cl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->v2cl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->v2cl.vref = (float)AD5371_VREF;
	clks->v2cl.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->v2cl.gain = (float)AD5371_VXC_GAIN;
	clks->v2cl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->v2cl.name, "v2cl");

	clks->v3ch.value = 0;
	clks->v3ch.reg = AD5371_CHANNEL_V3C_H;
	clks->v3ch.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->v3ch.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->v3ch.vref = (float)AD5371_VREF;
	clks->v3ch.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->v3ch.gain = (float)AD5371_VXC_GAIN;
	clks->v3ch.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->v3ch.name, "v3ch");

	clks->v3cl.value = 0;
	clks->v3cl.reg = AD5371_CHANNEL_V3C_L;
	clks->v3cl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->v3cl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->v3cl.vref = (float)AD5371_VREF;
	clks->v3cl.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->v3cl.gain = (float)AD5371_VXC_GAIN;
	clks->v3cl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->v3cl.name, "v3cl");

	clks->h1ah.value = 0;
	clks->h1ah.reg = AD5371_CHANNEL_H1A_H;
	clks->h1ah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h1ah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h1ah.vref = (float)AD5371_VREF;
	clks->h1ah.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->h1ah.gain = (float)AD5371_ALL_GAIN;
	clks->h1ah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h1ah.name, "h1ah");

	clks->h1al.value = 0;
	clks->h1al.reg = AD5371_CHANNEL_H1A_L;
	clks->h1al.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h1al.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h1al.vref = (float)AD5371_VREF;
	clks->h1al.offset = (float)AD5371_CH0_OFFSET_DEFAULT;
	clks->h1al.gain = (float)AD5371_ALL_GAIN;
	clks->h1al.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h1al.name, "h1al");

	/*
	 * DAC Channel 1.
	 */

	clks->h1bh.value = 0;
	clks->h1bh.reg = AD5371_CHANNEL_H1B_H;
	clks->h1bh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h1bh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h1bh.vref = (float)AD5371_VREF;
	clks->h1bh.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h1bh.gain = (float)AD5371_ALL_GAIN;
	clks->h1bh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h1bh.name, "h1bh");

	clks->h1bl.value = 0;
	clks->h1bl.reg = AD5371_CHANNEL_H1B_L;
	clks->h1bl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h1bl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h1bl.vref = (float)AD5371_VREF;
	clks->h1bl.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h1bl.gain = (float)AD5371_ALL_GAIN;
	clks->h1bl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h1bl.name, "h1bl");

	clks->h2ah.value = 0;
	clks->h2ah.reg = AD5371_CHANNEL_H2A_H;
	clks->h2ah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h2ah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h2ah.vref = (float)AD5371_VREF;
	clks->h2ah.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h2ah.gain = (float)AD5371_ALL_GAIN;
	clks->h2ah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h2ah.name, "h2ah");

	clks->h2al.value = 0;
	clks->h2al.reg = AD5371_CHANNEL_H2A_L;
	clks->h2al.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h2al.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h2al.vref = (float)AD5371_VREF;
	clks->h2al.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h2al.gain = (float)AD5371_ALL_GAIN;
	clks->h2al.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h2al.name, "h2al");

	clks->h2bh.value = 4;
	clks->h2bh.reg = AD5371_CHANNEL_H2B_H;
	clks->h2bh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h2bh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h2bh.vref = (float)AD5371_VREF;
	clks->h2bh.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h2bh.gain = (float)AD5371_ALL_GAIN;
	clks->h2bh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h2bh.name, "h2bh");

	clks->h2bl.value = -2;
	clks->h2bl.reg = AD5371_CHANNEL_H2B_L;
	clks->h2bl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h2bl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h2bl.vref = (float)AD5371_VREF;
	clks->h2bl.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h2bl.gain = (float)AD5371_ALL_GAIN;
	clks->h2bl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h2bl.name, "h2bl");

	clks->h3ah.value = 0;
	clks->h3ah.reg = AD5371_CHANNEL_H3A_H;
	clks->h3ah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h3ah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h3ah.vref = (float)AD5371_VREF;
	clks->h3ah.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h3ah.gain = (float)AD5371_ALL_GAIN;
	clks->h3ah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h3ah.name, "h3ah");

	clks->h3al.value = 0;
	clks->h3al.reg = AD5371_CHANNEL_H3A_L;
	clks->h3al.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h3al.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h3al.vref = (float)AD5371_VREF;
	clks->h3al.offset = (float)AD5371_CH1_OFFSET_DEFAULT;
	clks->h3al.gain = (float)AD5371_ALL_GAIN;
	clks->h3al.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h3al.name, "h3al");

	/*
	 * DAC Channel 2.
	 */

	clks->h3bh.value = 0;
	clks->h3bh.reg = AD5371_CHANNEL_H3B_H;
	clks->h3bh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h3bh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h3bh.vref = (float)AD5371_VREF;
	clks->h3bh.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->h3bh.gain = (float)AD5371_ALL_GAIN;
	clks->h3bh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h3bh.name, "h3bh");

	clks->h3bl.value = 0;
	clks->h3bl.reg = AD5371_CHANNEL_H3B_L;
	clks->h3bl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->h3bl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->h3bl.vref = (float)AD5371_VREF;
	clks->h3bl.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->h3bl.gain = (float)AD5371_ALL_GAIN;
	clks->h3bl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->h3bl.name, "h3bl");

	clks->swah.value = 0;
	clks->swah.reg = AD5371_CHANNEL_SWA_H;
	clks->swah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->swah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->swah.vref = (float)AD5371_VREF;
	clks->swah.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->swah.gain = (float)AD5371_ALL_GAIN;
	clks->swah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->swah.name, "swah");

	clks->swal.value = 0;
	clks->swal.reg = AD5371_CHANNEL_SWA_L;
	clks->swal.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->swal.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->swal.vref = (float)AD5371_VREF;
	clks->swal.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->swal.gain = (float)AD5371_ALL_GAIN;
	clks->swal.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->swal.name, "swal");

	clks->swbh.value = 0;
	clks->swbh.reg = AD5371_CHANNEL_SWB_H;
	clks->swbh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->swbh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->swbh.vref = (float)AD5371_VREF;
	clks->swbh.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->swbh.gain = (float)AD5371_ALL_GAIN;
	clks->swbh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->swbh.name, "swbh");

	clks->swbl.value = 0;
	clks->swbl.reg = AD5371_CHANNEL_SWB_L;
	clks->swbl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->swbl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->swbl.vref = (float)AD5371_VREF;
	clks->swbl.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->swbl.gain = (float)AD5371_ALL_GAIN;
	clks->swbl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->swbl.name, "swbl");

	clks->rgah.value = 0;
	clks->rgah.reg = AD5371_CHANNEL_RGA_H;
	clks->rgah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->rgah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->rgah.vref = (float)AD5371_VREF;
	clks->rgah.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->rgah.gain = (float)AD5371_ALL_GAIN;
	clks->rgah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->rgah.name, "rgah");

	clks->rgal.value = 0;
	clks->rgal.reg = AD5371_CHANNEL_RGA_L;
	clks->rgal.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->rgal.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->rgal.vref = (float)AD5371_VREF;
	clks->rgal.offset = (float)AD5371_CH2_OFFSET_DEFAULT;
	clks->rgal.gain = (float)AD5371_ALL_GAIN;
	clks->rgal.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->rgal.name, "rgal");

	/*
	 * DAC Channel 3.
	 */
	clks->rgbh.value = 0;
	clks->rgbh.reg = AD5371_CHANNEL_RGB_H;
	clks->rgbh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->rgbh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->rgbh.vref = (float)AD5371_VREF;
	clks->rgbh.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->rgbh.gain = (float)AD5371_ALL_GAIN;
	clks->rgbh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->rgbh.name, "rgbh");

	clks->rgbl.value = 0;
	clks->rgbl.reg = AD5371_CHANNEL_RGB_L;
	clks->rgbl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->rgbl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->rgbl.vref = (float)AD5371_VREF;
	clks->rgbl.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->rgbl.gain = (float)AD5371_ALL_GAIN;
	clks->rgbl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->rgbl.name, "rgbl");

	clks->ogah.value = 0;
	clks->ogah.reg = AD5371_CHANNEL_OGA_H;
	clks->ogah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->ogah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->ogah.vref = (float)AD5371_VREF;
	clks->ogah.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->ogah.gain = (float)AD5371_ALL_GAIN;
	clks->ogah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->ogah.name, "ogah");

	clks->ogal.value = 0;
	clks->ogal.reg = AD5371_CHANNEL_OGA_L;
	clks->ogal.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->ogal.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->ogal.vref = (float)AD5371_VREF;
	clks->ogal.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->ogal.gain = (float)AD5371_ALL_GAIN;
	clks->ogal.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->ogal.name, "ogal");

	clks->ogbh.value = 0;
	clks->ogbh.reg = AD5371_CHANNEL_OGB_H;
	clks->ogbh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->ogbh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->ogbh.vref = (float)AD5371_VREF;
	clks->ogbh.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->ogbh.gain = (float)AD5371_ALL_GAIN;
	clks->ogbh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->ogbh.name, "ogbh");

	clks->ogbl.value = 0;
	clks->ogbl.reg = AD5371_CHANNEL_OGB_L;
	clks->ogbl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->ogbl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->ogbl.vref = (float)AD5371_VREF;
	clks->ogbl.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->ogbl.gain = (float)AD5371_ALL_GAIN;
	clks->ogbl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->ogbl.name, "ogbl");

	clks->dgh.value = 0;
	clks->dgh.reg = AD5371_CHANNEL_DG_H;
	clks->dgh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->dgh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->dgh.vref = (float)AD5371_VREF;
	clks->dgh.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->dgh.gain = (float)AD5371_ALL_GAIN;
	clks->dgh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->dgh.name, "dgh");

	clks->dgl.value = 0;
	clks->dgl.reg = AD5371_CHANNEL_DG_L;
	clks->dgl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->dgl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->dgl.vref = (float)AD5371_VREF;
	clks->dgl.offset = (float)AD5371_CH3_OFFSET_DEFAULT;
	clks->dgl.gain = (float)AD5371_ALL_GAIN;
	clks->dgl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->dgl.name, "dgl");

	/*
	 * DAC Channel 4.
	 */

	clks->tgh.value = 0;
	clks->tgh.reg = AD5371_CHANNEL_TG_H;
	clks->tgh.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->tgh.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->tgh.vref = (float)AD5371_VREF;
	clks->tgh.offset = (float)AD5371_CH4_OFFSET_DEFAULT;
	clks->tgh.gain = (float)AD5371_ALL_GAIN;
	clks->tgh.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->tgh.name, "tgh");

	clks->tgl.value = 0;
	clks->tgl.reg = AD5371_CHANNEL_TG_L;
	clks->tgl.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->tgl.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->tgl.vref = (float)AD5371_VREF;
	clks->tgl.offset = (float)AD5371_CH4_OFFSET_DEFAULT;
	clks->tgl.gain = (float)AD5371_ALL_GAIN;
	clks->tgl.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->tgl.name, "tgl");

	clks->clkah.value = 0;
	clks->clkah.reg = AD5371_CHANNEL_SPR_CLKA_H;
	clks->clkah.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->clkah.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->clkah.vref = (float)AD5371_VREF;
	clks->clkah.offset = (float)AD5371_CH4_OFFSET_DEFAULT;
	clks->clkah.gain = (float)AD5371_ALL_GAIN;
	clks->clkah.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->clkah.name, "clkah");

	clks->clkal.value = 0;
	clks->clkal.reg = AD5371_CHANNEL_SPR_CLKA_L;
	clks->clkal.vmin = (float)AD5371_VOLTAGE_MIN;
	clks->clkal.vmax = (float)AD5371_VOLTAGE_MAX;
	clks->clkal.vref = (float)AD5371_VREF;
	clks->clkal.offset = (float)AD5371_CH4_OFFSET_DEFAULT;
	clks->clkal.gain = (float)AD5371_ALL_GAIN;
	clks->clkal.max_code = (float)(1 << AD5371_BITS);
	strcpy(clks->clkal.name, "clkal");


	//initialize offset groups
	int status = 0;
	// Channel offset.
	status = ad5371_write_sf(AD5371_cmd_OFS0, &(clks->v1ch));
	if (status != XST_SUCCESS)
	{
		mprint("Error setting DAC CH0 Offset\r\n");
		return status;
	}
	status = ad5371_write_sf(AD5371_cmd_OFS1, &(clks->h1bh));
 	if (status != XST_SUCCESS)
	{
 		mprint("Error setting DAC CH1 Offset\r\n");
		return status;
	}
	status = ad5371_write_sf(AD5371_cmd_OFS2, &(clks->h3bh));
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
		status = ad5371_set_voltage(clk, clk->value);

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
* @brief ad5371_read
********************/
int ad5371_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t n)
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

int ad5371_read_xcm(uint16_t addr_base, uint16_t channel, uint16_t *data)
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
	case AD5371_RB_X1AREG_BASE:
	case AD5371_RB_X1BREG_BASE:
	case AD5371_RB_CREG_BASE:
	case AD5371_RB_MREG_BASE:
		addr = addr_base + channel;
		break;

	default:
		addr = addr_base;

	}

	buf[0] = AD5371_cmd_Reg2read;
	buf[1] = ( (addr & AD5371_DATA_HIGH_MASK) >> 8 );
	buf[2] = (addr & AD5371_DATA_LOW_MASK);

	// Send command.
	ret = XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

	// If Transfer was successfully completed, go ahead.
	if (ret != XST_SUCCESS ) {
		return ret;
	}

	// Retrieve data by sending nop command.
	buf[0] = AD5371_cmd_nop;
	ret = XSpi_Transfer(&spi_dac_i, buf, buf, 3);

	*data = (buf[1] << 8) + buf[2];

	// Decode data.
	switch (addr_base)
	{
	case AD5371_RB_X1AREG_BASE:
	case AD5371_RB_X1BREG_BASE:
	case AD5371_RB_CREG_BASE:
	case AD5371_RB_MREG_BASE:
		*data = *data >> 2;
		break;

	default:
		break;
	}

	return ret;
}

/*******************
* @brief ad5371_init
********************/
int ad5371_write(uint8_t reg_mode, uint8_t reg_addr , uint16_t reg_data)
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
* @brief ad5371_write_sf
************************/
//int ad5371_write_sf(uint8_t cmd , uint16_t *reg_data)
int ad5371_write_sf(uint8_t cmd , clk_status_t *clk)
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
		case AD5371_cmd_nop:
			buf[0] = 0x00;
			buf[1] = 0x00;
			buf[2] = 0x00;
			return XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

		case AD5371_cmd_Reg2read:
			break;

		case AD5371_cmd_WCR:
			data = (uint16_t) clk->offset;
			buf[0] = AD5371_cmd_WCR;
			buf[1] = 0xFF;
			buf[2] = (uint8_t)data & 0xE7;
			return XSpi_Transfer(&spi_dac_i, buf, NULL, 3);

		case AD5371_cmd_OFS0:
		case AD5371_cmd_OFS1:
		case AD5371_cmd_OFS2:
		case AD5371_cmd_ABSelReg0:
		case AD5371_cmd_ABSelReg1:
		case AD5371_cmd_ABSelReg2:
		case AD5371_cmd_ABSelReg3:
		case AD5371_cmd_ABSelReg4:
		case AD5371_cmd_Block_ABSelReg:
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

int ad5371_change_switch_status(clk_sw_status_t * clk_sw_status, uint16_t *state, const uint8_t new_status)
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

int ad5371_set_voltage(clk_status_t *clk, float value)
{
	uint16_t reg_val;

	ad5371_write_sf(AD5371_cmd_OFS1, clk);

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
	return ad5371_write(AD5371_REG_X, clk->reg, reg_val);
}


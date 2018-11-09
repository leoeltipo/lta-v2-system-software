/*******************************************************************************
* @file ad5293.c
* @brief Implementation of AD5293 Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
********************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <xil_printf.h>
#include <xspi.h>
#include <xgpio.h>
#include "interrupt.h"
#include "defines.h"
#include "ad5293.h"
#include "io_func.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
// SPI driver variables.
XSpi_Config	*spi_ldo_cfg;
XSpi		spi_ldo_i;

// GPIO driver variables.
XGpio gpio_ldo_i;

// Structure to maintain state of gpios.
gpio_ldo_bits_state_t gpio_ldo_bits_state;

/*******************
* @brief ad5293_init
********************/
int ad5293_init(bias_group_status_t *biases, uint32_t spi_device_id, uint32_t gpio_device_id)
{
	int ret;
	uint32_t base_addr		= 0;
	uint32_t control_val 	= 0;

	// Init spi_ldo_i structure.
	ret = XSpi_Initialize(&spi_ldo_i, spi_device_id);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	ret = XSpi_Stop(&spi_ldo_i);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Get device configuration from generated structure.
	spi_ldo_cfg = XSpi_LookupConfig(spi_device_id);

	// Get BaseAddress from config.
	base_addr = spi_ldo_cfg->BaseAddress;

	// Initialize hardware device.
	ret = XSpi_CfgInitialize(&spi_ldo_i, spi_ldo_cfg, base_addr);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Configure SPI options.
	control_val = 	XSP_MASTER_OPTION 			|
					//XSP_CLK_ACTIVE_LOW_OPTION	|
					XSP_CLK_PHASE_1_OPTION		|
					XSP_MANUAL_SSELECT_OPTION;

	// Write options into hardware spi device.
	ret = XSpi_SetOptions(&spi_ldo_i, control_val);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Start the device.
	ret = XSpi_Start(&spi_ldo_i);

	// Disable interrupts for this device.
	XSpi_IntrGlobalDisable(&spi_ldo_i);

	// Init gpio_ldo_i structure.
	ret = XGpio_Initialize(&gpio_ldo_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_ldo_i, 1, 0x0);

	// Set default values for GPIOs as follows:
	/*
	 * CCD_VDRAIN_EN	0
	 * CCD_VDD_EN		0
	 * CCD_VR_EN		0
	 * CCD_VSUB_EN		0
	 * DIGPOT_RST_N		1
	 *
	 */
	XGpio_DiscreteWrite(&gpio_ldo_i, 1, 0x10);

	// Init gpio state structure.
	gpio_ldo_bits_state.vdrain_en = 0;
	gpio_ldo_bits_state.vdd_en = 0;
	gpio_ldo_bits_state.vr_en = 0;
	gpio_ldo_bits_state.vsub_en = 0;
	gpio_ldo_bits_state.digpot_rst_n = 1;
	gpio_ldo_bits_state.state = 0x10;

	tdelay_s(1);

	// Set AD5293 to allow modifying digpot values.
	ad5293_creg_wr(1, 0, SPI_LDO_VDRAIN_SLAVE_SELECT);
	ad5293_creg_wr(1, 0, SPI_LDO_VDD_SLAVE_SELECT);
	ad5293_creg_wr(1, 0, SPI_LDO_VR_SLAVE_SELECT);
	ad5293_creg_wr(1, 0, SPI_LDO_VSUB_SLAVE_SELECT);

	/*
	 * VDRAIN:
	 *
	 * VREF = -1.175 V.
	 * R1	= 98.8 KOHM.
	 * R2'	= 4.32 KOHM.
	 *
	 * R2 	= R1/(VOUT/VREF - 1)
	 * RPOT	= R2 - R2'
	 */
	biases->vdrain.value = -22;
	biases->vdrain.reg = SPI_LDO_VDRAIN_SLAVE_SELECT;
	biases->vdrain.vmin = AD5293_VDRAIN_VMIN;
	biases->vdrain.vmax = AD5293_VDRAIN_VMAX;
	biases->vdrain.vref = AD5293_VDRAIN_VREF;
	biases->vdrain.r1 = AD5293_VDRAIN_R1;
	biases->vdrain.r2p = AD5293_VDRAIN_R2P;
	biases->vdrain.bits = AD5293_VDRAIN_BITS;
	biases->vdrain.rm = (float)AD5293_VDRAIN_RFS/(float)(1 << biases->vdrain.bits);
	strcpy(biases->vdrain.name,"vdrain");

	//VDD initial values
	/*
			 * VDD:
			 *
			 * VREF = -1.175 V.
			 * R1	= 98.8 KOHM.
			 * R2'	= 4.32 KOHM.
			 *
			 * R2 	= R1/(VOUT/VREF - 1)
			 * RPOT	= R2 - R2'
	*/
	biases->vdd.value = -22;
	biases->vdd.reg = SPI_LDO_VDD_SLAVE_SELECT;
	biases->vdd.vmin = AD5293_VDD_VMIN;
	biases->vdd.vmax = AD5293_VDD_VMAX;
	biases->vdd.vref = AD5293_VDD_VREF;
	biases->vdd.r1 = AD5293_VDD_R1;
	biases->vdd.r2p = AD5293_VDD_R2P;
	biases->vdd.bits = AD5293_VDD_BITS;
	biases->vdd.rm =  (float)AD5293_VDD_RFS/(float)(1 << biases->vdd.bits);
	strcpy(biases->vdd.name,"vdd");

	//set VR initial values
	/*
	 * VR:
	 *
	 * VREF = -1.175 V.
	 * R1	= 98.8 KOHM.
	 * R2'	= 7.87 KOHM.
	 *
	 * R2 	= R1/(VOUT/VREF - 1)
	 * RPOT	= R2 - R2'
	 */
	biases->vr.value = -13;
	biases->vr.reg = SPI_LDO_VR_SLAVE_SELECT;
	biases->vr.vmin = AD5293_VR_VMIN;
	biases->vr.vmax = AD5293_VR_VMAX;
	biases->vr.vref = AD5293_VR_VREF;
	biases->vr.r1 = AD5293_VR_R1;
	biases->vr.r2p = AD5293_VR_R2P;
	biases->vr.bits = AD5293_VR_BITS;
	biases->vr.rm =  (float)AD5293_VR_RFS/(float)(1 << biases->vr.bits);
	strcpy(biases->vr.name, "vr");



	//set VSUB initial values
	/*
	 * VSUB:
	 *
	 * VREF = 1.175 V.
	 * R1	= 98.8 KOHM.
	 * R2'	= 1.33 KOHM.
	 *
	 * R2 	= R1/(VOUT/VREF - 1)
	 * RPOT	= R2 - R2'
	 */
	biases->vsub.value = 40;
	biases->vsub.reg = SPI_LDO_VSUB_SLAVE_SELECT;
	biases->vsub.vmin = AD5293_VSUB_VMIN;
	biases->vsub.vmax = AD5293_VSUB_VMAX;
	biases->vsub.vref = AD5293_VSUB_VREF;
	biases->vsub.r1 = AD5293_VSUB_R1;
	biases->vsub.r2p = AD5293_VSUB_R2P;
	biases->vsub.bits = AD5293_VSUB_BITS;
	biases->vsub.rm = (float)AD5293_VSUB_RFS/(float)(1 << biases->vsub.bits);
	strcpy(biases->vsub.name, "vsub");

	//set default values to hardware
	bias_status_t *bias = (bias_status_t *) biases;
	int status = 0;
	int nBiases = sizeof(bias_group_status_t)/sizeof(bias_status_t);
	for(int i = 0; i < nBiases; i++)
	{
		status = ad5293_set_voltage(bias, bias->value);
	 	if (status != XST_SUCCESS)
	 	{
	 		char str[50];
	 		io_sprintf(str, "Error setting %s to %f", bias->name, bias->value);
	 		mprint(str);
	 	}
	 	bias++;
	 }

	// Enable LDOs.
	ad5293_sw_en(GPIO_LDO_VDRAIN, 1);
   	ad5293_sw_en(GPIO_LDO_VDD, 1);
   	ad5293_sw_en(GPIO_LDO_VR, 1);
   	ad5293_sw_en(GPIO_LDO_VSUB, 1);

	return ret;
}

/********************** 
* @brief ad5293_rdac_wr 
***********************/
int ad5293_rdac_wr(uint16_t data,uint32_t ss)
{
	int ret;
	uint8_t buf[2];

	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | D9 | D8 | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 ||
	buf[0] = AD5293_CMD_RDAC_WRITE
		   | ( (data & AD5293_DATA_HIGH_MASK) >> 8 );

	buf[1] = (data & AD5293_DATA_LOW_MASK);

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_ldo_i, ss);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Send data.
	return XSpi_Transfer(&spi_ldo_i, buf, NULL, 2);
}

/**********************
* @brief ad5293_rdac_rd
***********************/
int ad5293_rdac_rd(uint16_t *data, uint32_t ss)
{
	uint8_t buf[2];
	int ret;

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_ldo_i, ss);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Send RDAC read command.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  ||
	buf[0] = AD5293_CMD_RDAC_READ;
	buf[1] = 0x00;
	ret = XSpi_Transfer(&spi_ldo_i, buf, NULL, 2);

	// If Transfer was successfully completed, go ahead.
	if (ret != XST_SUCCESS ) {
		return ret;
	}

	// Send NOP command to read data back.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  ||
	buf[0] = AD5293_CMD_NOP;
	buf[1] = 0x00;
	ret = XSpi_Transfer(&spi_ldo_i, buf, buf, 2);

	// Set results into data variable.
	*data = (buf[0] << 8) | buf[1];
	*data &= (AD5293_DATA_HIGH_MASK | AD5293_DATA_LOW_MASK);

	return ret;
}

/******************
* @brief ad5293_rst
*******************/
int ad5293_rst(uint32_t ss)
{
	int ret;
	uint8_t buf[2];

	// Send Reset read command.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  ||
	buf[0] = AD5293_CMD_RESET;
	buf[1] = 0x00;

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_ldo_i, ss);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Transfer data.
	return XSpi_Transfer(&spi_ldo_i, buf, NULL, 2);
}

/**********************
* @brief ad5293_creg_wr
***********************/
int ad5293_creg_wr(uint8_t c1, uint8_t c2, uint32_t ss)
{
	uint8_t buf[2];

	// Send Control Register Write command.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | C2 | C1 | X  ||
	buf[0] = AD5293_CMD_CREG_WRITE;
	buf[1] = ( (c2 & 1) << AD5293_CTRL_REG_C2_BIT_POS )
		   | ( (c1 & 1) << AD5293_CTRL_REG_C1_BIT_POS );

	// Select slave for this device.
	XSpi_SetSlaveSelect(&spi_ldo_i, ss);

	// Transfer data.
	return XSpi_Transfer(&spi_ldo_i, buf, NULL, 2);
}

/**********************
* @brief ad5293_creg_rd
***********************/
int ad5293_creg_rd(uint8_t *c1, uint8_t *c2, uint32_t ss)
{
	uint8_t buf[4];
	int ret;

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_ldo_i, ss);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Send CFG Register read command.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  ||
	buf[0] = AD5293_CMD_CREG_READ;
	buf[1] = 0x00;
	ret = XSpi_Transfer(&spi_ldo_i, buf, NULL, 2);

	// If Transfer was successfully completed, go ahead.
	if (ret != XST_SUCCESS ) {
		return ret;
	}

	// Send NOP command to read data back.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  ||
	buf[0] = AD5293_CMD_NOP;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	ret = XSpi_Transfer(&spi_ldo_i, buf, buf, 2);

	// Write results into c1 and c2 variables.
	*c1 = ( (buf[1] & AD5293_CTRL_REG_C1_BIT_MASK) >> AD5293_CTRL_REG_C1_BIT_POS );
	*c2 = ( (buf[1] & AD5293_CTRL_REG_C2_BIT_MASK) >> AD5293_CTRL_REG_C2_BIT_POS );

	return ret;
}

/********************
* @brief ad5293_pdown
*********************/
int ad5293_pdown(uint8_t d0, uint32_t ss)
{
	int ret;
	uint8_t buf[2];

	// Send Software power-down command.
	// Packet format:
	// BIT # || 15 | 14 | 13 | 12 | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  ||
	//       ||    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    ||
	// 	 	 || 0  |  0 | C3 | C2 | C1 | C0 | X  | X  | X  | X  | X  | X  | X  | X  | X  | D0 ||
	buf[0] = AD5293_CMD_PDOWN;
	buf[1] = ( (d0 & 1) << AD5293_PDOWN_D0_BIT_POS );

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_ldo_i, ss);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Transfer data.
	return XSpi_Transfer(&spi_ldo_i, buf, NULL, 2);
}

int ad5293_sw_en(uint8_t sw, uint8_t en)
{
	// Update variable.
	switch (sw)
	{
	case GPIO_LDO_VDRAIN:
		if (en)
		{
			gpio_ldo_bits_state.vdrain_en = 1;
			gpio_ldo_bits_state.state |= (1 << GPIO_LDO_VDRAIN);
		}
		else
		{
			gpio_ldo_bits_state.vdrain_en = 0;
			gpio_ldo_bits_state.state &= ~(1 << GPIO_LDO_VDRAIN);
		}
		break;

	case GPIO_LDO_VDD:
		if (en)
		{
			gpio_ldo_bits_state.vdd_en = 1;
			gpio_ldo_bits_state.state |= (1 << GPIO_LDO_VDD);
		}
		else
		{
			gpio_ldo_bits_state.vdd_en = 0;
			gpio_ldo_bits_state.state &= ~(1 << GPIO_LDO_VDD);
		}
		break;

	case GPIO_LDO_VR:
		if (en)
		{
			gpio_ldo_bits_state.vr_en = 1;
			gpio_ldo_bits_state.state |= (1 << GPIO_LDO_VR);
		}
		else
		{
			gpio_ldo_bits_state.vr_en = 0;
			gpio_ldo_bits_state.state &= ~(1 << GPIO_LDO_VR);
		}
		break;

	case GPIO_LDO_VSUB:
		if (en)
		{
			gpio_ldo_bits_state.vsub_en = 1;
			gpio_ldo_bits_state.state |= (1 << GPIO_LDO_VSUB);
		}
		else
		{
			gpio_ldo_bits_state.vsub_en = 0;
			gpio_ldo_bits_state.state &= ~(1 << GPIO_LDO_VSUB);
		}
		break;

	case GPIO_LDO_DIGPOT_RST_N:
		if (en)
		{
			gpio_ldo_bits_state.digpot_rst_n = 1;
			gpio_ldo_bits_state.state |= (1 << GPIO_LDO_DIGPOT_RST_N);
		}
		else
		{
			gpio_ldo_bits_state.digpot_rst_n = 0;
			gpio_ldo_bits_state.state &= ~(1 << GPIO_LDO_DIGPOT_RST_N);
		}
		break;

	default:
		break;
	}

	// Write gpio register.
	XGpio_DiscreteWrite(&gpio_ldo_i, 1, gpio_ldo_bits_state.state);

	return 0;
}

int ad5293_set_voltage(bias_status_t *bias, float value)
{
	float vout, vmin, vmax, vref, r1, r2p, rm, r2, rpot;
	//uint16_t bits;
	uint16_t reg_val;
	uint32_t reg;

	vout = value;
	vmin = bias->vmin;
	vmax= bias->vmax;
	vref= bias->vref;
	r1= bias->r1;
	r2p = bias->r2p;
	rm = bias->rm;
	reg = bias->reg;


	// Check min and max.
	if ( (vout < vmin) || (vout > vmax) )
	{
		return -1;
	}
	else
	{
		// Update value.
		bias->value = value;

		// Compute R2.
		r2 	 = r1/(vout/vref - 1);
		rpot = r2 - r2p;
		reg_val = (uint16_t)(rpot/rm + 0.5);
	}

	// Write value into register.
	return ad5293_rdac_wr(reg_val, reg);
}

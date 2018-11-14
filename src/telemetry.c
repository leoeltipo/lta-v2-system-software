/*
 * telemetry.c
 *
 *  Created on: Jul 24, 2018
 *      Author: lstefana
 */

#include <stdint.h>
#include <xspi.h>
#include <xgpio.h>
#include "telemetry.h"

// SPI driver variables.
XSpi_Config	*spi_telemetry_cfg;
XSpi		spi_telemetry_i;

// GPIO driver variables.
XGpio gpio_telemetry_i;

int telemetry_init(telemetry_group_t *sources, uint32_t spi_device_id, uint32_t gpio_device_id)
{
	int ret;
	uint32_t base_addr		= 0;
	uint32_t control_val 	= 0;

	// Init spi_ldo_i structure.
	ret = XSpi_Initialize(&spi_telemetry_i, spi_device_id);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	ret = XSpi_Stop(&spi_telemetry_i);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Get device configuration from generated structure.
	spi_telemetry_cfg = XSpi_LookupConfig(spi_device_id);

	// Get BaseAddress from config.
	base_addr = spi_telemetry_cfg->BaseAddress;

	// Initialize hardware device.
	ret = XSpi_CfgInitialize(&spi_telemetry_i, spi_telemetry_cfg, base_addr);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Configure SPI options.
	control_val = 	XSP_MASTER_OPTION 			|
					//XSP_CLK_ACTIVE_LOW_OPTION	|
					XSP_CLK_PHASE_1_OPTION		|
					XSP_MANUAL_SSELECT_OPTION;

	// Write options into hardware spi device.
	ret = XSpi_SetOptions(&spi_telemetry_i, control_val);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Start the device.
	ret = XSpi_Start(&spi_telemetry_i);

	// Disable interrupts for this device.
	XSpi_IntrGlobalDisable(&spi_telemetry_i);

	// Init gpio_ldo_i structure.
	ret = XGpio_Initialize(&gpio_telemetry_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_telemetry_i, 1, 0x0);

	// Set default values for GPIOs as follows:
	/*
	 * TEL_MUXEN0		0
	 * TEL_MUXEN1		0
	 * TEL_MUXEN2		0
	 * TEL_MUXA0		0
	 * TEL_MUXA1		0
	 * TEL_MUXA2		0
	 *
	 */
	XGpio_DiscreteWrite(&gpio_telemetry_i, 1, 0);

	// Initialize internal sources structure.
	strcpy(sources->swa.name,"swa");
	strcpy(sources->swa.description,"Summing Well A");
	sources->swa.mux_en = TELEMETRY_MUX_EN0;
	sources->swa.mux_ch = TELEMETRY_MUX_CH0;
	sources->swa.ad_ch = TELEMETRY_AD_CH0;
	sources->swa.gain = 1;

	strcpy(sources->swb.name,"swb");
	strcpy(sources->swb.description,"Summing Well B");
	sources->swb.mux_en = TELEMETRY_MUX_EN0;
	sources->swb.mux_ch = TELEMETRY_MUX_CH1;
	sources->swb.ad_ch = TELEMETRY_AD_CH0;
	sources->swb.gain = 1;

	strcpy(sources->oga.name,"oga");
	strcpy(sources->oga.description,"Output Gate A");
	sources->oga.mux_en = TELEMETRY_MUX_EN0;
	sources->oga.mux_ch = TELEMETRY_MUX_CH2;
	sources->oga.ad_ch = TELEMETRY_AD_CH0;
	sources->oga.gain = 1;

	strcpy(sources->ogb.name,"ogb");
	strcpy(sources->ogb.description,"Output Gate B");
	sources->ogb.mux_en = TELEMETRY_MUX_EN0;
	sources->ogb.mux_ch = TELEMETRY_MUX_CH3;
	sources->ogb.ad_ch = TELEMETRY_AD_CH0;
	sources->ogb.gain = 1;

	strcpy(sources->rga.name,"rga");
	strcpy(sources->rga.description,"Reset Gate A");
	sources->rga.mux_en = TELEMETRY_MUX_EN0;
	sources->rga.mux_ch = TELEMETRY_MUX_CH4;
	sources->rga.ad_ch = TELEMETRY_AD_CH0;
	sources->rga.gain = 1;

	strcpy(sources->rgb.name,"rgb");
	strcpy(sources->rgb.description,"Reset Gate B");
	sources->rgb.mux_en = TELEMETRY_MUX_EN0;
	sources->rgb.mux_ch = TELEMETRY_MUX_CH5;
	sources->rgb.ad_ch = TELEMETRY_AD_CH0;
	sources->rgb.gain = 1;

	strcpy(sources->dga.name,"dga");
	strcpy(sources->dga.description,"Dedo Gordo A");
	sources->dga.mux_en = TELEMETRY_MUX_EN0;
	sources->dga.mux_ch = TELEMETRY_MUX_CH6;
	sources->dga.ad_ch = TELEMETRY_AD_CH0;
	sources->dga.gain = 1;

	strcpy(sources->dgb.name,"dgb");
	strcpy(sources->dgb.description,"Dedo Gordo B");
	sources->dgb.mux_en = TELEMETRY_MUX_EN0;
	sources->dgb.mux_ch = TELEMETRY_MUX_CH7;
	sources->dgb.ad_ch = TELEMETRY_AD_CH0;
	sources->dgb.gain = 1;

	strcpy(sources->h1a.name,"h1a");
	strcpy(sources->h1a.description,"H1A Clock");
	sources->h1a.mux_en = TELEMETRY_MUX_EN0;
	sources->h1a.mux_ch = TELEMETRY_MUX_CH0;
	sources->h1a.ad_ch = TELEMETRY_AD_CH1;
	sources->h1a.gain = 1;

	strcpy(sources->h1b.name,"h1b");
	strcpy(sources->h1b.description,"H1B Clock");
	sources->h1b.mux_en = TELEMETRY_MUX_EN0;
	sources->h1b.mux_ch = TELEMETRY_MUX_CH1;
	sources->h1b.ad_ch = TELEMETRY_AD_CH1;
	sources->h1b.gain = 1;

	strcpy(sources->h2c.name,"h2c");
	strcpy(sources->h2c.description,"H2C Clock");
	sources->h2c.mux_en = TELEMETRY_MUX_EN0;
	sources->h2c.mux_ch = TELEMETRY_MUX_CH2;
	sources->h2c.ad_ch = TELEMETRY_AD_CH1;
	sources->h2c.gain = 1;

	strcpy(sources->v2c.name,"v2c");
	strcpy(sources->v2c.description,"V2C Clock");
	sources->v2c.mux_en = TELEMETRY_MUX_EN0;
	sources->v2c.mux_ch = TELEMETRY_MUX_CH3;
	sources->v2c.ad_ch = TELEMETRY_AD_CH1;
	sources->v2c.gain = 1;

	strcpy(sources->h3a.name,"h3a");
	strcpy(sources->h3a.description,"H3A Clock");
	sources->h3a.mux_en = TELEMETRY_MUX_EN0;
	sources->h3a.mux_ch = TELEMETRY_MUX_CH4;
	sources->h3a.ad_ch = TELEMETRY_AD_CH1;
	sources->h3a.gain = 1;

	strcpy(sources->h3b.name,"h3b");
	strcpy(sources->h3b.description,"H3B Clock");
	sources->h3b.mux_en = TELEMETRY_MUX_EN0;
	sources->h3b.mux_ch = TELEMETRY_MUX_CH5;
	sources->h3b.ad_ch = TELEMETRY_AD_CH1;
	sources->h3b.gain = 1;

	strcpy(sources->v1a.name,"v1a");
	strcpy(sources->v1a.description,"V1A Clock");
	sources->v1a.mux_en = TELEMETRY_MUX_EN0;
	sources->v1a.mux_ch = TELEMETRY_MUX_CH6;
	sources->v1a.ad_ch = TELEMETRY_AD_CH1;
	sources->v1a.gain = 1;

	strcpy(sources->v1b.name,"v1b");
	strcpy(sources->v1b.description,"V1B Clock");
	sources->v1b.mux_en = TELEMETRY_MUX_EN0;
	sources->v1b.mux_ch = TELEMETRY_MUX_CH7;
	sources->v1b.ad_ch = TELEMETRY_AD_CH1;
	sources->v1b.gain = 1;

	strcpy(sources->v3a.name,"v3a");
	strcpy(sources->v3a.description,"V3A Clock");
	sources->v3a.mux_en = TELEMETRY_MUX_EN1;
	sources->v3a.mux_ch = TELEMETRY_MUX_CH0;
	sources->v3a.ad_ch = TELEMETRY_AD_CH2;
	sources->v3a.gain = 1;

	strcpy(sources->v3b.name,"v3b");
	strcpy(sources->v3b.description,"V3B Clock");
	sources->v3b.mux_en = TELEMETRY_MUX_EN1;
	sources->v3b.mux_ch = TELEMETRY_MUX_CH1;
	sources->v3b.ad_ch = TELEMETRY_AD_CH2;
	sources->v3b.gain = 1;

	strcpy(sources->tga.name,"tga");
	strcpy(sources->tga.description,"TGA Offset");
	sources->tga.mux_en = TELEMETRY_MUX_EN1;
	sources->tga.mux_ch = TELEMETRY_MUX_CH2;
	sources->tga.ad_ch = TELEMETRY_AD_CH2;
	sources->tga.gain = 1;

	strcpy(sources->tgb.name,"tgb");
	strcpy(sources->tgb.description,"TGB Offset");
	sources->tgb.mux_en = TELEMETRY_MUX_EN1;
	sources->tgb.mux_ch = TELEMETRY_MUX_CH3;
	sources->tgb.ad_ch = TELEMETRY_AD_CH2;
	sources->tgb.gain = 1;

	strcpy(sources->v_p2v5.name,"v_p2v5");
	strcpy(sources->v_p2v5.description,"+2.5V Source");
	sources->v_p2v5.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p2v5.mux_ch = TELEMETRY_MUX_CH7;
	sources->v_p2v5.ad_ch = TELEMETRY_AD_CH2;
	sources->v_p2v5.gain = 1;

	strcpy(sources->v_p1v0.name,"v_p1v0");
	strcpy(sources->v_p1v0.description,"+1.0V Source");
	sources->v_p1v0.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p1v0.mux_ch = TELEMETRY_MUX_CH0;
	sources->v_p1v0.ad_ch = TELEMETRY_AD_CH3;
	sources->v_p1v0.gain = 1;

	strcpy(sources->v_p4v2.name,"v_p4v2");
	strcpy(sources->v_p4v2.description,"+4.2V Source");
	sources->v_p4v2.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p4v2.mux_ch = TELEMETRY_MUX_CH1;
	sources->v_p4v2.ad_ch = TELEMETRY_AD_CH3;
	sources->v_p4v2.gain = 1;

	strcpy(sources->v_p1v8.name,"v_p1v8");
	strcpy(sources->v_p1v8.description,"+1.8V Source");
	sources->v_p1v8.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p1v8.mux_ch = TELEMETRY_MUX_CH2;
	sources->v_p1v8.ad_ch = TELEMETRY_AD_CH3;
	sources->v_p1v8.gain = 1;

	strcpy(sources->v_p5v0.name,"v_p5v0");
	strcpy(sources->v_p5v0.description,"+5.0V Source");
	sources->v_p5v0.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p5v0.mux_ch = TELEMETRY_MUX_CH3;
	sources->v_p5v0.ad_ch = TELEMETRY_AD_CH3;
	sources->v_p5v0.gain = 1;

	strcpy(sources->v_p2v5a.name,"v_p2v5a");
	strcpy(sources->v_p2v5a.description,"+2.5V Source (A)");
	sources->v_p2v5a.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p2v5a.mux_ch = TELEMETRY_MUX_CH4;
	sources->v_p2v5a.ad_ch = TELEMETRY_AD_CH3;
	sources->v_p2v5a.gain = 1;

	strcpy(sources->v_p3v3.name,"v_p3v3");
	strcpy(sources->v_p3v3.description,"+v3.3V Source");
	sources->v_p3v3.mux_en = TELEMETRY_MUX_EN1;
	sources->v_p3v3.mux_ch = TELEMETRY_MUX_CH5;
	sources->v_p3v3.ad_ch = TELEMETRY_AD_CH3;
	sources->v_p3v3.gain = 1;

	strcpy(sources->v_m15v0.name,"v_m15v0");
	strcpy(sources->v_m15v0.description,"-15.0V Source");
	sources->v_m15v0.mux_en = TELEMETRY_MUX_EN2;
	sources->v_m15v0.mux_ch = TELEMETRY_MUX_CH2;
	sources->v_m15v0.ad_ch = TELEMETRY_AD_CH4;
	sources->v_m15v0.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	strcpy(sources->v_p12v0.name,"v_p12v0");
	strcpy(sources->v_p12v0.description,"+12.0V Source");
	sources->v_p12v0.mux_en = TELEMETRY_MUX_EN2;
	sources->v_p12v0.mux_ch = TELEMETRY_MUX_CH3;
	sources->v_p12v0.ad_ch = TELEMETRY_AD_CH4;
	sources->v_p12v0.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	strcpy(sources->v_p15v0.name,"v_p15v0");
	strcpy(sources->v_p15v0.description,"+15.0V Source");
	sources->v_p15v0.mux_en = TELEMETRY_MUX_EN2;
	sources->v_p15v0.mux_ch = TELEMETRY_MUX_CH4;
	sources->v_p15v0.ad_ch = TELEMETRY_AD_CH4;
	sources->v_p15v0.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	strcpy(sources->ccd_vdd.name,"ccd_vdd");
	strcpy(sources->ccd_vdd.description,"CCD VDD Source");
	sources->ccd_vdd.mux_en = TELEMETRY_MUX_EN2;
	sources->ccd_vdd.mux_ch = TELEMETRY_MUX_CH0;
	sources->ccd_vdd.ad_ch = TELEMETRY_AD_CH4;
	sources->ccd_vdd.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	strcpy(sources->ccd_vr.name,"ccd_vr");
	strcpy(sources->ccd_vr.description,"CCD VR Source");
	sources->ccd_vr.mux_en = TELEMETRY_MUX_EN2;
	sources->ccd_vr.mux_ch = TELEMETRY_MUX_CH1;
	sources->ccd_vr.ad_ch = TELEMETRY_AD_CH4;
	sources->ccd_vr.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	strcpy(sources->ccd_vsub.name,"ccd_vsub");
	strcpy(sources->ccd_vsub.description,"CCD VSUB Source");
	sources->ccd_vsub.mux_en = TELEMETRY_MUX_EN2;
	sources->ccd_vsub.mux_ch = TELEMETRY_MUX_CH5;
	sources->ccd_vsub.ad_ch = TELEMETRY_AD_CH4;
	sources->ccd_vsub.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	strcpy(sources->ccd_vdrain.name,"ccd_vdrain");
	strcpy(sources->ccd_vdrain.description,"CCD VDRAIN Source");
	sources->ccd_vdrain.mux_en = TELEMETRY_MUX_EN2;
	sources->ccd_vdrain.mux_ch = TELEMETRY_MUX_CH6;
	sources->ccd_vdrain.ad_ch = TELEMETRY_AD_CH4;
	sources->ccd_vdrain.gain = (float)TELEMETRY_GAIN_NUM/(float)TELEMETRY_GAIN_DEN;

	return ret;
}

int telemetry_read(telemetry_source_t *source, float *value)
{
	int ret;
	uint8_t buf[2];

	// Set gpio pins.
	uint32_t pins;
	pins = 	( source->mux_en	<< TELEMETRY_MUX_EN_OFFSET ) |
			( source->mux_ch	<< TELEMETRY_MUX_CH_OFFSET );

	XGpio_DiscreteWrite(&gpio_telemetry_i, 1, pins);

	// Set ADC to read value.

	// Select slave for this device.
	ret = XSpi_SetSlaveSelect(&spi_telemetry_i, 1);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Write Control reg command.
	// Packet format:
	// BIT # || 15 |  14  | 13  |  12  |  11  |  10  |  9  |  8  |  7   |  6   | 5  |  4   |   3   |   2   |   1   | 0  ||
	//       ||    |      |     |      |      |      |     |     |      |      |    |      |       |       |       |    ||
	// 	 	 || W  |  RS1 | RS2 | ADD2 | ADD1 | ADD0 | M1  | M0  | PM1  | PM0  | C  | REF  | SEQ1  | SEQ2  | W/TS  | 0  ||
	buf[0] = 	( TELEMETRY_CMD_WRITE_CTRL	<< TELEMETRY_CMD_OFFSET 	) |
				( source->ad_ch				<< TELEMETRY_AD_CH_OFFSET 	) |
				( TELEMETRY_MODE_8_SINGLE	<< TELEMETRY_MODE_OFFSET 	);

	buf[1] = 	( TELEMETRY_PMODE_NORMAL 	<< TELEMETRY_PMODE_OFFSET	) |
				( TELEMETRY_CODING_TWOS 	<< TELEMETRY_CODING_OFFSET	) |
				( TELEMETRY_REF_INT 		<< TELEMETRY_REF_OFFSET		) |
				( TELEMETRY_SEQ_NOT 		<< TELEMETRY_SEQ_OFFSET		) |
				( TELEMETRY_DOUT_TRI 		<< TELEMETRY_DOUT_OFFSET	);

	ret = XSpi_Transfer(&spi_telemetry_i, buf, buf, 2);

	// If Transfer was successfully completed, go ahead.
	if (ret != XST_SUCCESS ) {
		return ret;
	}

	// Send dummy command to read data back.
	// Packet format:
	// BIT # || 15 |  14  | 13  |  12  |  11  |  10  |  9  |  8  |  7   |  6   | 5  |  4   |   3   |   2   |   1   | 0  ||
	//       ||    |      |     |      |      |      |     |     |      |      |    |      |       |       |       |    ||
	// 	 	 || W  |  RS1 | RS2 | ADD2 | ADD1 | ADD0 | M1  | M0  | PM1  | PM0  | C  | REF  | SEQ1  | SEQ2  | W/TS  | 0  ||
	buf[0] = ( TELEMETRY_CMD_NONE << TELEMETRY_CMD_OFFSET );
	buf[1] = 0;

	ret = XSpi_Transfer(&spi_telemetry_i, buf, buf, 2);

	uint16_t data = ( buf[0] << 8 | buf[1] );
	int ad_ch 	= ( (data >> TELEMETRY_DATA_CHID_OFFSET) 	& TELEMETRY_DATA_CHID_MASK );
	int s 		= ( (data >> TELEMETRY_DATA_SIGN_OFFSET) 	& TELEMETRY_DATA_SIGN_MASK );
	int conv	= ( (data >> TELEMETRY_DATA_VAL_OFFSET) 	& TELEMETRY_DATA_VAL_MASK );

	// Check if it's a negative number.
	if (s) {
		conv = conv - TELEMETRY_AD_MAX_COUNT;
	}

	// Compute value.
	float conv_f = conv;
	if (ad_ch == source->ad_ch) {
		conv_f = conv_f * source->gain;
		conv_f = conv_f * (float)TELEMETRY_AD_MAX_VOLT;
		conv_f = conv_f / (float)TELEMETRY_AD_MAX_COUNT;
		*value = conv_f;
	}
	else {
		return -1;
	}


	return ret;
}



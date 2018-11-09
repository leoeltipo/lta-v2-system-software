/*
 * telemetry.h
 *
 *  Created on: Jul 24, 2018
 *      Author: lstefana
 */

#ifndef SRC_TELEMETRY_H_
#define SRC_TELEMETRY_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define TELEMETRY_MUX_EN0			1
#define TELEMETRY_MUX_EN1			2
#define TELEMETRY_MUX_EN2			4
#define TELEMETRY_MUX_EN_OFFSET 	0

#define TELEMETRY_MUX_CH0		0
#define TELEMETRY_MUX_CH1		1
#define TELEMETRY_MUX_CH2		2
#define TELEMETRY_MUX_CH3		3
#define TELEMETRY_MUX_CH4		4
#define TELEMETRY_MUX_CH5		5
#define TELEMETRY_MUX_CH6		6
#define TELEMETRY_MUX_CH7		7
#define TELEMETRY_MUX_CH_OFFSET	3

#define TELEMETRY_AD_CH0		0
#define TELEMETRY_AD_CH1		1
#define TELEMETRY_AD_CH2		2
#define TELEMETRY_AD_CH3		3
#define TELEMETRY_AD_CH4		4
#define TELEMETRY_AD_CH5		5
#define TELEMETRY_AD_CH6		6
#define TELEMETRY_AD_CH_OFFSET 	2

#define TELEMETRY_GAIN_NUM	1111
#define TELEMETRY_GAIN_DEN	111

// AD7328 Commands.
#define TELEMETRY_CMD_NONE			0
#define TELEMETRY_CMD_WRITE_CTRL	4
#define TELEMETRY_CMD_WRITE_RANGE1	5
#define TELEMETRY_CMD_WRITE_RANGE2	6
#define TELEMETRY_CMD_WRITE_SEQ		7
#define TELEMETRY_CMD_OFFSET		5

#define TELEMETRY_MODE_8_SINGLE		0
#define TELEMETRY_MODE_4_PSEUDO		1
#define TELEMETRY_MODE_4_DIFF		2
#define TELEMETRY_MODE_7_PSEUDO		3
#define TELEMETRY_MODE_OFFSET		0

#define TELEMETRY_PMODE_NORMAL		0
#define TELEMETRY_PMODE_AUTO_STDBY	1
#define TELEMETRY_PMODE_AUTO_PDOWN	2
#define TELEMETRY_PMODE_PDOWN		3
#define TELEMETRY_PMODE_OFFSET		6

#define TELEMETRY_CODING_TWOS		0
#define TELEMETRY_CODING_BINARY		1
#define TELEMETRY_CODING_OFFSET		5

#define TELEMETRY_REF_EXT			0
#define TELEMETRY_REF_INT			1
#define TELEMETRY_REF_OFFSET		4

#define TELEMETRY_SEQ_NOT			0
#define TELEMETRY_SEQ_PRG			1
#define TELEMETRY_SEQ_0TON			2
#define TELEMETRY_SEQ_OFFSET		2

#define TELEMETRY_DOUT_TRI			0
#define TELEMETRY_DOUT_WEAK			1
#define TELEMETRY_DOUT_OFFSET		1

// Data back from AD7328.
#define TELEMETRY_DATA_CHID_MASK 	7
#define TELEMETRY_DATA_CHID_OFFSET	13

#define TELEMETRY_DATA_SIGN_MASK	1
#define TELEMETRY_DATA_SIGN_OFFSET	12

#define TELEMETRY_DATA_VAL_MASK		0x0FFF
#define TELEMETRY_DATA_VAL_OFFSET	0

#define TELEMETRY_AD_MAX_COUNT	(1 << 12)
#define TELEMETRY_AD_MAX_VOLT	10

typedef struct {
	char name[20];
	char description[20];
	uint8_t mux_en;
	uint8_t mux_ch;
	uint8_t ad_ch;
	float gain;
} telemetry_source_t;

typedef struct {
	telemetry_source_t swa;
	telemetry_source_t swb;
	telemetry_source_t oga;
	telemetry_source_t ogb;
	telemetry_source_t rga;
	telemetry_source_t rgb;
	telemetry_source_t dg;
	telemetry_source_t tg;
	telemetry_source_t h1a;
	telemetry_source_t h1b;
	telemetry_source_t h2a;
	telemetry_source_t h2b;
	telemetry_source_t h3a;
	telemetry_source_t h3b;
	telemetry_source_t v1c;
	telemetry_source_t v2c;
	telemetry_source_t v3c;
	telemetry_source_t rtd;
	telemetry_source_t vida_offset;
	telemetry_source_t vidb_offset;
	telemetry_source_t vidc_offset;
	telemetry_source_t vidd_offset;
	telemetry_source_t v_p2v5;
	telemetry_source_t v_p1v0;
	telemetry_source_t v_p6v0;
	telemetry_source_t v_p1v8;
	telemetry_source_t v_p5v0;
	telemetry_source_t v_m2v0;
	telemetry_source_t v_p3v3;
	telemetry_source_t v_p1v2;
	telemetry_source_t v_m15v0;
	telemetry_source_t v_p12v0;
	telemetry_source_t v_p15v0;
	telemetry_source_t ccd_vdd;
	telemetry_source_t ccd_vr;
	telemetry_source_t ccd_vsub;
	telemetry_source_t ccd_vdrain;
} telemetry_group_t;

int telemetry_init(telemetry_group_t *sources, uint32_t spi_device_id, uint32_t gpio_device_id);
int telemetry_read(telemetry_source_t *source, float *value);

#endif /* SRC_TELEMETRY_H_ */

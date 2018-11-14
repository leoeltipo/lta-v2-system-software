/******************************************************************************
* @file LDOS.h
* @brief Header file of LDOS Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
*
* The LDOS is a digital potentiometer used to control the voltage output of a
* LDO. There are 4 LDO/LDOS pair in the board. Slave order should be taken care
* of at a higer level. Use LDOS_SLAVE_SELECT_X macros to re-asign for correct
* slave order.
*
* The LDOS is controlled by using a SPI I/F. Some additional GPIOs have been
* added for LDO enable and DIGPOT reset control. GPIOs should be mapped as follows
*
* Signal Name		Bit #
*
* CCD_VDRAIN_EN		0
* CCD_VDD_EN		1
* CCD_VR_EN			2
* CCD_VSUB_EN		3
* DIGPOT_RST_N		4
*
* Check FPGA Firmware version to ensure this is the right mapping.
*******************************************************************************/
#ifndef LDOS_H_
#define LDOS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/****************************** LDOS Commands *******************************/
/******************************************************************************/
#define LDOS_CMD_NOP			( 0x0UL << 2)	// Command 0
#define LDOS_CMD_RDAC_WRITE	( 0x1UL << 2) // Command 1
#define LDOS_CMD_RDAC_READ	( 0x2UL << 2) // Command 2
#define LDOS_CMD_RESET		( 0x4UL << 2) // Command 3
#define LDOS_CMD_CREG_WRITE	( 0x6UL << 2)	// Command 4
#define LDOS_CMD_CREG_READ	( 0x7UL << 2)	// Command 5
#define LDOS_CMD_PDOWN		( 0x8UL << 2) // Command 6

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define LDOS_PDOWN_D0_BIT_POS			0
#define LDOS_PDOWN_D0_BIT_MASK		0x01
#define LDOS_CTRL_REG_C1_BIT_POS		1
#define LDOS_CTRL_REG_C1_BIT_MASK		0x02
#define LDOS_CTRL_REG_C2_BIT_POS		2
#define LDOS_CTRL_REG_C2_BIT_MASK		0x04

#define LDOS_DATA_LOW_MASK	0x00FF
#define LDOS_DATA_HIGH_MASK	0x0300

#define LDOS_SLAVE_SELECT_0	0x1
#define LDOS_SLAVE_SELECT_1	0x2
#define LDOS_SLAVE_SELECT_2	0x4
#define LDOS_SLAVE_SELECT_3	0x8

#define SPI_LDO_VDRAIN_SLAVE_SELECT LDOS_SLAVE_SELECT_0
#define SPI_LDO_VDD_SLAVE_SELECT	LDOS_SLAVE_SELECT_1
#define SPI_LDO_VR_SLAVE_SELECT		LDOS_SLAVE_SELECT_2
#define SPI_LDO_VSUB_SLAVE_SELECT	LDOS_SLAVE_SELECT_3

#define GPIO_LDO_VDRAIN			0
#define GPIO_LDO_VDD			1
#define GPIO_LDO_VR				2
#define GPIO_LDO_VSUB			3
#define GPIO_LDO_DIGPOT_RST_N	4

// Defines used to configure voltages.
#define LDOS_VDRAIN			0
#define LDOS_VDRAIN_VMIN	-28
#define LDOS_VDRAIN_VMAX	-6
#define LDOS_VDRAIN_VREF	-1.175
#define LDOS_VDRAIN_R1		98800
#define LDOS_VDRAIN_R2P		4320
#define LDOS_VDRAIN_RFS		20000
#define LDOS_VDRAIN_BITS	10

#define LDOS_VDD			1
#define LDOS_VDD_VMIN		-28
#define LDOS_VDD_VMAX		-6
#define LDOS_VDD_VREF		-1.175
#define LDOS_VDD_R1			98800
#define LDOS_VDD_R2P		4320
#define LDOS_VDD_RFS		20000
#define LDOS_VDD_BITS		10

#define LDOS_VR				2
#define LDOS_VR_VMIN		-15
#define LDOS_VR_VMAX		-6
#define LDOS_VR_VREF		-1.175
#define LDOS_VR_R1			98800
#define LDOS_VR_R2P			7870
#define LDOS_VR_RFS			20000
#define LDOS_VR_BITS		10

#define LDOS_VSUB			3
#define LDOS_VSUB_VMIN		7
#define LDOS_VSUB_VMAX		88
#define LDOS_VSUB_VREF		1.175
#define LDOS_VSUB_R1		98800
#define LDOS_VSUB_R2P		1330
#define LDOS_VSUB_RFS		20000
#define LDOS_VSUB_BITS		10

typedef struct {
	uint8_t vdrain_en;
	uint8_t vdd_en;
	uint8_t vr_en;
	uint8_t vsub_en;
	uint8_t digpot_rst_n;
	uint16_t state;
} gpio_ldo_bits_state_t;

typedef struct {
	float value;
	uint32_t reg;
	float vmin;
	float vmax;
	float vref;
	float r1;
	float r2p;
	uint16_t bits;
	float rm;
	char name[10];
} bias_status_t;

typedef struct {
	bias_status_t vdrain;
	bias_status_t vdd;
	bias_status_t vr;
	bias_status_t vsub;
} bias_group_status_t;


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int ldos_init(bias_group_status_t *biases, uint32_t spi_device_id, uint32_t gpio_device_id);
int ldos_rdac_wr(uint16_t data, uint32_t ss);
int ldos_rdac_rd(uint16_t *data, uint32_t ss);
int ldos_rst(uint32_t ss);
int ldos_creg_wr(uint8_t c1, uint8_t c2, uint32_t ss);
int ldos_creg_rd(uint8_t *c1, uint8_t *c2, uint32_t ss);
int ldos_pdown(uint8_t d0, uint32_t ss);
int ldos_sw_en(uint8_t sw, uint8_t en);

int ldos_bias_status_init(bias_group_status_t *biases);
int ldos_set_voltage(bias_status_t *bias, float value);
#endif


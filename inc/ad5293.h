/******************************************************************************
* @file ad5293.h
* @brief Header file of AD5293 Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
*
* The AD5293 is a digital potentiometer used to control the voltage output of a
* LDO. There are 4 LDO/AD5293 pair in the board. Slave order should be taken care
* of at a higer level. Use AD5293_SLAVE_SELECT_X macros to re-asign for correct
* slave order.
*
* The AD5293 is controlled by using a SPI I/F. Some additional GPIOs have been
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
#ifndef AD5293_H_
#define AD5293_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/****************************** AD5293 Commands *******************************/
/******************************************************************************/
#define AD5293_CMD_NOP			( 0x0UL << 2)	// Command 0
#define AD5293_CMD_RDAC_WRITE	( 0x1UL << 2) // Command 1
#define AD5293_CMD_RDAC_READ	( 0x2UL << 2) // Command 2
#define AD5293_CMD_RESET		( 0x4UL << 2) // Command 3
#define AD5293_CMD_CREG_WRITE	( 0x6UL << 2)	// Command 4
#define AD5293_CMD_CREG_READ	( 0x7UL << 2)	// Command 5
#define AD5293_CMD_PDOWN		( 0x8UL << 2) // Command 6

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD5293_PDOWN_D0_BIT_POS			0
#define AD5293_PDOWN_D0_BIT_MASK		0x01
#define AD5293_CTRL_REG_C1_BIT_POS		1
#define AD5293_CTRL_REG_C1_BIT_MASK		0x02
#define AD5293_CTRL_REG_C2_BIT_POS		2
#define AD5293_CTRL_REG_C2_BIT_MASK		0x04

#define AD5293_DATA_LOW_MASK	0x00FF
#define AD5293_DATA_HIGH_MASK	0x0300

#define AD5293_SLAVE_SELECT_0	0x1
#define AD5293_SLAVE_SELECT_1	0x2
#define AD5293_SLAVE_SELECT_2	0x4
#define AD5293_SLAVE_SELECT_3	0x8

#define GPIO_LDO_VDRAIN			0
#define GPIO_LDO_VDD			1
#define GPIO_LDO_VR				2
#define GPIO_LDO_VSUB			3
#define GPIO_LDO_DIGPOT_RST_N	4

// Defines used to configure voltages.
#define AD5293_VDRAIN		0
#define AD5293_VDRAIN_VMIN	-28
#define AD5293_VDRAIN_VMAX	-6
#define AD5293_VDRAIN_VREF	-1.175
#define AD5293_VDRAIN_R1	98800
#define AD5293_VDRAIN_R2P	4320
#define AD5293_VDRAIN_RFS	20000
#define AD5293_VDRAIN_BITS	10

#define AD5293_VDD			1
#define AD5293_VDD_VMIN		-28
#define AD5293_VDD_VMAX		-6
#define AD5293_VDD_VREF		-1.175
#define AD5293_VDD_R1		98800
#define AD5293_VDD_R2P		4320
#define AD5293_VDD_RFS		20000
#define AD5293_VDD_BITS		10

#define AD5293_VR			2
#define AD5293_VR_VMIN		-15
#define AD5293_VR_VMAX		-6
#define AD5293_VR_VREF		-1.175
#define AD5293_VR_R1		98800
#define AD5293_VR_R2P		7870
#define AD5293_VR_RFS		20000
#define AD5293_VR_BITS		10

#define AD5293_VSUB			3
#define AD5293_VSUB_VMIN	7
#define AD5293_VSUB_VMAX	88
#define AD5293_VSUB_VREF	1.175
#define AD5293_VSUB_R1		98800
#define AD5293_VSUB_R2P		1330
#define AD5293_VSUB_RFS		20000
#define AD5293_VSUB_BITS	10

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
int ad5293_init(bias_group_status_t *biases, uint32_t spi_device_id, uint32_t gpio_device_id);
int ad5293_rdac_wr(uint16_t data, uint32_t ss);
int ad5293_rdac_rd(uint16_t *data, uint32_t ss);
int ad5293_rst(uint32_t ss);
int ad5293_creg_wr(uint8_t c1, uint8_t c2, uint32_t ss);
int ad5293_creg_rd(uint8_t *c1, uint8_t *c2, uint32_t ss);
int ad5293_pdown(uint8_t d0, uint32_t ss);
int ad5293_sw_en(uint8_t sw, uint8_t en);

int ad5293_bias_status_init(bias_group_status_t *biases);
int ad5293_set_voltage(bias_status_t *bias, float value);
#endif


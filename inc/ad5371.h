/***************************************************************************//**
* @file ad5371.h
* @brief Header file of AD5371 Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
********************************************************************************
Based on Angel Soto .h file
*******************************************************************************/

/*
 * AD5371 is the DAC used to set the High and Low voltages of the clock signals
 * used to drive the CCD. The outputs of the DAC go through a switch that can be
 * enabled/disabled. The state of the switch, together with some pins connected
 * to the DAC are maintained in the structure gpio_dac_bits_state. Refer to the
 * functions below for an explanation of each field.
 *
 */
#ifndef AD5371_H_
#define AD5371_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/*****************************************************************************/
/*********************** Default values for registers ************************/
/*****************************************************************************/
/*
 * Equation:
 * 2^14 = 16384
 * VREF = 5
 *
 * VOUT = 4*GAIN*VREF*(DAC_CODE - OFFSET_CODE)/2^14
 *
 * DAC_CODE = VOUT*2^14/(4*GAIN*VREF) + OFFSET_CODE
 *
 */
#define AD5371_CH0_OFFSET_DEFAULT			8192	// +-10V span.
#define AD5371_CH1_OFFSET_DEFAULT			8192	// +-10V span.
#define AD5371_CH2_OFFSET_DEFAULT			8192	// +-10V span.
#define AD5371_CH3_OFFSET_DEFAULT			AD5371_CH2_OFFSET_DEFAULT
#define AD5371_CH4_OFFSET_DEFAULT			AD5371_CH2_OFFSET_DEFAULT

// Change this values if OFFSET is modified.
#define AD5371_VOLTAGE_MIN					-10
#define AD5371_VOLTAGE_MAX					10

// Verticals have a gain of 2, the other signals a gain of 1.
#define AD5371_VXC_GAIN		2
#define AD5371_ALL_GAIN		1
#define AD5371_VREF			5
#define AD5371_BITS			14

#define AD5371_CHANNEL_V1C_H_DEFAULT		10240 	// +5V.		(gain = 2)
#define AD5371_CHANNEL_V1C_L_DEFAULT		6963	// -3V. 	(gain = 2)
#define AD5371_CHANNEL_V2C_H_DEFAULT		10240	// +5V. 	(gain = 2)
#define AD5371_CHANNEL_V2C_L_DEFAULT		6963	// -3V. 	(gain = 2)
#define AD5371_CHANNEL_V3C_H_DEFAULT		10240	// +5V. 	(gain = 2)
#define AD5371_CHANNEL_V3C_L_DEFAULT		6963	// -3V. 	(gain = 2)
#define AD5371_CHANNEL_H1A_H_DEFAULT		14654	// +7.4V. 	(gain = 1)
#define AD5371_CHANNEL_H1A_L_DEFAULT		5898	// -2.8V. 	(gain = 1)

#define AD5371_CHANNEL_H1B_H_DEFAULT		14654	// +7.4V. 	(gain = 1)
#define AD5371_CHANNEL_H1B_L_DEFAULT		5898	// -2.8V. 	(gain = 1)
#define AD5371_CHANNEL_H2A_H_DEFAULT		14654	// +7.4V. 	(gain = 1)
#define AD5371_CHANNEL_H2A_L_DEFAULT		5898	// -2.8V. 	(gain = 1)
#define AD5371_CHANNEL_H2B_H_DEFAULT		14654	// +7.4V. 	(gain = 1)
#define AD5371_CHANNEL_H2B_L_DEFAULT		5898	// -2.8V. 	(gain = 1)
#define AD5371_CHANNEL_H3A_H_DEFAULT		14654	// +7.4V. 	(gain = 1)
#define AD5371_CHANNEL_H3A_L_DEFAULT		5898	// -2.8V. 	(gain = 1)

#define AD5371_CHANNEL_H3B_H_DEFAULT		14654	// +7.4V. 	(gain = 1)
#define AD5371_CHANNEL_H3B_L_DEFAULT		5898	// -2.8V. 	(gain = 1)
#define AD5371_CHANNEL_SWA_H_DEFAULT		12388	// +5V. 	(gain = 1)
#define AD5371_CHANNEL_SWA_L_DEFAULT		5406	// -3.4V. 	(gain = 1)
#define AD5371_CHANNEL_SWB_H_DEFAULT		12388	// +5V. 	(gain = 1)
#define AD5371_CHANNEL_SWB_L_DEFAULT		5406	// -3.4V. 	(gain = 1)
#define AD5371_CHANNEL_RGA_H_DEFAULT		8192	//  0V. 	(gain = 1)
#define AD5371_CHANNEL_RGA_L_DEFAULT		3932	// -5.2V. 	(gain = 1)

#define AD5371_CHANNEL_RGB_H_DEFAULT		8192	//  0V. 	(gain = 1)
#define AD5371_CHANNEL_RGB_L_DEFAULT		3932	// -5.2V. 	(gain = 1)
#define AD5371_CHANNEL_OGA_H_DEFAULT		11304	// +3.8V. 	(gain = 1)
#define AD5371_CHANNEL_OGA_L_DEFAULT		8192	//  0V. 	(gain = 1)
#define AD5371_CHANNEL_OGB_H_DEFAULT		11304	// +3.8V. 	(gain = 1)
#define AD5371_CHANNEL_OGB_L_DEFAULT		8192	//  0V. 	(gain = 1)
#define AD5371_CHANNEL_DG_H_DEFAULT			12388	// +5V. 	(gain = 1)
#define AD5371_CHANNEL_DG_L_DEFAULT			4196	// -5V. 	(gain = 1)

#define AD5371_CHANNEL_TG_H_DEFAULT			12697	// +5.5V. 	(gain = 1)
#define AD5371_CHANNEL_TG_L_DEFAULT			6144	// -2.5V. 	(gain = 1)
#define AD5371_CHANNEL_SPR_CLKA_H_DEFAULT	12388	// +5V. 	(gain = 1)
#define AD5371_CHANNEL_SPR_CLKA_L_DEFAULT	4196	// -5V. 	(gain = 1)

#define AD5371_CHANNEL_V1C_H				AD5371_G0_Ch0
#define AD5371_CHANNEL_V1C_L				AD5371_G0_Ch1
#define AD5371_CHANNEL_V2C_H				AD5371_G0_Ch2
#define AD5371_CHANNEL_V2C_L				AD5371_G0_Ch3
#define AD5371_CHANNEL_V3C_H				AD5371_G0_Ch4
#define AD5371_CHANNEL_V3C_L				AD5371_G0_Ch5
#define AD5371_CHANNEL_H1A_H				AD5371_G0_Ch6
#define AD5371_CHANNEL_H1A_L				AD5371_G0_Ch7

#define AD5371_CHANNEL_H1B_H				AD5371_G1_Ch0
#define AD5371_CHANNEL_H1B_L				AD5371_G1_Ch1
#define AD5371_CHANNEL_H2A_H				AD5371_G1_Ch2
#define AD5371_CHANNEL_H2A_L				AD5371_G1_Ch3
#define AD5371_CHANNEL_H2B_H				AD5371_G1_Ch4
#define AD5371_CHANNEL_H2B_L				AD5371_G1_Ch5
#define AD5371_CHANNEL_H3A_H				AD5371_G1_Ch6
#define AD5371_CHANNEL_H3A_L				AD5371_G1_Ch7

#define AD5371_CHANNEL_H3B_H				AD5371_G2_Ch0
#define AD5371_CHANNEL_H3B_L				AD5371_G2_Ch1
#define AD5371_CHANNEL_SWA_H				AD5371_G2_Ch2
#define AD5371_CHANNEL_SWA_L				AD5371_G2_Ch3
#define AD5371_CHANNEL_SWB_H				AD5371_G2_Ch4
#define AD5371_CHANNEL_SWB_L				AD5371_G2_Ch5
#define AD5371_CHANNEL_RGA_H				AD5371_G2_Ch6
#define AD5371_CHANNEL_RGA_L				AD5371_G2_Ch7

#define AD5371_CHANNEL_RGB_H				AD5371_G3_Ch0
#define AD5371_CHANNEL_RGB_L				AD5371_G3_Ch1
#define AD5371_CHANNEL_OGA_H				AD5371_G3_Ch2
#define AD5371_CHANNEL_OGA_L				AD5371_G3_Ch3
#define AD5371_CHANNEL_OGB_H				AD5371_G3_Ch4
#define AD5371_CHANNEL_OGB_L				AD5371_G3_Ch5
#define AD5371_CHANNEL_DG_H					AD5371_G3_Ch6
#define AD5371_CHANNEL_DG_L					AD5371_G3_Ch7

#define AD5371_CHANNEL_TG_H					AD5371_G4_Ch0
#define AD5371_CHANNEL_TG_L					AD5371_G4_Ch1
#define AD5371_CHANNEL_SPR_CLKA_H			AD5371_G4_Ch2
#define AD5371_CHANNEL_SPR_CLKA_L			AD5371_G4_Ch3

/******************************************************************************/
/********************** Modes, X, C and M registers      **********************/
/******************************************************************************/
#define AD5371_REG_X						0x03
#define AD5371_REG_C						0x02
#define AD5371_REG_M						0x01

/******************************************************************************/
/************************** X, C and M registers ******************************/
/******************************************************************************/
#define AD5371_G0_Ch0						0x08
#define AD5371_G0_Ch1						0x09
#define AD5371_G0_Ch2						0x0A
#define AD5371_G0_Ch3						0x0B
#define AD5371_G0_Ch4						0x0C
#define AD5371_G0_Ch5						0x0D
#define AD5371_G0_Ch6						0x0E
#define AD5371_G0_Ch7						0x0F

#define AD5371_G1_Ch0						0x10
#define AD5371_G1_Ch1						0x11
#define AD5371_G1_Ch2						0x12
#define AD5371_G1_Ch3						0x13
#define AD5371_G1_Ch4						0x14
#define AD5371_G1_Ch5						0x15
#define AD5371_G1_Ch6						0x16
#define AD5371_G1_Ch7						0x17

#define AD5371_G2_Ch0						0x18
#define AD5371_G2_Ch1						0x19
#define AD5371_G2_Ch2						0x1A
#define AD5371_G2_Ch3						0x1B
#define AD5371_G2_Ch4						0x1C
#define AD5371_G2_Ch5						0x1D
#define AD5371_G2_Ch6						0x1E
#define AD5371_G2_Ch7						0x1F

#define AD5371_G3_Ch0						0x20
#define AD5371_G3_Ch1						0x21
#define AD5371_G3_Ch2						0x22
#define AD5371_G3_Ch3						0x23
#define AD5371_G3_Ch4						0x24
#define AD5371_G3_Ch5						0x25
#define AD5371_G3_Ch6						0x26
#define AD5371_G3_Ch7						0x27

#define AD5371_G4_Ch0						0x28
#define AD5371_G4_Ch1						0x29
#define AD5371_G4_Ch2						0x2A
#define AD5371_G4_Ch3						0x2B
#define AD5371_G4_Ch4						0x2C
#define AD5371_G4_Ch5						0x2D
#define AD5371_G4_Ch6						0x2E
#define AD5371_G4_Ch7						0x2F

#define AD5371_G_all_Ch_all					0x00
#define AD5371_G0_Ch_all					0x01
#define AD5371_G1_Ch_all					0x02
#define AD5371_G2_Ch_all					0x03
#define AD5371_G3_Ch_all					0x04
#define AD5371_G4_Ch_all					0x05

#define AD5371_G_all_Ch0					0x30
#define AD5371_G_all_Ch1					0x31
#define AD5371_G_all_Ch2					0x32
#define AD5371_G_all_Ch3					0x33
#define AD5371_G_all_Ch4					0x34
#define AD5371_G_all_Ch5					0x35
#define AD5371_G_all_Ch6					0x36
#define AD5371_G_all_Ch7					0x37

#define AD5371_G_all_n0_Ch0					0x38
#define AD5371_G_all_n0_Ch1					0x39
#define AD5371_G_all_n0_Ch2					0x3A
#define AD5371_G_all_n0_Ch3					0x3B
#define AD5371_G_all_n0_Ch4					0x3C
#define AD5371_G_all_n0_Ch5					0x3D
#define AD5371_G_all_n0_Ch6					0x3E
#define AD5371_G_all_n0_Ch7					0x3F

#define AD5371_cmd_nop						0x00
#define AD5371_cmd_WCR						0x01
#define AD5371_cmd_OFS0						0x02
#define AD5371_cmd_OFS1						0x03
#define AD5371_cmd_OFS2						0x04
#define AD5371_cmd_Reg2read					0x05
#define AD5371_cmd_ABSelReg0				0x06
#define AD5371_cmd_ABSelReg1				0x07
#define AD5371_cmd_ABSelReg2				0x08
#define AD5371_cmd_ABSelReg3				0x09
#define AD5371_cmd_ABSelReg4				0x0A
#define AD5371_cmd_Block_ABSelReg			0x0B

/******************************************************************************/
/********************** Readback XA1, XB1, M and C ****************************/
/******************************************************************************/
#define AD5371_RB_X1AREG_BASE				0x0400
#define AD5371_RB_X1BREG_BASE				0x1400
#define AD5371_RB_CREG_BASE					0x4400
#define AD5371_RB_MREG_BASE					0x6400

/******************************************************************************/
/********************** Readback CR, OF and A/B registers**********************/
/******************************************************************************/
#define AD5371_RB_CR							0x8080
#define AD5371_RB_OFS0							0x8100
#define AD5371_RB_OFS1							0x8180
#define AD5371_RB_OFS2							0x8200
#define AD5371_RB_ABSR0							0x8300
#define AD5371_RB_ABSR1							0x8380
#define AD5371_RB_ABSR2							0x8400
#define AD5371_RB_ABSR3							0x8480
#define AD5371_RB_ABSR4							0x8500

#define AD5371_DATA_LOW_MASK					0x00FF
#define AD5371_DATA_HIGH_MASK					0xFF00

#define GPIO_DAC_DAC_LDAC_N_POSITION			0
#define GPIO_DAC_DAC_CLR_N_POSITION				1
#define GPIO_DAC_DAC_RESET_N_POSITION			2
#define GPIO_DAC_DAC_SW_EN_POSITION				3

#define GPIO_DAC_LDAC_ENABLE	0
#define GPIO_DAC_LDAC_DISABLE	1
#define GPIO_DAC_CLR_ENABLE		0
#define GPIO_DAC_CLR_DISABLE	1
#define GPIO_DAC_RESET_ENABLE	0
#define GPIO_DAC_RESET_DISABLE	1
#define GPIO_DAC_SW_EN_ENABLE	0
#define GPIO_DAC_SW_EN_DISABLE 	1

/******************************************************************************/
/********************** CLOCK STATUS CONSTANTS*********************************/
/******************************************************************************/

typedef struct {
	uint8_t status;
	uint8_t min;
	uint8_t max;
	uint8_t bit_position;
	char name[10];
} clk_sw_status_t;

typedef struct {
	clk_sw_status_t ldac_n;
	clk_sw_status_t clr_n;
	clk_sw_status_t reset_n;
	clk_sw_status_t sw_en;
} clk_sw_group_status_t;

typedef struct {
	clk_sw_group_status_t sw_group;
	uint16_t state;
} clk_sw_t;

typedef struct {
	float value;
	uint8_t reg;
	float vmin;
	float vmax;
	float vref;
	float offset;
	float gain;
	float max_code;
	char name[10];
} clk_status_t;

//Variable for maintaining the state of the clocks
typedef struct {
	clk_status_t v1ch;
	clk_status_t v1cl;
	clk_status_t v2ch;
	clk_status_t v2cl;
	clk_status_t v3ch;
	clk_status_t v3cl;
	clk_status_t h1ah;
	clk_status_t h1al;
	clk_status_t h1bh;
	clk_status_t h1bl;
	clk_status_t h2ah;
	clk_status_t h2al;
	clk_status_t h2bh;
	clk_status_t h2bl;
	clk_status_t h3ah;
	clk_status_t h3al;
	clk_status_t h3bh;
	clk_status_t h3bl;
	clk_status_t swah;
	clk_status_t swal;
	clk_status_t swbh;
	clk_status_t swbl;
	clk_status_t rgah;
	clk_status_t rgal;
	clk_status_t rgbh;
	clk_status_t rgbl;
	clk_status_t ogah;
	clk_status_t ogal;
	clk_status_t ogbh;
	clk_status_t ogbl;
	clk_status_t dgh;
	clk_status_t dgl;
	clk_status_t tgh;
	clk_status_t tgl;
	clk_status_t clkah;
	clk_status_t clkal;
} clk_group_status_t;

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int ad5371_init(clk_sw_t * clk_sw, clk_group_status_t *clks, uint32_t spi_device_id, uint32_t gpio_device_id);
int ad5371_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t n);
int ad5371_read_xcm(uint16_t addr_base, uint16_t channel, uint16_t *data);
int ad5371_write(uint8_t reg_mode, uint8_t reg_addr , uint16_t reg_data);
int ad5371_write_sf(uint8_t cmd , clk_status_t *clk);

int ad5371_change_switch_status(clk_sw_status_t * clk_sw_status, uint16_t *state, const uint8_t new_status);
int ad5371_set_voltage(clk_status_t *clk, float value);

#endif

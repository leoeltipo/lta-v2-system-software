/***************************************************************************//**
* @file DAC.h
* @brief Header file of DAC Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
********************************************************************************
Based on Angel Soto .h file
*******************************************************************************/

/*
 * DAC is the DAC used to set the High and Low voltages of the clock signals
 * used to drive the CCD. The outputs of the DAC go through a switch that can be
 * enabled/disabled. The state of the switch, together with some pins connected
 * to the DAC are maintained in the structure gpio_dac_bits_state. Refer to the
 * functions below for an explanation of each field.
 *
 */
#ifndef DAC_H_
#define DAC_H_

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
#define DAC_CH0_OFFSET_DEFAULT			8192	// +-10V span.
#define DAC_CH1_OFFSET_DEFAULT			8192	// +-10V span.
#define DAC_CH2_OFFSET_DEFAULT			8192	// +-10V span.
#define DAC_CH3_OFFSET_DEFAULT			DAC_CH2_OFFSET_DEFAULT
#define DAC_CH4_OFFSET_DEFAULT			DAC_CH2_OFFSET_DEFAULT

// Change this values if OFFSET is modified.
#define DAC_VOLTAGE_MIN					-10
#define DAC_VOLTAGE_MAX					10

// Verticals have a gain of 2, the other signals a gain of 1.
#define DAC_VXC_GAIN		2
#define DAC_ALL_GAIN		1
#define DAC_VREF			5
#define DAC_BITS			14

/******************************************************************************/
/********************** Modes, X, C and M registers      **********************/
/******************************************************************************/
#define DAC_REG_X						0x03
#define DAC_REG_C						0x02
#define DAC_REG_M						0x01

/******************************************************************************/
/************************** X, C and M registers ******************************/
/******************************************************************************/
#define DAC_G0_Ch0						0x08
#define DAC_G0_Ch1						0x09
#define DAC_G0_Ch2						0x0A
#define DAC_G0_Ch3						0x0B
#define DAC_G0_Ch4						0x0C
#define DAC_G0_Ch5						0x0D
#define DAC_G0_Ch6						0x0E
#define DAC_G0_Ch7						0x0F

#define DAC_G1_Ch0						0x10
#define DAC_G1_Ch1						0x11
#define DAC_G1_Ch2						0x12
#define DAC_G1_Ch3						0x13
#define DAC_G1_Ch4						0x14
#define DAC_G1_Ch5						0x15
#define DAC_G1_Ch6						0x16
#define DAC_G1_Ch7						0x17

#define DAC_G2_Ch0						0x18
#define DAC_G2_Ch1						0x19
#define DAC_G2_Ch2						0x1A
#define DAC_G2_Ch3						0x1B
#define DAC_G2_Ch4						0x1C
#define DAC_G2_Ch5						0x1D
#define DAC_G2_Ch6						0x1E
#define DAC_G2_Ch7						0x1F

#define DAC_G3_Ch0						0x20
#define DAC_G3_Ch1						0x21
#define DAC_G3_Ch2						0x22
#define DAC_G3_Ch3						0x23
#define DAC_G3_Ch4						0x24
#define DAC_G3_Ch5						0x25
#define DAC_G3_Ch6						0x26
#define DAC_G3_Ch7						0x27

#define DAC_G4_Ch0						0x28
#define DAC_G4_Ch1						0x29
#define DAC_G4_Ch2						0x2A
#define DAC_G4_Ch3						0x2B
#define DAC_G4_Ch4						0x2C
#define DAC_G4_Ch5						0x2D
#define DAC_G4_Ch6						0x2E
#define DAC_G4_Ch7						0x2F

#define DAC_G_all_Ch_all					0x00
#define DAC_G0_Ch_all					0x01
#define DAC_G1_Ch_all					0x02
#define DAC_G2_Ch_all					0x03
#define DAC_G3_Ch_all					0x04
#define DAC_G4_Ch_all					0x05

#define DAC_G_all_Ch0					0x30
#define DAC_G_all_Ch1					0x31
#define DAC_G_all_Ch2					0x32
#define DAC_G_all_Ch3					0x33
#define DAC_G_all_Ch4					0x34
#define DAC_G_all_Ch5					0x35
#define DAC_G_all_Ch6					0x36
#define DAC_G_all_Ch7					0x37

#define DAC_G_all_n0_Ch0					0x38
#define DAC_G_all_n0_Ch1					0x39
#define DAC_G_all_n0_Ch2					0x3A
#define DAC_G_all_n0_Ch3					0x3B
#define DAC_G_all_n0_Ch4					0x3C
#define DAC_G_all_n0_Ch5					0x3D
#define DAC_G_all_n0_Ch6					0x3E
#define DAC_G_all_n0_Ch7					0x3F

#define DAC_cmd_nop						0x00
#define DAC_cmd_WCR						0x01
#define DAC_cmd_OFS0						0x02
#define DAC_cmd_OFS1						0x03
#define DAC_cmd_OFS2						0x04
#define DAC_cmd_Reg2read					0x05
#define DAC_cmd_ABSelReg0				0x06
#define DAC_cmd_ABSelReg1				0x07
#define DAC_cmd_ABSelReg2				0x08
#define DAC_cmd_ABSelReg3				0x09
#define DAC_cmd_ABSelReg4				0x0A
#define DAC_cmd_Block_ABSelReg			0x0B

/******************************************************************************/
/********************** Readback XA1, XB1, M and C ****************************/
/******************************************************************************/
#define DAC_RB_X1AREG_BASE				0x0400
#define DAC_RB_X1BREG_BASE				0x1400
#define DAC_RB_CREG_BASE					0x4400
#define DAC_RB_MREG_BASE					0x6400

/******************************************************************************/
/********************** Readback CR, OF and A/B registers**********************/
/******************************************************************************/
#define DAC_RB_CR							0x8080
#define DAC_RB_OFS0							0x8100
#define DAC_RB_OFS1							0x8180
#define DAC_RB_OFS2							0x8200
#define DAC_RB_ABSR0							0x8300
#define DAC_RB_ABSR1							0x8380
#define DAC_RB_ABSR2							0x8400
#define DAC_RB_ABSR3							0x8480
#define DAC_RB_ABSR4							0x8500

#define DAC_DATA_LOW_MASK					0x00FF
#define DAC_DATA_HIGH_MASK					0xFF00

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
	clk_status_t v1ah;
	clk_status_t v1al;
	clk_status_t v1bh;
	clk_status_t v1bl;
	clk_status_t v2ch;
	clk_status_t v2cl;
	clk_status_t v3ah;
	clk_status_t v3al;
	clk_status_t v3bh;
	clk_status_t v3bl;
	clk_status_t h1ah;
	clk_status_t h1al;
	clk_status_t h1bh;
	clk_status_t h1bl;
	clk_status_t h2ch;
	clk_status_t h2cl;
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
	clk_status_t dgah;
	clk_status_t dgal;
	clk_status_t dgbh;
	clk_status_t dgbl;
	clk_status_t tgah;
	clk_status_t tgal;
	clk_status_t tgbh;
	clk_status_t tgbl;
} clk_group_status_t;

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int dac_init(clk_sw_t * clk_sw, clk_group_status_t *clks, uint32_t spi_device_id, uint32_t gpio_device_id);
int dac_read(uint16_t reg_addr, uint8_t *reg_data, uint16_t n);
int dac_read_xcm(uint16_t addr_base, uint16_t channel, uint16_t *data);
int dac_write(uint8_t reg_mode, uint8_t reg_addr , uint16_t reg_data);
int dac_write_sf(uint8_t cmd , clk_status_t *clk);

int dac_change_switch_status(clk_sw_status_t * clk_sw_status, uint16_t *state, const uint8_t new_status);
int dac_set_voltage(clk_status_t *clk, float value);

#endif

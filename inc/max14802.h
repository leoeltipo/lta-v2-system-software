/*
* @file max14802.h
* @brief Header file of MAX14802 Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
*
* The MAX14802 is the Voltage Switch for bias supplies. Each voltage input
* can be enabled/disabled at the output 50-pin connector with this component.
*
* The structure volt_sw_state is used to maintain the state of the switches. This
* structure is updated before writing into the switch. It is important to respect
* this mechanism as the MAX14802 does not provide the possibility to read the state
* of the switches.
*
* Another structure gpio_volt_sw_state is also defined to maintain the state of two
* GPIOs connected to the microblaze to reset or disable the switch.
 */

#ifndef MAX14802_H_
#define MAX14802_H_

#define VOLT_SW_CCD_VDD_SWITCH		0
#define VOLT_SW_CCD_VDRAIN_SWITCH	2
#define VOLT_SW_CCD_VSUB_SWITCH		5
#define VOLT_SW_CCD_VR_SWITCH		10
#define VOLT_SW_P15V_SWITCH			13	// +15V
#define VOLT_SW_M15V_SWITCH			15	// -15V

#define MAX14802_DATA_LOW_MASK	0x00FF
#define MAX14802_DATA_HIGH_MASK	0xFF00

#define GPIO_VOLT_SW_CLR			0
#define GPIO_VOLT_SW_LE_N			1

typedef struct {
	uint8_t status;
	uint8_t min;
	uint8_t max;
	uint8_t bit_position;
	char name[10];
} bias_sw_status_t;

typedef struct {
	bias_sw_status_t clr;
	bias_sw_status_t le_n;
} gpio_sw_group_status_t;

typedef struct {
	gpio_sw_group_status_t sw_group;
	uint16_t state;
} gpio_sw_t;

typedef struct {
	bias_sw_status_t ccd_vdd_sw;
	bias_sw_status_t ccd_vdrain_sw;
	bias_sw_status_t ccd_vsub_sw;
	bias_sw_status_t ccd_vr_sw;
	bias_sw_status_t p15v_sw;
	bias_sw_status_t m15v_sw;
} bias_sw_group_status_t;

typedef struct {
	bias_sw_group_status_t sw_group;
	uint16_t state;
} bias_sw_t;

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

int max14802_init(uint32_t spi_device_id, uint32_t gpio_device_id, bias_sw_t *bias_sw, gpio_sw_t *gpio_sw);
void max14802_clear();
int max14802_wr(uint16_t data);
int max14802_rd(uint16_t *data);
int max14802_volt_sw_state_set(bias_sw_status_t *bias_sw_status, uint16_t *state, uint8_t val);
uint8_t max14802_volt_sw_state_get(bias_sw_status_t *bias_sw_status);
int max14802_volt_sw_transfer(uint16_t *state);

#endif /* SRC_VOLT_SWITCH_H_ */

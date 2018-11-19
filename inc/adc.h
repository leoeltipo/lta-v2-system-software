/*
 * adc.h
 * @file adc.h
 * @brief Driver for 15 MHz adc controller.
 * @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 *
 * A/D controller that allows reaching the maximum sampling frequency of
 * 15 MHz for the analog-to-digital converters used in the video signal
 * acquisition path (LT2387-18).
 *
 * Due to a bug in the routing of the signals, it is necessary to execute
 * a calibration routing for these converters. The routine requires the
 * four channels to be set into TEST_PATTERN mode, and then to activate
 * the BITSLIP mode. With this two steps, the block will search for the
 * known test pattern in the incoming data and set the output data position
 * to the right value. If this calibration is not done, data might be not
 * valid during normal operating mode.
 *
 * NOTE: this routing should be executed before using the A/D for data
 * acquisition. The routing should be executed after powering up the
 * board and after reset.
 *
 * To execute the routing, copy and paste the following lines in the
 * board initialization code:
 *
 * adc_change_sw_status(&(adc_var.sw_group.cha_test_pattern), &(adc_var.state), ADC_TEST_ON);
 * adc_change_sw_status(&(adc_var.sw_group.chb_test_pattern), &(adc_var.state), ADC_TEST_ON);
 * adc_change_sw_status(&(adc_var.sw_group.chc_test_pattern), &(adc_var.state), ADC_TEST_ON);
 * adc_change_sw_status(&(adc_var.sw_group.chd_test_pattern), &(adc_var.state), ADC_TEST_ON);
 *
 * adc_change_sw_status(&(adc_var.sw_group.cha_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_ON);
 * adc_change_sw_status(&(adc_var.sw_group.chb_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_ON);
 * adc_change_sw_status(&(adc_var.sw_group.chc_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_ON);
 * adc_change_sw_status(&(adc_var.sw_group.chd_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_ON);
 *
 * 2-3 seconds delay.
 *
 * adc_change_sw_status(&(adc_var.sw_group.cha_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_OFF);
 * adc_change_sw_status(&(adc_var.sw_group.chb_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_OFF);
 * adc_change_sw_status(&(adc_var.sw_group.chc_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_OFF);
 * adc_change_sw_status(&(adc_var.sw_group.chd_send_bitslip), &(adc_var.state), ADC_SEND_BITSLIP_OFF);
 *
 * adc_change_sw_status(&(adc_var.sw_group.cha_test_pattern), &(adc_var.state), ADC_TEST_OFF);
 * adc_change_sw_status(&(adc_var.sw_group.chb_test_pattern), &(adc_var.state), ADC_TEST_OFF);
 * adc_change_sw_status(&(adc_var.sw_group.chc_test_pattern), &(adc_var.state), ADC_TEST_OFF);
 * adc_change_sw_status(&(adc_var.sw_group.chd_test_pattern), &(adc_var.state), ADC_TEST_OFF);
 *
 * adc_var variable should be replaced by the actual variable.
 *
 * #################
 * ### Registers ###
 * #################
 * chx_enable 		: enable/disable the acquisition for channel x.
 * 	-> 0 : acquisition disabled.
 * 	-> 1 : acquisition enabled.
 *
 * chx_test_pattern	: enable/disable test mode for channel x.
 * 	-> 0 : test pattern mode disabled.
 * 	-> 1 : test pattern mode enabled.
 *
 * chx_send_bitslip : enable/disable bitslip mode for channel x.
 * 	-> 0 : bitslip mode disabled.
 * 	-> 1 : bitslip mode enabled.
 *
 * chx_pd_n			: power down channel x.
 * 	-> 0 : power down mode enabled.
 * 	-> 1 : power down mode disabled.
 */

#ifndef ADC_H_
#define ADC_H_

#define GPIO_ADC_CHA_ENABLE			0
#define GPIO_ADC_CHA_TEST_PATTERN 	1
#define GPIO_ADC_CHA_SEND_BITSLIP	2
#define GPIO_ADC_CHA_PD_N 			3

#define GPIO_ADC_CHB_ENABLE			4
#define GPIO_ADC_CHB_TEST_PATTERN 	5
#define GPIO_ADC_CHB_SEND_BITSLIP	6
#define GPIO_ADC_CHB_PD_N 			7

#define GPIO_ADC_CHC_ENABLE			8
#define GPIO_ADC_CHC_TEST_PATTERN 	9
#define GPIO_ADC_CHC_SEND_BITSLIP	10
#define GPIO_ADC_CHC_PD_N 			11

#define GPIO_ADC_CHD_ENABLE			12
#define GPIO_ADC_CHD_TEST_PATTERN 	13
#define GPIO_ADC_CHD_SEND_BITSLIP	14
#define GPIO_ADC_CHD_PD_N 			15

#define ADC_ENABLE		1
#define	ADC_DISABLE		0

#define ADC_PDOWN_ON	0
#define ADC_PDOWN_OFF	1

#define ADC_TEST_ON		1
#define ADC_TEST_OFF	0

#define ADC_SEND_BITSLIP_ON 	1
#define ADC_SEND_BITSLIP_OFF 	0

#define ADC_CHA	0
#define ADC_CHB	1
#define ADC_CHC	2
#define ADC_CHD	3

typedef struct {
	uint8_t status;
	uint8_t min;
	uint8_t max;
	uint8_t bit_position;
	char name[10];
} adc_sw_status_t;

typedef struct {
	// Channel A.
	adc_sw_status_t cha_enable;
	adc_sw_status_t cha_test_pattern;
	adc_sw_status_t cha_send_bitslip;
	adc_sw_status_t cha_pd_n;

	// Channel b.
	adc_sw_status_t chb_enable;
	adc_sw_status_t chb_test_pattern;
	adc_sw_status_t chb_send_bitslip;
	adc_sw_status_t chb_pd_n;

	// Channel C.
	adc_sw_status_t chc_enable;
	adc_sw_status_t chc_test_pattern;
	adc_sw_status_t chc_send_bitslip;
	adc_sw_status_t chc_pd_n;

	// Channel D.
	adc_sw_status_t chd_enable;
	adc_sw_status_t chd_test_pattern;
	adc_sw_status_t chd_send_bitslip;
	adc_sw_status_t chd_pd_n;
} adc_sw_group_status_t;

typedef struct {
	adc_sw_group_status_t sw_group;
	uint16_t state;
} gpio_adc_t;

int adc_init(uint32_t gpio_device_id, gpio_adc_t *gpio_adc);
int adc_change_sw_status(adc_sw_status_t *adc_sw_status, uint16_t *state,uint8_t value);

#endif

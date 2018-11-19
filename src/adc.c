/*
* @file adc.c
* @brief Implementation of ADC Driver.
* @author Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <xgpio.h>
#include "adc.h"
#include "interrupt.h"
#include "io_func.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
// XGpio device driver variables.
XGpio gpio_adc_i;

/********************
* @brief volt_sw_init
*********************/
int adc_init(uint32_t gpio_device_id, gpio_adc_t *gpio_adc)
{
	int ret;

	// Init gpio_dac_i structure.
	ret = XGpio_Initialize(&gpio_adc_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * 0 ENABLE
	 * 1 TEST_PATTERN
	 * 2 SEND_BITSLIP
	 * 3 PD_N
	 *
	 * After Init function:
	 * 	ENABLE 			= 1	: enable adc controller (all 4 channels).
	 * 	TEST_PATTERN	= 0 : disable test mode.
	 * 	SEND_BITSLIP	= 0	: disable bitslip function.
	 * 	PD_N			= 1 : pull Power Down Pin to logic '1' to enable LT2387.
	 *
	 */

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_adc_i, 1, 0x0);

	// Initialize structure.
	// Channel A.
	strcpy(gpio_adc->sw_group.cha_enable.name, "enA");
	gpio_adc->sw_group.cha_enable.status 				= ADC_ENABLE;
	gpio_adc->sw_group.cha_enable.bit_position 			= GPIO_ADC_CHA_ENABLE;
	gpio_adc->sw_group.cha_enable.min 					= 0;
	gpio_adc->sw_group.cha_enable.max 					= 1;

	strcpy(gpio_adc->sw_group.cha_test_pattern.name, "testPtrnA");
	gpio_adc->sw_group.cha_test_pattern.status 			= ADC_TEST_OFF;
	gpio_adc->sw_group.cha_test_pattern.bit_position 	= GPIO_ADC_CHA_TEST_PATTERN;
	gpio_adc->sw_group.cha_test_pattern.min 			= 0;
	gpio_adc->sw_group.cha_test_pattern.max 			= 1;

	strcpy(gpio_adc->sw_group.cha_send_bitslip.name, "bitSlipA");
	gpio_adc->sw_group.cha_send_bitslip.status 			= ADC_SEND_BITSLIP_OFF;
	gpio_adc->sw_group.cha_send_bitslip.bit_position 	= GPIO_ADC_CHA_SEND_BITSLIP;
	gpio_adc->sw_group.cha_send_bitslip.min = 0;
	gpio_adc->sw_group.cha_send_bitslip.max = 1;

	strcpy(gpio_adc->sw_group.cha_pd_n.name, "pdA");
	gpio_adc->sw_group.cha_pd_n.status 					= ADC_PDOWN_OFF;
	gpio_adc->sw_group.cha_pd_n.bit_position 			= GPIO_ADC_CHA_PD_N;
	gpio_adc->sw_group.cha_pd_n.min 					= 0;
	gpio_adc->sw_group.cha_pd_n.max 					= 1;

	// Channel B.
	strcpy(gpio_adc->sw_group.chb_enable.name, "enB");
	gpio_adc->sw_group.chb_enable.status 				= ADC_ENABLE;
	gpio_adc->sw_group.chb_enable.bit_position 			= GPIO_ADC_CHB_ENABLE;
	gpio_adc->sw_group.chb_enable.min 					= 0;
	gpio_adc->sw_group.chb_enable.max 					= 1;

	strcpy(gpio_adc->sw_group.chb_test_pattern.name, "testPtrnB");
	gpio_adc->sw_group.chb_test_pattern.status 			= ADC_TEST_OFF;
	gpio_adc->sw_group.chb_test_pattern.bit_position 	= GPIO_ADC_CHB_TEST_PATTERN;
	gpio_adc->sw_group.chb_test_pattern.min 			= 0;
	gpio_adc->sw_group.chb_test_pattern.max 			= 1;

	strcpy(gpio_adc->sw_group.chb_send_bitslip.name, "bitSlipB");
	gpio_adc->sw_group.chb_send_bitslip.status 			= ADC_SEND_BITSLIP_OFF;
	gpio_adc->sw_group.chb_send_bitslip.bit_position 	= GPIO_ADC_CHB_SEND_BITSLIP;
	gpio_adc->sw_group.chb_send_bitslip.min = 0;
	gpio_adc->sw_group.chb_send_bitslip.max = 1;

	strcpy(gpio_adc->sw_group.chb_pd_n.name, "pdB");
	gpio_adc->sw_group.chb_pd_n.status 					= ADC_PDOWN_OFF;
	gpio_adc->sw_group.chb_pd_n.bit_position 			= GPIO_ADC_CHB_PD_N;
	gpio_adc->sw_group.chb_pd_n.min 					= 0;
	gpio_adc->sw_group.chb_pd_n.max 					= 1;

	// Channel C.
	strcpy(gpio_adc->sw_group.chc_enable.name, "enC");
	gpio_adc->sw_group.chc_enable.status 				= ADC_ENABLE;
	gpio_adc->sw_group.chc_enable.bit_position 			= GPIO_ADC_CHC_ENABLE;
	gpio_adc->sw_group.chc_enable.min 					= 0;
	gpio_adc->sw_group.chc_enable.max 					= 1;

	strcpy(gpio_adc->sw_group.chc_test_pattern.name, "testPtrnC");
	gpio_adc->sw_group.chc_test_pattern.status 			= ADC_TEST_OFF;
	gpio_adc->sw_group.chc_test_pattern.bit_position 	= GPIO_ADC_CHC_TEST_PATTERN;
	gpio_adc->sw_group.chc_test_pattern.min 			= 0;
	gpio_adc->sw_group.chc_test_pattern.max 			= 1;

	strcpy(gpio_adc->sw_group.chc_send_bitslip.name, "bitSlipC");
	gpio_adc->sw_group.chc_send_bitslip.status 			= ADC_SEND_BITSLIP_OFF;
	gpio_adc->sw_group.chc_send_bitslip.bit_position 	= GPIO_ADC_CHC_SEND_BITSLIP;
	gpio_adc->sw_group.chc_send_bitslip.min = 0;
	gpio_adc->sw_group.chc_send_bitslip.max = 1;

	strcpy(gpio_adc->sw_group.chc_pd_n.name, "pdC");
	gpio_adc->sw_group.chc_pd_n.status 					= ADC_PDOWN_OFF;
	gpio_adc->sw_group.chc_pd_n.bit_position 			= GPIO_ADC_CHC_PD_N;
	gpio_adc->sw_group.chc_pd_n.min 					= 0;
	gpio_adc->sw_group.chc_pd_n.max 					= 1;

	// Channel D.
	strcpy(gpio_adc->sw_group.chd_enable.name, "enD");
	gpio_adc->sw_group.chd_enable.status 				= ADC_ENABLE;
	gpio_adc->sw_group.chd_enable.bit_position 			= GPIO_ADC_CHD_ENABLE;
	gpio_adc->sw_group.chd_enable.min 					= 0;
	gpio_adc->sw_group.chd_enable.max 					= 1;

	strcpy(gpio_adc->sw_group.chd_test_pattern.name, "testPtrnD");
	gpio_adc->sw_group.chd_test_pattern.status 			= ADC_TEST_OFF;
	gpio_adc->sw_group.chd_test_pattern.bit_position 	= GPIO_ADC_CHD_TEST_PATTERN;
	gpio_adc->sw_group.chd_test_pattern.min 			= 0;
	gpio_adc->sw_group.chd_test_pattern.max 			= 1;

	strcpy(gpio_adc->sw_group.chd_send_bitslip.name, "bitSlipD");
	gpio_adc->sw_group.chd_send_bitslip.status 			= ADC_SEND_BITSLIP_OFF;
	gpio_adc->sw_group.chd_send_bitslip.bit_position 	= GPIO_ADC_CHD_SEND_BITSLIP;
	gpio_adc->sw_group.chd_send_bitslip.min = 0;
	gpio_adc->sw_group.chd_send_bitslip.max = 1;

	strcpy(gpio_adc->sw_group.chd_pd_n.name, "pdD");
	gpio_adc->sw_group.chd_pd_n.status 					= ADC_PDOWN_OFF;
	gpio_adc->sw_group.chd_pd_n.bit_position 			= GPIO_ADC_CHD_PD_N;
	gpio_adc->sw_group.chd_pd_n.min 					= 0;
	gpio_adc->sw_group.chd_pd_n.max 					= 1;

	// Write values into hardware.

	gpio_adc->state = 0;

	// Channel A.
	adc_change_sw_status(&(gpio_adc->sw_group.cha_enable)		, &(gpio_adc->state), gpio_adc->sw_group.cha_enable.status);
	adc_change_sw_status(&(gpio_adc->sw_group.cha_test_pattern)	, &(gpio_adc->state), gpio_adc->sw_group.cha_test_pattern.status);
	adc_change_sw_status(&(gpio_adc->sw_group.cha_send_bitslip)	, &(gpio_adc->state), gpio_adc->sw_group.cha_send_bitslip.status);
	adc_change_sw_status(&(gpio_adc->sw_group.cha_pd_n)			, &(gpio_adc->state), gpio_adc->sw_group.cha_pd_n.status);

	// Channel B.
	adc_change_sw_status(&(gpio_adc->sw_group.chb_enable)		, &(gpio_adc->state), gpio_adc->sw_group.chb_enable.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_test_pattern)	, &(gpio_adc->state), gpio_adc->sw_group.chb_test_pattern.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_send_bitslip)	, &(gpio_adc->state), gpio_adc->sw_group.chb_send_bitslip.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_pd_n)			, &(gpio_adc->state), gpio_adc->sw_group.chb_pd_n.status);

	// Channel C.
	adc_change_sw_status(&(gpio_adc->sw_group.chc_enable)		, &(gpio_adc->state), gpio_adc->sw_group.chc_enable.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_test_pattern)	, &(gpio_adc->state), gpio_adc->sw_group.chc_test_pattern.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_send_bitslip)	, &(gpio_adc->state), gpio_adc->sw_group.chc_send_bitslip.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_pd_n)			, &(gpio_adc->state), gpio_adc->sw_group.chc_pd_n.status);

	// Channel D.
	adc_change_sw_status(&(gpio_adc->sw_group.chd_enable)		, &(gpio_adc->state), gpio_adc->sw_group.chd_enable.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_test_pattern)	, &(gpio_adc->state), gpio_adc->sw_group.chd_test_pattern.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_send_bitslip)	, &(gpio_adc->state), gpio_adc->sw_group.chd_send_bitslip.status);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_pd_n)			, &(gpio_adc->state), gpio_adc->sw_group.chd_pd_n.status);

	// A/D Calibration routine.
	adc_change_sw_status(&(gpio_adc->sw_group.cha_test_pattern)	, &(gpio_adc->state), ADC_TEST_ON);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_test_pattern)	, &(gpio_adc->state), ADC_TEST_ON);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_test_pattern)	, &(gpio_adc->state), ADC_TEST_ON);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_test_pattern)	, &(gpio_adc->state), ADC_TEST_ON);

	adc_change_sw_status(&(gpio_adc->sw_group.cha_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_ON);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_ON);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_ON);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_ON);

	tdelay_s(5);

	adc_change_sw_status(&(gpio_adc->sw_group.cha_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_OFF);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_OFF);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_OFF);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_send_bitslip)	, &(gpio_adc->state), ADC_SEND_BITSLIP_OFF);

	adc_change_sw_status(&(gpio_adc->sw_group.cha_test_pattern)	, &(gpio_adc->state), ADC_TEST_OFF);
	adc_change_sw_status(&(gpio_adc->sw_group.chb_test_pattern)	, &(gpio_adc->state), ADC_TEST_OFF);
	adc_change_sw_status(&(gpio_adc->sw_group.chc_test_pattern)	, &(gpio_adc->state), ADC_TEST_OFF);
	adc_change_sw_status(&(gpio_adc->sw_group.chd_test_pattern)	, &(gpio_adc->state), ADC_TEST_OFF);

	return ret;
}

int adc_change_sw_status(adc_sw_status_t *adc_sw_status, uint16_t *state,uint8_t value)
{
	if (value >= adc_sw_status->min && value <= adc_sw_status->max)
	{
		adc_sw_status->status = value;
	}
	else
	{
		return -1;
	}

	if (value)
	{
		*state |= (1 << adc_sw_status->bit_position);
	} else {
		*state &= ~(1 << adc_sw_status->bit_position);
	}

	// Write value to gpio.
	XGpio_DiscreteWrite(&gpio_adc_i, 1, (uint32_t) *state);

	return 0;
}

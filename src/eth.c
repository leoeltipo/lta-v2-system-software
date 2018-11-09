/*
 * eth.c
 *
 * Author: Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */

#include <stdio.h>
#include <xgpio.h>
#include "xparameters.h"
#include "interrupt.h"
#include "eth.h"

// XGpio device driver variables.
XGpio gpio_eth_i;

void eth_init(uint32_t gpio_device_id, eth_t *eth)
{
	int ret;

	// Init gpio_eth_i structure.
	ret = XGpio_Initialize(&gpio_eth_i, gpio_device_id);
	if (ret != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the direction for all signals to be outputs
	XGpio_SetDataDirection(&gpio_eth_i, 1, 0x0);

	// Initialize structure.
	strcpy(eth->ip_low.name, "ipLow");
	eth->ip_low.min = ETH_IP_LOW_MIN;
	eth->ip_low.max = ETH_IP_LOW_MAX;
	eth->ip_low.status = ETH_IP_LOW_DEFAULT;

	// Set value to hardware.
	gpio_eth_change_state(&(eth->ip_low), eth->ip_low.status);

	// Define void ptr to be able to point to any structure type.
	void * ptr;

	// First address of the low speed memory defined in xparameters.h.
	ptr = (void *)XPAR_RAM_ETH_CTRL_S_AXI_BASEADDR;
	eth_mbus = (void *)ptr;

	ptr += sizeof(eth_bus_t);
	eth_sbus = (void *)ptr;

	ptr += sizeof(eth_bus_t);
	eth_mdata = (void *)ptr;

	ptr += sizeof(eth_data_t);
	eth_sdata = (void *)ptr;

	// Initialize flags.
	eth_mbus->dready = 0;
	eth_mbus->dack = 0;
	eth_mbus->dlength = 0;
	eth_sbus->dready = 0;
	eth_sbus->dack = 0;
	eth_sbus->dlength = 0;
}

unsigned int eth_mdata_get(uint8_t *buf)
{

	unsigned int length = 0;

	if (eth_mbus->dready == 0x78787878)
	{

		// Read length.
		//uint32_t l = eth_mbus->dlength;

		// Read data.
		length = strlen(eth_mdata->data);
		strcpy(buf,eth_mdata->data);

		// Set ack signal.
		eth_mbus->dack = 0xABABABAB;

		// Wait for dready to go to 0;
		int flag = 0;
		for (int i=0; i<100; i++)
		{
			if (eth_mbus->dready == 0xCDCDCDCD)
			{
				flag = 1;
				break;
			}
			tdelay_ms(10);
		}

		// Clear dready signal.
		if (!flag)
		{
			eth_mbus->dready = 0xCDCDCDCD;
			print("ERROR : ETH GET Command Handshake Incomplete\r\n");
			tdelay_s(5);
		}

		// Clear dack.
		eth_mbus->dack = 0xEFEFEFEF;
	}

	//print("Sale de eth_mdata_get\r\n");

	return length;
}

void eth_sdata_put( const char *str )
{
	// Copy data into memory.
	strcpy(eth_sdata->data, str);

	// Set length.
	uint32_t l = strlen(str);
	eth_sbus->dlength = l;

	// Set dready.
	eth_sbus->dready = 0x78787878;

	// Wait for ack to go to 1;
	int flag = 0;
	for (int i=0; i<100; i++)
	{
		if (eth_sbus->dack == 0xABABABAB)
		{
			flag = 1;
			break;
		}
		tdelay_ms(10);
	}

	// If ack not received, set it back to 0.
	if (!flag)
	{
		eth_sbus->dack = 0xEFEFEFEF;
		print("ERROR : ETH PUT Command Handshake Incomplete\r\n");
		tdelay_s(5);

		// Clear dready and return.
		eth_sbus->dready = 0xCDCDCDCD;

		return;
	}

	// Clear dready.
	eth_sbus->dready = 0xCDCDCDCD;

	// Wait for ack to go to 0;
	flag = 0;
	for (int i=0; i<100; i++)
	{
		if (eth_sbus->dack == 0xEFEFEFEF)
		{
			flag = 1;
			break;
		}
		tdelay_ms(10);
	}

	// If ack not received, set it back to 0.
	if (!flag)
	{
		eth_sbus->dack = 0xEFEFEFEF;
		print("ERROR : ETH Command Handshake Incomplete\r\n");
		tdelay_s(5);

		// Clear dready and return.
		eth_sbus->dready = 0xCDCDCDCD;

		return;
	}

	// Clear dready.
	eth_sbus->dready = 0xCDCDCDCD;

	return;
}

int gpio_eth_change_state(eth_status_t *eth, uint8_t status)
{
	if (status >= eth->min && status <= eth->max)
	{
		// Update variable.
		eth->status = status;

		// Write gpio register.
		XGpio_DiscreteWrite(&gpio_eth_i, 1, status);

		return 0;
	}
	else
	{
		return -1;
	}
}

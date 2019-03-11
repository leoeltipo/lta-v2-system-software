/*
 * eth.c
 *
 * Author: Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */

#include <stdio.h>
#include <xgpio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "interrupt.h"
#include "eth.h"
#include "io_func.h"

// XGpio device driver variables.
XGpio gpio_eth_i;

int eth_init(uint32_t gpio_device_id, eth_t *eth)
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
	strcpy(eth->ipEth.name, "ipEth");
	eth->ipEth.min = ETH_IP_MIN;
	eth->ipEth.max = ETH_IP_MAX;
	eth->ipEth.val = ETH_IP_DEFAULT;
	eth_uint2ip(eth->ipEth.val, eth->ipEth.valStr);

	// Set value to hardware.
	gpio_eth_change_state(&(eth->ipEth), eth->ipEth.val);

	// Define void ptr to be able to point to any structure type.
	void * ptr;

	// First address of the low speed memory defined in xparameters.h.
	ptr = (void *)XPAR_ETH_HIE_RAM_ETH_CTRL_S_AXI_BASEADDR;
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

void eth_uint2ip(uint32_t ip, char *str)
{
	char ip0[4];
	char ip1[4];
	char ip2[4];
	char ip3[4];
	uint32_t ip_tmp = ip;

	io_uint2str((ip_tmp & 0xFF), ip0);
	ip_tmp >>= 8;
	io_uint2str((ip_tmp & 0xFF), ip1);
	ip_tmp >>= 8;
	io_uint2str((ip_tmp & 0xFF), ip2);
	ip_tmp >>= 8;
	io_uint2str((ip_tmp & 0xFF), ip3);

	io_sprintf(str, "%s.%s.%s.%s", ip3, ip2, ip1, ip0);
}

uint32_t eth_ip2uint(char *str)
{
	uint32_t ip = 0;
	eth_ip_words_t ipTerms[ETH_IP_TERMS];
	char *token;
	char *rest;

	// Clear ipTerms array.
	for (int i=0; i<ETH_IP_TERMS; i++)
	{
		for (int j=0; j<ETH_IP_TERM_LENGTH; j++)
		{
			ipTerms[i].word[j] = '\0';
		}
	}

	rest = str;

	// walk through other tokens
	int wordInd = 0;
	while( (token = strtok_r(rest, ".", &rest)) ) {
		//save word
		strcpy(ipTerms[wordInd].word,token);
		wordInd = wordInd + 1;
	}

	free(token);
	free(rest);

	// Convert to number.
	if (wordInd != 4)
	{
		return ip;
	}
	else
	{
		uint32_t tmp;
		for (int i=0; i<wordInd; i++)
		{
			tmp = (uint32_t) (atoi(ipTerms[wordInd-i-1].word));
			ip += (tmp << 8*i);
		}
	}

	return ip;
}

int eth_change_ip(eth_status_t *eth, char *ip)
{
	// Convert string with ip to uint32_t.
	uint32_t ip_u = eth_ip2uint(ip);

	// Update values in register structure.
	eth->val = ip_u;
	eth_uint2ip(ip_u, eth->valStr);

	// Upgrade value into hardware.
	gpio_eth_change_state(eth, ip_u);

	return 0;
}

int gpio_eth_change_state(eth_status_t *eth, uint32_t val)
{
	if (val >= eth->min && val <= eth->max)
	{
		// Update variable.
		eth->val = val;

		// Write gpio register.
		XGpio_DiscreteWrite(&gpio_eth_i, 1, val);

		return 0;
	}
	else
	{
		return -1;
	}
}

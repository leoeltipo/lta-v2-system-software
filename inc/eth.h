/*
 * eth.h
 *
 * Author: Leandro Stefanazzi (lstefanazzi@uns.edu.ar)
 */

#ifndef ETH_H_
#define ETH_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

#define ETH_MAX_DATALENGTH			256

#define ETH_IP_DEFAULT	0x00000000
#define ETH_IP_MIN		0x00000000
#define ETH_IP_MAX		0xFFFFFFFF

#define ETH_IP_TERM_LENGTH	6
#define ETH_IP_TERMS		6
#define ETH_IP_STR_LENGTH	20

typedef struct {
	char word[ETH_IP_TERM_LENGTH];
} eth_ip_words_t;

// Structs for IP gpio.
typedef struct {
	uint32_t val;
	uint32_t min;
	uint32_t max;
	char name[10];
	char valStr[ETH_IP_STR_LENGTH];
} eth_status_t;

typedef struct {
	eth_status_t ipEth;
} eth_t;

// Handshake structure.
typedef struct {
	volatile uint32_t dready;
	volatile uint32_t dack;
	volatile uint32_t dlength;
} eth_bus_t;

eth_bus_t * eth_mbus;
eth_bus_t * eth_sbus;

// Data structure.
typedef struct {
	volatile uint8_t data[ETH_MAX_DATALENGTH];
} eth_data_t;

eth_data_t * eth_mdata;
eth_data_t * eth_sdata;

/*
 * This function initializes the pointers to the right address. Call this function
 * before performing any read/write operation with the ethernet block.
 *
 * Configuration (low speed) memory is a completely separated memory block from that
 * used for burst data. Burst data memory cannot be accessed through this driver.
 *
 * NOTE: the order defined in eth_init to initialize the pointers will dictate how
 * data is expected to be organized in the memory.
 */
int eth_init(uint32_t gpio_device_id, eth_t *eth);

/*
 * This function checks if there is data to be read sent from the master.
 */
unsigned int eth_mdata_get(uint8_t *buf);
void eth_sdata_put(const char *str);

void eth_uint2ip(uint32_t ip, char *str);
uint32_t eth_ip2uint(char *str);

int eth_change_ip(eth_status_t *eth, char *ip);
int gpio_eth_change_state(eth_status_t *eth, uint32_t val);

#endif // ETH_H_


/*
 * flash.c
 *
 *  Created on: Oct 1, 2018
 *      Author: lstefana
 */

#include <stdint.h>
#include <xspi.h>
#include <xil_printf.h>
#include "flash.h"
#include "io_func.h"

// SPI driver variables.
XSpi_Config	*spi_flash_cfg;
XSpi		spi_flash_i;

int flash_init(uint32_t spi_device_id, flash_version_t *info)
{
	int ret;
	uint32_t base_addr		= 0;
	uint32_t control_val 	= 0;

	// Init spi_ldo_i structure.
	ret = XSpi_Initialize(&spi_flash_i, spi_device_id);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	ret = XSpi_Stop(&spi_flash_i);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Get device configuration from generated structure.
	spi_flash_cfg = XSpi_LookupConfig(spi_device_id);

	// Get BaseAddress from config.
	base_addr = spi_flash_cfg->BaseAddress;

	// Initialize hardware device.
	ret = XSpi_CfgInitialize(&spi_flash_i, spi_flash_cfg, base_addr);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Configure SPI options.
	control_val = 	XSP_MASTER_OPTION 			|
					//XSP_CLK_ACTIVE_LOW_OPTION	|
					//XSP_CLK_PHASE_1_OPTION		|
					XSP_MANUAL_SSELECT_OPTION;

	// Write options into hardware spi device.
	ret = XSpi_SetOptions(&spi_flash_i, control_val);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	// Start the device.
	ret = XSpi_Start(&spi_flash_i);

	// Disable interrupts for this device.
	XSpi_IntrGlobalDisable(&spi_flash_i);

	// Populate flash info structure.
	flash_readBoardInfo(info);

	return ret;
}

int flash_resetEnable()
{
	WriteBuffer[BYTE1] = COMMAND_RESET_ENABLE;

	// Select slave for this device.
	int status = XSpi_SetSlaveSelect(&spi_flash_i, 1);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// SPI Transfer.
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer, 1);

	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_resetMemory()
{
	WriteBuffer[BYTE1] = COMMAND_RESET_MEMORY;

	// Select slave for this device.
	int status = XSpi_SetSlaveSelect(&spi_flash_i, 1);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// SPI Transfer.
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer, 1);

	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_writeEnable(void)
{
	int status;

	// Wait for flash to be ready.
	status = flash_waitForFlashReady();
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_WRITE_ENABLE;
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, NULL, WRITE_ENABLE_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_writeDisable(void)
{
	int status;

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_WRITE_DISABLE;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, NULL, WRITE_ENABLE_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_4byteModeEnable(void)
{
	int status;

	/*
	 * Enable writes to the memory.
	 */
	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait while the Flash is busy.
	 */
	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Prepare the COMMNAD_ENTER_4BYTE_ADDRESS_MODE.
	 */
	WriteBuffer[BYTE1] = COMMAND_ENTER_4BYTE_ADDRESS_MODE;

	/*
	 * Initiate the Transfer.
	 */
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, NULL, WRITE_ENABLE_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Disable writes to the memory.
	 */
	status = flash_writeDisable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_4byteModeDisable(void)
{
	int status;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Wait while the Flash is busy.
	 */
	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Prepare the WriteBuffer.
	 */
	WriteBuffer[BYTE1] = COMMAND_EXIT_4BYTE_ADDRESS_MODE;

	/*
	 * Initiate the Transfer.
	 */
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, NULL, WRITE_ENABLE_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_read(u32 Addr, u32 ByteCount)
{
	if (ByteCount >= PAGE_SIZE)
	{
		xil_printf("Error: cannot read more than %d bytes at once!!\r\n", PAGE_SIZE);
		return XST_FAILURE;
	}

	int status;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			(ByteCount + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	uint32_t StartAddr = Addr;
	for (int i=0; i<ByteCount; i++)
	{
		xil_printf("0x%08x: 0x%02x\t\r\n", StartAddr, (ReadBuffer[i + READ_WRITE_EXTRA_BYTES]));
		StartAddr++;
	}

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_readByte(u32 Addr, u8 *val)
{
	int status;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			(1 + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set output.
	*val = ReadBuffer[READ_WRITE_EXTRA_BYTES];

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_readWord(u32 Addr, u16 *val)
{
	int status;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			(2 + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set output.
	*val = 0;
	for (int i=0; i<2; i++)
	{
		*val += (ReadBuffer[READ_WRITE_EXTRA_BYTES+i]<<8*i);
	}

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_readQWord(u32 Addr, u32 *val)
{
	int status;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			(4 + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set output.
	*val = 0;
	for (int i=0; i<4; i++)
	{
		*val += (ReadBuffer[READ_WRITE_EXTRA_BYTES+i]<<8*i);
	}

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_readPage(u32 Addr)
{
	// Starting address.
	uint32_t mask = ~(PAGE_SIZE - 1);
	uint32_t addrStart = (Addr & mask);
	uint32_t addrEnd = addrStart + (PAGE_SIZE - 1);

	Addr = addrStart;

	print("\n\r\n\rPerforming Flash Read Operation...\r\n");
	xil_printf("\n\rFlash Start Address:\t0x%08x",addrStart);
	xil_printf("\n\rFlash End Address:\t0x%08x",addrEnd);

	int status;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			(PAGE_SIZE + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	xil_printf("\n\rOffset(h):\t0x00\t0x01\t0x02\t0x03\t0x04\t0x05\t0x06\t0x07\n\r");
	uint32_t addrInc = addrStart;
	for (int i=0; i<(PAGE_SIZE/8); i++)
	{
		xil_printf("\n\r0x%08x:\t", addrInc);
		for (int j=0; j<8; j++)
		{
			xil_printf("0x%02x\t", (ReadBuffer[i*8+j + READ_WRITE_EXTRA_BYTES]));
		}
		addrInc += 8;
	}

	print("\r\n");


	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;

}

int flash_write(u32 Addr, u32 ByteCount, u8 *data)
{
	if (ByteCount >= PAGE_SIZE)
	{
		xil_printf("Error: cannot write more than %d bytes at once!!\r\n", PAGE_SIZE);
		return XST_FAILURE;
	}

	int status;

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_PAGE_PROGRAM;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	for(int i = 0; i < ByteCount; i++) {
		WriteBuffer[BYTE6+i] = data[i];
	}

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, NULL,
			(ByteCount + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForWriteEnd();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_readBoardInfo(flash_version_t *info)
{
	int status;
	u32 Addr = FLASH_BOARD_INFO_ADDR;

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			(FLASH_BOARD_INFO_LENGTH + READ_WRITE_EXTRA_BYTES));
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Fill board info structure.
	int idx = READ_WRITE_EXTRA_BYTES;
	char str[20];

	// Firmware version.
	info->firm_version.minor = ReadBuffer[idx++];
	info->firm_version.major = ReadBuffer[idx++];
	io_sprintf(	str, "%d.%d",
				info->firm_version.major,
				info->firm_version.minor);
	strcpy(info->firm_version.str, str);

	// Two reserverd bytes.
	idx += 2;

	// Firmware date.
	info->firm_date.month = ReadBuffer[idx++];
	info->firm_date.day = ReadBuffer[idx++];
	info->firm_date.year = ReadBuffer[idx++];
	info->firm_date.year += (uint16_t) (ReadBuffer[idx++]<<8);
	io_sprintf(	str, "%d/%d/%d",
				info->firm_date.month,
				info->firm_date.day,
				info->firm_date.year);
	strcpy(info->firm_date.str, str);

	// Software version.
	info->soft_version.minor = ReadBuffer[idx++];
	info->soft_version.major = ReadBuffer[idx++];
	io_sprintf(	str, "%d.%d",
				info->soft_version.major,
				info->soft_version.minor);
	strcpy(info->soft_version.str, str);

	// Two reserverd bytes.
	idx += 2;

	// Software date.
	info->soft_date.month = ReadBuffer[idx++];
	info->soft_date.day = ReadBuffer[idx++];
	info->soft_date.year = ReadBuffer[idx++];
	info->soft_date.year += (uint16_t) (ReadBuffer[idx++]<<8);
	io_sprintf(	str, "%d/%d/%d",
				info->soft_date.month,
				info->soft_date.day,
				info->soft_date.year);
	strcpy(info->soft_date.str, str);

	// Unique board id.
	info->id.val = ReadBuffer[idx++];
	info->id.val += (uint32_t) (ReadBuffer[idx++]<<8);
	info->id.val += (uint32_t) (ReadBuffer[idx++]<<16);
	info->id.val += (uint32_t) (ReadBuffer[idx++]<<24);

	// Board ip.
	info->ip.val = ReadBuffer[idx++];
	info->ip.val += (uint32_t) (ReadBuffer[idx++]<<8);
	info->ip.val += (uint32_t) (ReadBuffer[idx++]<<16);
	info->ip.val += (uint32_t) (ReadBuffer[idx++]<<24);
	io_sprintf(	str, "%d.%d.%d.%d",
				(info->ip.val>>24 	& 0xFF),
				(info->ip.val>>16 	& 0xFF),
				(info->ip.val>>8 	& 0xFF),
				(info->ip.val 		& 0xFF));
	strcpy(info->ip.str, str);

	// Flash info address on flash.
	info->addr = Addr;

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_printBoardInfo(flash_version_t *info)
{
	print("Board Information:\r\n");

	xil_printf("--> Base Address:\t0x%08x:\r\n", info->addr);
	xil_printf("--> Firmware version:\t%s\r\n", info->firm_version.str);
	xil_printf("--> Firmware date:\t%s\r\n", info->firm_date.str);
	xil_printf("--> Software version:\t%s\r\n", info->soft_version.str);
	xil_printf("--> Software date:\t%s\r\n", info->soft_date.str);
	xil_printf("--> Unique ID:\t\t0x%08x\r\n", info->id.val);
	xil_printf("--> Board IP:\t\t%s\r\n", info->ip.str);

	return XST_SUCCESS;
}

uint8_t flash_getIpLow(flash_version_t *info)
{
	u32 ip = info->ip.val;
	ip &= 0xFF;
	return (uint8_t)ip;
}

int flash_eraseSubSector(u32 Addr)
{
	uint32_t mask = ~(BYTE_PER_SUBSECTOR - 1);
	uint32_t addrStart = (Addr & mask);
	uint32_t addrEnd = addrStart + (BYTE_PER_SUBSECTOR - 1);

	print("Erase:\r\n");
	xil_printf("-->From\t: %x\r\n", addrStart);
	xil_printf("-->To\t: %x\r\n", addrEnd);

	int status;

	status = flash_4byteModeEnable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*status = flash_getExtendedAddress();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}*/

	status = flash_writeEnable();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_SUBSECTOR_ERASE;		//COMMAND_SUBSECTOR_ERASE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) (Addr);

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, NULL,
			SECTOR_ERASE_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForWriteEnd();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	status = flash_4byteModeDisable();
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_readID(void)
{
	int status;

	// Wait for flash ready.
	status = flash_waitForFlashReady();
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Read ID command.
	WriteBuffer[BYTE1] = COMMAND_READ_ID;

	// SPI Transfer.
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer, READ_WRITE_EXTRA_BYTES);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// Decode ID.
	if ( (ReadBuffer[1] == 0x20))
	{
		xil_printf("\n\rManufacturer ID:\t0x%x\t:= MICRON\n\r", ReadBuffer[1]);
		if ( (ReadBuffer[2] == 0xBA))
		{
			xil_printf("Memory Type:\t\t0x%x\t:= N25Q 3V0\n\r", ReadBuffer[2]);
		}
		else
		{
			if ((ReadBuffer[2] == 0xBB))
			{
				xil_printf("Memory Type:\t\t0x%x\t:= N25Q 1V8\n\r", ReadBuffer[2]);
			}
			else
			{
				xil_printf("Memory Type:\t\t0x%x\t:= QSPI Data\n\r", ReadBuffer[2]);
			}
		}
		if ((ReadBuffer[3] == 0x18))
		{
			xil_printf("Memory Capacity:\t0x%x\t:=\t128Mbit\n\r", ReadBuffer[3]);
		}
		else if ( (ReadBuffer[3] == 0x19))
		{
			xil_printf("Memory Capacity:\t0x%x\t:= 256Mbit\n\r", ReadBuffer[3]);
		}
		else if ((ReadBuffer[3] == 0x20))
		{
			xil_printf("Memory Capacity:\t0x%x\t:= 512Mbit\n\r", ReadBuffer[3]);
		}
		else if ((ReadBuffer[3] == 0x21))
		{
			xil_printf("Memory Capacity:\t0x%x\t:= 1024Mbit\n\r", ReadBuffer[3]);
		}
	}
	else if ((ReadBuffer[1] == 0x01))
	{
		xil_printf("\n\rManufacturer ID: \tSPANSION\n\r");
		if ((ReadBuffer[3] == 0x18))
		{
			xil_printf("Memory Capacity\t=\t256Mbit\n\r");
		}
		else if ((ReadBuffer[3] == 0x19))
		{
			xil_printf("Memory Capacity\t=\t512Mbit\n\r");
		}
		else if ((ReadBuffer[3] == 0x20))
		{
			xil_printf("Memory Capacity\t=\t1024Mbit\n\r");

		}
	}
	else if ((ReadBuffer[1] == 0xEF))
	{
		xil_printf("\n\rManufacturer ID\t=\tWINBOND\n\r");
		if ((ReadBuffer[3] == 0x18))
		{
			xil_printf("Memory Capacity\t=\t128Mbit\n\r");
		}
	}
	else
	{
		print("\r\nManufacturer Unknown\r\n");
	}

	return XST_SUCCESS;
}

int flash_getStatus(void)
{
	WriteBuffer[BYTE1] = COMMAND_STATUSREG_READ;

	// Select slave for this device.
	int status = XSpi_SetSlaveSelect(&spi_flash_i, 1);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// SPI Transfer.
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer, STATUS_READ_BYTES);

	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_getFlagStatus()
{
	int status;

	/*status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}*/
	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ_FLAG_STATUS;
	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			READ_WRITE_EXTRA_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int flash_getExtendedAddress(void)
{

	int status;

	/*status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}*/

	status = flash_waitForFlashReady();
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_READ_EXTENDED_ADDRESS;

	status = XSpi_Transfer(&spi_flash_i, WriteBuffer, ReadBuffer,
			READ_WRITE_EXTRA_BYTES);
	if(status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;

}

int flash_waitForFlashReady(void)
{
	int status;
	u8 statusReg;

	while(1)
	{
		status = flash_getStatus();
		if (status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}

		statusReg = ReadBuffer[1];
		if ( (statusReg & FLASH_SR_IS_READY_MASK) == 0)
		{
			break;
		}
		else
		{
			print("-");
		}
	}

	//print("\r\n");

	return XST_SUCCESS;
}

int flash_waitForWriteEnd()
{
	int status;
	u8 statusReg;

	while(1)
	{
		status = flash_getFlagStatus();
		if (status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}

		statusReg = ReadBuffer[1];
		if ( (statusReg & FLASH_FLAG_IS_READY_MASK) == FLASH_FLAG_IS_READY_MASK)
		{
			break;
		}
		else
		{
			print("-");
		}
	}

	//print("\r\n");

	return XST_SUCCESS;
}

/*
 * flash.h
 *
 *  Created on: Oct 1, 2018
 *      Author: lstefana
 */

#ifndef SRC_FLASH_H_
#define SRC_FLASH_H_

/*
 * Memory commands.
 */
#define COMMAND_RESET_ENABLE						0x66 /* Enable reset */
#define COMMAND_RESET_MEMORY						0x99 /* Reset the device */
#define COMMAND_WRITE_STATUS_REGISTER  				0x01 /* WRITE STATUS REGISTER 01h  */
#define COMMAND_STATUSREG_READ						0x05 /* Status read command */
#define	COMMAND_WRITE_ENABLE						0x06 /* Write Enable command */
#define COMMAND_ENTER_QUAD_MODE						0x35 /* ENTER QUAD mode */
#define COMMAND_EXIT_QUAD_MODE						0xF5 /* EXIT QUAD = QUAD F5h  */
#define COMMAND_ENTER_4BYTE_ADDRESS_MODE			0xB7 /* ENTER 4-BYTE ADDRESS MODE B7 */
#define COMMAND_EXIT_4BYTE_ADDRESS_MODE				0xE9 /* EXIT 4-BYTE ADDRESS MODE E9h */
#define COMMAND_READ_FLAG_STATUS 					0x70
#define	COMMAND_CLEAR_FLAG_STATUS					0x50
#define	COMMAND_WRITE_DISABLE						0x04 /* Write Enable command */
#define COMMAND_READ_EXTENDED_ADDRESS				0xC8
#define COMMAND_WRITE_EXTENDED_ADDRESS				0xC5
/*************************************************************************************************/
#define COMMAND_PAGE_PROGRAM						0x02 /* Page Program command */
/*************************************************************************************************/
#define COMMAND_READ_ID								0x9E /* READ ID 9E/9Fh  */
#define COMMAND_READ_DISCOVERY						0x5A /* READ SERIAL FLASH DISCOVERY PARAMETER 5Ah */
#define COMMAND_READ								0x03 /* Random read command */
/*************************************************************************************************/
#define COMMAND_SECTOR_ERASE						0xD8 /* Sector Erase command */
#define COMMAND_BULK_ERASE							0xC7 /* Bulk Erase command */
#define COMMAND_SUBSECTOR_ERASE 					0x20 /* SUBSECTOR ERASE 20h */
#define COMMAND_4BYTE_SUBSECTOR_ERASE 				0x21 /* 4-BYTE SUBSECTOR ERASE 21h */
/*************************************************************************************************/

/*
 * This definitions specify the EXTRA bytes for each command.
 */
#define READ_WRITE_EXTRA_BYTES		5 /* Read/Write extra bytes */
#define	WRITE_ENABLE_BYTES			1 /* Write Enable bytes */
#define STATUS_READ_BYTES			2 /* Status read bytes count */
#define STATUS_WRITE_BYTES			2 /* Status write bytes count */
#define SECTOR_ERASE_BYTES			5 /* Sector erase extra bytes */
#define BULK_ERASE_BYTES			1 /* Bulk erase extra bytes */
#define FLASH_SR_IS_READY_MASK		0x01 /* Ready mask */
#define FLASH_FLAG_IS_READY_MASK	0x80

/*
 * Number of bytes per page in the flash device.
 */
#define PAGE_SIZE					256
#define NUMB_SECTORS				1024
#define	BYTE_PER_SECTOR				65536
#define	NUMB_SUBSECTORS				16384
#define	BYTE_PER_SUBSECTOR			4096
#define NOB_PAGES					262144

/*
 * Byte Positions.
 */
#define BYTE1						0 /* Byte 1 position */
#define BYTE2						1 /* Byte 2 position */
#define BYTE3						2 /* Byte 3 position */
#define BYTE4						3 /* Byte 4 position */
#define BYTE5						4 /* Byte 5 position */
#define BYTE6						5 /* Byte 6 position */
#define BYTE7						6 /* Byte 7 position */
#define BYTE8						7 /* Byte 8 position */

#define FLASH_BOARD_INFO_ADDR		0x3ffff00
#define FLASH_BOARD_INFO_LENGTH		24

/*
 * Variable definitions.
 */
u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 5];
u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 5];

/*
 * Typedefs.
 */
typedef struct {
	u8 minor;
	u8 major;
	char str[20];
} flash_fs_version_t;

typedef struct {
	u8 month;
	u8 day;
	u16 year;
	char str[20];
} flash_date_t;

typedef struct {
	u32 val;
} flash_id_t;

typedef struct {
	u32 val;
	char str[20];
} flash_ip_t;

typedef struct {
	flash_fs_version_t firm_version;
	flash_date_t firm_date;
	flash_fs_version_t soft_version;
	flash_date_t soft_date;
	flash_id_t id;
	flash_ip_t ip;
	u32 addr;
} flash_version_t;
/*
 * Function prototypes.
 */
int flash_init(uint32_t spi_device_id, flash_version_t *info);

int flash_resetEnable(void);
int flash_resetMemory(void);
int flash_writeEnable(void);
int flash_writeDisable(void);
int flash_4byteModeEnable(void);
int flash_4byteModeDisable(void);

int flash_read(u32 Addr, u32 ByteCount);
int flash_readByte(u32 Addr, u8 *val);
int flash_readWord(u32 Addr, u16 *val);
int flash_readQWord(u32 Addr, u32 *val);
int flash_readPage(u32 Addr);
int flash_write(u32 Addr, u32 ByteCount, u8 *data);

int flash_readBoardInfo(flash_version_t *info);
int flash_printBoardInfo(flash_version_t *info);
uint8_t flash_getIpLow(flash_version_t *info);

int flash_eraseSubSector(u32 Addr);

int flash_readID(void);
int flash_getStatus(void);
int flash_getFlagStatus(void);
int flash_getExtendedAddress(void);

int flash_waitForFlashReady(void);
int flash_waitForWriteEnd(void);

#endif /* SRC_FLASH_H_ */

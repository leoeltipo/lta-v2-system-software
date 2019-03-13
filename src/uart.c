#include <stdio.h>
#include <stdlib.h>

#include "xuartlite.h"

#include "uart.h"
#include "defines.h"
#include "flash.h"

XUartLite Uart;

int uart_init(uint16_t device_id)
{
	return XUartLite_Initialize(&Uart, device_id);
}


unsigned int uart_rcv(uint8_t *buf)
{
	/*
	 * Receive the buffer from the UartLite waiting til there's data by (block),
	 * if the specified number of bytes was not received successfully, then
	 * an error occurred.
	 */

	return XUartLite_Recv(&Uart, buf, 1);
}

void uart_eraseBuffer(uint8_t *buf, unsigned int n)
{
	for (int i=0; i<n; i++)
	{
		buf[i] = 0;
	}
}

void uart_parseCmd(char *buf)
{
	// Separate command with white spaces.
	user_words_t commandWord[USERNUMBWORDS];
	char *token;
	char *rest;

	rest = buf;
	// walk through other tokens
	int wordInd = 0;
	while( (token = strtok_r(rest, " ", &rest)) ) {
		//save word
		strcpy(commandWord[wordInd].word,token);
		wordInd = wordInd + 1;
	}

	free(token);
	free(rest);

	switch(wordInd){
	case NO_WORD:
		print("No word\r\n");
		break;

	case ONE_WORD:
		if (strcmp(commandWord[0].word, "help") == 0)
			uart_printMenu();
		break;

	case TWO_WORD:
		if (strcmp(commandWord[0].word, "flash") ==0)
		{
			if (strcmp(commandWord[1].word, "reset") ==0)
			{
				uart_cmdReset();
			}
			else if (strcmp(commandWord[1].word, "info") ==0)
			{
				uart_cmdBoardInfo();
			}
		}
		else if (strcmp(commandWord[0].word, "erase") ==0)
		{
			uart_cmdErase(commandWord[1].word);
		}
		else if (strcmp(commandWord[0].word, "read") ==0)
		{
			uart_cmdReadPage(commandWord[1].word);
		}

		break;

	case THREE_WORDS:
		if (strcmp(commandWord[0].word, "read") == 0)
		{
			if (strcmp(commandWord[1].word, "byte") == 0)
			{
				uart_cmdReadByte(commandWord[2].word);
			}
			else if (strcmp(commandWord[1].word, "word") == 0)
			{
				uart_cmdReadWord(commandWord[2].word);
			}
			else if (strcmp(commandWord[1].word, "qword") == 0)
			{
				uart_cmdReadQWord(commandWord[2].word);
			}
			else
			{
				uart_cmdRead(commandWord[1].word, commandWord[2].word);
			}
		}

		break;

	case FOUR_WORDS:
		if (strcmp(commandWord[0].word, "write") == 0)
			uart_cmdWrite(commandWord[1].word, commandWord[2].word, commandWord[3].word);
		break;

	default:
		print("More than four words\r\n");
		break;
	}
}

void uart_printMenu()
{
	print("\033[2J");
	print("\033[H");
	print("\r\n************************************************");
	print("\r\n************ Flash Programming Tool ************");
	print("\r\n************************************************\r\n");
	print("\r\nChose from the options below: \r\n");
	print("read <addr> <n>\t\t: read \'n\' bytes starting at \'addr\'\r\n");
	print("read <addr>\t\t: read one page containing \'addr\'\r\n");
	print("write byte <addr> <val>\t: write byte \'val\' at \'addr\'\r\n");
	print("write word <addr> <val>\t: write word (16 bits) \'val\' at \'addr\'\r\n");
	print("write qword <addr> <val>: write qword (32 bits) \'val\' at \'addr\'\r\n");
	print("erase <addr>\t\t: erase sub-sector (4KB) containing \'addr\'\r\n");
	print("flash reset\t\t: resets the memory\r\n");
	print("flash info\t\t: reads board info\r\n");
}

void uart_cmdReset()
{
	flash_resetEnable();
	flash_resetMemory();
}

void uart_cmdBoardInfo()
{
	// Temporal board info struct.
	flash_version_t flash_info;

	flash_readBoardInfo(&flash_info);
	flash_printBoardInfo(&flash_info);
}

void uart_cmdRead(char *addr, char *nBytes)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);
	uint32_t nBytesI = str2num(nBytes);

	flash_read(addrI, nBytesI);
}

void uart_cmdReadByte(char *addr)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);
	uint8_t val;

	flash_readByte(addrI, &val);
	xil_printf("0x%08x: 0x%02x\r\n", addrI, val);
}

void uart_cmdReadWord(char *addr)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);
	uint16_t val;

	flash_readWord(addrI, &val);
	xil_printf("0x%08x: 0x%04x\r\n", addrI, val);
}

void uart_cmdReadQWord(char *addr)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);
	uint32_t val;

	flash_readQWord(addrI, &val);
	xil_printf("0x%08x: 0x%08x\r\n", addrI, val);
}

void uart_cmdReadPage(char *addr)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);

	flash_readPage(addrI);
}

void uart_cmdWrite(char *type, char *addr, char *val)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);
	uint32_t valI = str2num(val);

	if (strcmp(type, "byte") == 0)
	{
		u8 data = (u8) (valI & 0xFF);
		flash_write(addrI, 1, &data);
	}
	else if (strcmp(type, "word") == 0)
	{
		u8 data[2];
		data[0] = (u8) (valI    & 0xFF);
		data[1] = (u8) (valI>>8 & 0xFF);
		flash_write(addrI, 2, data);
	}
	else if (strcmp(type, "qword") == 0)
	{
		u8 data[4];
		data[0] = (u8) (valI     & 0xFF);
		data[1] = (u8) (valI>>8  & 0xFF);
		data[2] = (u8) (valI>>16 & 0xFF);
		data[3] = (u8) (valI>>24 & 0xFF);
		flash_write(addrI, 4, data);
	}
}

void uart_cmdErase(char *addr)
{
	// Convert to number.
	uint32_t addrI = str2num(addr);
	flash_eraseSubSector(addrI);
}


uint32_t str2num(char *str)
{
	uint32_t ret;

	char hx[10];
	hx[0] = str[0];
	hx[1] = str[1];
	hx[2] = '\0';

	if (strcmp(hx, "0x") == 0)
	{
		ret = convertToDecimal(str);
	}
	else
	{
		long long temp;
		temp = atoll(str);
		ret = (uint32_t) temp;
	}

	return ret;
}

uint32_t convertToDecimal(char *str)
{
	uint32_t  result = 0;
	char *inpstr = str;
	char  charhex;
	int charhexint;

	int len = strlen(str);
	for (int i=2;i<len;i++)
	//while( ( (charhex = *inpstr) != NULL ) && ((charhex = *inpstr) != '\r') && ((charhex = *inpstr) != '\n'))
	{
		charhex = inpstr[i];
		uint32_t add;
		charhexint = toupper(charhex);

		result <<= 4;
		if (charhexint != '\b')
		{
			if( charhexint >= 48 &&  charhexint <= 57 )
				add = charhexint - 48;
			else if( charhexint >= 65 && charhexint <= 70)
				add = charhexint - 65 + 10;
			else
			{
				print("\n\rUnrecognized hex   "); putchar(charhex);
			}

			result += add;
		}
	}

	return result;
}

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

int uart_init(uint16_t device_id);
unsigned int uart_rcv(uint8_t *buf);

void uart_eraseBuffer(uint8_t *buf, unsigned int n);

void uart_parseCmd(char *buf);
void uart_printMenu();

// Commands for controlling the flash memory.
void uart_cmdReset();
void uart_cmdBoardInfo();
void uart_cmdRead(char *addr, char *nBytes);
void uart_cmdReadByte(char *addr);
void uart_cmdReadWord(char *addr);
void uart_cmdReadQWord(char *addr);
void uart_cmdReadPage(char *addr);
void uart_cmdWrite(char *type, char *addr, char *val);
void uart_cmdErase(char *addr);

// Utility functions.
uint32_t str2num(char *str);
uint32_t convertToDecimal(char *str);

#endif

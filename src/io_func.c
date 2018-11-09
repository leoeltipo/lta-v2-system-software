/*
 * io_func.c
 *
 *  Created on: Sep 11, 2018
 *      Author: lstefana
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "io_func.h"
#include "eth.h"

system_state_t *io_sys;

void io_init(system_state_t *sys)
{
	io_sys = sys;
}

void io_sprintf(char *str, char *fmt, ...)
{
	// Output string.
	char outbuf[100];

	for (int i=0; i<100; i++)
	{
		outbuf[i] = '\0';
	}

	// Initialize variable arguments functions.
	va_list argp;
	va_start(argp, fmt);

	unsigned int l = strlen(fmt);

	int find_dfs = 0;
	int idx = 0;
	for (int i=0; i<l; i++)
	{
		switch(fmt[i])
		{
		case '%':
			find_dfs = 1;
			break;

		case 'd':
			if (find_dfs)
			{
				// Convert int to string.
				int32_t n = (int32_t)va_arg(argp,int);
				char ns[15];
				io_int2str(n, ns);
				strcat(outbuf,ns);
				find_dfs = 0;
				idx += strlen(ns);
			}
			else
			{
				outbuf[idx] = fmt[i];
				idx++;
			}
			break;

		case 'u':
			if (find_dfs)
			{
				// Convert uint to string.
				uint32_t n = (uint32_t)va_arg(argp,unsigned);
				char ns[15];
				io_uint2str(n, ns);
				strcat(outbuf,ns);
				find_dfs = 0;
				idx += strlen(ns);
			}
			else
			{
				outbuf[idx] = fmt[i];
				idx++;
			}
			break;

		case 'f':
			if (find_dfs)
			{
				// Convert float to string.
				float n = va_arg(argp,double);
				char ns[15];
				io_float2str(n, ns);
				strcat(outbuf,ns);
				find_dfs = 0;
				idx += strlen(ns);
			}
			else
			{
				outbuf[idx] = fmt[i];
				idx++;
			}
			break;

		case 's':
			if (find_dfs)
			{
				// Get string.
				char *s = va_arg(argp,char*);
				strcat(outbuf,s);
				find_dfs = 0;
				idx += strlen(s);
			}
			else
			{
				outbuf[idx] = fmt[i];
				idx++;
			}
			break;
		default:
			outbuf[idx] = fmt[i];
			idx++;
			break;
		}
	}

	// Finish reading arguments.
	va_end(argp);

	// Copy buffer.
	strcpy(str, outbuf);

	return;
}

void io_int2str(int32_t n, char *str)
{
	// Output buffer.
	char outbuf[12];

	for (int i=0; i<12; i++)
	{
		outbuf[i] = '\0';
	}

	int32_t num = n;

	// Check negative.
	int neg = 0;
	if (num < 0)
	{
		num = -num;
		neg = 1;
	}

	// Digits.
	const char digits[] = "0123456789";

	// Do replacement for str length.
	int l = sizeof(outbuf)/sizeof(char);
	for (int i=0; i<l; i++)
	{
		outbuf[i] = digits[(num % 10)];
		num /= 10;
		if (num == 0) break;
	}

	// Add minus sign.
	if (neg)
	{
		int i = strlen(outbuf);
		outbuf[i] = '-';
	}

	// Copy output buffer (backwards).
	l = strlen(outbuf);
	for (int i=0; i<l; i++)
	{
		str[i] = outbuf[l-i-1];
	}
	str[l] = '\0';
}

void io_uint2str(uint32_t n, char *str)
{
	// Output buffer.
	char outbuf[12];

	for (int i=0; i<12; i++)
	{
		outbuf[i] = '\0';
	}

	uint32_t num = n;

	// Digits.
	const char digits[] = "0123456789";

	// Do replacement for str length.
	int l = sizeof(outbuf)/sizeof(char);
	for (int i=0; i<l; i++)
	{
		outbuf[i] = digits[(num % 10)];
		num /= 10;
		if (num == 0) break;
	}

	// Copy output buffer (backwards).
	l = strlen(outbuf);
	for (int i=0; i<l; i++)
	{
		str[i] = outbuf[l-i-1];
	}
	str[l] = '\0';
}

void io_float2str(float n, char *str)
{
	// Output buffer.
	char outbuf[10];
	char outbuf_dp[10];

	for (int i=0; i<10; i++)
	{
		outbuf[i] = '\0';
		outbuf_dp[i] = '\0';
	}

	// Digits.
	const char digits[] = "0123456789";

	// Do replacement for str length.
	int l = sizeof(outbuf)/sizeof(char);
	int num = n*1000;

	// Check negative.
	int neg = 0;
	if (num < 0)
	{
		num = -num;
		neg = 1;
	}

	// Check NaN.
	if (num > 99999)
	{
		strcpy(str, "NaN");
		return;
	}

	if (num != 0)
	{
		for (int i=0; i<l; i++)
		{
			outbuf[i] = digits[(num % 10)];
			num /= 10;
			if (num == 0) break;
		}
	}
	else
	{
		outbuf[0] = '0';
		outbuf[1] = '0';
		outbuf[2] = '0';
		outbuf[3] = '0';
	}

	// Zero padding.
	l = strlen(outbuf);
	if (l == 1)
	{
		outbuf[1] = '0';
		outbuf[2] = '0';
		outbuf[3] = '0';
	}
	else if (l == 2)
	{
		outbuf[2] = '0';
		outbuf[3] = '0';
	}
	else if (l == 3)
	{
		outbuf[3] = '0';
	}

	l = strlen(outbuf);
	int idx = 0;

	// Add the decimal point.
	for (int i=0; i<3; i++)
	{
		outbuf_dp[idx] = outbuf[i];
		idx++;
	}

	outbuf_dp[idx] = '.';
	idx++;

	for (int i=3;i<l;i++)
	{
		outbuf_dp[idx] = outbuf[i];
		idx++;
	}

	// Add the minus sign.
	if (neg)
	{
		outbuf_dp[idx] = '-';
		idx++;
	}

	// Copy output buffer (backwards).
	l = strlen(outbuf_dp);
	for (int i=0; i<l; i++)
	{
		str[i] = outbuf_dp[l-i-1];
	}
	str[l] = '\0';
}

void mprint(const char *str)
{
	if (io_sys->generic_vars.outeth.value)
	{
		eth_sdata_put(str);
	}
	else
	{
		print(str);
	}
}

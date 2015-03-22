/* Copyright (C) 2015 Baruch Even
 *
 * This file is part of the B3603 alternative firmware.
 *
 *  B3603 alternative firmware is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  B3603 alternative firmware is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with B3603 alternative firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "uart.h"
#include "fixedpoint.h"
#include "stm8s.h"

uint8_t uart_write_buf[255];
uint8_t uart_write_start;
uint8_t uart_write_len;

uint8_t uart_read_buf[64];
uint8_t uart_read_len;
uint8_t read_newline;

void uart_init()
{
	USART1_CR1 = 0; // 8 bits, no parity
	USART1_CR2 = 0;
	USART1_CR3 = 0;

	USART1_BRR2 = 0x1;
	USART1_BRR1 = 0x1A; // 38400 baud, order important between BRRs, BRR1 must be last

	USART1_CR2 = USART_CR2_TEN | USART_CR2_REN; // Allow TX & RX

	uart_write_len = 0;
	uart_write_start = 0;
	uart_read_len = 0;
	read_newline = 0;
}

inline uint8_t uart_write_ready(void)
{
	return (USART1_SR & USART_SR_TXE);
}

void uart_write_ch(const char ch)
{
	if (uart_write_len < sizeof(uart_write_buf))
		uart_write_buf[uart_write_len++] = ch;
}

void uart_write_str(const char *str)
{
	uint8_t i;

	// Move the buffer to the start
	if (uart_write_start > 0) {
		for (i = 0; i < uart_write_len; i++) {
			uart_write_buf[i] = uart_write_buf[i+uart_write_start];
		}
		uart_write_start = 0;
	}

	for(i = 0; str[i] != 0 && uart_write_len < sizeof(uart_write_buf); i++) {
		uart_write_buf[uart_write_len] = str[i];
		uart_write_len++;
	}
}

uint8_t digits_buf[12];
static uint8_t int_to_digits(uint16_t val)
{
	uint8_t i;
	uint8_t num_digits = 0;

	digits_buf[0] = '0';

	for (i = 0; i < 6 && val != 0; i++) {
		uint8_t digit = val % 10;
		digits_buf[i] = '0' + digit;
		val /= 10;
		if (digit) // We only really want to know about non-zero digits
			num_digits = i;
	}

	return num_digits + 1;
}

void uart_write_int(uint16_t val)
{
	int8_t i;
	uint8_t highest_nonzero;

	highest_nonzero = int_to_digits(val);

	for (i = highest_nonzero-1; i >= 0; i--) {
		uart_write_ch(digits_buf[i]);
	}
}

static uint8_t int32_to_digits(uint32_t val)
{
	uint8_t i;
	uint8_t num_digits = 0;

	digits_buf[0] = '0';

	for (i = 0; i < 12 && val != 0; i++) {
		uint8_t digit = val % 10;
		digits_buf[i] = '0' + digit;
		val /= 10;
		if (digit) // We only really want to know about non-zero digits
			num_digits = i;
	}

	return num_digits + 1;
}

void uart_write_int32(uint32_t val)
{
	int8_t i;
	uint8_t highest_nonzero;

	highest_nonzero = int32_to_digits(val);

	for (i = highest_nonzero-1; i >= 0; i--) {
		uart_write_ch(digits_buf[i]);
	}
}

void uart_write_milliamp(uint16_t val)
{
	int8_t i;
	uint8_t highest_nonzero;

	highest_nonzero = int_to_digits(val);

	for (i = highest_nonzero-1; i >= 0; i--) {
		if (i == 2)
			uart_write_ch('.');
		uart_write_ch(digits_buf[i]);
	}
}

void uart_write_millivolt(uint16_t val)
{
	int8_t i;
	uint8_t highest_nonzero;

	highest_nonzero = int_to_digits(val);

	for (i = highest_nonzero-1; i >= 0; i--) {
		if (i == 2)
			uart_write_ch('.');
		uart_write_ch(digits_buf[i]);
	}
}

void uart_write_fixed_point(uint32_t val)
{
	uint32_t tmp;

	// Print the integer part
	tmp = val >> FIXED_SHIFT;
	uart_write_int(tmp);
	uart_write_ch('.');

	// Remove the integer part
	tmp = val & FIXED_FRACTION_MASK;

	// Take three decimal digits from the fraction part
	tmp = fixed_round(tmp*10000);

	// Pad with zeros if the number is too small
	if (tmp < 1000)
		uart_write_ch('0');
	if (tmp < 100)
		uart_write_ch('0');
	if (tmp < 10)
		uart_write_ch('0');

	// Write the remaining fractional part
	uart_write_int32(tmp);
}

void uart_write_from_buf(void)
{
	USART1_DR = uart_write_buf[uart_write_start];
	uart_write_start++;
	uart_write_len--;

	if (uart_write_len == 0)
		uart_write_start = 0;
}

inline uint8_t uart_read_ch(void)
{
	return USART1_DR;
}

void uart_read_to_buf(void)
{
	// Don't read if we are writing
	uint8_t ch = uart_read_ch();

	if (ch >= 'a' && ch <= 'z')
		ch = ch - 'a' + 'A'; // Convert letters to uppercase

	uart_read_buf[uart_read_len] = ch;
	uart_read_len++;

	if (ch == '\r' || ch == '\n')
		read_newline = 1;

	// Empty the read buf if we are overfilling and there is no full command in there
	if (uart_read_len == sizeof(uart_read_buf) && !read_newline) {
		uart_read_len = 0;
		uart_write_str("READ OVERFLOW\r\n");
	}
}

void uart_drive(void)
{
	uint8_t sr = USART1_SR;

	if (sr & USART_SR_RXNE) {
		uart_read_to_buf();
	}
	if ((sr & USART_SR_TXE) && uart_write_len) {
		uart_write_from_buf();
	}
}

void uart_flush_writes(void)
{
	while (uart_write_len > 0)
		uart_drive();
}

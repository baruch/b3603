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

#include "parse.h"
#include "uart.h"

uint16_t parse_num(uint8_t *s, uint8_t **stop, uint8_t *digits_seen)
{
	uint8_t digit;
	uint16_t num = 0;

	*digits_seen = 0;

	for (; *s >= '0' && *s <= '9'; s++) {
		digit = *s - '0';
		num *= 10;
		num += digit;
		(*digits_seen)++;
	}

	*stop = s;
	return num;
}

uint16_t parse_millinum(uint8_t *s)
{
	uint8_t *t = s;
	uint8_t *stop;
	uint32_t fraction_digits = 0;
	uint16_t whole_digits = 0;
	uint8_t digits_seen;

	whole_digits = parse_num(s, &stop, &digits_seen);
	if (whole_digits > 62 || digits_seen > 2)
		goto invalid_number;

	whole_digits *= 1000;

	if (*stop == '\0')
		return whole_digits;

	if (*stop != '.')
		goto invalid_number;

	fraction_digits = parse_num(stop+1, &stop, &digits_seen);
	if (fraction_digits > 999 || digits_seen > 3)
		goto invalid_number;

	if (digits_seen == 1)
		fraction_digits *= 100;
	else if (digits_seen == 2)
		fraction_digits *= 10;

	return whole_digits + fraction_digits;

invalid_number:
	uart_write_str("INVALID NUMBER '");
	uart_write_str(t);
	uart_write_ch('\'');
	uart_write_str("\r\n");
	return 0xFFFF;
}

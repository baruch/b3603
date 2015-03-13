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

#include <stdint.h>

void uart_write_ch(const char ch) { (void)ch; }
void uart_write_str(const char *s) { (void)s; }

#include "parse.c"

#include <stdio.h>

#define TEST_PARSE(s, expected) if (parse_millinum((uint8_t*)s) != expected) { printf("Parsing of %s yielded %u but expected %u\n", s, parse_millinum((uint8_t*)s), expected);}

int main()
{
	TEST_PARSE("3.3", 3300);
	TEST_PARSE("3.30", 3300);
	TEST_PARSE("3.300", 3300);

	TEST_PARSE("5", 5000);
	TEST_PARSE("5.0", 5000);
	TEST_PARSE("5.00", 5000);
	TEST_PARSE("5.000", 5000);

	TEST_PARSE("0.001", 1);
	TEST_PARSE("0.010", 10);
	TEST_PARSE("0.100", 100);
	TEST_PARSE("1.111", 1111);

	TEST_PARSE("0.009", 9);
	TEST_PARSE("0.090", 90);
	TEST_PARSE("0.900", 900);
	TEST_PARSE("9.999", 9999);

	return 0;
}

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
void uart_write_int(uint16_t v) { (void)v; }
void uart_write_fixed_point13(uint16_t f) { (void)f; }

#include "fixedpoint.c"
#include "calibrate.c"
#include "adc.c"

#include <stdio.h>
#include <memory.h>

int failed;

void test_calibration(uint16_t y1, uint16_t x1, uint16_t y2, uint16_t x2, uint16_t expected_a, uint16_t expected_b)
{
	calibrate_t c;
	uint16_t back;

	memset(&c, 0, sizeof(c));

	calibrate_vin(1, y1, x1, &c);
	calibrate_vin(2, y2, x2, &c);

	if (c.a != expected_a && c.b != expected_b) {
		printf("Calibration of (%u, %u), (%u, %u) failed and yielded (%u, %u) instead of (%u, %u)\n",
				y1, x1,
				y2, x2,
				c.a, c.b,
				expected_a, expected_b);
		failed = 1;
	}

	back = adc_to_volt(x1, &c);
	if (back != y1) {
		printf("Result of calibration is not perfect for x1/y1, got %u expected %u\n", back, y1);
		failed = 1;
	}
	back = adc_to_volt(x2, &c);
	if (back != y2) {
		printf("Result of calibration is not perfect for x2/y2, got %u expected %u\n", back, y2);
		failed = 1;
	}
}

#define TEST_CALIBRATE(y1, x1, y2, x2, expected_a, expected_b) test_calibration(y1, x1, y2, x2, expected_a, expected_b)

int main()
{

	TEST_CALIBRATE(1935, 447, 4360, 877, 46199, 585);

	if (!failed)
		printf("OK!\n");

	return 0;
}

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

#include "calibrate.h"
#include "uart.h"
#include "fixedpoint.h"

struct calibrate_points {
	uint16_t set_point;
	uint16_t val;
};

struct calibrate_points vin_points[2];
struct calibrate_points vout_points[2];
struct calibrate_points cout_points[2];

inline void calibrate_calc(struct calibrate_points *points, calibrate_t *c)
{
	if (points[0].set_point && points[0].val && points[1].set_point && points[1].val) {
		uint32_t tmp1;
		uint32_t tmp2;
		uint32_t tmp3;

		tmp1 = (points[1].set_point - points[0].set_point);
		tmp1 <<= FIXED_SHIFT13;
		tmp3 = points[1].val - points[0].val;
		tmp1 /= tmp3;

		tmp2 = fixed_mult13(points[1].val, tmp1);
		while (tmp2 < points[1].set_point)
		{
			tmp2 = points[1].val;
			tmp1++;
			tmp2 *= tmp1;
			tmp2 >>= FIXED_SHIFT;
		}
		tmp2 -= points[1].set_point;

		c->a = tmp1;
		c->b = tmp2;
	}
}

static void calibrate_point(struct calibrate_points *points, int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c)
{
	if (point < 1 && point > 2)
		return;

	uart_write_ch('0' + point);
	uart_write_str(": VIN=");
	uart_write_millivolt(set_point);
	uart_write_str(" ADC=");
	uart_write_int(adc_val);
	uart_write_str("\r\n");

	points[point-1].set_point = set_point;
	points[point-1].val = adc_val;

	calibrate_calc(points, c);
}

void calibrate_vin(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c)
{
	uart_write_str("CALVIN");
	calibrate_point(vin_points, point, set_point, adc_val, c);
}

void calibrate_vout(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c)
{
	uart_write_str("CALVOUT");
	calibrate_point(vout_points, point, set_point, adc_val, c);
}

void calibrate_cout(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c)
{
	uart_write_str("CALCOUT");
	calibrate_point(cout_points, point, set_point, adc_val, c);
}

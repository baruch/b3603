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

#include "fixedpoint.c"
#include "outputs.c"
#include "eeprom.c"
#include "config.c"

#include <stdio.h>

int main()
{
	uint16_t val;
	uint16_t pwm;
	cfg_system_t sys;

	config_load_system(&sys);

	for (val = 10; val < 35000; val+=10) {
		pwm = pwm_from_set(val, &(sys.vout_pwm));
		printf("%u %u.%03u %u %.2f\n", val, val/1000, val % 1000, pwm, ((double)pwm)/8191.0*100.0);
	}
	return 0;
}

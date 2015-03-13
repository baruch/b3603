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


#include "fixedpoint.c"
#include "adc.c"
#include "eeprom.c"
#include "config.c"

#include <stdio.h>

int main()
{
	uint16_t adc;
	uint16_t mvolt;
	cfg_system_t sys;

	config_load_system(&sys);

	for (adc = 0; adc < 8192; adc++) {
		mvolt = adc_to_volt(adc, &(sys.vin_adc));
		printf("%u %u %u.%03u\n", adc, mvolt, mvolt/1000, mvolt%1000);
	}
	return 0;
}

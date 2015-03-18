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

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "fixedpoint.h"
#include "config.h"

void adc_init(void);
void adc_start(uint8_t channel);
fixed_t adc_to_volt(uint16_t adc, calibrate_t *cal);
uint16_t adc_read(void);
uint8_t adc_channel(void);
uint8_t adc_ready(void);

#endif

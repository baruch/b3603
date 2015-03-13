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

#include "config.h"

void calibrate_vin(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c);
void calibrate_vout(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c);
void calibrate_cout(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c);

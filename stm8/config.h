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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "fixedpoint.h"

typedef struct {
	uint8_t version;
	fixed_t vset;
	fixed_t cset;
	fixed_t vshutdown;
	fixed_t cshutdown;
} cfg_output_t;

// These parameters correspond to the linear formula:
// y = a*x + b
// where a and b are the coefficients, x is the input and y the calculated output
typedef struct {
	fixed_t a;
	fixed_t b;
} calibrate_t;

typedef struct {
	uint8_t version;
	uint8_t name[17];
	uint8_t default_on;
	uint8_t output;
	uint8_t autocommit;

	calibrate_t vin_adc;
	calibrate_t vout_adc;
	calibrate_t cout_adc;

	calibrate_t vout_pwm;
	calibrate_t cout_pwm;
} cfg_system_t;

typedef struct {
	uint16_t vin_raw;
	uint16_t vout_raw;
	uint16_t cout_raw;
	fixed_t vin;
	fixed_t vout;
	fixed_t cout;
	uint8_t constant_current; // If false, we are in constant voltage
	uint8_t pc3;
} state_t;

void config_load_system(cfg_system_t *sys);
void config_save_system(cfg_system_t *sys);
void config_load_output(cfg_output_t *cfg);
void config_save_output(cfg_output_t *cfg);

#endif

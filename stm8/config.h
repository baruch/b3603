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
	uint16_t vset; // mV
	uint16_t cset; // mA
	uint16_t vshutdown; // mV
	uint16_t cshutdown; // mA
} cfg_output_t;

// These parameters correspond to the linear formula:
// y = a*x + b
// where a and b are the coefficients, x is the input and y the calculated output
typedef struct {
	uint32_t a;
	uint32_t b;
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
	uint16_t vin; // mV
	uint16_t vout; // mV
	uint16_t cout; // mA
	uint8_t constant_current; // If false, we are in constant voltage
#if DEBUG
	uint8_t pc3;
#endif
} state_t;

void config_load_system(cfg_system_t *sys);
void config_save_system(cfg_system_t *sys);
void config_default_system(cfg_system_t *sys);
void config_load_output(cfg_output_t *cfg);
void config_save_output(cfg_output_t *cfg);
void config_default_output(cfg_output_t *cfg);

#endif

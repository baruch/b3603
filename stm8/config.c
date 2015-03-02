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

#include "config.h"
#include "eeprom.h"
#include "fixedpoint.h"

#include <string.h>

#define SYSTEM_CONFIG ((cfg_system_t *)0x4000)
#define OUTPUT_CONFIG ((cfg_output_t *)0x4040)

#define SYSTEM_CFG_VERSION 1
#define OUTPUT_CFG_VERSION 1

inline void validate_system_config(cfg_system_t *sys)
{
	if (sys->version != SYSTEM_CFG_VERSION) {
		memset(sys, 0, sizeof(*sys));
		sys->version = SYSTEM_CFG_VERSION;
		// TODO: If we want easy upgradability we can implement it here to
		// upgrade from an old struct to a new one.
	}

	if (sys->name[0] == 0) {
		strcpy(sys->name, "Unnamed");
		sys->default_on = 0;
		sys->output = 0;
		sys->autocommit = 1;
	}

	if (sys->vin_adc.a == 0) {
		sys->vin_adc.a = FLOAT_TO_FIXED(16*3.3);
		sys->vin_adc.b = 0;
	}

	if (sys->vout_adc.a == 0) {
		sys->vout_adc.a = FLOAT_TO_FIXED(3.3/0.073);
		sys->vout_adc.b = FLOAT_TO_FIXED(0.452);
	}

	if (sys->cout_adc.a == 0) {
		sys->cout_adc.a = FLOAT_TO_FIXED(3.3*1.25);
		sys->cout_adc.b = FLOAT_TO_FIXED(0.2);
	}

	if (sys->vout_pwm.a == 0) {
		sys->vout_pwm.a = FLOAT_TO_FIXED(0.073);
		sys->vout_pwm.b = FLOAT_TO_FIXED(0.033);
	}

	if (sys->cout_pwm.a == 0) {
		sys->cout_pwm.a = FLOAT_TO_FIXED(0.8);
		sys->cout_pwm.b = FLOAT_TO_FIXED(0.160);
	}
}

void config_load_system(cfg_system_t *sys)
{
#if TEST
	memset(sys, 0, sizeof(*sys));
#else
	memcpy(sys, SYSTEM_CONFIG, sizeof(*sys));
#endif
	validate_system_config(sys);
}

void config_save_system(cfg_system_t *sys)
{
	eeprom_save_data((uint8_t*)SYSTEM_CONFIG, (uint8_t*)sys, sizeof(*sys));
}

inline void validate_output_config(cfg_output_t *cfg)
{
	if (cfg->version != OUTPUT_CFG_VERSION || cfg->vset == 0 || cfg->cset == 0) {
		cfg->vset = FLOAT_TO_FIXED(5); // 5V
		cfg->cset = FLOAT_TO_FIXED(0.5); // 0.5A
		cfg->vshutdown = 0;
		cfg->cshutdown = 0;
	}
}

void config_load_output(cfg_output_t *cfg)
{
#if TEST
	memset(cfg, 0, sizeof(*cfg));
#else
	memcpy(cfg, OUTPUT_CONFIG, sizeof(*cfg));
#endif
	validate_output_config(cfg);
}

void config_save_output(cfg_output_t *cfg)
{
	eeprom_save_data((uint8_t*)OUTPUT_CONFIG, (uint8_t*)cfg, sizeof(*cfg));
}

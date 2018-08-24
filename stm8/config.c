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

#define SYSTEM_CFG_VERSION 2
#define OUTPUT_CFG_VERSION 1

#define DEFAULT_NAME_STR "Unnamed"

cfg_system_t default_cfg_system = {
	.version = SYSTEM_CFG_VERSION,
	.name = "Unnamed",
	.default_on = 0,
	.output = 0,
	.autocommit = 1,

	.vin_adc = { .a = FLOAT_TO_FIXED(16*3.3/8.0), .b = 0 },
	.vout_adc = { .a = FLOAT_TO_FIXED(3.3/0.073/8.0), .b = FLOAT_TO_FIXED(452) },
	.cout_adc = { .a = FLOAT_TO_FIXED(3.3*1.25/8.0), .b = FLOAT_TO_FIXED(200) },
	.vout_pwm = { .a = FLOAT_TO_FIXED(8*0.073/3.3), .b = FLOAT_TO_FIXED(33) },
	.cout_pwm = { .a = FLOAT_TO_FIXED(8*0.8/3.3), .b = FLOAT_TO_FIXED(160) },
};

cfg_output_t default_cfg_output = {
	OUTPUT_CFG_VERSION,
	5000, // 5V
	500, // 0.5A
	0,
	0,
};

void config_default_system(cfg_system_t *sys)
{
	memcpy(sys, &default_cfg_system, sizeof(default_cfg_system));
}

inline void validate_system_config(cfg_system_t *sys)
{
	if (sys->version != SYSTEM_CFG_VERSION ||
			sys->name[0] == 0 ||
			sys->vin_adc.a == 0 ||
			sys->vout_adc.a == 0 ||
			sys->cout_adc.a == 0 ||
			sys->vout_pwm.a == 0 ||
			sys->cout_pwm.a == 0
			)
	{
		config_default_system(sys);
		// TODO: If we want easy upgradability we can implement it here to
		// upgrade from an old struct to a new one.
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

void config_default_output(cfg_output_t *cfg)
{
	memcpy(cfg, &default_cfg_output, sizeof(default_cfg_output));
}

inline void validate_output_config(cfg_output_t *cfg)
{
	if (cfg->version != OUTPUT_CFG_VERSION || cfg->vset == 0 || cfg->cset == 0) {
		config_default_output(cfg);
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

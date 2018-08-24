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

#include "adc.h"
#include "stm8s.h"

// We only have a 10-bit ADC, so oversample for 3 bits of additional accuracy
#define OVERSAMPLE_BITS 3
#define OVERSAMPLE_DIVIDE (1 << OVERSAMPLE_BITS)
#define OVERSAMPLE_COUNT (OVERSAMPLE_DIVIDE << OVERSAMPLE_BITS)

static uint32_t sum;
static uint8_t count;

void adc_init(void)
{
	ADC1_CR1 = 0x70; // Power down, clock/18
	ADC1_CR2 = 0x08; // Right alignment
	ADC1_CR3 = 0x00;
	ADC1_CSR = 0x00;

	ADC1_TDRL = 0x0F;

	ADC1_CR1 |= 0x01; // Turn on the ADC
}

inline void _adc_start(void)
{
	ADC1_CSR &= 0x7F; // Turn off EOC
	ADC1_CR1 |= 1; // Trigger conversion
}

void adc_start(uint8_t channel)
{
	uint8_t csr = ADC1_CSR;
	csr &= 0x70; // Turn off EOC, Clear Channel
	csr |= channel; // Select channel
	ADC1_CSR = csr;

	ADC1_CR1 |= 1; // Trigger conversion

	sum = 0;
	count = 0;
}

fixed_t adc_to_volt(uint16_t adc, calibrate_t *cal)
{
	uint32_t tmp;

	tmp = adc * cal->a;

	if (tmp > cal->b)
		tmp -= cal->b;
	else
		tmp = 0;

	return fixed_round(tmp);
}

inline uint16_t _adc_read(void)
{
		uint16_t val = ADC1_DRL;
		uint16_t valh = ADC1_DRH;

		return val | (valh<<8);
}

uint16_t adc_read(void)
{
	return sum / OVERSAMPLE_DIVIDE;
}

uint8_t adc_channel(void)
{
		return ADC1_CSR & 0x0F;
}

uint8_t adc_ready(void)
{
	if (ADC1_CSR & 0x80) {
		sum += _adc_read();
		count++;
		if (count < OVERSAMPLE_COUNT) {
			_adc_start();
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

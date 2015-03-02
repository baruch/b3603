#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "fixedpoint.h"
#include "calibrate.h"
#include "stm8s.h"

void adc_init(void);
void adc_start(uint8_t channel);
fixed_t adc_to_volt(uint16_t adc, calibrate_t *cal);

inline uint8_t adc_ready()
{
	return ADC1_CSR & 0x80;
}

#endif

#include "adc.h"
#include "stm8s.h"

void adc_init(void)
{
	ADC1_CR1 = 0x70; // Power down, clock/18
	ADC1_CR2 = 0x08; // Right alignment
	ADC1_CR3 = 0x00;
	ADC1_CSR = 0x00;

	ADC1_TDRL = 0x0F;

	ADC1_CR1 |= 0x01; // Turn on the ADC
}

void adc_start(uint8_t channel)
{
	uint8_t csr = ADC1_CSR;
	csr &= 0x70; // Turn off EOC, Clear Channel
	csr |= channel; // Select channel
	ADC1_CSR = csr;

	ADC1_CR1 |= 1; // Trigger conversion
}

fixed_t adc_to_volt(uint16_t adc, calibrate_t *cal)
{
	fixed_t tmp;

	tmp = fixed_mult(adc, cal->a);

	if (tmp > cal->b)
		tmp -= cal->b;

	return tmp;
}



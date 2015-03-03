#include "adc.h"
#include "stm8s.h"

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
	fixed_t tmp;

	tmp = fixed_mult13(adc, cal->a);

	if (tmp > cal->b)
		tmp -= cal->b;

	return tmp;
}

inline uint16_t _adc_read(void)
{
		uint16_t val = ADC1_DRL;
		uint16_t valh = ADC1_DRH;

		return val | (valh<<8);
}

uint16_t adc_read(void)
{
	return sum/8;
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
		if (count < 64) {
			_adc_start();
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

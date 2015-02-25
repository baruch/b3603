#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef struct {
	uint8_t output;
	uint16_t vset;
	uint16_t cset;
	uint16_t vshutdown;
	uint16_t cshutdown;
} cfg_output_t;

// These parameters correspond to the linear formula:
// y = a*x + b
// where a and b are the coefficients, x is the input and y the calculated output
typedef struct {
	uint16_t a;
	uint16_t b;
} calibrate_t;

typedef struct {
	uint8_t name[17];
	uint8_t default_on;

	calibrate_t vin_adc;
	calibrate_t vout_adc;
	calibrate_t cout_adc;

	calibrate_t vout_pwm;
	calibrate_t cout_pwm;
} cfg_system_t;

#endif

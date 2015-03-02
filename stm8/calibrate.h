#include <stdint.h>

#include "config.h"

void calibrate_vin(int point, fixed_t set_point, uint16_t adc_val, calibrate_t *c);
void calibrate_vout(int point, fixed_t set_point, uint16_t adc_val, calibrate_t *c);

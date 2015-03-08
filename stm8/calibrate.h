#include <stdint.h>

#include "config.h"

void calibrate_vin(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c);
void calibrate_vout(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c);
void calibrate_cout(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c);

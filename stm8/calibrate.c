#include "calibrate.h"
#include "uart.h"

struct calibrate_points {
	uint16_t set_point;
	uint16_t val;
};

struct calibrate_points vin_points[2];

void calibrate_vin(int point, uint16_t set_point, uint16_t adc_val, calibrate_t *c)
{
	if (point < 1 && point > 2)
		return;

	uart_write_str("CALVIN");
	uart_write_ch('0' + point);
	uart_write_str(": VIN=");
	uart_write_fixed_point(set_point);
	uart_write_str(" ADC=");
	uart_write_int(adc_val);
	uart_write_str("\r\n");

	vin_points[point-1].set_point = set_point;
	vin_points[point-1].val = adc_val;

	if (vin_points[0].set_point && vin_points[0].val && vin_points[1].set_point && vin_points[1].val) {
		uint32_t tmp1;
		uint32_t tmp2;

		tmp1 = (vin_points[1].set_point - vin_points[0].set_point);
		tmp1 <<= 10;
		tmp2 = vin_points[1].val - vin_points[0].val;
		tmp1 /= tmp2;

		if (tmp1 > 65535)
			uart_write_str("A OVERFLOW\r\n");

		tmp2 = vin_points[0].val * tmp1;
		tmp2 >>= 10;
		tmp2 = vin_points[0].set_point - tmp2;

		c->a = tmp1;
		c->b = tmp2;
	}
}

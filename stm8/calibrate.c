#include "calibrate.h"
#include "uart.h"
#include "fixedpoint.h"

struct calibrate_points {
	fixed_t set_point;
	uint16_t val;
};

struct calibrate_points vin_points[2];
struct calibrate_points vout_points[2];

inline void calibrate_calc(struct calibrate_points *points, calibrate_t *c)
{
	if (points[0].set_point && points[0].val && points[1].set_point && points[1].val) {
		uint32_t tmp1;
		uint32_t tmp2;
		uint32_t tmp3;

		tmp1 = (points[1].set_point - points[0].set_point);
		tmp1 <<= FIXED_SHIFT;
		tmp3 = points[1].val - points[0].val;
		tmp1 /= tmp3;

		tmp2 = points[0].val;
		tmp2 *= tmp1;
		tmp2 >>= 10; // This is adc value, always a fraction of 10 bits, not dependent on FIXED_SHIFT
		while (tmp2 < points[0].set_point)
		{
			tmp2 = points[0].val;
			tmp1++;
			tmp2 *= tmp1;
			tmp2 >>= FIXED_SHIFT;
		}
		tmp2 -= points[0].set_point;

		c->a = tmp1;
		c->b = tmp2;
	}
}

static void calibrate_point(struct calibrate_points *points, int point, fixed_t set_point, uint16_t adc_val, calibrate_t *c)
{
	if (point < 1 && point > 2)
		return;

	uart_write_ch('0' + point);
	uart_write_str(": VIN=");
	uart_write_fixed_point(set_point);
	uart_write_str(" ADC=");
	uart_write_int(adc_val);
	uart_write_str("\r\n");

	points[point-1].set_point = set_point;
	points[point-1].val = adc_val;

	calibrate_calc(points, c);
}

void calibrate_vin(int point, fixed_t set_point, uint16_t adc_val, calibrate_t *c)
{
	uart_write_str("CALVIN");
	calibrate_point(vin_points, point, set_point, adc_val, c);
}

void calibrate_vout(int point, fixed_t set_point, uint16_t adc_val, calibrate_t *c)
{
	uart_write_str("CALVOUT");
	calibrate_point(vout_points, point, set_point, adc_val, c);
}

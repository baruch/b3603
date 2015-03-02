#include "parse.h"
#include "uart.h"
#include "fixedpoint.h"

fixed_t parse_fixed_point(uint8_t *s)
{
	uint8_t *t = s;
	uint16_t val = 0;
	uint32_t fraction_digits = 0;
	uint16_t whole_digits = 0;
	uint16_t fraction_factor = 1;

	for (; *s != 0; s++) {
		if (*s == '.') {
			s++; // Skip the dot
			break;
		}
		if (*s >= '0' && *s <= '9') {
			val = *s - '0';
			whole_digits *= 10;
			whole_digits += val;
			if (whole_digits > 62)
				goto invalid_number;
		} else {
			goto invalid_number;
		}
	}

	whole_digits <<= FIXED_SHIFT;

	for (; *s != 0 && fraction_factor < 1000; s++) {
		if (*s >= '0' && *s <= '9') {
			val = *s - '0';
			fraction_digits *= 10;
			fraction_digits += val;
			fraction_factor *= 10;
		} else {
			goto invalid_number;
		}
	}

	fraction_digits <<= FIXED_SHIFT;
	fraction_digits /= fraction_factor;

	return whole_digits + fraction_digits + 1;

invalid_number:
	uart_write_str("INVALID NUMBER '");
	uart_write_str(t);
	uart_write_ch('\'');
	uart_write_str("\r\n");
	return 0xFFFF;
}

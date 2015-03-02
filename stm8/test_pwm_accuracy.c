#include <stdint.h>

void uart_write_ch(const char ch) { (void)ch; }
void uart_write_str(const char *s) { (void)s; }


#include "fixedpoint.c"
#include "outputs.c"
#include "parse.c"
#include "eeprom.c"
#include "config.c"

#include <stdio.h>

int main()
{
	uint8_t str[10];
	uint16_t val;
	uint16_t pwm;
	int digit1, digit2, digit3;
	cfg_system_t sys;

	config_load_system(&sys);

	str[0] = '0';
	str[1] = '.';
	str[2] = '0';
	str[3] = '0';
	str[4] = 0;

	for (digit1 = '0'; digit1 <= '9'; digit1++) {
		str[0] = digit1;
		for (digit2 = '0'; digit2 <= '9'; digit2++) {
			str[2] = digit2;
			for (digit3 = '0'; digit3 <= '9'; digit3++) {
				str[3] = digit3;

				val = parse_fixed_point(str);
				pwm = pwm_from_set(val, &(sys.vout_pwm));
				printf("%s %u %u\n", str, val, pwm);
			}
		}
	}
return 0;
}

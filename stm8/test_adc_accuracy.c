#include <stdint.h>

void uart_write_ch(const char ch) { (void)ch; }
void uart_write_str(const char *s) { (void)s; }


#include "fixedpoint.c"
#include "adc.c"
#include "eeprom.c"
#include "config.c"

#include <stdio.h>

int main()
{
	uint16_t adc;
	uint16_t mvolt;
	cfg_system_t sys;

	config_load_system(&sys);

	for (adc = 0; adc < 8192; adc++) {
		mvolt = adc_to_volt(adc, &(sys.vin_adc));
		printf("%u %u %u.%03u\n", adc, mvolt, mvolt/1000, mvolt%1000);
	}
	return 0;
}

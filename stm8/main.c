#define FW_VERSION "0.1"

#include "stm8s.h"
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "display.h"
#include "fixedpoint.h"
#include "uart.h"
#include "eeprom.h"
#include "outputs.h"
#include "config.h"

const uint16_t cap_vmin = (1<<10) / 100; // 10 mV
const uint16_t cap_vmax = 35<<10; // 35 V
const uint16_t cap_vstep = (1<<10) / 100; // 10mV

const uint16_t cap_cmin = (1<<10) / 1000; // 1 mA
const uint16_t cap_cmax = 3<<10; // 3 A
const uint16_t cap_cstep = (1<<10) / 1000; // 1 mA

cfg_system_t cfg_system;
cfg_output_t cfg_output;

uint16_t state_vin_raw;
uint16_t state_vout_raw;
uint16_t state_cout_raw;
uint16_t state_vin;
uint16_t state_vout;
uint16_t state_cout;
uint8_t state_constant_current; // If false, we are in constant voltage
uint8_t state_pc3;

inline iwatchdog_init(void)
{
	IWDG_KR = 0xCC; // Enable IWDG
	// The default values give us about 15msec between pings
}

inline iwatchdog_tick(void)
{
	IWDG_KR = 0xAA; // Reset the counter
}

void set_name(uint8_t *name)
{
	uint8_t idx;

	for (idx = 0; name[idx] != 0; idx++) {
		if (!isprint(name[idx]))
			name[idx] = '.'; // Eliminate non-printable chars
	}

	strncpy(cfg_system.name, name, sizeof(cfg_system.name));
	cfg_system.name[sizeof(cfg_system.name)-1] = 0;

	uart_write_str("SNAME: ");
	uart_write_str(cfg_system.name);
	uart_write_str("\r\n");
}

void set_output(uint8_t *s)
{
	if (s[1] != 0) {
//		uart_write_str("OUTPUT takes either 0 for OFF or 1 for ON, received: \"");
		uart_write_str(s);
		uart_write_str("\"\r\n");
		return;
	}

	if (s[0] == '0') {
		cfg_output.output = 0;
		uart_write_str("OUTPUT: OFF\r\n");
	} else if (s[0] == '1') {
		cfg_output.output = 1;
		uart_write_str("OUTPUT: ON\r\n");
	} else {
//		uart_write_str("OUTPUT takes either 0 for OFF or 1 for ON, received: \"");
		uart_write_str(s);
		uart_write_str("\"\r\n");
	}

	if (cfg_system.autocommit) {
		output_commit(&cfg_output, &cfg_system);
		output_check_state(&cfg_output, state_constant_current);
	}
}

uint16_t parse_fixed_point(uint8_t *s)
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

	whole_digits <<= 10;

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

	fraction_digits <<= 10;
	fraction_digits /= fraction_factor;

	return whole_digits + fraction_digits + 1;

invalid_number:
	uart_write_str("INVALID NUMBER '");
	uart_write_str(t);
	uart_write_ch('\'');
	uart_write_str("\r\n");
	return 0xFFFF;
}

void set_voltage(uint8_t *s)
{
	uint16_t val;

	val = parse_fixed_point(s);
	if (val == 0xFFFF)
		return;

	if (val > cap_vmax) {
		uart_write_str("VOLTAGE VALUE TOO HIGH\r\n");
		return;
	}
	if (val < cap_vmin) {
		uart_write_str("VOLTAGE VALUE TOO LOW\r\n");
		return;
	}

	uart_write_str("VOLTAGE: SET ");
	uart_write_fixed_point(val);
	uart_write_str("\r\n");
	cfg_output.vset = val;

	if (cfg_system.autocommit)
		output_commit(&cfg_output, &cfg_system);
}

void set_current(uint8_t *s)
{
	uint16_t val;

	val = parse_fixed_point(s);
	if (val == 0xFFFF)
		return;

	if (val > cap_cmax) {
		uart_write_str("CURRENT VALUE TOO HIGH\r\n");
		return;
	}
	if (val < cap_cmin) {
		uart_write_str("CURRENT VALUE TOO LOW\r\n");
		return;
	}

	uart_write_str("CURRENT: SET ");
	uart_write_fixed_point(val);
	uart_write_str("\r\n");
	cfg_output.cset = val;

	if (cfg_system.autocommit)
		output_commit(&cfg_output, &cfg_system);
}

void set_autocommit(uint8_t *s)
{
	if (strcmp(s, "1") == 0 || strcmp(s, "YES") == 0) {
		cfg_system.autocommit = 1;
		uart_write_str("AUTOCOMMIT: YES\r\n");
	} else if (strcmp(s, "0") == 0 || strcmp(s, "NO") == 0) {
		cfg_system.autocommit = 0;
		uart_write_str("AUTOCOMMIT: NO\r\n");
	} else {
		uart_write_str("UNKNOWN AUTOCOMMIT ARG: ");
		uart_write_str(s);
		uart_write_str("\r\n");
	}
}

void process_input()
{
	// Eliminate the CR/LF character
	uart_read_buf[uart_read_len-1] = 0;

	if (strcmp(uart_read_buf, "MODEL") == 0) {
		uart_write_str("MODEL: B3606\r\n");
	} else if (strcmp(uart_read_buf, "VERSION") == 0) {
		uart_write_str("VERSION: " FW_VERSION "\r\n");
	} else if (strcmp(uart_read_buf, "SYSTEM") == 0) {
		uart_write_str("MODEL: B3606\r\n");
		uart_write_str("VERSION: " FW_VERSION "\r\n");
		uart_write_str("NAME: ");
		uart_write_str(cfg_system.name);
		uart_write_str("\r\n");
		uart_write_str("ONSTARTUP: ");
		uart_write_str(cfg_system.default_on ? "ON" : "OFF");
		uart_write_str("\r\n");
		uart_write_str("AUTOCOMMIT: ");
		uart_write_str(cfg_system.autocommit ? "YES" : "NO");
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "CALIBRATION") == 0) {
		uart_write_str("CALIBRATE VIN ADC: ");
		uart_write_fixed_point(cfg_system.vin_adc.a);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_system.vin_adc.b);
		uart_write_str("\r\n");
		uart_write_str("CALIBRATE VOUT ADC: ");
		uart_write_fixed_point(cfg_system.vout_adc.a);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_system.vout_adc.b);
		uart_write_str("\r\n");
		uart_write_str("CALIBRATE COUT ADC: ");
		uart_write_fixed_point(cfg_system.cout_adc.a);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_system.cout_adc.b);
		uart_write_str("\r\n");
		uart_write_str("CALIBRATE VOUT PWM: ");
		uart_write_fixed_point(cfg_system.vout_pwm.a);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_system.vout_pwm.b);
		uart_write_str("\r\n");
		uart_write_str("CALIBRATE COUT PWM: ");
		uart_write_fixed_point(cfg_system.cout_pwm.a);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_system.cout_pwm.b);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "VLIST") == 0) {
		uart_write_str("VLIST:\r\nVOLTAGE MIN: ");
		uart_write_fixed_point(cap_vmin);
		uart_write_str("\r\nVOLTAGE MAX: ");
		uart_write_fixed_point(cap_vmax);
		uart_write_str("\r\nVOLTAGE STEP: ");
		uart_write_fixed_point(cap_vstep);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "CLIST") == 0) {
		uart_write_str("CLIST:\r\nCURRENT MIN: ");
		uart_write_fixed_point(cap_cmin);
		uart_write_str("\r\nCURRENT MAX: ");
		uart_write_fixed_point(cap_cmax);
		uart_write_str("\r\nCURRENT STEP: ");
		uart_write_fixed_point(cap_cstep);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "CONFIG") == 0) {
		uart_write_str("CONFIG:\r\nOUTPUT: ");
		uart_write_str(cfg_output.output ? "ON" : "OFF");
		uart_write_str("\r\nVOLTAGE SET: ");
		uart_write_fixed_point(cfg_output.vset);
		uart_write_str("\r\nCURRENT SET: ");
		uart_write_fixed_point(cfg_output.cset);
		uart_write_str("\r\nVOLTAGE SHUTDOWN: ");
		if (cfg_output.vshutdown == 0)
			uart_write_str("DISABLED");
		else
			uart_write_fixed_point(cfg_output.vshutdown);
		uart_write_str("\r\nCURRENT SHUTDOWN: ");
		if (cfg_output.cshutdown == 0)
			uart_write_str("DISABLED");
		else
			uart_write_fixed_point(cfg_output.cshutdown);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "STATUS") == 0) {
		uart_write_str("STATUS:\r\nOUTPUT: ");
		uart_write_str(cfg_output.output ? "ON" : "OFF");
		uart_write_str("\r\nVOLTAGE IN: ");
		uart_write_fixed_point(state_vin);
		uart_write_str("\r\nVOLTAGE OUT: ");
		uart_write_fixed_point(cfg_output.output ? state_vout : 0);
		uart_write_str("\r\nCURRENT OUT: ");
		uart_write_fixed_point(cfg_output.output ? state_cout : 0);
		uart_write_str("\r\nCONSTANT: ");
		uart_write_str(state_constant_current ? "CURRENT" : "VOLTAGE");
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "RSTATUS") == 0) {
		uart_write_str("RSTATUS:\r\nOUTPUT: ");
		uart_write_str(cfg_output.output ? "ON" : "OFF");
		uart_write_str("\r\nVOLTAGE IN ADC: ");
		uart_write_int(state_vin_raw);
		uart_write_str("\r\nVOLTAGE OUT ADC: ");
		uart_write_int(state_vout_raw);
		uart_write_str("\r\nCURRENT OUT ADC: ");
		uart_write_int(state_cout_raw);
		uart_write_str("\r\nCONSTANT: ");
		uart_write_str(state_constant_current ? "CURRENT" : "VOLTAGE");
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "COMMIT") == 0) {
		output_commit(&cfg_output, &cfg_system);
		output_check_state(&cfg_output, state_constant_current);
#if DEBUG
	} else if (strcmp(uart_read_buf, "STUCK") == 0) {
		// Allows debugging of the IWDG feature
		uart_write_str("STUCK\r\n");
		while (1)
			uart_write_from_buf(); // Flush write buf outside
#endif
	} else {
		// Process commands with arguments
		uint8_t idx;
		uint8_t space_found = 0;

		for (idx = 0; idx < uart_read_len; idx++) {
			if (uart_read_buf[idx] == ' ') {
				uart_read_buf[idx] = 0;
				space_found = 1;
				break;
			}
		}

		if (space_found) {
			if (strcmp(uart_read_buf, "SNAME") == 0) {
				set_name(uart_read_buf + idx + 1);
			} else if (strcmp(uart_read_buf, "OUTPUT") == 0) {
				set_output(uart_read_buf + idx + 1);
			} else if (strcmp(uart_read_buf, "VOLTAGE") == 0) {
				set_voltage(uart_read_buf + idx + 1);
			} else if (strcmp(uart_read_buf, "CURRENT") == 0) {
				set_current(uart_read_buf + idx + 1);
			} else if (strcmp(uart_read_buf, "AUTOCOMMIT") == 0) {
				set_autocommit(uart_read_buf + idx + 1);
			} else {
				uart_write_str("UNKNOWN COMMAND!\r\n");
			}
		} else {
			uart_write_str("UNKNOWN COMMAND\r\n");
		}
	}

	uart_read_len = 0;
	read_newline = 0;
}

void clk_init()
{
	CLK_CKDIVR = 0x00; // Set the frequency to 16 MHz
}

void pinout_init()
{
	// PA1 is 74HC595 SHCP, output
	// PA2 is 74HC595 STCP, output
	// PA3 is CV/CC leds, output (& input to disable)
	PA_ODR = 0;
	PA_DDR = (1<<1) | (1<<2);
	PA_CR1 = (1<<1) | (1<<2) | (1<<3);
	PA_CR2 = (1<<1) | (1<<2) | (1<<3);

	// PB4 is Enable control, output
	// PB5 is CV/CC sense, input
	PB_ODR = (1<<4); // For safety we start with off-state
	PB_DDR = (1<<4) | (1<<5);
	PB_CR1 = (1<<4);
	PB_CR2 = 0;

	// PC3 is unknown, input
	// PC4 is Iout sense, input adc, AIN2
	// PC5 is Vout control, output
	// PC6 is Iout control, output
	// PC7 is Button 1, input
	PC_ODR = 0;
	PC_DDR = (1<<5) || (1<<6);
	PC_CR1 = (1<<7); // For the button
	PC_CR2 = (1<<5) | (1<<6);

	// PD1 is Button 2, input
	// PD2 is Vout sense, input adc, AIN3
	// PD3 is Vin sense, input adc, AIN4
	// PD4 is 74HC595 DS, output
	PD_DDR = (1<<4);
	PD_CR1 = (1<<1) | (1<<4); // For the button
	PD_CR2 = (1<<4);
}

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

uint8_t adc_ready()
{
	return ADC1_CSR & 0x80;
}

void config_load(void)
{
	strcpy(cfg_system.name, "Unnamed");
	cfg_system.default_on = 0;
	cfg_system.autocommit = 1;

	cfg_output.output = 0;
	cfg_output.vset = 5<<10; // 5V
	cfg_output.cset = (1<<10) / 2; // 0.5A
	cfg_output.vshutdown = 0;
	cfg_output.cshutdown = 0;

	cfg_system.vin_adc.a = 16 << 10;
	cfg_system.vin_adc.b = 0;

	cfg_system.vout_adc.a = 14027; // 1/0.073 << 10
	cfg_system.vout_adc.b = 462; // (452<<10) / 1000; giving constant here since it can overflow in uint16_t

	cfg_system.cout_adc.a = 1280; //(125<<10)/100; giving constant here since it can overflow in uint16_t
	cfg_system.cout_adc.b = (2<<10)/10;

	cfg_system.vout_pwm.a = 75; // 0.073 = (73<<10)/1000
	cfg_system.vout_pwm.b = 34; // 0.033 = (33<<10)/1000

	cfg_system.cout_pwm.a = 819; // 0.8 = (8<<10)/10
	cfg_system.cout_pwm.b = 164; // 0.160 = (16<<10)/100

	state_pc3 = 1;

	if (cfg_system.default_on)
		cfg_output.output = 1;

}

uint16_t adc_to_volt(uint16_t adc, calibrate_t *cal)
{
	uint32_t tmp;
	uint16_t tmp16;

	tmp = adc;
	tmp *= cal->a;
	tmp *= 33;
	tmp /= 10;
	tmp >>= 10;

	tmp16 = tmp;
	if (tmp16 > cal->b)
		tmp16 -= cal->b;

	return tmp16;
}

void read_state(void)
{
	uint8_t tmp;

	tmp = (PC_IDR & (1<<3)) ? 1 : 0;
	if (state_pc3 != tmp) {
		uart_write_str("PC3 is now ");
		uart_write_ch('0' + tmp);
		uart_write_str("\r\n");
		state_pc3 = tmp;
	}

	tmp = (PB_IDR & (1<<5)) ? 1 : 0;
	if (state_constant_current != tmp) {
		state_constant_current = tmp;
		output_check_state(&cfg_output, state_constant_current);
	}

	if ((ADC1_CSR & 0x0F) == 0) {
		adc_start(2);
	} else if (adc_ready()) {
		uint16_t val = ADC1_DRL;
		uint16_t valh = ADC1_DRH;
		uint8_t ch = ADC1_CSR & 0x0F;

		val |= valh << 8;

		switch (ch) {
			case 2:
				state_cout_raw = val;
				// Calculation: val * cal_cout_a * 3.3 / 1024 - cal_cout_b
				state_cout = adc_to_volt(val, &cfg_system.cout_adc);
				ch = 3;
				break;
			case 3:
				state_vout_raw = val;
				// Calculation: val * cal_vout_a * 3.3 / 1024 - cal_vout_b
				state_vout = adc_to_volt(val, &cfg_system.vout_adc);
				ch = 4;
				break;
			case 4:
				state_vin_raw = val;
				// Calculation: val * cal_vin * 3.3 / 1024
				state_vin = adc_to_volt(val, &cfg_system.vin_adc);
				ch = 2;
				{
					uint8_t ch1;
					uint8_t ch2;
					uint8_t ch3;
					uint8_t ch4;

					val = state_vin >> 10;

					ch2 = '0' + (val % 10);
					ch1 = '0' + (val / 10) % 10;

					val = state_vin - (val<<10);
					val *= 100;
					val >>= 10;

					ch4 = '0' + (val % 10);
					ch3 = '0' + (val / 10) % 10;

					display_show(ch1, 0, ch2, 1, ch3, 0, ch4, 0);
				}
				break;
		}

		adc_start(ch);
	}
}

void flush_uart_writes(void)
{
	int i;
	for (i = 0; i < 10000; i++) {
		uart_write_from_buf();
	}
}

void ensure_afr0_set(void)
{
	if ((OPT2 & 1) == 0) {
		flush_uart_writes();
		if (eeprom_set_afr0()) {
			uart_write_str("AFR0 set, reseting the unit\r\n");
			flush_uart_writes();
			iwatchdog_init();
			while (1); // Force a reset in a few msec
		}
		else {
			uart_write_str("AFR0 not set and programming failed!\r\n");
		}
	}
}

int main()
{
	unsigned long i = 0;

	pinout_init();
	clk_init();
	uart_init();
	pwm_init();
	adc_init();

	config_load();

	uart_write_str("\r\nB3606 starting: Version " FW_VERSION "\r\n");

	ensure_afr0_set();

	iwatchdog_init();
	output_commit(&cfg_output, &cfg_system);

	do {
		iwatchdog_tick();
		uart_write_from_buf();

		read_state();
		display_refresh();

		uart_read_to_buf();
		if (read_newline) {
			process_input();
		}
	} while(1);
}

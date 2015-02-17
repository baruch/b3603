#define FW_VERSION "0.0.1"

#include "stm8l.h"
#include <string.h>
#include <stdint.h>
#include <ctype.h>

const uint16_t cap_vmin = 10; // 10 mV
const uint16_t cap_vmax = 35000; // 35 V
const uint16_t cap_vstep = 10; // 10mV

const uint16_t cap_cmin = 10; // 10 mA
const uint16_t cap_cmax = 3000; // 3 A
const uint16_t cap_cstep = 10; // 10 mA

uint8_t uart_write_buf[64];
uint8_t uart_write_start;
uint8_t uart_write_len;

uint8_t uart_read_buf[64];
uint8_t uart_read_len;
uint8_t read_newline;

float cal_vin_a;
float cal_vin_b;
float cal_vout;
float cal_cout;

uint8_t cfg_name[17];
uint8_t cfg_default;
uint8_t cfg_output;
uint16_t cfg_vset;
uint16_t cfg_cset;
uint16_t cfg_vshutdown;
uint16_t cfg_cshutdown;

uint16_t state_vin_raw;
uint16_t state_vout_raw;
uint16_t state_cout_raw;
uint16_t state_vin;
uint16_t state_vout;
uint16_t state_cout;
uint8_t state_constant_current; // If false, we are in constant voltage
uint8_t state_power_good;
uint8_t state_power_good_prev;

uint8_t uart_write_ready(void)
{
	return (USART1_SR & USART_SR_TXE);
}

uint8_t uart_read_available(void)
{
	return (USART1_SR & USART_SR_RXNE);
}

void uart_write_ch(const char ch)
{
	if (uart_write_len < sizeof(uart_write_buf))
		uart_write_buf[uart_write_len++] = ch;
}

void uart_flush_write()
{
	while(!(USART1_SR & USART_SR_TXE));
}

uint8_t uart_write_free_space(void)
{
	return sizeof(uart_write_buf) - uart_write_len;
}

void uart_write_str(const char *str)
{
	uint8_t i;

	// Move the buffer to the start
	if (uart_write_start > 0) {
		for (i = 0; i < uart_write_len; i++) {
			uart_write_buf[i] = uart_write_buf[i+uart_write_start];
		}
		uart_write_start = 0;
	}

	for(i = 0; str[i] != 0 && uart_write_len < sizeof(uart_write_buf); i++) {
		uart_write_buf[uart_write_len] = str[i];
		uart_write_len++;
	}
}

void uart_write_digit(uint16_t digit)
{
	if (digit > 9)
		uart_write_ch('X');
	else
		uart_write_ch('0' + digit);
}

void uart_write_int_dot(uint16_t val, int dot_pos)
{
	uint8_t ch[6];
	uint8_t i;
	uint8_t highest_nonzero = 0;

	ch[0] = '0';

	for (i = 0; i < 6 && val != 0; i++) {
		uint16_t digit = val % 10;
		ch[i] = '0' + digit;
		val /= 10;
		if (digit)
			highest_nonzero = i;
	}

	// Print value before dot
	for (i = highest_nonzero+1; i > dot_pos; i--) {
		uart_write_ch(ch[i-1]);
	}
	if (highest_nonzero+1 <= dot_pos)
		uart_write_ch('0'); // Emit leading zero

	if (dot_pos == 0)
		return;

	// Print dot
	uart_write_ch('.');

	// Print value after dot
	for (; i > 0; i--) {
		uart_write_ch(ch[i-1]);
	}
}

#define uart_write_int(val) uart_write_int_dot(val, 0)
#define uart_write_fixed_point(val) uart_write_int_dot(val, 3)

void uart_write_from_buf(void)
{
	if (uart_write_len > 0 && uart_write_ready()) {
		USART1_DR = uart_write_buf[uart_write_start];
		uart_write_start++;
		uart_write_len--;

		if (uart_write_len == 0)
			uart_write_start = 0;
	}
}

uint8_t uart_read_ch(void)
{
	return USART1_DR;
}

void uart_read_to_buf(void)
{
	// Don't read if we are writing
	if (uart_write_len > 0 || !uart_write_ready())
		return;

	if (uart_read_available()) {
		uint8_t ch = uart_read_ch();
		uart_read_buf[uart_read_len] = ch;
		uart_read_len++;

		if (ch == '\r' || ch == '\n')
			read_newline = 1;

		// Empty the read buf if we are overfilling and there is no full command in there
		if (uart_read_len == sizeof(uart_read_buf) && !read_newline) {
			uart_read_len = 0;
			uart_write_str("READ OVERFLOW\r\n");
		}
	}
}

void set_name(uint8_t *name)
{
	uint8_t idx;

	for (idx = 0; name[idx] != 0; idx++) {
		if (!isprint(name[idx]))
			name[idx] = '.'; // Eliminate non-printable chars
	}

	strncpy(cfg_name, name, sizeof(cfg_name));
	cfg_name[sizeof(cfg_name)-1] = 0;

	uart_write_str("SNAME: ");
	uart_write_str(cfg_name);
	uart_write_str("\r\n");
}

void process_input()
{
	// Eliminate the CR/LF character
	uart_read_buf[uart_read_len-1] = 0;

	if (strcmp(uart_read_buf, "MODEL") == 0) {
		uart_write_str("MODEL: B3606\r\n");
	} else if (strcmp(uart_read_buf, "VERSION") == 0) {
		uart_write_str("VERSION: " FW_VERSION "\r\n");
	} else if (strcmp(uart_read_buf, "NAME") == 0) {
		uart_write_str("NAME: ");
		uart_write_str(cfg_name);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "VLIST") == 0) {
		uart_write_str("VLIST: ");
		uart_write_fixed_point(cap_vmin);
		uart_write_ch('/');
		uart_write_fixed_point(cap_vmax);
		uart_write_ch('/');
		uart_write_fixed_point(cap_vstep);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "CLIST") == 0) {
		uart_write_str("CLIST: ");
		uart_write_fixed_point(cap_cmin);
		uart_write_ch('/');
		uart_write_fixed_point(cap_cmax);
		uart_write_ch('/');
		uart_write_fixed_point(cap_cstep);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "CONFIG") == 0) {
		uart_write_str("CONFIG: ");
		uart_write_ch('0' + cfg_output);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_vset);
		uart_write_ch('/');
		uart_write_fixed_point(cfg_cset);
		uart_write_ch('/');
		if (cfg_vshutdown == 0)
			uart_write_str("DISABLED");
		else
			uart_write_fixed_point(cfg_vshutdown);
		uart_write_ch('/');
		if (cfg_cshutdown == 0)
			uart_write_str("DISABLED");
		else
			uart_write_fixed_point(cfg_cshutdown);
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "STATUS") == 0) {
		uart_write_str("STATUS: ");
		uart_write_ch('0' + cfg_output);
		uart_write_ch('/');
		uart_write_fixed_point(state_vin);
		uart_write_ch('/');
		uart_write_fixed_point(state_vout);
		uart_write_ch('/');
		uart_write_fixed_point(state_cout);
		uart_write_ch('/');
		uart_write_ch(state_constant_current ? 'C' : 'V');
		uart_write_str("\r\n");
	} else if (strcmp(uart_read_buf, "RSTATUS") == 0) {
		uart_write_str("RSTATUS: ");
		uart_write_ch('0' + cfg_output);
		uart_write_ch('/');
		uart_write_int(state_vin_raw);
		uart_write_ch('/');
		uart_write_int(state_vout_raw);
		uart_write_ch('/');
		uart_write_int(state_cout_raw);
		uart_write_ch('/');
		uart_write_ch(state_constant_current ? 'C' : 'V');
		uart_write_str("\r\n");
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
	CLK_DIVR = 0x00; // Set the frequency to 16 MHz
}

void uart_init()
{
	USART1_CR1 = 0; // 8 bits, no parity
    USART1_CR3 &= ~(USART_CR3_STOP1 | USART_CR3_STOP2); // 1 stop bit

	USART1_BRR2 = 0x00;
	USART1_BRR1 = 0x0D; // 9600 baud, order important between BRRs, BRR1 must be last

	USART1_CR2 = USART_CR2_TEN | USART_CR2_REN; // Allow TX & RX

	uart_write_len = 0;
	uart_write_start = 0;
	uart_read_len = 0;
	read_newline = 0;
}

void pinout_init()
{
	// PA1 is 74HC595 SHCP, output
	// PA2 is 74HC595 STCP, output
	PA_DDR = (1<<1) | (1<<2);
	PA_CR1 = 0;
	PA_CR2 = 0;

	// PB4 is Enable control, output
	// PB5 is CV/CC sense, input
	PB_DDR = (1<<4);
	PB_CR1 = 0;
	PB_CR2 = 0;
	PB_ODR = (1<<4); // For safety we start with off-state

	// PC3 is power good, input
	// PC4 is Iout sense, input adc, AIN2
	// PC5 is Vout control, output
	// PC6 is Iout control, output
	// PC7 is Button 1, input
	PC_DDR = (1<<5) || (1<<6);
	PC_CR1 = (1<<7); // For the button
	PC_CR2 = 0;

	// PD1 is Button 2, input
	// PD2 is Vout sense, input adc, AIN3
	// PD3 is Vin sense, input adc, AIN4
	PD_DDR = 0;
	PD_CR1 = (1<<1); // For the button
	PD_CR2 = 0;
}

void pwm_init(void)
{
	/* Timer 1 Channel 1 for Iout control */
	TIM1_CR1 = 0x10; // Down direction
	TIM1_ARRH = 0x03; // Reload counter = 64535
	TIM1_ARRL = 0xFF;
	TIM1_PSCRH = 0; // Prescaler 0 means division by 1
	TIM1_PSCRL = 0;
	TIM1_RCR = 0; // Continuous

	TIM1_CCMR1 = 0x70;    //  Set up to use PWM mode 2.
	TIM1_CCER1 = 0x01;    //  Output is enabled for channel 1
	TIM1_CCR1H = 0x00;      //  Start with the PWM signal set
	TIM1_CCR1L = 0x20;

	TIM1_BKR = 0x80;       //  Enable the main output.

	/* Timer 2 Channel 1 for Vout control */
	TIM2_ARRH = 0x03; // Reload counter = 64535
	TIM2_ARRL = 0xFF;
	TIM2_PSCR = 0; // Prescaler 0 means division by 1
	TIM2_CR1 = 0x10; // Down direction

	TIM2_CCMR1 = 0x70;    //  Set up to use PWM mode 2.
	TIM2_CCER1 = 0x01;    //  Output is enabled for channel 1
	TIM2_CCR1H = 0x00;      //  Start with the PWM signal set
	TIM2_CCR1L = 0x20;

	// Enable the timers
	TIM1_CR1 |= 0x01; // Enable the counter
	TIM2_CR1 |= 0x01; // Enable the counter
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
	strcpy(cfg_name, "Unnamed");
	cfg_default = 0;
	cfg_output = 0;
	cfg_vset = 5000;
	cfg_cset = 1000;
	cfg_vshutdown = 0;
	cfg_cshutdown = 0;

	cal_vin_a = 53.67915;
	cal_vin_b = 0.25368;

	cal_vout = 54.0;
	cal_cout = 2.0;

	state_power_good = state_power_good_prev = 1;
}

void read_state(void)
{
	state_constant_current = (PB_IDR & (1<<5)) ? 1 : 0;
	state_power_good = (PC_IDR & (1<<3)) ? 1 : 0;
	if (state_power_good_prev != state_power_good) {
		uart_write_str("POWER LOST\r\n");
		state_power_good_prev = state_power_good;
		cfg_output = 0;
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
				state_cout = (val * cal_cout) / 1024.0 * 1000.0;
				ch = 3;
				break;
			case 3:
				state_vout_raw = val;
				state_vout = (val * cal_vout) / 1024.0 * 1000.0;
				ch = 4;
				break;
			case 4:
				state_vin_raw = val;
				state_vin = (( val * cal_vin_a ) / 1024.0 + cal_vin_b) * 1000.0;
				ch = 2;
				break;
		}

		adc_start(ch);
	}
}

uint8_t output_state(void)
{
	return (PB_ODR & (1<<4)) ? 0 : 1;
}

void control_outputs(void)
{
	if (cfg_output != output_state()) {
		PB_ODR ^= (1<<4);
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

	if (cfg_default)
		cfg_output = 1;

	control_outputs();

	uart_write_str("\r\nB3606 starting: Version " FW_VERSION "\r\n");

	do {
		uart_write_from_buf();

		read_state();
		control_outputs();

		uart_read_to_buf();
		if (read_newline) {
			process_input();
		}
	} while(1);
}

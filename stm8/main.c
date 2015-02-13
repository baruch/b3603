#define STM8S003 1
#include "stm8l.h"
#include <string.h>
#include <stdint.h>

void uart_write(const char *str)
{
	char i;
	for(i = 0; str[i] != 0; i++) {
		while(!(USART1_SR & USART_SR_TXE));
		USART1_DR = str[i];
	}
}

void clk_init()
{
	CLK_DIVR = 0x00; // Set the frequency to 16 MHz
	CLK_PCKENR1 = 0xFF; // Enable peripherals
}

void uart_init()
{
	PD_DDR = 0x10; // Put TX line on
	PD_CR1 = 0x10;

	USART1_CR1 = 0; // 8 bits, no parity
    USART1_CR3 &= ~(USART_CR3_STOP1 | USART_CR3_STOP2); // 1 stop bit

	USART1_BRR2 = 0x00;
	USART1_BRR1 = 0x0D; // 9600 baud, order important between BRRs, BRR1 must be last

//	USART1_CR2 = USART_CR2_TEN | UART_CR2_REN; // Allow TX & RX
	USART1_CR2 = USART_CR2_TEN; // Allow TX
}

int main()
{
	unsigned long i = 0;

	clk_init();
	uart_init();

	do {
		uart_write("Hello World!\r\n");
		for(i = 0; i < 147456; i++) { } // Sleep
	} while(1);
}

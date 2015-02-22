#include <stdint.h>

extern uint8_t uart_read_buf[64];
extern uint8_t uart_read_len;
extern uint8_t read_newline;

void uart_init(void);
uint8_t uart_write_ready(void);
uint8_t uart_read_available(void);
void uart_write_ch(const char ch);
void uart_write_str(const char *str);
void uart_write_int(uint16_t val);
void uart_write_fixed_point(uint16_t val);
void uart_write_from_buf(void);
void uart_read_to_buf(void);

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef struct {
	uint8_t output;
	uint16_t vset;
	uint16_t cset;
	uint16_t vshutdown;
	uint16_t cshutdown;
} cfg_output_t;

typedef struct {
	uint8_t name[17];
	uint8_t default_on;
} cfg_system_t;

#endif

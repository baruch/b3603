#include "fixedpoint.h"

uint16_t fixed_mult(uint16_t x, uint16_t y)
{
	uint32_t tmp;

	tmp = x;
	tmp *= y;
	tmp >>= FIXED_SHIFT;

	return tmp;
}

#include "fixedpoint.h"

/* For better accuracy we round to the nearest number, either up or down.
 * We save one extra bit temporarily and if it is 1 we round up and if it is 0
 * we round down.
 */
fixed_t fixed_mult(fixed_t x, fixed_t y)
{
	uint32_t tmp;
	uint8_t round;
	fixed_t tmp16;

	tmp = x;
	tmp *= y;
	tmp >>= FIXED_SHIFT-1;

	round = tmp&1;
	tmp16 = tmp >> 1;

	if (round)
		return tmp16+1;
	else
		return tmp16;
}

uint32_t fixed_mult13(uint32_t x, uint32_t y)
{
	uint32_t tmp;
	uint8_t round;

	tmp = x;
	tmp *= y;
	tmp >>= FIXED_SHIFT13-1;

	round = tmp&1;
	tmp = tmp >> 1;

	if (round)
		return tmp+1;
	else
		return tmp;
}

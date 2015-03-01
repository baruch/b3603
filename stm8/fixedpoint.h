#define FIXED_SHIFT 10

/* The FLOAT_TO_FIXED(f) macro takes a float number and converts it to a fixed
 * point number with FIXED_SHIFT bits fraction part.
 *
 * To make sure we are as close as possible we also handle the rounding
 * properly by adding one extra bit and if it is 1 we will round up, if it is 0
 * we will round down.
 */
#define FLOAT_TO_FIXED_BASE(f) (uint32_t)( (f)*(uint32_t)(1<<(FIXED_SHIFT+1)) )
#define FLOAT_TO_FIXED_ROUNDING(f) (FLOAT_TO_FIXED_BASE(f) & 1)
#define FLOAT_TO_FIXED(f) (uint16_t)((FLOAT_TO_FIXED_BASE(f) >> 1) + FLOAT_TO_FIXED_ROUNDING(f))

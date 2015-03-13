/* Copyright (C) 2015 Baruch Even
 *
 * This file is part of the B3603 alternative firmware.
 *
 *  B3603 alternative firmware is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  B3603 alternative firmware is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with B3603 alternative firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <stdint.h>

#define FIXED_SHIFT 10

#define FIXED_FRACTION_MASK ((1<<FIXED_SHIFT)-1)

typedef uint16_t fixed_t;

/* The FLOAT_TO_FIXED(f) macro takes a float number and converts it to a fixed
 * point number with FIXED_SHIFT bits fraction part.
 *
 * To make sure we are as close as possible we also handle the rounding
 * properly by adding one extra bit and if it is 1 we will round up, if it is 0
 * we will round down.
 */
#define FLOAT_TO_FIXED_BASE(f) (uint32_t)( (f)*(uint32_t)(1<<(FIXED_SHIFT+1)) )
#define FLOAT_TO_FIXED_ROUNDING(f) (FLOAT_TO_FIXED_BASE(f) & 1)
#define FLOAT_TO_FIXED(f) (fixed_t)((FLOAT_TO_FIXED_BASE(f) >> 1) + FLOAT_TO_FIXED_ROUNDING(f))


#define FIXED_SHIFT13 13
#define FIXED_FRACTION_MASK13 ((1<<FIXED_SHIFT13)-1)
#define FLOAT_TO_FIXED_BASE13(f) (uint32_t)( (f)*(uint32_t)(1<<(FIXED_SHIFT13+1)) )
#define FLOAT_TO_FIXED_ROUNDING13(f) (FLOAT_TO_FIXED_BASE13(f) & 1)
#define FLOAT_TO_FIXED13(f) (uint16_t)((FLOAT_TO_FIXED_BASE13(f) >> 1) + FLOAT_TO_FIXED_ROUNDING13(f))
uint32_t fixed_mult13(uint32_t x, uint32_t y);
fixed_t fixed_mult(fixed_t x, fixed_t y);

#endif

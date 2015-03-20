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

typedef uint16_t fixed_t;

/* The FLOAT_TO_FIXED(f) macro takes a float number and converts it to a fixed
 * point number with FIXED_SHIFT bits fraction part.
 *
 * To make sure we are as close as possible we also handle the rounding
 * properly by adding one extra bit and if it is 1 we will round up, if it is 0
 * we will round down.
 */
#define FIXED_SHIFT 16
#define FIXED_FRACTION_MASK (uint32_t)(((1LU)<<FIXED_SHIFT)-1LU)
#define FLOAT_TO_FIXED_BASE(f) (uint32_t)( (f)*(1LU<<(FIXED_SHIFT+1)) )
#define FLOAT_TO_FIXED_ROUNDING(f) (FLOAT_TO_FIXED_BASE(f) & 1)
#define FLOAT_TO_FIXED(f) (uint32_t)((FLOAT_TO_FIXED_BASE(f) >> 1) + FLOAT_TO_FIXED_ROUNDING(f))
uint32_t fixed_round(uint32_t x);

#endif

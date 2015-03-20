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

#include "fixedpoint.h"

/* For better accuracy we round to the nearest number, either up or down.
 * We save one extra bit temporarily and if it is 1 we round up and if it is 0
 * we round down.
 */
uint32_t fixed_round(uint32_t x)
{
	uint8_t round;

	x >>= FIXED_SHIFT-1;

	round = x&1;
	x >>= 1;

	return x+round;
}

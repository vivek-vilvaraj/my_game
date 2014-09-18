/* Asteroids3D - a first person game of blowing up asteroids
 * Copyright (C) 2000 Stuart Mark Pomerantz <smp [at] psc edu>
 * Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2003 - 2005
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  Stuart Pomerantz
 *  3935 Stonecliffe drive
 *  Monroeville, PA 15146
 */

#include "asteroids3D.h"

Vector dust_part[NUM_DUST_PART][NUM_CHILD_DUST_PART]; // dust particle array

//-----------------------------------------------------------------------------
void init_dust(void)
{
	Vector *d;
	int i, j;

	for (i = 0; i < NUM_DUST_PART; ++i) {
		d    = &dust_part[i][0];
		d->x = gen_rand_float(-ABOUND, ABOUND);
		d->y = gen_rand_float(-ABOUND, ABOUND);
		d->z = gen_rand_float(-ABOUND, ABOUND);

		for (j = 1; j < NUM_CHILD_DUST_PART; ++j) {
			d    = &dust_part[i][j];
			d->x = gen_rand_float(-CHILD_DUST_PART_DELTA,
			       CHILD_DUST_PART_DELTA);
			d->y = gen_rand_float(-CHILD_DUST_PART_DELTA,
			       CHILD_DUST_PART_DELTA);
			d->z = gen_rand_float(-CHILD_DUST_PART_DELTA,
			       CHILD_DUST_PART_DELTA);
		}
	}

	return;
}

void process_dust(void)
{
	Vector *d;
	int i;

	// wrap the dust around player
	for (i = 0; i < NUM_DUST_PART; ++i) {
		d = &dust_part[i][0];
		if (d->x > pos.x + ABOUND)
			d->x = pos.x - ABOUND + 10;
		if (d->x < pos.x - ABOUND)
			d->x = pos.x + ABOUND - 10;
		if (d->y > pos.y + ABOUND)
			d->y = pos.y - ABOUND + 10;
		if (d->y < pos.y - ABOUND)
			d->y = pos.y + ABOUND - 10;
		if (d->z > pos.z + ABOUND)
			d->z = pos.z - ABOUND + 10;
		if (d->z < pos.z - ABOUND)
			d->z = pos.z + ABOUND - 10;
	}

	return;
}

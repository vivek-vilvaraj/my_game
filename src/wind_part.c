/* Asteroids3D - a first person game of blowing up asteroids
 * Copyright (C) 2000 Stuart Mark Pomerantz <smp [at] psc edu>
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

#define MAX_WP_TTL 2 // in seconds

// Windsheid particle array
WindPart winshield_particle[MAX_BLASTS * NUM_WP_PER_BLAST];

//-----------------------------------------------------------------------------
void init_wind_parts(void)
{
	double secs = static_cast(double, ticks) / tps;
	int i;

	for (i = 0; i < MAX_BLASTS * NUM_WP_PER_BLAST; ++i)
		gen_rand_wind_part( &winshield_particle[i], secs);

	return;
}

void gen_rand_wind_part(WindPart *p, double secs)
{
	p->pos.x   = gen_rand_float(-1, 1);
	p->pos.y   = gen_rand_float(-1, 1);
	p->pos.z   = -1; /* right on the windshield glass */
	p->size    = 1 + gen_rand_int(0, 5);
	p->t_start = secs + gen_rand_float(0, 0.2);
	p->t_end   = p->t_start + MAX_WP_TTL * gen_rand_float(0.1, 1);
	return;
}

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

#include <stdio.h>
#include "asteroids3D.h"
#include "vecops.h"

static void gen_rand_asteroid_at_pos(const Vector *, Asteroid *, int);

//-----------------------------------------------------------------------------
/* generate new asteroids at position pos smaller than type */
void add_new_asteroids(const Vector *ppos, int type)
{
	static const int rocks[] = {2, 2, 2, 3, 3, 3, 3, 4, 4, 5};
	Node *current = NULL;
	int num_new_rocks, i;

	/* Decide how many new rocks to generate. Weighted to be 2 or 3. */
	num_new_rocks = rocks[gen_rand_int(0, 9)];
	current = al_tail;

	for (i = 0; i < num_new_rocks; ++i) {
		if (current->next == NULL)
			current->next = add_node();

		current = current->next;
		gen_rand_asteroid_at_pos(ppos, &current->rock, type);
	}

	al_tail = current;
	return;
}


static void gen_rand_asteroid_at_pos(const Vector *ppos, Asteroid *a,
    int type)
{
        double k;

	a->type   = gen_rand_int(0,type-1) ;
	a->mass   = MIN_AST_MASS + AST_MASS_K * a->type; /* based on type */
        a->radius = static_cast(double, a->type+1) / 2; // based on type
        a->pos    = *ppos;

	k = gen_rand_float(0, MAX_APD);
	gen_rand_vec(&a->velocity);
        vec_smul(&a->velocity, &a->velocity, k);
	a->rot = 0;
	a->rot_delta = gen_rand_int(-MAX_ARD, MAX_ARD);
	gen_rand_vec(&a->rot_axis);
	return;
}

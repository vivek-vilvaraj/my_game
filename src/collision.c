/* Asteroids3D - a first person game of blowing up asteroids
 * Copyright (C) 2000 Stuart Mark Pomerantz <smp [at] psc edu>
 * Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2003 - 2006
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asteroids3D.h"
#include "vecops.h"

// Space ship size and mass
#define SHIP_SZ 3.0
#define SHIP_MASS 1.0

// Shield flash duration
#define SHIELD_TTL 1.0

// Maximum time to flicker the hud gauges after a collision
#define MAX_HUD_FLICKER_TTL 5.0

//-----------------------------------------------------------------------------
void process_deleted_ast(void)
{
	Node *current = NULL, *temp = NULL;
	int i, j;

	// delete any nodes in the delete_next_ast array
	for (i = 0; i < MAX_TORPS; ++i) {
		if (delete_next_ast[i] == NULL)
			continue;

		current = delete_next_ast[i]->next;

		/* Why this inner loop? Two checks are performed.
		 *
		 * First: if the asteroid which is about to be deleted (current) is
		 * also somewhere in the delete_next_ast list as a pointer to a second
		 * asteroid to be deleted, then, of course there's a problem. When it
		 * comes time to delete the second asteroid, the pointer to it will
		 * have disappeared. To fix this, I set the pointer to the second
		 * asteroid to be what it should be. Namely, the pointer to the
		 * asteroid being deleted. A diagram deleting two asteroids in a row is
		 * very helpful to visualize this.
		 *
		 * Second: In some circumstances an asteroid may get on the list to be
		 * deleted twice. Say, if by chance two torps hit the asteroid
		 * simultaniously. If this occurs, then the first deletion will be
		 * fine, but of course the second deletion will fail. So to prevent
		 * this, I check the list for duplicates, and if I find one, set the
		 * duplicate to NULL so that I don't attempt to delete the asteroid
		 * twice.
		 */
		for (j = i + 1; j < MAX_TORPS; ++j) {
			if (current == delete_next_ast[j])
				delete_next_ast[j] = delete_next_ast[i];
			if (delete_next_ast[i] == delete_next_ast[j])
				delete_next_ast[j] = NULL;
		}

		// if I'm deleting the tail, I have to reassign it
		if (current->next == NULL)
			al_tail = delete_next_ast[i];

		temp = current;
		delete_next_ast[i]->next = current->next;
		free(temp);
	}

	// re-initialize the delete_next_ast array for next frame
	memset(delete_next_ast, 0, sizeof(Node *) * MAX_TORPS);
	return;
}

void check_collision(Node *previous)
{
	Node *current = NULL;
	double distance, collision_dist;
	int i , j;
	Vector temp1, temp2;
	Asteroid *cv;

	current = previous->next;
	cv      = &current->rock;

	/* code to check for asteroid and ship collisions */
	distance = vec_dist(&cv->pos, &pos);
	collision_dist = SHIP_SZ + cv->radius;

	if (distance <= collision_dist) {
		/* if there is a collision then first run the collision "model"
		 * if the "model" were any simpler then you'd just pass through things...
		 */
		apply_collision_v(&cv->velocity, cv->mass);

		/* next, here's a hack to predict whether or not the asteroid
		 * and the ship will still be inside their collision radius
		 * in the next frame (dt). This could happen if distance < collision_dist
		 * rather than distance = collision_dist.
		 *
		 * If turns out that they are, then run the correct_positions hack
		 * to nudge ship & rock into the right spots.
		 */
		vec_addmul(&temp1, &pos, dt, &velocity);
		vec_addmul(&temp2, &cv->pos, dt, &cv->velocity);

		if (vec_dist(&temp1, &temp2) < collision_dist)
			correct_positions(&cv->velocity, &cv->pos,
			                  collision_dist);

		/* set up the shield graphics */
		shield_activated_flag = 1;
		shield_ttl = SHIELD_TTL * tps + ticks;

		/* play the shield sound */
		play_sound(SOUND_COLLISION, 0);

		/* flicker the hud gauges */
		flicker_hud_ttl = ticks + MAX_HUD_FLICKER_TTL * tps * gen_rand_float(0, 1);
	} /* end collision if block */

	/* check for asteroid and torpedo collisions */
	for (i = 0; i < MAX_TORPS; ++i) {
		Torpedo *t = &torp[i];

		if (t->ttl == 0 || !t->in_flight_flag)
			continue;

		distance = vec_dist(&cv->pos, &t->pos);
		collision_dist = 2 * TORP_RADIUS + cv->radius;

		if (distance > collision_dist)
			continue;

		activate_explosion(&t->pos);
		play_sound(SOUND_ASTEROID_HIT, cv->type);

		if (cv->mass > MIN_AST_MASS)
			add_new_asteroids(&cv->pos, cv->type);

		t->ttl = 0; // this flag kills the torp in process_torpedo_motion()

		/* Find an empty slot in delete_next_ast at point it at the
		asteroid prior to the one to be deleted. */
		for (j = 0; j < MAX_TORPS; ++j)
			if (delete_next_ast[j] == NULL) {
				delete_next_ast[j] = previous;
				break;
			}

		if (is_targeted(&current->rock))
			drop_target();

		add_rock_to_score(&current->rock);
	} /* end TORP for loop */

	return;
}

/*
 * This function uses a very simple collision model
 *
 * I have chosen to model collisions as completely elastic...see schaums basic physics
 *
 * M(a) is the mass of body "a"
 * V(af) is the final (result) velocity vector of body "a"
 * V(ai) is the initial velocity of body "a"
 *
 * Using two equations in two variables:
 *
 * the sum of the final momentum = sum of the initial momentum
 * M(a)V(af) + M(b)V(bf) = M(a)V(ai) + M(b)*V(bi)
 *
 * the difference of the final velocities = negative of the difference of the initial velocities
 * V(af) - V(bf) = -( V(ai) - V(bi) )
 *
 */

/* V(af) - V(bf) = -( V(ai) - V(bi) )
 *       - V(bf) = -( V(ai) - V(bi) ) - V(af)
 * 		   V(bf) =    V(ai) - V(bi)   + V(af)
 *
 * M(a)V(af) + M(b)V(bf)                          = M(a)V(ai) + M(b)*V(bi)
 * M(a)V(af) + M(b)( V(ai) - V(bi)   + V(af) )    = M(a)V(ai) + M(b)*V(bi)
 * M(a)V(af) + M(b) V(ai) - M(b)V(bi) + M(b)V(af) = M(a)V(ai) + M(b)*V(bi)
 * M(a)V(af) + M(b)V(af)                          = M(a)V(ai) + M(b)*V(bi) + M(b)V(bi) - M(b)V(ai)
 *( M(a) + M(b) ) V(af)                           = M(a)V(ai) + M(b)*V(bi) + M(b)V(bi) - M(b)V(ai)
 *
 * V(af)  = ( M(a)V(ai) + M(b)*V(bi) + M(b)V(bi) - M(b)V(ai)  )/( M(a) + M(b) )
 *
 * V(af)  = ( M(a)V(ai) + 2*M(b)*V(bi) - M(b)V(ai)  )/( M(a) + M(b) )
 *
 * and now V(bf) is:
 *
 *    V(bf) = V(ai) - V(bi)  + V(af)
 */

/* body "a" is the ship, and body "b" is the asteroid */

void apply_collision_v(Vector *v, int mass)
{
	double ast_mass = mass, ship_mass = SHIP_MASS, k;
	Vector temp = velocity;

	/* V(af)  = ( M(a)V(ai) + 2*M(b)*V(bi) - M(b)V(ai)  )/( M(a) + M(b) ) */

	k = 1.0 / (ast_mass + ship_mass);
	velocity.x = k * (ship_mass * velocity.x + 2 * ast_mass * v->x -
	             ast_mass * velocity.x);
	velocity.y = k * (ship_mass * velocity.y + 2 * ast_mass * v->y -
	             ast_mass * velocity.y);
	velocity.z = k * (ship_mass * velocity.z + 2 * ast_mass * v->z -
	             ast_mass *velocity.z);

	/*  adjust the asteroid's vector */
 	/*  V(bf) = V(ai) - V(bi)  + V(af) */

	v->x = temp.x - v->x + velocity.x;
	v->y = temp.y - v->y + velocity.y;
	v->z = temp.z - v->z + velocity.z;

	/* now reduce the shield sensibly */
	shield_strength -= ast_mass * vec_length(&velocity);
	return;
}

/* if the asteroid and the ship collide and the distance between them is less
 * than it should be ( i.e. they have passed into each other )
 * I need to correct the position of the asteroid and the ship so that
 * they are exactly the right distance from one another before I apply the
 * collision. If I don't, they will hit multiple times.
 *
 * This routine is a rather bad hack. I Basically, the premise is that rather
 * than thinking up some clever mathematical solution, I simply ride the vectors
 * backward from their current positions until I get "close" to the correct one,
 * and then I use that.
 */
void correct_positions(Vector *rock_v, Vector *rock_p, double distance)
{
	Vector cand_a, cand_s; /* candidate positions for the moment of impact */
	double k;

	/* these are the future positions of the asteroid and ship */
	vec_addmul(&cand_s, &pos, dt, &velocity);
	vec_addmul(&cand_a, rock_p, dt, rock_v);
	cand_s.w = 0;
	cand_a.w = 0;

	/* now just nudge the asteroid so that the future distance is outside the
	 * collision radius. We'll leave the ship untouched, though its easy to move it.
	 * The graphics seem to be smoother if only the asteroid is nudged.
	 */
	k = dt;
	while (vec_dist(&cand_s, &cand_a) < distance) {
		k += 0.01;
		vec_addmul(&cand_a, rock_p, k, rock_v);
	}
	*rock_p = cand_a;
	return;
}

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

#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include "asteroids3D.h"
#include "vecops.h"

static void gen_rand_explosion_particles(int, int, double, int);
static void init_explosion_data(void);
static void init_explosion_dl(void);
static void init_explosion_winshield_particle_dl(void);

/* Explosion array, which is defined to be MAX_BLASTS > MAX_TORPS size since
there may be more than MAX_TORPS number of simultanious explosions, since the
explosions (currently) last longer then the torp ttl. */
Blast explosion[MAX_BLASTS];
int explosions_active_flag = 0; // are there explosions activated
int expl_dl[NUM_BLAST_TYPES];
int expl_winshield_particle_dl;
int expl_winshield_particle_count = 0;

//-----------------------------------------------------------------------------
void init_explosions(void)
{
	init_explosion_data();
	init_explosion_dl();
	init_explosion_winshield_particle_dl();
	return;
}

static void init_explosion_data(void)
{
	int i;

	explosions_active_flag = 0 ;
	memset(explosion, 0, sizeof(explosion));

	for (i = 0; i < MAX_BLASTS; ++i)
		explosion[i].scalar = 1;

	return;
}

void activate_explosion(const Vector *ppos)
{
	int i;

	/*
	 * check for a free explosion slot.
	 * use the first free slot found. This code
	 * is almost exactly the same as the torp code.
	 */
	for (i = 0; i < MAX_BLASTS; ++i)
		if (!explosion[i].is_active)
			break;

	if (i == MAX_BLASTS) /* no free slot , so don't draw one */
		return;

	/* set up explosion parameters */
	explosion[i].is_active = 1;
	explosion[i].pos = *ppos;
	explosion[i].ttl = BLAST_TTL * tps + ticks;
	explosion[i].scalar = 1;
	explosion[i].type = gen_rand_int(0, NUM_BLAST_TYPES - 1);

	explosions_active_flag = 1;
	return;
}

void process_explosion(void)
{
	int num_active_blasts = 0;
	Blast *e;
	int i;

	expl_winshield_particle_count = 0;
	if (!explosions_active_flag)
		return;

	for (i = 0; i < MAX_BLASTS; ++i) {
		e = &explosion[i];

		if (!e->is_active)
			continue;

		++num_active_blasts;
		if (vec_dist(&pos, &e->pos) <= e->scalar)
			++expl_winshield_particle_count;

		e->scalar += dt * BLAST_ER;
		if (ticks >= explosion[i].ttl)
			e->is_active = 0;
	}

	if (num_active_blasts <= 0)
		explosions_active_flag = 0;

	return;
}


static void init_explosion_dl(void)
{
	int i;

	for (i = 0; i < NUM_BLAST_TYPES; ++i) {
		expl_dl[i] = glGenLists(1);
		gen_rand_explosion_particles(15 + i, 15 + i, 0.5, expl_dl[i]);
	}
	return;
}


static void gen_rand_explosion_particles(int ps, int ts, double pert, int dl)
{
	double phi, theta;
	int p_slices = ps;
	int t_slices = ts;
	double delta_p = M_PI / p_slices;
	double delta_t = 2 * M_PI / t_slices;
	int rows = p_slices - 1;
	int cols = t_slices;
	int i, j;
	double q = pert;

	/*
	 * This is the same code that generates the randomly perturbed sphere which
	 * I turned into an asteroid. What was that you said about applying the
	 * same trick again and again, Dr. Wetzel ? :-)
	 *
	 */
	glNewList(dl, GL_COMPILE);
	glPointSize(1);
	glBegin(GL_POINTS);

	for (i = 0, phi = delta_p; i < rows; phi += delta_p, ++i) {
		double sinphi = sin(phi), cosphi = cos(phi);

		for (j = 0, theta = 0; j < cols; theta += delta_t, ++j)
			glVertex3f(cos(theta) * sinphi + gen_rand_float(-q, q),
			           sin(theta) * sinphi + gen_rand_float(-q, q),
			           cosphi + gen_rand_float(-q, q));
	}

	glEnd();
	glEndList();
	return;
}

static void init_explosion_winshield_particle_dl(void)
{
	static const float emit_on[]  = {0.0, 0.6, 0.0, 1.0};
	static const float emit_off[] = {0.0, 0.0, 0.0, 1.0};
	static const float ad[]       = {0.0, 0.0, 0.0, 0.5};
	double t;

	expl_winshield_particle_dl = glGenLists(1);
	glNewList(expl_winshield_particle_dl, GL_COMPILE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
	glMaterialfv(GL_FRONT, GL_EMISSION, emit_on);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (t = 0; t < 2 * M_PI + M_PI / 10; t += M_PI / 10)
		glVertex3f(0.01 * cos(t), 0.01 * sin(t), 0);
	glEnd();
	glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
	glDisable(GL_BLEND);
	glEndList();
	return;
}

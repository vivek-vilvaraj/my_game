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
#include <stdlib.h>
#include <string.h>
#include "asteroids3D.h"
#include "vecops.h"

#define NO_TRAIL -1

static int compare_torps(const void *, const void *);
static int get_free_torpedo_trail(void);

// Define a torpedo array to hold, say, MAX_TORPS worth of them.
Torpedo torp[MAX_TORPS];

// Doing this so alpha blending works correctly, this is scratch space.
Torpedo sorted_torp[MAX_TORPS];

int torp_dl;           // torpedo display list
int torp_billboard_dl; // billboard for torpedo texture
unsigned int torp_tex; // torpedo billboard texture
int torps_in_flight_flag = 0; // are any torpedos in flight
int tracer_light_flag = 0;    // turn on the tracer light

// Torpedo trails
TorpedoTrail torp_trail[MAX_TORP_TRAILS];

//-----------------------------------------------------------------------------
void init_torps(void)
{
	init_torpedo();
	init_torpedo_trails();
	return;
}

void init_torpedo_trails(void)
{
	memset(torp_trail, 0, sizeof(torp_trail));
	return;
}

static int get_free_torpedo_trail(void)
{
	int i;

	for (i = 0; i < MAX_TORP_TRAILS; ++i)
		if (!torp_trail[i].is_running_flag &&
		    !torp_trail[i].is_ending_flag)
			return i;

	return NO_TRAIL;
}

void init_torpedo(void)
{
	static const float emit0[]    = {1.0,  1.0, 1.0, 1.0}; /* torp material properties */
	static const float ad0[]      = {0.0,  0.0, 0.0, 1.0};
	static const float emit1[]    = {1.0,  1.0, 1.0, 1.0};
	static const float ad1[]      = {0.0,  0.0, 0.0, 0.4};
	static const float emit2[]    = {0.0,  0.6, 1.0, 1.0};
	static const float ad2[]      = {0.0,  0.0, 0.0, 0.3};
	static const float emit_off[] = {0.0 , 0.0, 0.0, 1.0};
	static const float diffuse[]  = {0.0,  0.6, 1.0, 1.0}; /* tracer light property */
	int i, k;

	torps_in_flight_flag = 0;
	tracer_light_flag    = 0;
	memset(torp, 0, sizeof(torp));

	for (i = 0; i < MAX_TORPS; ++i) {
		torp[i].ttl       = TORP_TTL * tps;
		torp[i].trail_num = NO_TRAIL;
	}

	/* set up torpedo display list */
	torp_dl = glGenLists(1);
	glNewList(torp_dl, GL_COMPILE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit0);
		glutSolidSphere(TORP_RADIUS - 2 * TORP_RADIUS_DELTA, 10, 10);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad1);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit1);
		glutSolidSphere(TORP_RADIUS - TORP_RADIUS_DELTA, 10, 10);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad2);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit2) ;
		glutSolidSphere(TORP_RADIUS + TORP_RADIUS_DELTA, 10, 10);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* set up the torpedo billboard */
	k = 3; /* scaling factor */
	torp_billboard_dl = glGenLists(1);
	glNewList(torp_billboard_dl, GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
			glTexCoord2f(1, 0);
			glVertex3f(k / 2, -k / 2, 0);
			glTexCoord2f(1, 1);
			glVertex3f(k / 2, k / 2, 0);
			glTexCoord2f(0, 1);
			glVertex3f(-k / 2, k / 2, 0);
			glTexCoord2f(0, 0);
			glVertex3f(-k / 2, -k / 2, 0);
		glEnd();
		glDisable(GL_BLEND);
	glEndList();

	/* set up tracer torpedo light properties */
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50);
	return;
}

void fire_torpedo(void)
{
	int i;
	static int tracer_count = MAX_TORPS;

	/*
	 * check for a free torpedo slot.
	 * use the first free slot found, or
	 * return if none are found.
	 */

	for (i = 0; i < MAX_TORPS; ++i)
		if (!torp[i].in_flight_flag)
			break;

	if (i == MAX_TORPS) /* no free slot */
		return;

	/* when tracer_count = 0, do a tracer shot */
	if (optmap.draw_tracer_flag)
		--tracer_count;

	/* set up the torpedo's fight parameters */

	torp[i].in_flight_flag = 1;   /* let her fly... */
	torp[i].ttl            = TORP_TTL * tps + ticks; /* fly for "this long" */
	play_sound(SOUND_TORP_FIRED, 0); /* play the torp fired sound */

	if (tracer_count == 0) {
		torp[i].is_tracer = 1;
		tracer_count = MAX_TORPS;
		tracer_light_flag = 1; /* turn on tracer light */
	} else {
		torp[i].is_tracer = 0;
	}

	/* set the initial position of the torpedo just in front of the player */
	vec_addmul(&torp[i].pos, &pos, 2, &zaxis);
	vec_addmul(&torp[i].pos_delta, &velocity, TORP_DELTA, &zaxis);
	torp[i].bb_zrot = 0 ;

	/* set up the torpedo trails */
	torp[i].trail_num = get_free_torpedo_trail();
	if (torp[i].trail_num != NO_TRAIL) {
		TorpedoTrail *t = &torp_trail[torp[i].trail_num];
		t->is_running_flag = 1;
		t->is_ending_flag  = 0;
		t->trail_start     = 0;
		t->trail_end       = 0;
		t->pos[0]          = torp[i].pos;
	}

	/* found a free torp slot, so we're going to have a torp in flight */
	torps_in_flight_flag = 1;
	return;
}

void process_torpedo_motion(int value)
{
	int num_in_flight = 0, i;
	TorpedoTrail *t;

	if (!torps_in_flight_flag)
		return;

	for (i = 0; i < MAX_TORPS; ++i) {
		if (!torp[i].in_flight_flag)
			continue;

		++num_in_flight;
		vec_addmul(&torp[i].pos, &torp[i].pos, dt, &torp[i].pos_delta);
		torp[i].bb_zrot += TORP_BB_ZROT;

		if (ticks >= torp[i].ttl) {
			torp[i].in_flight_flag = 0;
			if (torp[i].is_tracer)
				tracer_light_flag = 0; // turn off tracer light
		}

		if (torp[i].trail_num == NO_TRAIL)
			continue;

		t = &torp_trail[torp[i].trail_num];
		if (!torp[i].in_flight_flag) {
			t->is_running_flag = 0;
			t->is_ending_flag  = 1;
		}

		// Increment the trail array's index and wrap start & end indicators
		++t->trail_start;
		if (t->trail_start == TORP_TRAIL_LEN) {
			t->trail_start = 0;
			t->trail_end   = 1;
		}

		if (t->trail_start == t->trail_end) {
			++t->trail_end;
			t->trail_end %= TORP_TRAIL_LEN;
		}

		// add a new point to the trail
		t->pos[t->trail_start] = torp[i].pos;
	}

	if (num_in_flight <= 0)
		torps_in_flight_flag = 0;

	/* Why sort the torpedos: Well, when you alpha blend things, for the alpha
	blending to work you must draw everything behind the blended polygon before
	you draw the polygon. If you don't, the blending will be wrong. So the two
	steps below sort the torpedos by distance from the eye, so that distant
	torpedos are drawn before close torpedos, so that the blending works
	correctly. */
	memcpy(sorted_torp, torp, sizeof(torp));
	qsort(sorted_torp, MAX_TORPS, sizeof(Torpedo), compare_torps);
	return;
}

void process_torpedo_trails(void)
{
	TorpedoTrail *t;
	int i;

	for (i = 0; i < MAX_TORP_TRAILS; ++i) {
		t = &torp_trail[i];
		if (!t->is_ending_flag)
			continue;

		++t->trail_end;
		t->trail_end %= TORP_TRAIL_LEN;

		if (t->trail_end == t->trail_start)
			t->is_ending_flag = 0;
	}
	return;
}

void calc_torp_billboard_rot(const Vector *v, double *x, double *y)
{
	Vector temp;
	vec_sub(&temp, &pos, v);
	find_rotation_angles(&temp, x, y);
	return;
}

static int compare_torps(const void *pa, const void *pb)
{
	const Torpedo *a = pa, *b = pb;
	double d1, d2;

	d1 = (a->pos.x - pos.x) * (a->pos.x - pos.x) +
	     (a->pos.y - pos.y) * (a->pos.y - pos.y) +
	     (a->pos.z - pos.z) * (a->pos.z - pos.z);

	d2 = (b->pos.x - pos.x) * (b->pos.x - pos.x) +
	     (b->pos.y - pos.y) * (b->pos.y - pos.y) +
	     (b->pos.z - pos.z) * (b->pos.z - pos.z);

	return d2 - d1;
}

void draw_torp_plasma(void)
{
	float emit0[] = {1, 1, 1, 1}; /* torp material properties */
	float ad0[]   = {0, 0, 0, 1};
	int k;

	glEnable(GL_BLEND);

	for (k = 0; k < 20; ++k) { /* how about 20 plasma bolts for each torp */
		/* use a random alpha component for each bolt */
		ad0[3] = gen_rand_float(0, 1);

		/* use random (and equal) values for red and green so bolts vary from blue to white */
		emit0[0] = emit0[1] = gen_rand_float(0, 1);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit0);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);

		if (k % 7 == 0)
			/* every once in a while draw a long plasma bolt */
			glVertex3f(gen_rand_float(0, 2 * TORP_RADIUS) - TORP_RADIUS,
			           gen_rand_float(0, 2 * TORP_RADIUS) - TORP_RADIUS,
			           gen_rand_float(0, 2 * TORP_RADIUS) - TORP_RADIUS);
		else
			/* usually draw a short plasma bolt */
			glVertex3f(gen_rand_float(0, TORP_RADIUS) - TORP_RADIUS / 2,
			           gen_rand_float(0, TORP_RADIUS) - TORP_RADIUS / 2,
			           gen_rand_float(0, TORP_RADIUS) - TORP_RADIUS / 2);
		glEnd();
	}
	glDisable(GL_BLEND);
	return;
}

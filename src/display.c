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
#include <pthread.h>
#include <stdio.h>
#include "asteroids3D.h"
#include "vecops.h"

static void draw_asteroids(void);
static void draw_dust(void);
static void draw_expl_winshield_particles(void);
static void draw_explosions(void);
static void draw_hud(void);
static void draw_mouse_pos(void);
static void draw_nebula_billboards(void);
static void draw_shield(void);
static void draw_sun(void);
static void draw_torp_trails(void);
static void draw_torps(void);

static const float l2_pos[] = {0, 0, 0, 1};

//-----------------------------------------------------------------------------
void display(void)
{
	/* do some error checking for each frame. */
	glutReportErrors();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z,
	          pos.x + zaxis.x, pos.y + zaxis.y, pos.z + zaxis.z,
	          yaxis.x, yaxis.y, yaxis.z);

	if (optmap.antialias_flag)
		glEnable(GL_LINE_SMOOTH);
	else
		glDisable(GL_LINE_SMOOTH);

	/* must apply the modelview transformation to light0
	 * so that it continues to come from the same direction
	 * relative to the other objects in space when the view is
	 * rotated. Removing this line would make it seem as though
	 * the light were fixed and the ojbects were rotating. I want
	 * it to seem like the light and the objects are fixed, and the
	 * view is rotating.
	 */
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

	/* must always track headlight position, even if its not on. */
	glPushMatrix();
	glLoadIdentity();
	glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);
	glPopMatrix();

	// Tracer light
	if (tracer_light_flag)
		glEnable(GL_LIGHT2);
	else
		glDisable(GL_LIGHT2);

	// Debugging optics
	pthread_mutex_lock(&run_yield);
	if (optmap.axes_flag) {
		draw_debug_grid();
		draw_debug_axes();
	}
	pthread_mutex_unlock(&run_yield);

	// Sun and nebula textures
	pthread_mutex_lock(&run_yield);
	if (!optmap.nebula_texture_flag)
		draw_sun();
	else
		draw_nebula_billboards();
	pthread_mutex_unlock(&run_yield);

	// Asteroids
	pthread_mutex_lock(&run_yield);
	draw_asteroids();
	pthread_mutex_unlock(&run_yield);

	// Background spacedust
	pthread_mutex_lock(&run_yield);
	if (optmap.draw_dust_flag)
		draw_dust();
	pthread_mutex_unlock(&run_yield);

	// Explosions
	pthread_mutex_lock(&run_yield);
	if (explosions_active_flag)
		draw_explosions();
	pthread_mutex_unlock(&run_yield);

	// Torpedoes
	pthread_mutex_lock(&run_yield);
	if (torps_in_flight_flag)
		draw_torps();
	pthread_mutex_unlock(&run_yield);

	// Torpedo trails
	pthread_mutex_lock(&run_yield);
	draw_torp_trails();
	pthread_mutex_unlock(&run_yield);

	/* draw mouse position */
	pthread_mutex_lock(&run_yield);
	draw_mouse_pos();
	pthread_mutex_unlock(&run_yield);

	/* alpha blended shield farther than cross hair */
	pthread_mutex_lock(&run_yield);
	if (optmap.draw_shield_flag && shield_activated_flag)
		draw_shield();
	pthread_mutex_unlock(&run_yield);

	// Windshield particles
	pthread_mutex_lock(&run_yield);
	if (expl_winshield_particle_count > 0 && optmap.draw_shield_flag)
		draw_expl_winshield_particles();
	pthread_mutex_unlock(&run_yield);

	/*
	 * alpha blended cross hair is drawn last so that,
	 * in effect, everything drawn before it is blended
	 * into the crosshair. If you put it first, it appears
	 * opaque, even though the alpha blending is on.
	 */
	pthread_mutex_lock(&run_yield);
	if (draw_gauge())
		draw_hud();
	pthread_mutex_unlock(&run_yield);

	// Lens flares
	pthread_mutex_lock(&run_yield);
	// First, the sun's lens flares
	if (!optmap.nebula_texture_flag)
		draw_sun_lensflares();
	// Draw the torp flares
	if (optmap.draw_torp_lensflare_flag)
		draw_torp_lensflares();
	pthread_mutex_unlock(&run_yield);

	glutSwapBuffers();
	return;
}

static void draw_asteroids(void)
{
	const Node *current = NULL;
	Vector temp;

	current = al_head->next;
	while (current != NULL) {
		/* first off, cull any rock that's behind the ship */
		vec_sub(&temp, &current->rock.pos, &pos);

		/* cull the model if its behind us */
		if (vec_dot(&temp, &zaxis) < 0) {
			current = current->next;
			continue;
		}

		/* well, then draw the rock */
		glPushMatrix();
		glTranslatef(current->rock.pos.x, current->rock.pos.y,
		             current->rock.pos.z);
		glRotatef(current->rock.rot, current->rock.rot_axis.x,
		          current->rock.rot_axis.y, current->rock.rot_axis.z);

		if (optmap.texture_flag) {
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
			          GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D,
			              ast_tex[current->rock.type]);
		}

		glCallList(ast_dl[current->rock.type]); /* oh yea, draw the rock */

		if (optmap.texture_flag)
			glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix();
		current = current->next;
	}
	return;
}

static void draw_torps(void)
{
	const Torpedo *t;
	double x, y;
	int i;

	for (i = 0; i < MAX_TORPS; ++i) {
		if (!sorted_torp[i].in_flight_flag)
			continue;

		// draw the torp if its in flight
		t = &sorted_torp[i];
		glPushMatrix();
		glTranslatef(t->pos.x, t->pos.y, t->pos.z);

		if (t->is_tracer)
			glLightfv(GL_LIGHT2, GL_POSITION, l2_pos);

		if (!optmap.torp_texture_flag) {
			draw_torp_plasma();
			glCallList(torp_dl);
		} else {
			calc_torp_billboard_rot(&t->pos, &x, &y);
			glRotatef(y, 0, 1, 0);
			glRotatef(x, 1, 0, 0);
			/* rotate the billboard about z BEFORE rotating it into
			position to see. */
			glRotatef(t->bb_zrot, 0, 0, 1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
			          GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, torp_tex);
			glCallList(torp_billboard_dl);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glPopMatrix();
	}
	return;
}

static void draw_shield(void)
{
	if (ticks > shield_ttl) {
		shield_activated_flag = 0;
		return;
	}

	glCallList(shield_dl);
	return;
}

static void draw_sun(void)
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z); /* keep sun fixed wrt ship */
	glCallList(sun_dl);
	glPopMatrix();
	return;
}

static void draw_explosions(void)
{
	static const float emit_off[] = {0, 0, 0, 1};
	float emit[] = {1, 1, 1, 1};
	const Blast *e;
	int i;

	for (i = 0; i < MAX_BLASTS; ++i) {
		e = &explosion[i];
		if (!e->is_active)
			continue;

		// draw the explosion if its active
		glPushMatrix();
		glTranslatef(e->pos.x, e->pos.y, e->pos.z);
		glScalef(e->scalar, e->scalar, e->scalar);

		/* these two lines fade the explosion out over time */
		emit[0] = emit[1] = emit[2] =
			1.0 - e->scalar / (BLAST_TTL * BLAST_ER * 2.0);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit);
		glCallList(expl_dl[e->type]);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glPopMatrix();
	}

	return;
}

static void draw_hud(void)
{
	char buf[64];
	Vector result;

	result.x = vec_dot(&xaxis, &velocity);
	result.y = vec_dot(&yaxis, &velocity);
	result.z = vec_dot(&zaxis, &velocity);

	snprintf(buf, sizeof(buf), "velocity: %2.2f %2.2f %2.2f",
	         result.x, result.y, result.z);
	draw_text_in_color(-0.9, -0.9, buf, HUD_R,HUD_G,HUD_B,HUD_A) ;

	snprintf(buf, sizeof(buf), "score: %u",
	         static_cast(unsigned int, score * difficulty_multiplier));
	draw_text_in_color(-0.9, 0.9, buf, HUD_R, HUD_G, HUD_B, HUD_A);

	snprintf(buf, sizeof(buf), "asteroids left: %d",
	         num_asteroids_on_scanner);
	draw_text_in_color(0.5, 0.9, buf, HUD_R, HUD_G, HUD_B, HUD_A);

	/* these should be drawn before the alpha blended gauges */
	if (currently_targeted_asteroid != NULL) {
		draw_lcs(&currently_targeted_asteroid->rock);
		draw_steering_cue(&currently_targeted_asteroid->rock);
		draw_target_vector(&currently_targeted_asteroid->rock);
		draw_target_box(&currently_targeted_asteroid->rock);
	}

	if (optmap.crosshair_flag)
		draw_crosshair();

	draw_shield_gauge();
	draw_collision_warning_gauge();
	draw_torp_gauge();
	return;
}

static void draw_mouse_pos(void)
{
	static const float emit_on[]  = {0, 1, 0, 1};
	static const float ad[]       = {0, 0, 0, 1};
	static const float emit_off[] = {0, 0, 0, 1};

	glMaterialfv(GL_FRONT, GL_EMISSION, emit_on);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);

	glPushMatrix();
	glLoadIdentity();
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3f(2.0 * mouse_x / optmap.windowx + FRUSTUM_LEFT,
	           -2.0 * mouse_y / optmap.windowy + FRUSTUM_TOP, -1);
	glEnd();
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
	return;
}

static void draw_dust(void)
{
	static const float emit_on[]  = {1, 1, 1, 1};
	static const float ad[]       = {0, 0, 0, 1};
	static const float emit_off[] = {0, 0, 0, 1};
	int i, j;

	glMaterialfv(GL_FRONT, GL_EMISSION, emit_on);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);

	glPushMatrix();
	glPointSize(1);
	glBegin(GL_POINTS);
	for (i = 0; i < NUM_DUST_PART; ++i) {
		glVertex3f(dust_part[i][0].x, dust_part[i][0].y,
		           dust_part[i][0].z);

		for (j = 1; j < NUM_CHILD_DUST_PART; ++j)
			glVertex3f(dust_part[i][0].x + dust_part[i][j].x,
			           dust_part[i][0].y + dust_part[i][j].y,
			           dust_part[i][0].z + dust_part[i][j].z);
	}
	glEnd();
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
	return;
}

static void draw_nebula_billboards(void)
{
	int i;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z); /* keep billboards fixed wrt ship */
	for (i = 0; i < NUM_NEBULA_TEX; ++i)
		glCallList(nebula_dl[i]);
	glPopMatrix();
	return;
}

static void draw_expl_winshield_particles(void)
{
	double secs, ds;
	const WindPart *w;
	int i, k;

	secs = static_cast(double, ticks) / tps;
	k = expl_winshield_particle_count * NUM_WP_PER_BLAST;

	for (i = 0; i < k; ++i) {
		w = &winshield_particle[i];
		if (w->t_end < secs) {
			gen_rand_wind_part(&winshield_particle[i], secs);
			// take the shield down a bit since a particle hit it.
			shield_strength -= 1;
		}

		if (w->t_start > secs) /* not ready to display */
			continue;

		/* Calculate the size of the particle based on the
		amount of time it has been alive. */
		ds = (secs - w->t_end) / (w->t_end - w->t_start);

		// draw the particle
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(w->pos.x, w->pos.y, w->pos.z);
		glScalef(ds * w->size, ds * w->size, ds * w->size);
		glCallList(expl_winshield_particle_dl);
		glPopMatrix();
	}
	return;
}

static void draw_torp_trails(void)
{
	static const float emit_off[] = {0, 0, 0, 1};
	float emit_on[] = { 0.8, 0.8, 0.8, 1.0 };
	float ad[]      = { 0.0, 0.0, 0.0, 1.0 };
	double m; // modulate the brightness of the torp trail
	const TorpedoTrail *t;
	int i, j;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
	glMaterialfv(GL_FRONT, GL_EMISSION, emit_on);

	for (i = 0; i < MAX_TORP_TRAILS; ++i) {
		t = &torp_trail[i];
		if (!t->is_running_flag && !t->is_ending_flag)
			continue;

		glBegin(GL_LINE_STRIP);
		m = 1;
		for (j = t->trail_start; j != t->trail_end; --j) {
			emit_on[0] = emit_on[1] = emit_on[2] = m;
			glMaterialfv(GL_FRONT, GL_EMISSION, emit_on);
			ad[3] = m;
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
			glVertex3f(t->pos[j].x, t->pos[j].y, t->pos[j].z);
			m -= 0.5 / MAX_TORP_TRAILS;

			/* Yes, this is correct. When I loop back to the top of the for
			loop j gets decremented BEFORE the comparison with t->trail_end is
			made. When the comparision is done, j will correctly be
			TORP_TRAIL_LEN - 1. */
			if (j == 0)
				j = TORP_TRAIL_LEN;
		} // end inner for
		glEnd();
	}

	glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
	glDisable(GL_BLEND);
	return;
}

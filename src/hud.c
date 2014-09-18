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
#include <stdio.h>
#include <string.h>
#include "asteroids3D.h"
#include "vecops.h"

static void init_collision_warning_gauge(void);
static void init_crosshair(void);
static void init_torp_gauge(void);

// Time to end hud gauge flicker, intially off. Turned on after a collision.
double flicker_hud_ttl = 0;

// Collision warning gauge: also 0 = off, 1 = on
int cwg_rear, cwg_above, cwg_below, cwg_left, cwg_right;

static int torp_gauge_dl[2]; // Torp gauge dl: 0 = off, 1 = on
static int cwg_rear_dl[2], cwg_above_dl[2], cwg_below_dl[2];
static int cwg_left_dl[2], cwg_right_dl[2];
static int crossh_dl; // crosshair display list

//-----------------------------------------------------------------------------
void init_hud(void)
{
	init_crosshair();
	init_collision_warning_gauge();
	init_torp_gauge();
	return;
}

static void init_crosshair(void)
{
	static const float emit0[]    = {0.0, 0.0, 0.7, 1.0};
	static const float ad0[]      = {0.0, 0.0, 0.0, 0.5};
	static const float emit_off[] = {0.0, 0.0, 0.0, 1.0};
	static const float width = 0.06, height = 0.15, offset = 0.08;

	crossh_dl = glGenLists(1);
	glNewList(crossh_dl, GL_COMPILE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit0);

		glPushMatrix();
			glLoadIdentity();
			glTranslatef(0, 0, -1);
			glBegin(GL_TRIANGLES);

				/* top */
				glVertex3f( width, height + offset, 0);
				glVertex3f(-width, height + offset, 0);
				glVertex3f(0, offset, 0);

				/* bottom */
				glVertex3f(-width, -height - offset, 0);
				glVertex3f( width, -height - offset, 0);
				glVertex3f(0, -offset, 0);

				/* left */
				glVertex3f(-height - offset,  width, 0);
				glVertex3f(-height - offset, -width, 0);
				glVertex3f(-offset, 0, 0);

				/* right */
				glVertex3f(height + offset, -width, 0);
				glVertex3f(height + offset,  width, 0);
				glVertex3f(offset, 0, 0);

			glEnd();
			glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();
	return;
}

void draw_crosshair(void)
{
	glCallList(crossh_dl);
	return;
}

void draw_text_in_color(double x, double y, const char *string, double r,
    double g, double b, double a)
{
	static const float emit_off[] = {0, 0, 0, 1};
	float emit0[] = {r, g, b, 1};
	float ad0[]   = {0, 0, 0, a};
	int len, i;

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(x, y, -1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);

		/* draw text */
		glRasterPos2f(0, 0);
		len = strlen(string);

		for (i = 0; i < len; ++i)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,
			                    string[i]);

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glPopMatrix();
	return;
}

void draw_shield_gauge(void)
{
	static const float emit_green[] = {HUD_R, HUD_G, HUD_B, 1.0};
	static const float emit_red[]   = {0.7, 0.0, 0.0, 1.0};
	static const float ad[]         = {0.0, 0.0, 0.0, HUD_A};
	static const float emit_off[]   = {0.0, 0.0, 0.0, 1.0};
	char buf[64];
	double str, left, right, mid;

	/* str is the percentage shield strength */
	str = static_cast(double, shield_strength) / MAX_SHIELD;

	snprintf(buf, sizeof(buf), "shield: %2.2f%% ", str * 100);
	draw_text_in_color(0.5, -0.9, buf, HUD_R, HUD_G, HUD_B, HUD_A);

	left  = 0.5;
	right = left + 0.3;
	mid   = left + 0.3 * str; /* str=1 => mid = right, str=0 => mid = left */

	glPushMatrix();
		glLoadIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);

		glBegin(GL_QUADS);
			glMaterialfv(GL_FRONT, GL_EMISSION, emit_green);
			glVertex3f(left,  -0.85, -1);
			glVertex3f(mid,   -0.85, -1);

			glVertex3f(mid,   -0.80, -1);
			glVertex3f(left,  -0.80, -1);

			glMaterialfv(GL_FRONT, GL_EMISSION, emit_red);
			glVertex3f(mid,   -0.85, -1);
			glVertex3f(right, -0.85, -1);

			glVertex3f(right, -0.80, -1);
			glVertex3f(mid,   -0.80, -1);

			glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glEnd();
		glDisable(GL_BLEND);
	glPopMatrix();
	return;
}

static void init_collision_warning_gauge(void)
{
	static const float emit_red_off[] = {0.6, 0.0, 0.0, 1.0};
	static const float emit_red_on[]  = {1.0, 0.0, 0.0, 1.0};
	static const float ad[]           = {0.0, 0.0, 0.0, 0.5};
	static const float emit_off[]     = {0.0, 0.0, 0.0, 1.0};
	int circle, arrow;
	double t;

	/* a generic circle */
	circle = glGenLists(1);
	glNewList(circle, GL_COMPILE);
		glVertex3f(0, 0, 0);
		glScalef(0.1, 0.05, 0.1);
		glBegin(GL_TRIANGLE_FAN);
			for (t = 0; t < 2 * M_PI; t += M_PI / 10)
				glVertex3f(cos(t), sin(t), 0);
		glEnd();
	glEndList();

	/* the OFF rear dl (based on circle dl above) */
	cwg_rear_dl[0] = glGenLists(1);
	glNewList(cwg_rear_dl[0], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_off);
		glCallList(circle);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the ON rear dl (based on circle dl above) */
	cwg_rear_dl[1] = glGenLists(1);
	glNewList(cwg_rear_dl[1], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_on);
		glCallList(circle);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* a generic arrow */
	arrow = glGenLists(1);
	glNewList(arrow, GL_COMPILE);
		glScalef(0.05, 0.05, 0.05);
		glBegin(GL_TRIANGLES);
			glVertex3f( 1, 0, 0);
			glVertex3f( 0, 1, 0);
			glVertex3f(-1, 0, 0);
		glEnd();
	glEndList();

	/* the OFF above dl (based on arrow dl above) */
	cwg_above_dl[0] = glGenLists(1);
	glNewList(cwg_above_dl[0], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_off);
		glCallList(arrow);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the ON above dl (based on arrow dl above) */
	cwg_above_dl[1] = glGenLists(1);
	glNewList(cwg_above_dl[1], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_on);
		glCallList(arrow);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the OFF below dl (based on arrow dl above) */
	cwg_below_dl[0] = glGenLists(1);
	glNewList(cwg_below_dl[0], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_off);

		glPushMatrix();
			glRotatef(180, 0, 0, 1);
			glCallList(arrow);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the ON below dl (based on arrow dl above) */
	cwg_below_dl[1] = glGenLists(1);
	glNewList(cwg_below_dl[1], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_on);

		glPushMatrix();
			glRotatef(180, 0, 0, 1);
			glCallList(arrow);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the OFF left dl (based on arrow dl above) */
	cwg_left_dl[0] = glGenLists(1);
	glNewList(cwg_left_dl[0], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_off);

		glPushMatrix();
			glRotatef(90, 0, 0, 1);
			glCallList(arrow);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the ON left dl (based on arrow dl above) */
	cwg_left_dl[1] = glGenLists(1);
	glNewList(cwg_left_dl[1], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_on);

		glPushMatrix();
			glRotatef(90, 0, 0, 1);
			glCallList(arrow);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the OFF right dl (based on arrow dl above) */
	cwg_right_dl[0] = glGenLists(1);
	glNewList(cwg_right_dl[0], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_off);

		glPushMatrix();
			glRotatef(270, 0, 0, 1);
			glCallList(arrow);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* the ON right dl (based on arrow dl above) */
	cwg_right_dl[1] = glGenLists(1);
	glNewList(cwg_right_dl[1], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_red_on);

		glPushMatrix();
			glRotatef(270, 0, 0, 1);
			glCallList(arrow);
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();
	return;
}

void draw_collision_warning_gauge(void)
{
	Vector c;
	double dx, dy;

	/* this defines the center of the gauge and all the display
	 * lists are placed relative to the center c
	 */
	c.x =  0.0;
	c.y = -0.7;
	c.z = -1.0;

	/* dx and dy represent the shifts of the arrows from the center of the
	 * gauge ( which is the rear oval )
	 */
	dx = 0.12;
	dy = 0.08;

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(c.x, c.y, c.z);
		glCallList(cwg_rear_dl[cwg_rear]);
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(c.x, c.y + dy, c.z);
		glCallList(cwg_above_dl[cwg_above]);
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(c.x, c.y - dy, c.z);
		glCallList(cwg_below_dl[cwg_below]);
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(c.x - dx, c.y, c.z);
		glCallList(cwg_left_dl[cwg_left]);
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(c.x + dx, c.y, c.z);
		glCallList(cwg_right_dl[cwg_right]);
	glPopMatrix();

	return;
}

void scan_for_collision_warning(Asteroid *a)
{
	Vector ast_pos;
	double len, theta;

	/* first get a normalized asteroid vector */
	vec_sub(&ast_pos, &a->pos, &pos);
	len = vec_length(&ast_pos);
	vec_sdiv(&ast_pos, &ast_pos, len);

	/* If the rock is "far away" then return: that's not useful info.
	Process the distance at first, because more asteroids are far-away
	than close ones. */
	if (vec_dist(&a->pos, &pos) > 12)
		return;

	/* if the rock is in front of the ship then return: its not represented on the gauge */
	theta = vec_dot(&zaxis, &ast_pos);
	if (theta > 0 && acos(theta) * 180 / M_PI < 50)
		return;

	/* If I get here, there's a close rock. Now I have to determine
	 * from which direction its coming, and set the apropriate cwg_* flag
	 */
	if (vec_dot(&zaxis, &ast_pos) < 0)
		cwg_rear = 1;
	if (vec_dot(&yaxis, &ast_pos) > 0)
		cwg_above = 1;
	if (vec_dot(&yaxis, &ast_pos) < 0)
		cwg_below = 1;
	if (vec_dot(&xaxis, &ast_pos) < 0) /* NOTE: right handed rule!!! */
		cwg_right = 1;
	if (vec_dot(&xaxis, &ast_pos) > 0)
		cwg_left = 1;
	return;
}

/* return 1 to draw the gauge, and 0 to not draw it */
int draw_gauge(void)
{
	/*
	Only draw the gauge when:
	- the time to flicker the hud has expired, or
	- if I am flickering, then randomly draw the gauge
	*/
	return ticks > flicker_hud_ttl || gen_rand_int(0, tps) == 1;
}

static void init_torp_gauge(void)
{
	static const float emit_low[]  = {0.6, 0.0, 0.6, 1.0};
	static const float emit_high[] = {1.0, 0.0, 1.0, 1.0};
	static const float emit_off[]  = {0.0, 0.0, 0.0, 1.0};
	static const float ad[]        = {0.0, 0.0, 0.0, 0.5};
	double scalar = 0.035, t;
	int circle;

	/* a generic circle */
	circle = glGenLists(1);
	glNewList(circle, GL_COMPILE);
		glVertex3f(0, 0, 0);
		glScalef(scalar, scalar, scalar);
		glBegin(GL_TRIANGLE_FAN);
			for (t = 0; t < 2 * M_PI; t += M_PI / 10)
				glVertex3f(cos(t), sin(t), 0);
		glEnd();
	glEndList();

	/* torp gauge off list */
	torp_gauge_dl[0] = glGenLists(1);
	glNewList(torp_gauge_dl[0], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_low);
		glCallList(circle);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	/* torp gauge on list */
	torp_gauge_dl[1] = glGenLists(1);
	glNewList(torp_gauge_dl[1], GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_high);
		glCallList(circle);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	return;
}

void draw_torp_gauge(void)
{
	int i;
	Vector tg_pos; /* torp gauge position */

	tg_pos.x = -0.9;
	tg_pos.y =  0.2;
	tg_pos.z = -1.0;

	for (i = 0; i < MAX_TORPS; ++i) {
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(tg_pos.x, tg_pos.y, tg_pos.z);
			glCallList(torp_gauge_dl[!torp[i].in_flight_flag]);
		glPopMatrix();
		tg_pos.y -= 0.1;
	}
	return;
}

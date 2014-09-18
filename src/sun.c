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
#include "asteroids3D.h"
#include "vecops.h"

#define MAX_FLARES 10

typedef struct lens_flare {
	float pos_scalar, size_scalar, r, g, b, a, delta_z;
	int dl;
} LensFlare;

int sun_dl;

// Lens flares
#define TINY  0.02
#define SMALL 0.04
#define MED   0.1
#define LARGE 0.2
#define DZ    0.01
static LensFlare sun_flare[] = {
	{ 0.1, SMALL, 1, 0.4, 0.4, 0.3, 7 * DZ},
	{-2,   SMALL, 1, 0.4, 0.4, 0.3, 7 * DZ},
	{ 0.3,   MED, 1, 0.4, 0.4, 0.3, 6 * DZ},
	{-2.6,   MED, 1, 0.4, 0.4, 0.3, 6 * DZ},
	{ 0.9,  TINY, 1, 0.4, 0.4, 0.3, 5 * DZ},
	{ 1.5,  TINY, 1, 0.4, 0.4, 0.3, 4 * DZ},
	{ 1.7, SMALL, 1, 0.4, 0.4, 0.3, 3 * DZ},
	{ 2.1,   MED, 1, 0.4, 0.4, 0.3, 2 * DZ},
	{ 2,    TINY, 1, 0.4, 0.4, 0.3, DZ},
	{ 2.4, LARGE, 1, 0.4, 0.4, 0.3, 0},
}, torp_flare[] = {
	{ 0.1, SMALL, 1.0, 1.0, 1, 0.30, 7 * DZ},
	{-2,   SMALL, 1.0, 1.0, 1, 0.30, 7 * DZ},
	{ 0.3,   MED, 0.8, 0.8, 1, 0.30, 6 * DZ},
	{-2.6,   MED, 0.0, 0.0, 1, 0.28, 6 * DZ},
	{ 0.9,  TINY, 1.0, 1.0, 1, 0.30, 5 * DZ},
	{ 1.5,  TINY, 0.8, 0.8, 1, 0.30, 4 * DZ},
	{ 1.7, SMALL, 1.0, 1.0, 1, 0.30, 3 * DZ},
	{ 2.1,   MED, 1.0, 1.0, 1, 0.30, 2 * DZ},
	{ 2,    TINY, 0.8, 0.8, 1, 0.30, DZ},
	{ 2.4, LARGE, 0.0, 0.0, 1, 0.25, 0},
};
#undef TINY
#undef SMALL
#undef MED
#undef LARGE
#undef DZ
#define MIN_VIEW_ANGLE 0.55

//-----------------------------------------------------------------------------
void init_sun(void)
{
	static const float emit0[]    = {1.0, 1.0, 0.4, 1.0};
	static const float ad0[]      = {0.0, 0.0, 0.0, 1.0};
	static const float emit_off[] = {0.0, 0.0, 0.0, 0.1};
	float theta;
	float r = 3.0;

	sun_dl = glGenLists (1);
	glNewList (sun_dl, GL_COMPILE);
		glPushMatrix() ;
		glMaterialfv(GL_FRONT,GL_EMISSION,emit0) ;
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,ad0) ;
		glTranslatef( l0_pos[0] , l0_pos[1] , l0_pos[2] ) ;
		glRotatef(270,0,1,0) ;
		glBegin(GL_TRIANGLE_FAN) ;
			glVertex3f( 0.0 , 0.0 , 0.0 ) ;
			for (theta = 0; theta <= 2 * M_PI; theta += 0.1)
				glVertex3f(r * cos(theta), r * sin(theta), 0);
			glVertex3f(r * cos(0), r * sin(0), 0);
		glEnd();
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glPopMatrix();
	glEndList();
	return;
}

void init_nebula_billboards(void)
{
	float k;

	/* doing lots of encapsulation here,
	 * a good thing, I think
	 */

	/* nebula texture 0 goes in the spot where the sun is */
	k = 80;
	nebula_dl[0] = glGenLists(1);
	glNewList(nebula_dl[0], GL_COMPILE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, nebula_tex[0]);
		glPushMatrix();
			glTranslatef(l0_pos[0], l0_pos[1], l0_pos[2]);
			glRotatef(90, 0, 1, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex3f(-k / 2, -k / 2, 0);
				glTexCoord2f(0, 1);
				glVertex3f(-k / 2, k / 2, 0);
				glTexCoord2f(1, 1);
				glVertex3f(k / 2, k / 2, 0);
				glTexCoord2f(1, 0);
				glVertex3f(k / 2, -k / 2, 0);
			glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
	glEndList();

	/* nebula 1 & 2 are here to look pretty (and slow the game down! :-) )*/
	k = -40;
	nebula_dl[1] = glGenLists(1);
	glNewList(nebula_dl[1], GL_COMPILE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, nebula_tex[1]);

		glPushMatrix();
			glTranslatef(0, 0, 65);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex3f(0, 0, 0);
				glTexCoord2f(0, 1);
				glVertex3f(0, k, 0);
				glTexCoord2f(1, 1);
				glVertex3f(k, k, 0);
				glTexCoord2f(1, 0);
				glVertex3f(k, 0, 0);
			glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
	glEndList();

	k = -50;
	nebula_dl[2] = glGenLists(1);
	glNewList(nebula_dl[2], GL_COMPILE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, nebula_tex[2]);

		glPushMatrix();
			glTranslatef(0, 0, -65);
			glRotatef(180, 1, 0, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex3f(0, 0, 0);
				glTexCoord2f(0, 1);
				glVertex3f(0, k, 0);
				glTexCoord2f(1, 1);
				glVertex3f(k, k, 0);
				glTexCoord2f(1, 0);
				glVertex3f(k, 0, 0);
			glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
	glEndList();

	k = -45 ;
	nebula_dl[3] = glGenLists(1);
	glNewList(nebula_dl[3], GL_COMPILE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, nebula_tex[3]);

		glPushMatrix();
			glTranslatef(0, 65, 0);
			glRotatef(-90, 1, 0, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex3f(0, 0, 0);
				glTexCoord2f(0, 1);
				glVertex3f(0, k, 0);
				glTexCoord2f(1, 1);
				glVertex3f(k, k, 0);
				glTexCoord2f(1, 0);
				glVertex3f(k, 0, 0);
			glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
	glEndList();
	return;
}

void init_lens_flares(void)
{
	static const float emit_off[] = {0, 0, 0, 1};
	float emit[4], ad[4];
	LensFlare *fl;
	int circle;
	float t;
	int i;

	/* a generic circle */

	circle = glGenLists(1);
	glNewList(circle, GL_COMPILE);
		glVertex3f(0, 0, 0);
		glBegin(GL_TRIANGLE_FAN);
		for (t = 0; t < 2 * M_PI; t += M_PI / 10)
			glVertex3f(cos(t), sin(t), 0);
		glEnd();
	glEndList();

	/* so, now that I have my generic working circle,
	 * I'm going to set up the data/display lists for each part of the flare
	 */
	/* Note well that the flares are drawn in ascending order 0 to MAX_FLARES so,
	 * if you're going to alpha blend them, the farthest ones must be lower array
	 * indices than closer ones. The purpose of the delta_z is to place the relative
	 * z values of the different flares in such a way that they blend into each other.
	 */

	/* setting up the display list for sun_flare[i] */
        emit[3] = 1;
        ad[0] = ad[1] = ad[2] = 0;

	for (i = 0; i < MAX_FLARES; ++i) {
		fl = &sun_flare[i];
		emit[0] = fl->r;
		emit[1] = fl->g;
		emit[2] = fl->b;
		ad[3]   = fl->a;
		fl->dl  = glGenLists(1);
		glNewList(fl->dl, GL_COMPILE);
			glMaterialfv(GL_FRONT, GL_EMISSION, emit);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
			glScalef(fl->size_scalar, fl->size_scalar,
			         fl->size_scalar);
			glCallList(circle);
			glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glEndList();
	}

	/* setting up the display list for torp_flare[i] */
	for (i = 0; i < MAX_FLARES; ++i) {
		fl = &torp_flare[i];
		emit[0] = fl->r;
		emit[1] = fl->g;
		emit[2] = fl->b;
		ad[3]   = fl->a;
		fl->dl  = glGenLists(1);
		glNewList(fl->dl, GL_COMPILE);
			glMaterialfv(GL_FRONT, GL_EMISSION, emit);
			/* the transparency of the torpedo flare is set dynamically */
			glScalef(fl->size_scalar, fl->size_scalar,
			         fl->size_scalar);
			glCallList(circle);
			glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glEndList();
	}
	return;
}

void draw_sun_lensflares(void)
{
	Vector sun, flare_pos;
	int i;

	sun.x = l0_pos[0];
	sun.y = l0_pos[1];
	sun.z = l0_pos[2];
	vec_sdiv(&sun, &sun, vec_length(&sun));

	if (vec_dot(&sun, &zaxis) < MIN_VIEW_ANGLE)
		return;

	// Set the final sun flare position
	flare_pos.x = vec_dot(&xaxis, &sun);
	flare_pos.y = -vec_dot(&yaxis, &sun);
	flare_pos.z = -1;

	// Draw the flares
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();

	for (i = 0; i < MAX_FLARES; ++i) {
		glPushMatrix();
		glTranslatef(flare_pos.x * sun_flare[i].pos_scalar,
		             flare_pos.y * sun_flare[i].pos_scalar,
		             flare_pos.z - sun_flare[i].delta_z);
		glCallList(sun_flare[i].dl);
		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glPopMatrix();
	return;
}

void draw_torp_lensflares(void)
{
	float len, fade[] = {0, 0, 0, 1};
	Vector flare_pos, torp_pos;
	int i, j;

	/* NOTE that the torps array is run through in the REVERSE order because
	of the sorting/alpha blending effect. */
	for (i = MAX_TORPS - 1; i >= 0; --i) {
		if (!sorted_torp[i].in_flight_flag)
			continue;

		vec_sub(&torp_pos, &sorted_torp[i].pos, &pos);
		len = vec_length(&torp_pos);
		vec_sdiv(&torp_pos, &torp_pos, len);

		if (vec_dot(&torp_pos, &zaxis) < MIN_VIEW_ANGLE)
			continue;

		// Set the final torp flare position
		flare_pos.x = vec_dot(&xaxis, &torp_pos);
		flare_pos.y = -vec_dot(&yaxis, &torp_pos);
		flare_pos.z = -1;

		// Draw the flares
		glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLoadIdentity();

		for (j = 0; j < MAX_FLARES; ++j) {
			glPushMatrix();
			// Set the transparency of the flare
			fade[3] = (sorted_torp[i].ttl - ticks) *
					  torp_flare[j].a / (TORP_TTL * tps);
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,fade);
			glTranslatef(flare_pos.x * torp_flare[j].pos_scalar,
			             flare_pos.y * torp_flare[j].pos_scalar,
			             flare_pos.z - torp_flare[j].delta_z);
			glCallList(torp_flare[j].dl);
			glPopMatrix();
		}
		glDisable(GL_BLEND);
		glPopMatrix();
	}
	return;
}

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
#include "asteroids3D.h"
#include "vecops.h"

// A pointer to, well, that's pretty obvious
Node *currently_targeted_asteroid = NULL;

// Target box display list
static int target_box_dl, target_circle_dl, target_arrow_on_dl,
    target_arrow_off_dl, target_steering_cue_dl;

//-----------------------------------------------------------------------------
void auto_target_asteroid(void)
{
	Node *target[NUM_AST_TYPES] = {}, *current = NULL;
	int i, num_targets_found = 0;

	/* The idea here is that I'm going to scan the linked list for
	 * a target of each target type, and fill up the targets
	 * array with pointers to each target type.
	 *
	 * Then I'm going to lock on to the smallest non-null target type.
	 */
	current = al_head->next;

	while (current != NULL) {
		if (target[current->rock.type] == NULL) {
			target[current->rock.type] = current;
			++num_targets_found;
		}
		if (num_targets_found == NUM_AST_TYPES) /* all slots filled */
			break;
		current = current->next;
	}

	for (i = 0; i < NUM_AST_TYPES; ++i)
		if (target[i] != NULL) {
			currently_targeted_asteroid = target[i];
			play_sound(SOUND_TARGET_AQUIRED, 0);
			break;
		}

	return;
}

void target_asteroid(void)
{
	Node *current = NULL, *candidate = NULL;
	Vector temp1, temp2, point_onz;
	double dist_alongz, dist_fromz;

	current = al_head->next;

	while (current != NULL) {
		vec_sub(&temp1, &current->rock.pos, &pos);
		dist_alongz = vec_dot(&zaxis, &temp1);
		vec_smul(&point_onz, &zaxis, dist_alongz);
		vec_sub(&temp2, &point_onz, &temp1);
		dist_fromz = vec_length(&temp2);

		/* this test isn't precisely correct, but it works well */
		if (dist_fromz < current->rock.radius * 2)
		    candidate = current;

		current = current->next;
	}

	if (candidate == NULL) {
		currently_targeted_asteroid = NULL;
		return;
	}

	currently_targeted_asteroid = candidate;
	play_sound(SOUND_TARGET_AQUIRED, 0);
	return;
}

void init_target_dls(void)
{
	static const float emit0[]    = {HUD_R, HUD_G, HUD_B, 1.0};
	static const float ad0[]      = {0.0, 0.0, 0.0, HUD_A};
	static const float emit_off[] = {0.0, 0.0, 0.0, 1.0};
	static const float sc_emit0[] = {0.7, 0.7, 0.7, 1.0};
	static const float sc_ad0[]   = {0.0, 0.0, 0.0, 0.5};
	double t;

	target_box_dl = glGenLists(1);
	glNewList(target_box_dl, GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit0);
		glPushMatrix();
		switch (optmap.target_box_type) {
			case 1:
				glutWireOctahedron();
				break;
			case 2:
				glutWireSphere(1, 8, 8);
				break;
			case 3:
				glutWireTetrahedron();
				break;
			default:
				glutWireCube(1);
				break;
		}
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();

	target_arrow_on_dl = glGenLists(1);
	glNewList(target_arrow_on_dl, GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit0);
		glDisable(GL_BLEND);
	glEndList();

	target_arrow_off_dl = glGenLists (1);
	glNewList(target_arrow_off_dl, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
	glEndList();

	target_circle_dl = glGenLists(1);
	glNewList(target_circle_dl, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad0);
		glBegin(GL_QUADS);
		for (t = 0; t < 2 * M_PI + M_PI / 10; t += M_PI / 10) {
			glVertex3f(cos(t), sin(t), 0);
			glVertex3f(1.5 * cos(t), 1.5 * sin(t), 0);
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		for (t = 0; t < 2 * M_PI + M_PI / 10; t += M_PI / 10)
			glVertex3f(cos(t), sin(t), 0);
		glEnd();

		glBegin(GL_LINE_STRIP);
		for (t = 0; t < 2 * M_PI + M_PI / 10; t += M_PI / 10)
			glVertex3f(1.5 * cos(t), 1.5 * sin(t), 0);
		glEnd();
	glEndList();

	target_steering_cue_dl = glGenLists(1);
	glNewList(target_steering_cue_dl, GL_COMPILE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, sc_ad0);
		glMaterialfv(GL_FRONT, GL_EMISSION, sc_emit0);
		glPushMatrix();
			glBegin(GL_TRIANGLES);
				glVertex3f(0.08, 0, 0);
				glVertex3f(0, 0.05, 0);
				glVertex3f(0, -0.05, 0);
			glEnd();
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
		glDisable(GL_BLEND);
	glEndList();
	return;
}

void draw_target_vector(const Asteroid *a)
{
	glPushMatrix();
		glTranslatef(a->pos.x, a->pos.y, a->pos.z);
		glCallList(target_arrow_on_dl);
		glScalef(a->radius, a->radius, a->radius);
		glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(a->velocity.x, a->velocity.y,
			           a->velocity.z);
		glEnd();
		glCallList(target_arrow_off_dl);
	glPopMatrix();
	return;
}

void draw_target_box(const Asteroid *a)
{
	glPushMatrix();
		glTranslatef(a->pos.x, a->pos.y, a->pos.z);
		glRotatef(a->rot, a->rot_axis.x, a->rot_axis.y, a->rot_axis.z);
		glScalef(a->radius * 2, a->radius * 2, a->radius * 2);
		glCallList(target_box_dl);
	glPopMatrix();
	return;
}

void draw_lcs(const Asteroid *a)
{
	static const float emit_inrange[]    = {1, 0, 0, 1};
	static const float emit_outofrange[] = {1, 1, 0, 1};
	static const float ad[]              = {0, 0, 0, 1};
	static const float emit_off[]        = {0, 0, 0, 1};
	Vector torp_vel, ast_pos, torp_pos;
	double tv_mag, distance, k, r, s, t, scalar;
	Vector temp, apos, tpos;

	vec_smul(&torp_vel, &zaxis, TORP_DELTA);
	tv_mag = vec_length(&torp_vel);
	k = pos.x - a->pos.x;
	r = pos.y - a->pos.y;
	s = pos.z - a->pos.z;

	distance = vp_length(k, r, s);
	t = distance / tv_mag;

	vec_addmul(&ast_pos, &a->pos, t, &a->velocity);
	vec_add(&temp, &torp_vel, &velocity);
	vec_addmul(&torp_pos, &pos, t, &temp);

/* this commented out section was my first cut at the lead
 * computing sight, it actually plots the predicted asteroid
 * position and the predicted torpedo position in space
 * rather than projecting them back onto the winshield
 */

/*	glPointSize(10);
 *	glPushMatrix();
 *		glBegin(GL_POINTS);
 *			glMaterialfv(GL_FRONT, GL_EMISSION, emit_green);
 *			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
 *			glVertex3f( ast_pos.x, ast_pos.y, ast_pos.z);
 *
 *			glMaterialfv(GL_FRONT, GL_EMISSION, emit_red);
 *			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
 *			glVertex3f(torp_pos.x, torp_pos.y, torp_pos.z);
 *		glEnd();
 *	glPopMatrix();
 *	glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
 */
	/* calculate the projection of the future asteroid
	 * position on to the screen plane
	 */
	vec_sub(&temp, &ast_pos, &pos);
	apos.x = -vec_dot(&xaxis, &temp);
	apos.y = vec_dot(&yaxis, &temp);
	apos.z = vec_dot(&zaxis, &temp);

	/* perspective scale apos, then set apos.z to be the screen plane .
	 * NOTE: that to perspective scale apos, you don't want to divide by
	 * temp.z which is the straight line distance between the ship position
	 * and the asteroid position. Rather you want to use apos.z, which is
	 * the projection of the distance of the asteroid's position along the
	 * zaxis, which is REALLY the distance from the eye.
	 */
	apos.x /= apos.z;
	apos.y /= apos.z;
	apos.z = -1;

	/* calculate the projection of the future torpedo
	 * position on to the screen plane
	 */
	vec_sub(&temp, &torp_pos, &pos);
	tpos.x = -vec_dot(&xaxis, &temp);
	tpos.y = vec_dot(&yaxis, &temp);
	tpos.z = vec_dot(&zaxis, &temp);

	/* once again, perspective scale apos, then set apos.z to be the screen plane .*/
	tpos.x /= tpos.z;
	tpos.y /= tpos.z;
	tpos.z = -1;

	/* draw the sight */
	glPushMatrix();
		glPointSize(4);
		glLoadIdentity();

		if (t > TORP_TTL)
			glMaterialfv(GL_FRONT, GL_EMISSION, emit_outofrange);
		else
			glMaterialfv(GL_FRONT, GL_EMISSION, emit_inrange);

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);

		/* torpedo position dot */
		glBegin(GL_POINTS);
			glVertex3f(tpos.x, tpos.y, tpos.z);
		glEnd();

		/* the asteroid position circle */
		temp.x = -k;
		temp.y = -r;
		temp.z = -s;
		if (vec_dot(&temp, &zaxis) >= 0) {
			scalar = (TORP_RADIUS + a->radius) / distance;
			glTranslatef(apos.x, apos.y, apos.z);
			glScalef(scalar, scalar, scalar);
			glRotatef(10 * vec_dist(&pos, &a->pos), 0, 0, 1);
			glEnable(GL_BLEND);
			glCallList(target_circle_dl);
			glDisable(GL_BLEND);
		}
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, emit_off);
	return;
}

void draw_steering_cue(const Asteroid *a)
{
	Vector ast_pos, u_ast_pos, temp1, temp2;
	double k, theta;

	/* asteroid position relative to the ship */
	vec_sub(&ast_pos, &a->pos, &pos);

	/* don't want to draw the steering cue if the asteroid
	 * is close to the sight already
	 */
	vec_sdiv(&u_ast_pos, &ast_pos, vec_length(&ast_pos));

	theta = acos(vec_dot(&zaxis, &u_ast_pos)) * 180 / M_PI;
	if (theta < 30 && vec_dot(&zaxis, &u_ast_pos) > 0)
		return;

	/* okay, made it here so calculate the spot to
	 * draw the sterring cue, which amounts to the correct
	 * angle to rotate the little triangle around
	 */

	/* distance of asteroid vector along ship's z axis */
	k = vec_dot(&zaxis, &ast_pos);

	/* vector along z axis the length of k */
	vec_smul(&temp1, &zaxis, k);

	/* vector from zaxis to asteroid */
	vec_sub(&temp2, &ast_pos, &temp1);
	vec_sdiv(&temp2, &temp2, vec_length(&temp2));

	theta = acos(vec_dot(&temp2, &xaxis)) * 180 / M_PI;

	if (vec_dot(&ast_pos, &yaxis) < 0)
		theta += 180;
	if (vec_dot(&ast_pos, &yaxis) > 0)
		theta = 180 - theta;

	glPushMatrix();
		glLoadIdentity();
		glRotatef(theta, 0, 0, 1);
		glTranslatef(0.25, 0, -1);
		glCallList(target_steering_cue_dl);
	glPopMatrix();
	return;
}

/* return 1 if the asteroid IS targeted, return zero if it is NOT */
int is_targeted(const Asteroid *a)
{
	const Node *c = currently_targeted_asteroid;
	return c != NULL && a->pos.x == c->rock.pos.x &&
	       a->pos.y == c->rock.pos.y && a->pos.z == c->rock.pos.z &&
	       a->type == c->rock.type;
}

void drop_target(void)
{
	currently_targeted_asteroid = NULL;
	return;
}

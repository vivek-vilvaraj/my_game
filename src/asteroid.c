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
#include <stdlib.h>
#include <string.h>
#include "asteroids3D.h"
#include "vecops.h"

static void gen_rand_ast(double, int, double);
static void gen_rand_asteroid(Asteroid *);
static void init_asteroid_data(void);
static void init_asteroid_dls(void);

/* Asteroid linked list head and tail pointers */
Node *al_head, *al_tail;

/* Asteroid display list */
int ast_dl[NUM_AST_TYPES];

/* Asteroid texture object list */
unsigned int ast_tex[NUM_AST_TYPES];

/*
 * List of asteroids to be deleted each frame.
 * This list actually contains the node PRIOR to the node to be deleted (so I can
hook PRIOR node to the deleted node's "next" node. Hence the name. In any given
frame, the largest number of asteroids that can possibly be deleted is
MAX_TORPS so I make an array of this number of node pointers, just in case.
Initialized in init_asteroids(). */
Node *delete_next_ast[MAX_TORPS];

int num_asteroids_on_scanner; // Number of asteroids (for ship's "scanner")

//-----------------------------------------------------------------------------
void init_asteroids(void)
{
	init_asteroid_data();
	init_asteroid_dls();

	/* initialize asteroid deletion list */
	memset(delete_next_ast, 0, sizeof(delete_next_ast));
	return;
}

static void init_asteroid_data(void)
{
	Node *current = NULL;
	int i;

	/* al_head is a dummy node, the first real node is al_head->next */
	al_head = add_node();
	al_tail = al_head;
	current = al_head;

	for (i = 0; i < optmap.num_asteroids; ++i) {
		if (current->next == NULL)
			current->next = add_node();

		current = current->next;
		gen_rand_asteroid(&current->rock);
	}

	al_tail = current;
	return;
}

static void gen_rand_asteroid(Asteroid *a)
{
	double k;

	a->type   = gen_rand_int(0, NUM_AST_TYPES - 1);
	a->mass   = MIN_AST_MASS + AST_MASS_K * a->type;  /* based on type */
	a->radius = static_cast(double, a->type + 1) / 2; // based on type
	a->pos.x  = gen_rand_float(-AGB, AGB);
	a->pos.y  = gen_rand_float(-AGB, AGB);
	a->pos.z  = gen_rand_float(-AGB, AGB);

	k = gen_rand_float(0, MAX_APD);
	gen_rand_vec(&a->velocity);
	vec_smul(&a->velocity, &a->velocity, k);
	a->rot = 0;
	a->rot_delta = gen_rand_int(-MAX_ARD, MAX_ARD);
	gen_rand_vec(&a->rot_axis);
	return;
}

void process_asteroid_motion(int value)
{
	Node *previous = al_head, *current = al_head->next;

	num_asteroids_on_scanner = 0;
	cwg_rear = cwg_above = cwg_below = cwg_left = cwg_right = 0;

	while (current != NULL) {
		Asteroid *q = &current->rock;
		++num_asteroids_on_scanner;

		check_collision(previous);

		vec_addmul(&q->pos, &q->pos, dt, &q->velocity);
		q->rot += q->rot_delta * dt;
		q->rot %= 360;

		// wrap the asteroids around player
		if (q->pos.x > pos.x + ABOUND)
			q->pos.x = pos.x - ABOUND + 10;
		if (q->pos.x < pos.x - ABOUND)
			q->pos.x = pos.x + ABOUND - 10;
		if (q->pos.y > pos.y + ABOUND)
			q->pos.y = pos.y - ABOUND + 10;
		if (q->pos.y < pos.y - ABOUND)
			q->pos.y = pos.y + ABOUND - 10;
		if (q->pos.z > pos.z + ABOUND)
			q->pos.z = pos.z - ABOUND + 10;
		if (q->pos.z < pos.z - ABOUND)
			q->pos.z = pos.z + ABOUND - 10;

		scan_for_collision_warning(q);
		previous = current;
		current  = current->next;
	}

	return;
}

Node *add_node(void)
{
	return calloc(1, sizeof(Node));
}

static void init_asteroid_dls(void)
{
	double k = 0.5;
	int i;

	for (i = 0; i < NUM_AST_TYPES; ++i) {
		ast_dl[i] = glGenLists(1);
		gen_rand_ast(0.2, ast_dl[i], k);
		k += 0.5;
	}

	return;
}

static void gen_rand_ast(double pert, int dl, double scalar)
{
#define P_SLICES 8
#define T_SLICES 8
	static const float ad[] = {1, 1, 1, 1};
	double phi, theta;
	double delta_p = M_PI / P_SLICES;
	double delta_t = 2 * M_PI / T_SLICES;
	int rows = P_SLICES - 1;
	int cols = T_SLICES;
	int i, j;
	Vector sphere[P_SLICES - 1][T_SLICES];
	double q = pert;
	double tc[P_SLICES - 1][T_SLICES][2] ; /* these last variables are for texture coords */
	double dts = 2.0 / rows, dtt = 4.0 / cols;
	double s, t, len;
	int side, side_len;

	/* generate the points on the sphere and load them into a data structure
	 *
	 * This whole algorithm below makes sense if you know that theta
	 * generates circles of points and phi moves from one end of the sphere
	 * to the other modulating the radius of the circles generated by theta.
	 *
	 * so, each circular slice through the sphere is represented by "i" and
	 * each point on a particular circle is represented by "j"
	 *
	 * use GL_LINE_LOOP instead of GL_QUAD_STRIP to display the sphere and you'll
	 * see what I mean.
	 */

	s = 0;
	for (i = 0, phi = delta_p; i < rows; phi += delta_p, ++i) {
		double sinphi = sin(phi), cosphi = cos(phi);
		t = 0;
		for (j = 0, theta = 0; j < cols; theta += delta_t, ++j) {
			sphere[i][j].x = cos(theta) * sinphi;
			sphere[i][j].y = sin(theta) * sinphi;
			sphere[i][j].z = cosphi;
			tc[i][j][0] = s;
			tc[i][j][1] = t;
			t += dtt;
		}
		s += dts;
	}

	/* randomly perturb the points q = 0.1 is good */
	for (i = 0, phi = delta_p; i < rows; phi += delta_p, ++i)
		for (j = 0, theta = 0; j < cols; theta += delta_t, ++j) {
			sphere[i][j].x += gen_rand_float(-q, q);
			sphere[i][j].y += gen_rand_float(-q, q);
			sphere[i][j].z += gen_rand_float(-q, q);
		}

	/* create asteroid display list */
	glNewList(dl, GL_COMPILE);
	glShadeModel(GL_FLAT);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
	glScalef(scalar, scalar, scalar);

	/* asteroid body */
	glBegin(GL_QUAD_STRIP);
	for (i = 0, phi = delta_p; i < rows - 1; phi += delta_p, ++i) {
		for (j = 0, theta = 0; j < cols; theta += delta_t, ++j) {
			len = vec_length(&sphere[i][j]);
			glNormal3f(sphere[i][j].x / len, sphere[i][j].y / len,
			           sphere[i][j].z / len);
			glTexCoord2f(tc[i][j][0], tc[i][j][1]);
			glVertex3f(sphere[i][j].x, sphere[i][j].y,
			           sphere[i][j].z);

			len = vec_length(&sphere[i+1][j]);
			glNormal3f(sphere[i+1][j].x / len,
			           sphere[i+1][j].y / len,
			           sphere[i+1][j].z / len);
			glTexCoord2f(tc[i+1][j][0], tc[i+1][j][1]);
			glVertex3f(sphere[i+1][j].x, sphere[i+1][j].y,
			           sphere[i+1][j].z);
		}

		len = vec_length(&sphere[i][0]);
		glNormal3f(sphere[i][0].x / len, sphere[i][0].y / len,
		           sphere[i][0].z / len);
		glTexCoord2f(tc[i][j-1][0] + dts, tc[i][j-1][1] + dtt);
		glVertex3f(sphere[i][0].x, sphere[i][0].y, sphere[i][0].z);

		len = vec_length(&sphere[i+1][0]);
		glNormal3f(sphere[i+1][0].x / len, sphere[i+1][0].y / len,
		           sphere[i+1][0].z / len);
		glTexCoord2f(tc[i+1][j-1][0] + dts, tc[i+1][j-1][1] + dtt);
		glVertex3f(sphere[i+1][0].x, sphere[i+1][0].y,
		           sphere[i+1][0].z);
	}
	glEnd();
	/* asteroid ends */

	/* generate the texture coords for this asteroid end */
	s = t = 0;
	side = 0;
	side_len = cols / 4;
	dts = 1.0 / side_len;

	for (i = 0; i < cols; ++i) {
		tc[0][i][0] = s;
		tc[0][i][1] = t;

		if (i % side_len == 0)
		    ++side;
		if (side == 1)
			s += dts;
		if (side == 2)
			t += dts;
		if (side == 3)
			s -= dts;
		if (side == 4)
			t -= dts;
	}

	glBegin(GL_TRIANGLE_FAN) ;
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0, 0, 1);

		for (i = 0; i < cols; ++i) {
			len = vec_length(&sphere[0][i]);
			glNormal3f(sphere[0][i].x / len, sphere[0][i].y / len,
			           sphere[0][i].z / len);
			glTexCoord2f(tc[0][i][0], tc[0][i][1]);
			glVertex3f(sphere[0][i].x, sphere[0][i].y,
			           sphere[0][i].z);
		}

		len = vec_length(&sphere[0][0]);
		glNormal3f(sphere[0][0].x / len, sphere[0][0].y / len,
		           sphere[0][0].z / len);
		glTexCoord2f(tc[0][i-1][0], tc[0][i-1][1] - dts);
		glVertex3f(sphere[0][0].x, sphere[0][0].y, sphere[0][0].z);
	glEnd();

	/* generate the texture coords for this asteroid end */
	s = t = 0;
	side = 0;
	side_len = cols / 4;
	dts = 1.0 / side_len;

	for (i = 0; i < cols; ++i) {
		tc[0][i][0] = s;
		tc[0][i][1] = t;

		if (i % side_len == 0)
		    ++side;
		if (side == 1)
			s += dts;
		if (side == 2)
			t += dts;
		if (side == 3)
			s -= dts;
		if (side == 4)
			t -= dts;
	}

	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0, 0, -1);

		for (i = cols - 1; i >= 0; --i) {
			len = vec_length(&sphere[rows-1][i]);
			glNormal3f(sphere[rows-1][i].x / len,
			           sphere[rows-1][i].y / len,
			           sphere[rows-1][i].z / len);
			glTexCoord2f(tc[rows-1][i][0], tc[rows-1][i][1]);
			glVertex3f(sphere[rows-1][i].x,
			           sphere[rows-1][i].y,
			           sphere[rows-1][i].z);
		}

		len = vec_length(&sphere[rows-1][cols-1]);
		glNormal3f(sphere[rows-1][cols-1].x / len,
		           sphere[rows-1][cols-1].y / len,
		           sphere[rows-1][cols-1].z / len);
		glTexCoord2f(tc[rows-1][cols-1][0],
		             tc[rows-1][cols-1][1] + dts);
		glVertex3f(sphere[rows-1][cols-1].x,
		           sphere[rows-1][cols-1].y,
		           sphere[rows-1][cols-1].z);
	glEnd();
	glEndList();
	return;
#undef P_SLICES
#undef T_SLICES
}

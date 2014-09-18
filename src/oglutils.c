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
#include <stdlib.h>
#include "asteroids3D.h"
#include "vecops.h"

//-----------------------------------------------------------------------------
/* find the rotation matrix for rotating
 * angle DEGREES about vector V
 */
void find_rotation_mat(double angle, const Vector *v, double *m)
{
	double x, y, z;     // direction cosines
	double len;         // length of the vector V
	double cos_a, sin_a;

	/* calculate the normalized vector v and store its components in x,y,z */
	len = vec_length(v);
	x = v->x / len;
	y = v->y / len;
	z = v->z / len;

	/* pre-compute the cos(angle), sin(angle) */
	cos_a = cos(angle * M_PI / 180);
	sin_a = sin(angle * M_PI / 180);

	/* calculate the elements of the matrix */
	/* first row */
	m[0] = x*x + cos_a * (1 - x * x);
	m[1] = x*y - cos_a * x * y - z * sin_a;
	m[2] = x*z - cos_a * x * z + y * sin_a;
	m[3] = 0;

	/* second row */
	m[4] = y * x - cos_a * y * x + z * sin_a;
	m[5] = y * y + cos_a * (1 - y * y);
	m[6] = y * z - cos_a * y * z - x * sin_a;
	m[7] = 0;

	/* third row */
	m[8]  = z * x - cos_a * z * x - y * sin_a;
	m[9]  = z * y - cos_a * z * y + x * sin_a;
	m[10] = z * z + cos_a * (1 - z * z);
	m[11] = 0;

	/* fourth row */
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;

	return;
}

void find_rotation_angles(const Vector *v, double *x, double *y)
{
	double rho, phi, theta;
	double cos_a; // cosine of the angle

	/* @i, @j and @k are just for correcting the angle if @v is
	not in the +x, +y, +z quadrant. */
	Vector i = {1, 0, 0, 1};

	/* given a vector in rectangular co-ordinates,
	 * convert it to spherical coordinates.
	 * This will yield the vector's rotation about the
	 * y-axis (theta) and its rotation about the x-axis (phi)
	 *
	 * see schaum's calculus book on page 539 & 540
	 *
	 * since my coordinate system is layed out differently
	 * than schaums, my spherical coorinates look like this:
	 *
	 * x = p sin(phi) sin(theta)
	 * y = p cos(theta)
	 * z = p sin(phi) cos(theta)
	 *
	 * e.g. in my coordinate system, z comes out of the page, and
	 * x and y are in the plane of the page. In schaums, x comes out of
	 * the page and z and y are in the plane of the page.
	 *
	 */

	/*
	 * Note that after the rotations are computed,
	 * they must be applied like this:
	 *
	 *	glPushMatrix() ;
	 *		glRotatef(y,0,1,0) ;
	 *		glRotatef(x,1,0,0) ;
	 *
	 *      display the object
	 *
	 * 	glPopMatrix() ;
     */

	/* one last rub, I must make sure that
	 * the number I'm feeding to acos is between -1 and 1
	 */


	/* get vector into spherical coordinates */
	rho = vec_length(v);
	cos_a = v->y / rho;

	if (cos_a < -1)
		cos_a = -1;
	if (cos_a > 1 )
		cos_a = 1;

	phi = acos(cos_a);
	cos_a = v->z / (rho * sin(phi));

	if (cos_a < -1)
		cos_a = -1;
	if (cos_a > 1)
		cos_a = 1;

	theta = acos(cos_a);
	phi   = phi * 180 / M_PI;
	theta = theta * 180 / M_PI;
	*y    = theta;
	*x    = -90 + phi;

	/* the subroutine could return here, except that I have
	 * to account for all 8 octants, I have to do some checking
	 */

	if (vec_dot(v, &i) < 0)
		*x = 180 - *x;
	if (vec_dot(v, &i) < 0)
		*y = 180 - *y;

	return;
}

void gen_rand_vec(Vector *v)
{
	int x, y, z;
	double m[16];
	Vector a, u = {0, 0, 1, 0};

	x = 359.0 * rand() / (RAND_MAX + 1.0);
	y = 359.0 * rand() / (RAND_MAX + 1.0);
	z = 359.0 * rand() / (RAND_MAX + 1.0);

	/* set the axis a to be the x axis first */
	a.x = 1;
	a.y = 0;
	a.z = 0;
	a.w = 1;

	/* rotate u */
	find_rotation_mat(x, &a, m);
	apply_v2m(&u, m);

	/* set the axis a to be the y axis next */
	a.x = 0;
	a.y = 1;
	a.z = 0;
	a.w = 1;

	/* rotate u */
	find_rotation_mat(y, &a, m);
	apply_v2m(&u, m);

	/* set the axis a to be the z axis finally */
	a.x = 0;
	a.y = 0;
	a.z = 1;
	a.w = 1;

	/* rotate u */
	find_rotation_mat(z, &a, m);
	apply_v2m(&u, m);

	/* put the results back into v */
	*v = u;
	return;
}

/* generate random floats in .1 (tenth) increments */
double gen_rand_float(double min, double max)
{
	double k, len = max - min;

	/* generate a number between 0 and len*10 */
	k = 10 * len * rand() / (RAND_MAX + 1.0);

	/* scale the number back down */
	k = k / 10.0 - fabs(min);

	return k;
}

int gen_rand_int(int lo, int hi)
{
	return static_cast(int, static_cast(double, rand()) *
	       (++hi - lo) / RAND_MAX) + lo;
}

void apply_v2m(Vector *v, const double *m)
{
	Vector a = *v;
	v->x = a.x * m[0] + a.y * m[1] + a.z * m[2]  + a.w * m[3];
	v->y = a.x * m[4] + a.y * m[5] + a.z * m[6]  + a.w * m[7];
	v->z = a.x * m[8] + a.y * m[9] + a.z * m[10] + a.w * m[11];
	return;
}

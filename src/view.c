/* Asteroids3D - a first person game of blowing up asteroids
 * Copyright (C) 2000 Stuart Mark Pomerantz <smp [at] psc edu>
 * Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2003 - 2005
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
#include "asteroids3D.h"
#include "vecops.h"

Vector xaxis, yaxis, zaxis; // eye's local coordinate system
Vector pos;                 // eye's position in space

//-----------------------------------------------------------------------------
void reset_view(void)
{
	/* initial world position is at the origin */
	vec_zero(&pos);

	/* initial axes are the standard unit axes */
	xaxis.x = 1; xaxis.y = 0; xaxis.z = 0;
	yaxis.x = 0; yaxis.y = 1; yaxis.z = 0;
	zaxis.x = 0; zaxis.y = 0; zaxis.z = 1;

	/*
	 * for the purposes of this program, the view
	 * is going to be fixed down the POSITIVE z axis
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos.x, pos.y, pos.z,
	          pos.x + zaxis.x, pos.y + zaxis.y, pos.z + zaxis.z,
	          yaxis.x, yaxis.y, yaxis.z);
	return;
}

void rotate_view_aboutx(double a)
{
	double m[16];
	find_rotation_mat(-a, &xaxis, m);
	apply_v2m(&yaxis, m);
	apply_v2m(&zaxis, m);
	return;
}

void rotate_view_abouty(double a)
{
	double m[16];
	find_rotation_mat(-a, &yaxis, m);
	apply_v2m(&xaxis, m);
	apply_v2m(&zaxis, m);
	return;
}

void rotate_view_aboutz(double a)
{
	double m[16];
	find_rotation_mat(-a, &zaxis, m);
	apply_v2m(&xaxis, m);
	apply_v2m(&yaxis, m);
	return;
}

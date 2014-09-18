/*=============================================================================
Asteroids3D - a first person game of blowing up asteroids
  Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2003 - 2006

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with this program kit; if not, write to:
  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
  Boston, MA  02110-1301  USA
=============================================================================*/
#include <GL/glut.h>
#include "asteroids3D.h"

// Functions
static void init_debug_axes(void);
static void init_debug_grid(void);

// Variables
static const unsigned int cubenet_cells = 4;
static const double cubenet_width = 25;
static int axes_dl, cubenet_dl;

//-----------------------------------------------------------------------------
void init_debug(void)
{
	init_debug_axes();
	init_debug_grid();
	return;
}

static void init_debug_axes(void)
{
	static const float
		high_red[]   = {1.0, 0.0, 0.0, 1.0},
		low_red[]    = {0.3, 0.0, 0.0, 1.0},
		high_green[] = {0.0, 1.0, 0.0, 1.0},
		low_green[]  = {0.0, 0.3, 0.0, 1.0},
		high_blue[]  = {0.0, 0.0, 1.0, 1.0},
		low_blue[]   = {0.0, 0.0, 0.4, 1.0},
		white[]      = {1.0, 1.0, 1.0, 1.0},
		ad[]         = {0.0, 0.0, 0.0, 0.1},
		normal[]     = {0.0, 0.0, 0.0, 1.0};
	const double k = 33;

	axes_dl = glGenLists(1);
	glNewList(axes_dl, GL_COMPILE);

	glLineWidth(3);
	glPointSize(6);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
	glBegin(GL_LINES);
		// x axis RED
		glMaterialfv(GL_FRONT, GL_EMISSION, high_red);
		glVertex3f(k, 0, 0);
		glVertex3f(0, 0, 0);

		glMaterialfv(GL_FRONT, GL_EMISSION, low_red);
		glVertex3f(-k, 0, 0);
		glVertex3f(0, 0, 0);

		// y axis GREEN
		glMaterialfv(GL_FRONT, GL_EMISSION, high_green);
		glVertex3f(0, k, 0);
		glVertex3f(0, 0, 0);

		glMaterialfv(GL_FRONT, GL_EMISSION, low_green);
		glVertex3f(0, -k, 0);
		glVertex3f(0, 0, 0);

		// z axis BLUE
		glMaterialfv(GL_FRONT, GL_EMISSION, high_blue);
		glVertex3f(0, 0, k);
		glVertex3f(0, 0, 0);

		glMaterialfv(GL_FRONT, GL_EMISSION, low_blue);
		glVertex3f(0, 0, -k);
		glVertex3f(0, 0, 0);
	glEnd();

	glBegin(GL_POINTS);
		glMaterialfv(GL_FRONT, GL_EMISSION, white);
		glVertex3d(k, 0, 0);
		glVertex3d(0, k, 0);
		glVertex3d(0, 0, k);
		glVertex3d(-k, 0, 0);
		glVertex3d(0, -k, 0);
		glVertex3d(0, 0, -k);
	glEnd();

	glMaterialfv(GL_FRONT, GL_EMISSION, normal);
	glLineWidth(1);
	glPointSize(1);
	glEndList();
	return;
}

static void init_debug_grid(void)
{
	static const float
		cl_gray[]  = {0.35, 0.4, 0.35, 1},
		ad[]       = {0.0, 0.0, 0.0, 1};
	const double s = cubenet_width, hs = cubenet_width / 2;
	int x, y, z;

	cubenet_dl = glGenLists(1);
	glNewList(cubenet_dl, GL_COMPILE);

	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ad);
	glMaterialfv(GL_FRONT, GL_EMISSION, cl_gray);
	glPushMatrix();
		glTranslated(-hs - s, -hs - s, -hs - s);

		for (x = 0; x < cubenet_cells; ++x)
			for (y = 0; y < cubenet_cells; ++y)
				for (z = 0; z < cubenet_cells; ++z) {
					glPushMatrix();
					glTranslated(s * x, s * y, s * z);
					glutWireCube(25);
					glPopMatrix();
				}

	glPopMatrix();
	glEnable(GL_LINE_SMOOTH);
	glEndList();
	return;
}

void draw_debug_axes(void)
{
	glCallList(axes_dl);
	return;
}

void draw_debug_grid(void)
{
	const double d = cubenet_width;
	int x = static_cast(int, pos.x / d) * d,
	    y = static_cast(int, pos.y / d) * d,
	    z = static_cast(int, pos.z / d) * d;

	glPushMatrix();
		glTranslated(x, y, z);
		glCallList(cubenet_dl);
	glPopMatrix();
	return;
}

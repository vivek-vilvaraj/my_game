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
#include "asteroids3D.h"

/* light0 (the sun) position
 * light1 (headlight) position & direction
 * light2 torpedo tracer
 */
float l0_pos[4], l1_pos[4], l1_dir[4];

//-----------------------------------------------------------------------------
void gl_init_light(void)
{
	static const float global_ambient[] = {0.1, 0.1, 0.1, 1.0}; /* original */
	//static const float global_ambient[] = {0.5, 0.5, 0.5, 1.0}; /* for debugging */

	static const float ambient[]  = {0.0, 0.0, 0.0, 1.0};
	static const float diffuse[]  = {1.0, 1.0, 0.8, 1.0}; /* slightly yellow sun */
	static const float specular[] = {0.0, 0.0, 0.0, 1.0};

	/* w = 0 implies a directional source (light coming parallel to this vector)*/
	l0_pos[0] = 70;
	l0_pos[1] = 0;
	l0_pos[2] = 0;
	l0_pos[3] = 0;

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

	return;
}

void toggle_headlight(void)
{
	static int headlight_activated = 0;

	if (!headlight_activated)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);

	headlight_activated = !headlight_activated;
	return;
}

void gl_init_headlight(void)
{
	static const float ambient[]  = {0, 0, 0, 1};
	static const float diffuse[]  = {1, 1, 1, 1}; /* white headlight */
	static const float specular[] = {0, 0, 0, 1};

	/* set the initial headlight position to be at the viewer's position */
	l1_pos[0] = pos.x;
	l1_pos[1] = pos.y;
	l1_pos[2] = pos.z;
	l1_pos[3] = 1;

	/* set the initial headlight direction to be down the positive z-axis,
	 * which is where the view was set to be in reset_view()
	 */
	l1_dir[0] = pos.x + zaxis.x;
	l1_dir[1] = pos.y + zaxis.y;
	l1_dir[2] = pos.z + zaxis.z;
	l1_dir[3] = 1;

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1_dir);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
/*
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  0.04);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.04);
*/
	return;
}

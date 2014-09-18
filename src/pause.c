/* Asteroids3D - a first person game of blowing up asteroids
 * Copyright (C) 2000 Stuart Mark Pomerantz <smp [at] psc edu>
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

static void stop_pause(void);
static void paused_mouse_handler(int, int, int, int);
static void paused_passive_mouse_handler(int, int);
static void paused_keyboard_handler(unsigned char, int, int);

int paused_flag = 0; // is the game paused

//-----------------------------------------------------------------------------
/* this is just a simple function that checks the condition
 * of the paused flag and returns 0 if the game is unpaused,
 * and 1 if the game is paused.
 *
 * I did this so that it might be easier to change later if
 * I need to.
 */

int paused(void)
{
	return paused_flag;
}

void start_pause(void)
{
	paused_flag = 1;
/*	glutDisplayFunc(paused_display); */
	glutMouseFunc(paused_mouse_handler);
	glutKeyboardFunc(paused_keyboard_handler);
	glutPassiveMotionFunc(paused_passive_mouse_handler);
	return;
}

static void stop_pause(void)
{
	paused_flag = 0;
	glutDisplayFunc(display);
	glutMouseFunc(mouse_handler);
	glutKeyboardFunc(keyboard_handler);
	glutPassiveMotionFunc(passive_mouse_handler);
	return;
}

void paused_display(void)
{
	/* may want to do something here someday, but
	 * for now, the regular display function is still used
	 */
	return;
}

static void paused_mouse_handler(int button, int state, int x, int y)
{
	/* take no mouse input during pause */
	return;
}

static void paused_passive_mouse_handler(int x, int y)
{
	/* take no passive mouse input during pause */
	return;
}

static void paused_keyboard_handler(unsigned char key, int x, int y)
{
	switch (key) {
		default:
			stop_pause();
			break;
	}
	return;
}

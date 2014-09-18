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
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "asteroids3D.h"
#include "config.h"

/* The code which generates data for rendering will cycle some number of ticks
per second.
On a "slow" computer, this code will go slower than the number of ticks per
second. The limiting factor should always be the time to render, however.
On a "fast" computer, this code will cycle no faster than ticks per second, so
the game should never be un-playable on a "fast" computer. */
#define TPS 60.0

static void display_copyright_on_terminal(void);
static void glut_initialization(int *, char **);
static void gl_initialization(void);
static void idle(void);
static void my_initialization(void);
static void parse_command_line(int, char **);
static void reshape(int, int);

/**
 ** These variables are ALL the global variables in the game
 **/


/* Flags for internal program control */
int mouse_roty_flag = 0;        /* set to 1 if the mouse has been moved out of the dead zone to rotate the ship about the y axis */
int mouse_rotx_flag = 0;        /* set to 1 if the mouse has been moved out of the dead zone to rotate the ship about the x axis */
int game_end_flag = 0;          /* is the game over */

/* track the mouse cursor */
int mouse_x, mouse_y;

/* the proportion the mouse has moved out of the dead zone */
double mouse_xprop = 0;
double mouse_yprop = 0;

/* the number of game time ticks that have past */
unsigned long ticks = 0;

/* dt = delta time, the amount of time (in seconds)
 *  which passes with each game tick
 */
double dt = 1.0 / TPS;

/* this is the number of ticks per second, which I may want to change dynamically */
double tps = TPS;

/* frame calculation thread id, and the run_yield
 * mutex for interleaving calculation and display updates
 */
pthread_mutex_t run_yield = PTHREAD_MUTEX_INITIALIZER;
static pthread_t fc_tid;

// Dead zone boundaries */
static int mouse_left_bound, mouse_right_bound, mouse_top_bound,
	mouse_bottom_bound;

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
#ifdef DATADIR
	if (chdir(DATADIR) != 0) {
		fprintf(stderr, "Could not chdir to %s: %s\n",
		        DATADIR, strerror(errno));
		return EXIT_FAILURE;
	}
#endif
	srand(time(NULL));
	parse_command_line(argc, argv);
	display_copyright_on_terminal();
	read_rcfile();
	glut_initialization(&argc, argv);
	gl_initialization();
	my_initialization();
	glutMainLoop();
	return EXIT_SUCCESS;
}

static void glut_initialization(int *argc, char **argv)
{
	/* pass command line args to glut */
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(optmap.windowx, optmap.windowy);
	glutInitWindowPosition(optmap.window_xpos, optmap.window_ypos);
	glutCreateWindow(PACKAGE_STRING);
	glutDisplayFunc(display);
	glutMouseFunc(mouse_handler);
	glutKeyboardFunc(keyboard_handler);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(passive_mouse_handler);

	/* center the mouse cursor -- see glutPassiveMouseFunc */
	glutWarpPointer(optmap.windowx / 2, optmap.windowy / 2);

	/* looks better than the arrow, I think */
	/* glutSetCursor(GLUT_CURSOR_CROSSHAIR); */
	glutSetCursor(GLUT_CURSOR_NONE);
	return;
}

static void gl_initialization(void)
{
	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
	glFrustum(FRUSTUM_LEFT, 1 /* right */, -1 /* bottom */, FRUSTUM_TOP,
	          1 /* near */, sqrt(3 * 100 * 100) /* far */);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	return;
}

static void my_initialization(void)
{
	printf("Initializing...\n");
	reset_view();
	gl_init_light();
	gl_init_headlight(); /* reset_view() must be called before gl_init_headlight */
	init_thrust();
	init_torps();
	init_textures();
	init_asteroids();
	init_shield();
	init_hud();
	init_sun(); /* must be called after  gl_init_light */
	init_explosions();
	init_dust();
	init_target_dls();
	init_nebula_billboards(); /* must be called after  gl_init_light, and init_textures */
	init_wind_parts();
	init_sound();
	init_lens_flares();
	init_debug();

	/* finally, and always last,
	 * set up the run_yield semaphore and
	 * crank up the frame calculation thread
	 */
	pthread_create(&fc_tid, NULL, calculate_frame, NULL);
	printf("Done.\n");
	return;
}

static void idle(void)
{
	/* calculate_frame() is now a thread by itself, hope this works */
	/* calculate_frame(); */
	glutPostRedisplay();
	return;
}

static void reshape(int w, int h)
{
	unsigned int m;
	glViewport(0, 0, w, h);

	optmap.windowx = w;
	optmap.windowy = h;

	m = optmap.windowx / 2;
	mouse_left_bound  = m - 50;
	mouse_right_bound = m + 50;
	m = optmap.windowy / 2;
	mouse_top_bound = m - 50;
	mouse_bottom_bound = m + 50;
	return;
}

void mouse_handler(int button, int state, int x, int y)
{
	/* mouse x,y are given in window coordinates
	 */
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
				fire_torpedo();
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN)
				target_asteroid();
			break;
	}
	return;
}

void passive_mouse_handler(int x, int y)
{
	/* set the mouse position globals */
	mouse_x = x;
	mouse_y = y;

	/* if mouse is inside dead zone */
	if (x > mouse_left_bound && x < mouse_right_bound &&
	    y > mouse_top_bound && y < mouse_bottom_bound) {
		mouse_roty_flag = 0;
		mouse_rotx_flag = 0;
		return;
	}

	/* if mouse is not in the dead zone set the flag */
	if (x > mouse_right_bound) {
		mouse_roty_flag = 1;
		mouse_yprop = static_cast(double, x - mouse_right_bound) /
		              (optmap.windowx - mouse_right_bound);
	}

	if (x < mouse_left_bound) {
		mouse_roty_flag = 1;
		mouse_yprop = -1 + static_cast(double, x) / mouse_left_bound;
	}

	if (y < mouse_top_bound) {
		mouse_rotx_flag = 1;
		mouse_xprop = 1 - static_cast(double, y) / mouse_top_bound;
	}

	if (y > mouse_bottom_bound) {
		mouse_rotx_flag = 1;
		mouse_xprop = static_cast(double, -(y - mouse_bottom_bound)) /
		              (optmap.windowy - mouse_bottom_bound);
	}

	/* for reversing mouse direction */
	mouse_yprop = mouse_yprop * optmap.mouse_ydir;
	mouse_xprop = mouse_xprop * optmap.mouse_xdir;
	return;
}

void end_game(void)
{
	game_end_flag = 1;  /* this causes the calculation thread to exit */
	init_score_display();
	return;
}

static void parse_command_line(int argc, char **argv)
{
	int i;
	int temp1, temp2, temp3, temp4;

	for (i = 0; i < argc; ++i) {
		if (strcmp("-help", argv[i]) == 0 ||
		    strcmp("-h", argv[i]) == 0 ||
		    strcmp("--help", argv[i]) == 0 ||
		    strcmp("-?", argv[i]) == 0)
		{
			printf("valid command line options:\n") ;
			printf("   -num <number>\n"
			       "         to set the number of asteroids\n\n") ;
			printf("   -geometry <width>x<height>+<xoffset>+<yoffset>\n"
			       "         to set the geometry of the window (in pixels); e.g. 640x480+150+50\n\n");
			printf("   -rcfile <filename>\n"
			       "         to use <filename> as the .a3drc file instead of $HOME/.a3drc\n\n") ;
			printf("   -freshrc <filename>\n"
			       "         to print out a fresh .a3drc file\n\n") ;
			printf("   -freshrc1 <filename>\n"
			       "         same as -freshrc, but use old key mappings\n\n") ;
			exit(EXIT_SUCCESS);
		} else if (strcmp("-num", argv[i]) == 0) {
			if (i + 1 < argc) {
				if (sscanf(argv[i+1], "%d\n", &temp1) == 1) {
					if (temp1 > 1)
						optmap.num_asteroids = temp1;
					++i;
				}
			}
		} else if (strcmp("-geometry", argv[i]) == 0) {
			if (i + 1 < argc) {
				if (sscanf(argv[i+1], "%dx%d+%d+%d\n", &temp1, &temp2, &temp3, &temp4) >= 2) {
					if (temp1 > 0 && temp2 > 0) {
						optmap.windowx = temp1;
						optmap.windowy = temp2;
					}
					if (temp3 >= 0)
						optmap.window_xpos = temp3;
					if (temp4 >= 0)
						optmap.window_ypos = temp4;
				}
			}
		} else if (strcmp("-rcfile", argv[i]) == 0) {
			if (i + 1 < argc) {
				temp1  = strlen(argv[i+1]) + 1;
				rcfile = calloc(1, temp1);
				memcpy(rcfile, argv[i+1], temp1);
			}
		} else if (strcmp("-freshrc", argv[i]) == 0) {
			write_rcfile_to_terminal(0);
			exit(EXIT_SUCCESS);
		} else if (strcmp("-freshrc1", argv[i]) == 0) {
			write_rcfile_to_terminal(1);
			exit(EXIT_SUCCESS);
		}
	} /* end for each argument */
	return;
}

static void display_copyright_on_terminal(void)
{
	printf(
		"Asteroids3D v0.2.2 Copyright (C) 2000 Stuart Mark Pomerantz\n"
		"Jan Engelhardt, 2002 - 2007\n"
		"Asteroids3D comes with ABSOLUTELY NO WARRANTY; for details\n"
		"see the file COPYRIGHT which should have come with this program.\n"
		"This is free software, and you are welcome to redistribute it\n"
		"under certain conditions; see the file COPYRIGHT\n"
		"which should have come with this program for details.\n");
	return;
}

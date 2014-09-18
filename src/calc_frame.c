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

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "asteroids3D.h"

// Maximum rotation that can be input per unit time in degrees
#define MAX_DPS_INPUT 75.0

//-----------------------------------------------------------------------------
void *calculate_frame(void *unused)
{
	while (1) {
		usleep(dt * 1000000);

		if (paused()) /* must continue to compute dt even if paused. */
			continue;
		if (game_end_flag)
			break;

		pthread_mutex_lock(&run_yield);

		capture_passive_mouse_input();
		apply_thrust(1); /* keyboard & mouse motion */
		process_motion(1); /* ship motion */
		process_asteroid_motion(1);	/* ...and asteroid motion. Comment this out to browse the data set */
		process_deleted_ast();
		process_torpedo_motion(1);
		process_torpedo_trails();
		process_explosion();
		process_dust();
		process_shield();
		process_sound();
		++ticks;

		pthread_mutex_unlock(&run_yield);
	}

	return NULL;
}

void capture_passive_mouse_input(void)
{
	if (paused()) /* must continue to compute dt even if paused. HACK FIXME? */
		return;

	/* capture passive mouse changes */

	if (mouse_roty_flag)
		turn_abouty(MAX_DPS_INPUT * mouse_yprop);
	if (mouse_rotx_flag)
		turn_aboutx(MAX_DPS_INPUT * mouse_xprop);

	/* these added for keyboard rotation input */
	if (keyb_rotx_scalar != 0) {
		turn_aboutx(MAX_DPS_INPUT * KEYB_BASE_ROT_SCALE *
		            keyb_rotx_scalar);
		decay_keyb_rotx_scalar();
	}
	if (keyb_roty_scalar != 0) {
		turn_abouty(MAX_DPS_INPUT * KEYB_BASE_ROT_SCALE *
		            keyb_roty_scalar);
		decay_keyb_roty_scalar();
	}
	if (keyb_rotz_scalar != 0) {
		turn_aboutz(MAX_DPS_INPUT * KEYB_BASE_ROT_SCALE *
		            keyb_rotz_scalar);
		decay_keyb_rotz_scalar();
	}
	return;
}

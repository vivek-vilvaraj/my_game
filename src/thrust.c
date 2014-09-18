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

#include <stdlib.h>
#include "asteroids3D.h"
#include "vecops.h"

// Maximum thrust per unit time
#define MAX_TPS 25.0

/*
 * 1) thrust
 * is the the vector which sums the thrust vectors which the user has input but
 * which have not yet been applied. This is a "request".
 *
 * 2) velocity
 * is the sum of the parts of the thrust vector which have been applied (by the
 * engine) and are now acting (in a newtonian sense) to move the ship.
 */
Vector thrust, velocity;

//-----------------------------------------------------------------------------
void init_thrust(void)
{
	vec_zero(&thrust);
	vec_zero(&velocity);
	vec_zero(&rotation);
	return;
}

void apply_thrust(int value)
{
	Vector t; /* thrust which is actually applied */
	double delta_thrust;

	/*
	 * Here's what's going on:
	 *
	 * dt is the time since the loop was last processed, "delta t"
	 *
	 * So if its been 0.25 seconds since the loop was last processed
	 * and MAX_TPS ( max thrust per second ) is 1 (m/s) then this
	 * time throught the loop take thrust 0.25 m/s and save the next 0.75
	 * for the next time around. Of course, if MAX_TPS is, say 5, then
	 * you have to take 5*0.25 to get the amount of thrust that can potentially
	 * be applied at this time.
	 *
	 * If it turns out that this amount of thrust is greater than what's left to
	 * be applied, then just apply what's left.
	 *
	 * and do this for all umpteen vectors.
	 *
	 * note that another approach could be to take these three if
	 * statements out of here -- performance ?! -- and put them in
	 * stop() below, because currently, that's the only time
	 * thrust may be > MAX_TINPUT.
	 */

	/* note also that since last_time = 0 zero on the very first run
	 * through this loop, there could be a huge blow up if somebody is
	 * holding a key down at the start of the game.
	 */
	delta_thrust = dt * MAX_TPS;

	if (abs(thrust.x) > delta_thrust) {
		if (thrust.x > 0) {
			t.x = delta_thrust;
			thrust.x -= delta_thrust;
		} else {
			t.x = -delta_thrust;
			thrust.x += delta_thrust;
		}
	} else {
		t.x = thrust.x;
		thrust.x = 0;
	}

	if (abs(thrust.y) > delta_thrust) {
		if (thrust.y > 0) {
			t.y = delta_thrust;
			thrust.y -= delta_thrust;
		} else {
			t.y = -delta_thrust;
			thrust.y += delta_thrust;
		}
	} else {
		t.y = thrust.y;
		thrust.y = 0;
	}

	if (abs(thrust.z) > delta_thrust) {
		if (thrust.z > 0) {
			t.z = delta_thrust;
			thrust.z -= delta_thrust;
		} else {
			t.z = -delta_thrust;
			thrust.z += delta_thrust;
		}
	} else {
		t.z = thrust.z;
		thrust.z = 0;
	}

	/* apply the thrust */
	vec_add(&velocity, &velocity, &t);
	return;
}

void process_motion(int value)
{
	vec_addmul(&pos, &pos, dt, &velocity);

	/* notice that after the rotation is done the rotational_vel is set to
	 * zero because it would absolutely suck to continue rotating after
	 * the users's rotation request is fullfilled. That's the way real life
	 * works, but who lives in reality anyway ?
	 */
	rotate_view_aboutx(rotation.x * dt);
	rotate_view_abouty(rotation.y * dt);
	rotate_view_aboutz(rotation.z * dt);

	vec_zero(&rotation);
	return;
}

void stop(void)
{
	/* this applies thrust in the exact opposite
	 * direction of your current vector of velocity.
	 */
	vec_sub(&thrust, &ZERO_VEC, &velocity);
	return;
}

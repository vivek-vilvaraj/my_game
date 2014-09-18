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

#include "asteroids3D.h"
#include "vecops.h"

/* three important vectors (also see thrust.c):
 *
 * 3) rotation
 *
 * is the vector which holds the input requests for a rotation about an axis.
 * this is a "request" modulated by dt
 *
 */
Vector rotation;

//-----------------------------------------------------------------------------
void slide_alongx(double d)
{
	vec_addmul(&thrust, &thrust, d, &xaxis);
	return;
}

void slide_alongy(double d)
{
	vec_addmul(&thrust, &thrust, d, &yaxis);
	return;
}

void slide_alongz(double d)
{
	vec_addmul(&thrust, &thrust, d, &zaxis);
	return;
}

void turn_aboutx(double t)
{
	rotation.x += t;
	return;
}

void turn_abouty(double t)
{
	rotation.y += t;
	return;
}

void turn_aboutz(double t)
{
	rotation.z += t;
	return;
}

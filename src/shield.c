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

// Shield display list, shield ttl, and shield strength
int shield_dl, shield_ttl, shield_activated_flag = 0;
float shield_strength;

//-----------------------------------------------------------------------------
void init_shield(void) {
	static const float emit0[]    = {0.0, 0.6, 0.0, 1.0};
	static const float ad0[]      = {0.0, 0.0, 0.0, 0.5};
	static const float emit_off[] = {0.0, 0.0, 0.0, 1.0};
	static const float width = 2.0, height = 2.0; /* width and height should be chosen to cover window */

	/* set the initial shield strength */

	shield_strength = MAX_SHIELD ;

	/* initialize the shield display list */

	shield_dl = glGenLists (1);
	glNewList (shield_dl, GL_COMPILE);
		glEnable(GL_BLEND) ;
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA) ;
		glMaterialfv(GL_FRONT,GL_EMISSION,emit0) ;
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,ad0) ;
		glPushMatrix() ;
			glLoadIdentity() ;
			glTranslatef(0, 0, -2) ;
			glBegin( GL_QUADS ) ;
				glVertex3f(  width , height, 0.0 ) ;
				glVertex3f( -width , height , 0.0 ) ;
				glVertex3f( -width , -height , 0.0 ) ;
				glVertex3f( width ,  -height , 0.0 ) ;
			glEnd() ;
		glPopMatrix() ;
		glMaterialfv(GL_FRONT,GL_EMISSION,emit_off) ;
		glDisable(GL_BLEND) ;
	glEndList ();

	return ;
}

void process_shield(void) {
	/* FIXME do something cool here... */
	if (shield_strength < 0)
		end_game();

	if (shield_strength < MAX_SHIELD)
		shield_strength += SHIELD_REGEN_PS * dt;
	if (shield_strength > MAX_SHIELD)
		shield_strength = MAX_SHIELD;
	return;
}

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

// Maximum thrust that can be input
#define MAX_TINPUT 0.5

static inline void toggle_int_flag(int *);

// Define the keyboard map
KeyboardMap keymap = {
	.asteroid_texture_toggle    = 'm',
	.auto_target_asteroid       = 'b', // was '\t'
	.axes_toggle                = '$',
	.crosshair_toggle           = '+',
	.draw_dust_toggle           = '(',
	.draw_shield_toggle         = ')',
	.draw_torp_lensflare_toggle = '*',
	.draw_tracer_toggle         = '_',
	.end_game                   = 27 /* == '\e' */,
	.fire_torp                  = ' ',
	.headlight_toggle           = '[',
	.nebula_texture_toggle      = 'k',
	.pause                      = 'p',
	.play_sound_toggle          = '#',
	.reverse_mouse_x_toggle     = '!',
	.reverse_mouse_y_toggle     = '@',
	.rot_aboutx_ccw             = '2',
	.rot_aboutx_cw              = '8',
	.rot_abouty_ccw             = '4',
	.rot_abouty_cw              = '6',
	.rot_aboutz_ccw             = 'e',
	.rot_aboutz_cw              = 'q',
	.slide_backward             = 'w', // was 'v'
	.slide_down                 = 'x', // was 't'
	.slide_forward              = 's', // was 'r'
	.slide_left                 = 'a', // was 'd'
	.slide_right                = 'd', // was 'g'
	.slide_up                   = 'c',
	.stop                       = 'f',
	.target_asteroid            = '\t', // was 'b'
	.torp_texture_toggle        = 'o',
};
KeyboardMap keymap_old = {
	.asteroid_texture_toggle    = '&',
	.auto_target_asteroid       = '\t',
	.axes_toggle                = '$',
	.crosshair_toggle           = '+',
	.draw_dust_toggle           = '(',
	.draw_shield_toggle         = ')',
	.draw_torp_lensflare_toggle = '*',
	.draw_tracer_toggle         = '_',
	.end_game                   = 27 /* == '\e' */,
	.fire_torp                  = ' ',
	.headlight_toggle           = '[',
	.nebula_texture_toggle      = '^',
	.pause                      = 'p',
	.play_sound_toggle          = '#',
	.reverse_mouse_x_toggle     = '!',
	.reverse_mouse_y_toggle     = '@',
	.rot_aboutx_ccw             = '2',
	.rot_aboutx_cw              = '8',
	.rot_abouty_ccw             = '4',
	.rot_abouty_cw              = '6',
	.rot_aboutz_ccw             = '9',
	.rot_aboutz_cw              = '7',
	.slide_backward             = 'v',
	.slide_down                 = 't',
	.slide_forward              = 'r',
	.slide_left                 = 'd',
	.slide_right                = 'g',
	.slide_up                   = 'c',
	.stop                       = 'f',
	.target_asteroid            = 'b',
	.torp_texture_toggle        = '%',
};

// These are used for scaling keyboard rotation input
int keyb_rotx_scalar = 0, keyb_roty_scalar = 0, keyb_rotz_scalar = 0;

//-----------------------------------------------------------------------------
void keyboard_handler(unsigned char key, int x, int y)
{
	if (key == keymap.end_game)
		end_game();
	else if (key == keymap.pause)
		start_pause();
	else if (key == keymap.slide_forward)
		slide_alongz(MAX_TINPUT);
	else if (key == keymap.slide_backward)
		slide_alongz(-MAX_TINPUT);
	else if (key == keymap.slide_left)
		slide_alongx(MAX_TINPUT);
	else if (key == keymap.slide_right)
		slide_alongx(-MAX_TINPUT);
	else if (key == keymap.slide_up)
		slide_alongy(MAX_TINPUT);
	else if (key == keymap.slide_down)
		slide_alongy(-MAX_TINPUT);
	else if (key == keymap.stop)
		stop();
	else if (key == keymap.rot_aboutx_cw)
		(keyb_rotx_scalar == 0) ? keyb_rotx_scalar = -4 :
		                          --keyb_rotx_scalar;
	else if (key == keymap.rot_aboutx_ccw)
		(keyb_rotx_scalar == 0) ? keyb_rotx_scalar =  4 :
		                          ++keyb_rotx_scalar;
	else if (key == keymap.rot_abouty_cw)
		(keyb_roty_scalar == 0) ? keyb_roty_scalar =  4 :
		                          ++keyb_roty_scalar;
	else if (key == keymap.rot_abouty_ccw)
		(keyb_roty_scalar == 0) ? keyb_roty_scalar = -4 :
		                          --keyb_roty_scalar;
	else if (key == keymap.rot_aboutz_cw)
		(keyb_rotz_scalar == 0) ? keyb_rotz_scalar =  4 :
		                          ++keyb_rotz_scalar;
	else if (key == keymap.rot_aboutz_ccw)
		(keyb_rotz_scalar == 0) ? keyb_rotz_scalar = -4 :
		                          --keyb_rotz_scalar;
	else if (key == keymap.headlight_toggle)
		toggle_headlight();
	else if (key == keymap.target_asteroid)
		target_asteroid();
	else if (key == keymap.auto_target_asteroid)
		auto_target_asteroid();
	else if (key == keymap.fire_torp)
		fire_torpedo();
	else if (key == keymap.crosshair_toggle)
		toggle_int_flag(&optmap.crosshair_flag);
	else if (key == keymap.draw_tracer_toggle)
		toggle_int_flag(&optmap.draw_tracer_flag);
	else if (key == keymap.draw_shield_toggle)
		toggle_int_flag(&optmap.draw_shield_flag);
	else if (key == keymap.draw_dust_toggle)
		toggle_int_flag(&optmap.draw_dust_flag);
	else if (key == keymap.draw_torp_lensflare_toggle)
		toggle_int_flag(&optmap.draw_torp_lensflare_flag);
	else if (key == keymap.asteroid_texture_toggle)
		toggle_int_flag(&optmap.texture_flag);
	else if (key == keymap.nebula_texture_toggle)
		toggle_int_flag(&optmap.nebula_texture_flag);
	else if (key == keymap.torp_texture_toggle)
		toggle_int_flag(&optmap.torp_texture_flag);
	else if (key == keymap.axes_toggle)
		toggle_int_flag(&optmap.axes_flag);
	else if (key == keymap.play_sound_toggle)
		toggle_int_flag(&optmap.play_sound_flag);
	else if (key == keymap.reverse_mouse_x_toggle)
		optmap.mouse_xdir *= -1;
	else if (key == keymap.reverse_mouse_y_toggle)
		optmap.mouse_ydir *= -1;
	return;
}

static inline void toggle_int_flag(int *flag)
{
	*flag = !*flag;
	return;
}

/*
 * The purpose of these three decay_keyb_rotX_scalar() functions
 * is a bit hard to explain. First, I determined that more than
 * one frame of animation may be drawn between the time the keyboard
 * is polled for successive keypresses. The consequence of this is that if one
 * simply turns the ship X degrees each time a key is determined to be
 * pressed, then the motion will be choppy on "slow" machines.
 *
 * Secondly, the speed of rotation when using the mouse is proportional
 * to the distance the mouse is from the center of the screen. I thought
 * it would be good ( for people who want to play using only the keyboard )
 * to have an analogous quality for keyboard rotation inputs.
 *
 * So when a rotation key is hit keyb_rotX_scalar is incremented from zero.
 * when the rotation is actualy applied in capture_passive_mouse_input()
 * the decay functions are called to reduce ( but not immediately zero )
 * keyb_rotX_scalar so that the next frames amount of rotation is
 * less but not completely zero.
 *
 * I hope this makes some sense to me when I read it again...
 */
void decay_keyb_rotx_scalar(void)
{
	static int count = 0;

	++count;
	count %= KEYB_SCALE_DECAY;

	if (keyb_rotx_scalar > KEYB_MAX_ROT_SCALE)
		keyb_rotx_scalar = KEYB_MAX_ROT_SCALE ;
	if (keyb_rotx_scalar < -KEYB_MAX_ROT_SCALE)
		keyb_rotx_scalar = -KEYB_MAX_ROT_SCALE;
	if (count != 0)
		return;
	if (keyb_rotx_scalar > 0)
		--keyb_rotx_scalar;
	if (keyb_rotx_scalar < 0)
		++keyb_rotx_scalar;
	return;
}

void decay_keyb_roty_scalar(void)
{
	static int count = 0;

	++count;
	count %= KEYB_SCALE_DECAY;

	if (keyb_roty_scalar > KEYB_MAX_ROT_SCALE)
		keyb_roty_scalar = KEYB_MAX_ROT_SCALE;
	if (keyb_roty_scalar < -KEYB_MAX_ROT_SCALE)
		keyb_roty_scalar = -KEYB_MAX_ROT_SCALE;
	if (count != 0)
		return;
	if (keyb_roty_scalar > 0 )
		--keyb_roty_scalar;
	if (keyb_roty_scalar < 0 )
		++keyb_roty_scalar;
	return;
}

void decay_keyb_rotz_scalar(void)
{
	static int count = 0;

	++count;
	count %= KEYB_SCALE_DECAY;

	if (keyb_rotz_scalar > KEYB_MAX_ROT_SCALE)
		keyb_rotz_scalar = KEYB_MAX_ROT_SCALE;
	if (keyb_rotz_scalar < -KEYB_MAX_ROT_SCALE)
		keyb_rotz_scalar = -KEYB_MAX_ROT_SCALE;
	if (count != 0)
		return;
	if (keyb_rotz_scalar > 0)
		--keyb_rotz_scalar;
	if (keyb_rotz_scalar < 0)
		++keyb_rotz_scalar;
	return;
}

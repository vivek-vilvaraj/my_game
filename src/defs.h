/*=============================================================================
Asteroids3D - a first person game of blowing up asteroids
  Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2005

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
#ifndef AS3D_DEFS_H
#define AS3D_DEFS_H 1

#ifdef __cplusplus
extern "C" {
#endif

// for Frustum perspective projection (main.c, display.c)
#define FRUSTUM_LEFT -1
#define FRUSTUM_TOP 1

/*
 * Asteroid generation parameters
 */

/*
 * generate asteroids within a cubic space which is
 * no further than this distance from the origin
 */
#define AGB 50.0      /* That's asteroid generation boundary */
#define MAX_ARD 180.0   /* max asteroid rotation delta (per unit time)*/
#define MAX_APD 20.0   /* max asteroid position delta length (per unit time)*/
#define ABOUND  90  /* asteroid torus wrapping boundary */
#define NUM_AST_TYPES 6 /* number of different kinds of asteroids/display lists */
#define MIN_AST_MASS 5.0
#define AST_MASS_K 10.0

/* Torpedos are the "bullets" in the game.
 * You can only fire a few at a time
 */
#define MAX_TORPS 5
#define TORP_TTL 1.8
#define TORP_DELTA 25.0
#define TORP_RADIUS 0.5
#define TORP_RADIUS_DELTA 0.2
#define TORP_BB_ZROT 5  /* the number of degrees to rotate the torp billboard about the z axis */

/* torpedoes leave trails behind them */
#define MAX_TORP_TRAILS (MAX_TORPS*3)
#define TORP_TRAIL_LEN  25

/* number of dust particles */
#define NUM_DUST_PART 50
#define NUM_CHILD_DUST_PART 10
#define CHILD_DUST_PART_DELTA 20 /* maximum number of units a child can be from the parent dust particle */

/* for explosions */
#define MAX_BLASTS (MAX_TORPS+10)
#define BLAST_TTL 5
#define BLAST_ER 2.0 /* blast expansion rate */
#define NUM_BLAST_TYPES 5

/* initial shield/max shield strength, and shield recharge rate per second */
#define MAX_SHIELD 4000
#define SHIELD_REGEN_PS 10

/* When you run through an explosion the particles smack up
 * against your "windshield" pretty much just a little eye candy
 * anyway you could potentially ( though unlikely ) run through
 * MAX_BLASTS explosions. And I need to also set about how many
 * particles you impact per blast which is:
 */
#define NUM_WP_PER_BLAST 10

/* define the types of sounds that play_sound() recognizes */
#define SOUND_ASTEROID_HIT     0
#define SOUND_TARGET_AQUIRED   1
#define SOUND_COLLISION_WARN   2
#define SOUND_COLLISION        4
#define SOUND_TORP_FIRED       5

/* standard hud colors */
#define HUD_R 0.3
#define HUD_G 1.0
#define HUD_B 0.3
#define HUD_A 0.7

// Number of nebula billboard textures
#define NUM_NEBULA_TEX 4

// Stu says: initial number of asteroids 150 is good
// Jan says: Nowhere. You almost collide at start even when there is only 50.
#define NUM_ASTEROIDS 50

/* these are keyboard rotation scaling factors notice that if you multiply
 * KEYB_BASE_ROT_SCALE by KEYB_MAX_ROT_SCALE you get 1
 */

#define KEYB_BASE_ROT_SCALE 0.05
#define KEYB_MAX_ROT_SCALE  20
#define KEYB_SCALE_DECAY 2

// Code annotations
#define reinterpret_cast(type, expr)    ((type)(expr))
#define static_cast(type, expr)         ((type)(expr))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AS3D__H

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
#ifndef AS3D__H
#define AS3D__H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "structs.h"

/*
 *      ASTEROID.C
 */
extern Node *add_node(void);
extern void init_asteroids(void);
extern void process_asteroid_motion(int);

extern Node *al_head, *al_tail, *delete_next_ast[];
extern int ast_dl[], num_asteroids_on_scanner;
extern unsigned int ast_tex[];

/*
 *      CALC_FRAME.C
 */
extern void *calculate_frame(void *);
extern void capture_passive_mouse_input(void);

/*
 *      COLLISION.C
 */
extern void apply_collision_v(Vector *, int);
extern void check_collision(Node *);
extern void correct_positions(Vector *, Vector *, double);
extern void process_deleted_ast(void);

/*
 *      DEBUG.C
 */
extern void init_debug(void);
extern void draw_debug_axes(void);
extern void draw_debug_grid(void);

/*
 *      DISPLAY.C
 */
extern void display(void);

/*
 *      DUST.C
 */
extern void init_dust(void);
extern void process_dust(void);

extern Vector dust_part[][NUM_CHILD_DUST_PART];

/*
 *      EXPLOSION.C
 */
extern void activate_explosion(const Vector *);
extern void init_explosions(void);
extern void process_explosion(void);

extern Blast explosion[];
extern int expl_dl[], expl_winshield_particle_dl,
    expl_winshield_particle_count, explosions_active_flag;

/*
 *      HUD.C
 */
extern void draw_crosshair(void);
extern void draw_text_in_color(double, double, const char *, double, double,
    double, double);
extern void draw_shield_gauge(void);
extern void init_hud(void);
extern void draw_collision_warning_gauge(void);
extern void scan_for_collision_warning(Asteroid *);
extern int draw_gauge(void);
extern void draw_torp_gauge(void);

extern double flicker_hud_ttl;
extern int cwg_rear, cwg_above, cwg_below, cwg_left, cwg_right;

/*
 *      KEYB.C
 */
extern void decay_keyb_rotx_scalar(void);
extern void decay_keyb_roty_scalar(void);
extern void decay_keyb_rotz_scalar(void);
extern void keyboard_handler(unsigned char, int, int);

extern KeyboardMap keymap, keymap_old;
extern int keyb_rotx_scalar, keyb_roty_scalar, keyb_rotz_scalar;

/*
 *      LIGHT.C
 */
extern void gl_init_headlight(void);
extern void gl_init_light(void);
extern void toggle_headlight(void);
extern void update_headlight(void);

extern float l0_pos[], l1_pos[], l1_dir[];

/*
 *      MAIN.C
 */
extern void end_game(void);
extern void mouse_handler(int, int, int, int);
extern void passive_mouse_handler(int, int);

extern double dt, tps;
extern int game_end_flag;
extern int mouse_rotx_flag, mouse_roty_flag, mouse_x, mouse_y;
extern double mouse_xprop, mouse_yprop;
extern pthread_mutex_t run_yield;
extern unsigned long ticks;

/*
 *      MOVE.C
 */
extern void slide_alongx(double);
extern void slide_alongy(double);
extern void slide_alongz(double);
extern void turn_aboutx(double);
extern void turn_abouty(double);
extern void turn_aboutz(double);

extern Vector rotation;

/*
 *      OGLUTILS.C
 */
extern void apply_v2m(Vector *, const double *);
extern int gen_rand_int(int, int);
extern void gen_rand_vec(Vector *);
extern double gen_rand_float(double, double);
extern void find_rotation_angles(const Vector *, double *, double *);
extern void find_rotation_mat(double, const Vector *, double *);

/*
 *      PAUSE.C
 */
extern int paused(void);
extern void paused_display(void);
extern void start_pause(void);

extern int paused_flag;

/*
 *      RCFILE.C
 */
extern char *key_char2str(char, char *, size_t);
extern void read_rcfile(void) ;
extern char *strip_nl(char *);
extern void write_rcfile_to_terminal(int);

extern UserOptionsMap optmap;
extern char *rcfile;

/*
 *      SCORE.C
 */
extern void add_rock_to_score(const Asteroid *);
extern void init_score_display(void);

extern double difficulty_multiplier;
extern unsigned int score;

/*
 *      SHIELD.C
 */
extern int shield_dl, shield_ttl, shield_activated_flag;
extern float shield_strength;

extern void init_shield(void);
extern void process_shield(void);

/*
 *      SOUND.C
 */
extern void init_sound(void);
extern void play_sound(int, int);
extern void process_sound(void);

/*
 *      SPLIT.C
 */
extern void add_new_asteroids(const Vector *, int);

/*
 *      SUN.C
 */
extern void init_sun(void);
extern void init_nebula_billboards(void);
extern void init_lens_flares(void);
extern void draw_sun_lensflares(void);
extern void draw_torp_lensflares(void);

extern int sun_dl;

/*
 *      TARGET.C
 */
extern void auto_target_asteroid(void);
extern void draw_lcs(const Asteroid *);
extern void draw_steering_cue(const Asteroid *);
extern void draw_target_box(const Asteroid *);
extern void draw_target_vector(const Asteroid *);
extern void drop_target(void);
extern void init_target_dls(void);
extern int is_targeted(const Asteroid *);
extern void target_asteroid(void);

extern Node *currently_targeted_asteroid;

/*
 *      TEXTURE.C
 */
extern void init_textures(void);

extern int nebula_dl[];
extern unsigned int nebula_tex[];

/*
 *      THRUST.C
 */
extern void apply_thrust(int);
extern void init_thrust(void) ;
extern void process_motion(int);
extern void stop(void) ;

extern Vector thrust, velocity;

/*
 *      TORPEDO.C
 */
extern void calc_torp_billboard_rot(const Vector *, double *, double *);
extern void draw_torp_plasma(void);
extern void fire_torpedo(void);
extern void init_torpedo(void);
extern void init_torpedo_trails(void);
extern void init_torps(void);
extern void process_torpedo_motion(int);
extern void process_torpedo_trails(void);

extern Torpedo torp[], sorted_torp[];
extern int torp_dl, torp_billboard_dl, torps_in_flight_flag, tracer_light_flag;
extern unsigned int torp_tex;
extern TorpedoTrail torp_trail[];

/*
 *      VIEW.C
 */
extern void reset_view(void);
extern void rotate_view_aboutx(double);
extern void rotate_view_abouty(double);
extern void rotate_view_aboutz(double);

extern Vector pos, xaxis, yaxis, zaxis;

/*
 *      WIND_PART.C
 */
extern void gen_rand_wind_part(WindPart *, double);
extern void init_wind_parts(void);

extern WindPart winshield_particle[];

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AS3D__H

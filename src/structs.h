/*=============================================================================
Asteroids3D - a first person game of blowing up asteroids
  Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2003 - 2005

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
#ifndef AS3D_STRUCTS_H
#define AS3D_STRUCTS_H 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vector {
    double x, y, z, w;
} Vector;

typedef struct asteroid {
    double mass;        // BASED ON TYPE
    int    type;        // which asteroid display list to use
    double radius;      // radius to use for collisions -- BASED ON TYPE
    Vector pos;         // current position
    Vector velocity;    // vector along which asteroid moves
    int    rot;         // current degrees of rotation
    int    rot_delta;   // degress to rotate per unit time
    Vector rot_axis;    // axis asteroid rotates around
} Asteroid;

typedef struct torpedo {
    int in_flight_flag;
    int is_tracer;
    int trail_num;
    double ttl; // time to live
    Vector pos;
    Vector pos_delta;
    // the number of degrees to rotate the torp billboard about the z axis
    int bb_zrot;
} Torpedo;

typedef struct torpedo_trail {
    int is_running_flag;
    int is_ending_flag;
    int trail_start;
    int trail_end;
    Vector pos[TORP_TRAIL_LEN];
} TorpedoTrail;

typedef struct node {
    Asteroid rock;
    struct node *next;
} Node;

typedef struct blast {
    int    is_active;
    double ttl;
    Vector pos;
    float  scalar;
    int    type; // which explosion display list to use
} Blast;

typedef struct wind_part {
    Vector pos;                 // position on windshield glass
    double size;                // size of impact circle
    double t_start, t_end;      // time to start/end
} WindPart;

typedef struct keyboard_map {
    char

    // primary control keys
    end_game, pause,

    // movement keys
    slide_forward, slide_backward,
    slide_left, slide_right,
    slide_up, slide_down,
    stop,
    rot_aboutx_cw, rot_abouty_cw, rot_aboutz_cw,
    rot_aboutx_ccw, rot_abouty_ccw, rot_aboutz_ccw,

    headlight_toggle,
    target_asteroid,
    auto_target_asteroid,
    fire_torp,

    // feature toggles
    crosshair_toggle,
    draw_tracer_toggle,
    draw_shield_toggle,
    draw_dust_toggle,
    draw_torp_lensflare_toggle,
    asteroid_texture_toggle,
    nebula_texture_toggle,
    torp_texture_toggle,
    axes_toggle,
    play_sound_toggle,
    reverse_mouse_x_toggle, reverse_mouse_y_toggle
    ;
} KeyboardMap;

typedef struct user_options_map {
    // Flags for user preferences (0=off, 1=on)
    int antialias_flag;          // smoothen out lines
    int axes_flag;               // turn on and off the absolute x,y,z axes. I use this for debugging.
    int crosshair_flag;          // turn the blue crosshair on or off
    int draw_tracer_flag;        // should the torpedo tracer light be drawn
    int draw_shield_flag;        // should the green alpha-blended shield effect be used
    int draw_dust_flag;          // should the dust particles be drawn
    int texture_flag;            // should the asteroids be textured
    int nebula_texture_flag;     // should the nebula textures be used
    int play_sound_flag;         // should sound be used
    int torp_texture_flag;       // should the torps be textured
    int draw_torp_lensflare_flag; // should the torp lens flares be drawn

    // more user preferences
    double mouse_xdir;  // 1 = mouse movement turns the normal direction, -1 = mouse movement turns opposite direction (should be -1 or 1 only)
    double mouse_ydir;  // 1 = mouse movement turns the normal direction, -1 = mouse movement turns opposite direction (should be -1 or 1 only)
    int num_asteroids;  // number of asteroids, set by #define or command line
    int windowx;        // window x size, changed in reshape(). set by #define or on command line
    int windowy;        // window y size, changed in reshape(). set by #define or on command line
    int window_xpos;    // window left corner x offset
    int window_ypos;    // window left corner y offset

    // 0=cube, 1=octahedron, 2=sphere, 3=tetrahedron
    unsigned int target_box_type;
} UserOptionsMap;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AS3D_STRUCTS_H

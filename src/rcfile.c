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

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asteroids3D.h"

// This stuff for the rc file
#define RC_FILENAME "/.a3drc"

static FILE *fopen_rcfile(void);
static char key_string2char(const char *);
static const char *key_flag2str(int);
static const char *key_mouse2str(double);

UserOptionsMap optmap = {
	.antialias_flag           = 1,
	.axes_flag                = 0,
	.crosshair_flag           = 1,
	.draw_dust_flag           = 1,
	.draw_shield_flag         = 1,
	.draw_torp_lensflare_flag = 1, // was 0
	.draw_tracer_flag         = 1,
	.mouse_xdir               = 1, // 1 or -1
	.mouse_ydir               = 1,
	.nebula_texture_flag      = 0,
	.num_asteroids            = NUM_ASTEROIDS,
	.play_sound_flag          = 1,
	.target_box_type          = 0,
	.texture_flag             = 1,
	.torp_texture_flag        = 1,
	.windowx                  = 800, // window size in pixels
	.window_xpos              = 0,   // window position offset (was 150,50)
	.windowy                  = 600,
	.window_ypos              = 0,
};

char *rcfile = NULL; // the full pathname of the rcfile

//-----------------------------------------------------------------------------
void read_rcfile(void)
{
#define TOK_F(Pk, Pv) /* flag */ \
	if (strcmp((Pk), key) == 0) { \
		(Pv) = strcmp("on", val) == 0; \
		continue; \
	}

#define TOK_K(Pk, Pv) /* key */ \
	if (strcmp((Pk), key) == 0) { \
		(Pv) = key_string2char(val); \
		continue; \
	}

#define TOK_M(Pk, Pv) /* mouse flag */ \
	if (strcmp((Pk), key) == 0) { \
		(Pv) = (strcmp("on", buf) == 0) ? -1 : 1; \
		continue; \
	}

#define TOK_N(Pk, Pv, Pn) /* value */ \
	if (strcmp((Pk), key) == 0) { \
		if ((temp = strtoul(val, NULL, 0)) >= (Pn)) \
			(Pv) = temp; \
		else \
			(Pv) = (Pn); \
		continue; \
	}

	unsigned long temp;
	char buf[128];
	FILE *fp;

	if ((fp = fopen_rcfile()) == NULL)
		return;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		char *key = buf, *k, *val;
		strip_nl(buf);

		if (*buf == '\0' || *buf == '#') {
			// Ignore empty and comment lines
			continue;
		}
		if ((val = strchr(buf, '=')) == NULL) {
			fprintf(stderr, "Errorneus line in config file:\n"
			        "  %s\n", buf);
			continue;
		}

		// Remove trailing whitespace of key and leading whitespace of val
		k = val - 1;
		while (isspace(*k))
			--k;
		*++k = *val++ = '\0';
		while (isspace(*val))
			++val;

		// Keys
		TOK_K("asteroid_texture_toggle_key", keymap.asteroid_texture_toggle);
		TOK_K("auto_target_asteroid_key", keymap.auto_target_asteroid);
		TOK_K("axes_toggle_key", keymap.axes_toggle);
		TOK_K("crosshair_toggle_key", keymap.crosshair_toggle);
		TOK_K("draw_dust_toggle_key", keymap.draw_dust_toggle);
		TOK_K("draw_shield_toggle_key", keymap.draw_shield_toggle);
		TOK_K("draw_torp_lensflare_toggle_key", keymap.draw_torp_lensflare_toggle);
		TOK_K("draw_tracer_toggle_key", keymap.draw_tracer_toggle);
		TOK_K("end_game_key", keymap.end_game);
		TOK_K("fire_torpedo_key", keymap.fire_torp);
		TOK_K("headlight_key", keymap.headlight_toggle);
		TOK_K("nebula_texture_toggle_key", keymap.nebula_texture_toggle);
		TOK_K("pause_key", keymap.pause);
		TOK_K("play_sound_toggle_key", keymap.play_sound_toggle);
		TOK_K("reverse_mouse_x_toggle_key", keymap.reverse_mouse_x_toggle);
		TOK_K("reverse_mouse_y_toggle_key", keymap.reverse_mouse_y_toggle);
		TOK_K("rot_aboutx_ccw_key", keymap.rot_aboutx_ccw);
		TOK_K("rot_aboutx_cw_key", keymap.rot_aboutx_cw);
		TOK_K("rot_abouty_ccw_key", keymap.rot_abouty_ccw);
		TOK_K("rot_abouty_cw_key", keymap.rot_abouty_cw);
		TOK_K("rot_aboutz_ccw_key", keymap.rot_aboutz_ccw);
		TOK_K("rot_aboutz_cw_key", keymap.rot_aboutz_cw);
		TOK_K("slide_backward_key", keymap.slide_backward);
		TOK_K("slide_down_key", keymap.slide_down);
		TOK_K("slide_forward_key", keymap.slide_forward);
		TOK_K("slide_left_key", keymap.slide_left);
		TOK_K("slide_right_key", keymap.slide_right);
		TOK_K("slide_up_key", keymap.slide_up);
		TOK_K("stop_key", keymap.stop);
		TOK_K("target_asteroid_key", keymap.target_asteroid);
		TOK_K("torp_texture_toggle_key", keymap.torp_texture_toggle);

		// Options
		TOK_F("asteroid_textures", optmap.texture_flag);
		TOK_F("crosshair", optmap.crosshair_flag);
		TOK_F("debugging_axes", optmap.axes_flag);
		TOK_F("dust_effect", optmap.draw_dust_flag);
		TOK_F("nebula_textures", optmap.nebula_texture_flag);
		TOK_F("shield_effect", optmap.draw_shield_flag);
		TOK_F("sound", optmap.play_sound_flag);
		TOK_F("torp_lensflares", optmap.draw_torp_lensflare_flag);
		TOK_F("torp_textures", optmap.torp_texture_flag);
		TOK_F("torp_tracer_effect", optmap.draw_tracer_flag);

		TOK_M("reverse_mouse_x", optmap.mouse_xdir);
		TOK_M("reverse_mouse_y", optmap.mouse_ydir);

		TOK_N("num_asteroids", optmap.num_asteroids, 2);
		TOK_N("target_box_type", optmap.target_box_type, 0);
		TOK_N("win_height", optmap.windowy, 0);
		TOK_N("win_width", optmap.windowx, 0);
		TOK_N("win_xpos", optmap.window_xpos, 0);
		TOK_N("win_ypos", optmap.window_ypos, 0);
		fprintf(stderr, "Unrecognized token \"%s\" in rcfile %s\n",
		        buf, rcfile);
	}

	fclose(fp);
	return;
#undef TOK_F
#undef TOK_K
#undef TOK_M
#undef TOK_N
}

static FILE *fopen_rcfile(void)
{
	char *homedir;
	int homedir_size;

	/* if rcfile was not set on the command line, use the default */
	if (rcfile == NULL) {
		homedir = getenv("HOME");
		homedir_size = strlen(homedir);
		rcfile = malloc(homedir_size + strlen(RC_FILENAME) + 1);
		strcpy(rcfile, homedir);
		strcat(rcfile, RC_FILENAME);
	}

	return fopen(rcfile, "r");
}

void write_rcfile_to_terminal(int oldmap)
{
#define TOK_F(Pname, Pvar, Pcomment) \
	printf("# " Pcomment "\n"); \
	printf(Pname "=%s\n\n", key_flag2str(Pvar));

#define TOK_K(Pname, Pvar, Pcomment) \
	printf("# " Pcomment "\n"); \
	printf(Pname "=%s\n\n", key_char2str((Pvar), buf, sizeof(buf)));

#define TOK_M(Pname, Pvar, Pcomment) \
	printf("# " Pcomment "\n"); \
	printf(Pname "=%s\n\n", key_mouse2str(Pvar));

#define TOK_N(Pname, Pvar, Pcomment) \
	printf("# " Pcomment "\n"); \
	printf(Pname "=%d\n\n", (Pvar));

	const KeyboardMap *kmap = oldmap ? &keymap_old : &keymap;
	char buf[128];

	printf(
		"### Asteroids3D default .a3drc file\n"
		"#\n"
		"\n"
		"# Blank lines are ignored.\n"
		"# Lines which begin with a '#' are ignored.\n"
		"\n"
	);

	printf(
		"################\n"
		"# Game Key Map #\n"
		"################\n"
		"#\n"
		"# Just about any keyboard key can be mapped.\n"
		"# Here are the execeptions:\n"
		"#\n"
		"# Key To Map        Use (no quotes)\n"
		"# ----------       ---------------\n"
		"# tab              'tab'\n"
		"# enter            'enter'\n"
		"# space            'space'\n"
		"# escape           'esc'\n"
		"# backspace        'backspace'\n"
		"# delete           'delete'\n"
		"# '#'              '\\#'\n"
		"# '='              '\\='\n"
		"# '\\'              '\\\\'\n"
		"#\n"
	);
	TOK_K("asteroid_texture_toggle_key", kmap->asteroid_texture_toggle, "key to toggle the asteroid textures on and off");
	TOK_K("auto_target_asteroid_key", kmap->auto_target_asteroid, "auto-target asteroid");
	TOK_K("axes_toggle_key", kmap->axes_toggle, "key to toggle the debug axes on and off");
	TOK_K("crosshair_toggle_key", kmap->crosshair_toggle, "key to toggle the crosshair on and off");
	TOK_K("draw_dust_toggle_key", kmap->draw_dust_toggle, "key to toggle the dust effect on and off");
	TOK_K("draw_shield_toggle_key", kmap->draw_shield_toggle, "key to toggle the shield effect on and off");
	TOK_K("draw_torp_lensflare_toggle_key", kmap->draw_torp_lensflare_toggle, "key to toggle the torpedo lens flare effect on and off");
	TOK_K("draw_tracer_toggle_key", kmap->draw_tracer_toggle, "key to toggle the tracer effect on and off");
	TOK_K("end_game_key", kmap->end_game, "end game");
	TOK_K("fire_torpedo_key", kmap->fire_torp, "fire torpedo");
	TOK_K("headlight_key", kmap->headlight_toggle, "headlight toggle");
	TOK_K("nebula_texture_toggle_key", kmap->nebula_texture_toggle, "key to toggle the nebula textures on and off");
	TOK_K("pause_key", kmap->pause, "pause game");
	TOK_K("play_sound_toggle_key", kmap->play_sound_toggle, "key to toggle the sound on and off");
	TOK_K("reverse_mouse_x_toggle_key", kmap->reverse_mouse_x_toggle, "key to toggle the x-axis mouse reverse on and off");
	TOK_K("reverse_mouse_y_toggle_key", kmap->reverse_mouse_y_toggle, "key to toggle the y-axis mouse reverse on and off");
	TOK_K("rot_aboutx_ccw_key", kmap->rot_aboutx_cw, "rotate counter-clockwise about x-axis");
	TOK_K("rot_aboutx_cw_key", kmap->rot_aboutx_cw, "rotate clockwise about x-axis");
	TOK_K("rot_abouty_ccw_key", kmap->rot_aboutx_cw, "rotate counter-clockwise about y-axis");
	TOK_K("rot_abouty_cw_key", kmap->rot_aboutx_cw, "rotate clockwise about y-axis");
	TOK_K("rot_aboutz_ccw_key", kmap->rot_aboutx_cw, "rotate counter-clockwise about z-axis");
	TOK_K("rot_aboutz_cw_key", kmap->rot_aboutx_cw, "rotate clockwise about z-axis");
	TOK_K("slide_backward_key", kmap->slide_backward, "slide backward");
	TOK_K("slide_down_key", kmap->slide_down, "slide down");
	TOK_K("slide_forward_key", kmap->slide_forward, "slide forward");
	TOK_K("slide_left_key", kmap->slide_left, "slide left");
	TOK_K("slide_right_key", kmap->slide_right, "slide right");
	TOK_K("slide_up_key", kmap->slide_up, "slide up");
	TOK_K("stop_key", kmap->stop, "stop");
	TOK_K("target_asteroid_key", kmap->target_asteroid, "target asteroid in crosshair");
	TOK_K("torp_texture_toggle_key", kmap->torp_texture_toggle, "key to toggle the torpedo textures on and off");

	printf("# Game options\n");
	TOK_F("asteroid_textures", optmap.texture_flag, "texture map the asteroids");
	TOK_F("crosshair", optmap.crosshair_flag, "crosshair");
	TOK_F("debugging_axes", optmap.axes_flag, "my debugging axes");
	TOK_F("dust_effect", optmap.draw_dust_flag, "dust effect");
	TOK_F("nebula_textures", optmap.nebula_texture_flag, "display textured nebula billboards");
	TOK_F("shield_effect", optmap.draw_shield_flag, "shield effect");
	TOK_F("sound", optmap.play_sound_flag, "sounds");
	TOK_F("torp_lensflares", optmap.draw_torp_lensflare_flag, "torp lens flare");
	TOK_F("torp_textures", optmap.torp_texture_flag, "texture map the torps");
	TOK_F("torp_tracer_effect", optmap.draw_tracer_flag, "torpedo tracer light effect");

	TOK_M("reverse_mouse_x", optmap.mouse_xdir, "reverse mouse x direction");
	TOK_M("reverse_mouse_y", optmap.mouse_ydir, "reverse mouse y direction");

	TOK_N("num_asteroids", optmap.num_asteroids, "starting number of asteroids in the game");
	TOK_N("win_height", optmap.windowy, "window height");
	TOK_N("win_width", optmap.windowx, "window width");
	TOK_N("win_xpos", optmap.window_xpos, "window x position offset");
	TOK_N("win_ypos", optmap.window_ypos, "window y position offset");
	return;
}

/* convert string tokens read from the rcfile
 * into characters for the keymap. This is sort of the inverse
 * of key_char2string()
 */
static char key_string2char(const char *buf)
{
	// grab non-pritable chararacters
	if (strcmp(buf, "tab")       == 0)
		return '\t';
	if (strcmp(buf, "enter")     == 0)
		return '\n';
	if (strcmp(buf, "space")     == 0)
		return ' ';
	if (strcmp(buf, "backspace") == 0)
		return '\b';
	if (strcmp(buf, "esc")       == 0)
		return 27; // '\e'
	if (strcmp(buf, "delete")    == 0)
		return 127;

	/* Note that the escaped characters, like \#, will come through as
	themselves, so I don't have to handle those specially.
	Now, if it is anything else, just go ahead and map it. */
	return *buf;
}

/* convert #defined keyboard characters into strings for
 * printing of -freshrc
 */
char *key_char2str(char ch, char *buf, size_t sz)
{
	buf[sz-1] = '\0';
	switch (ch) {
		// These non-printable chararacters should be mappable
		case '\t':
			return strncpy(buf, "tab", sz - 1);
		case '\n':
			return strncpy(buf, "enter", sz - 1);
		case ' ':
			return strncpy(buf, "space", sz - 1);
		case '\b':
			return strncpy(buf, "backspace", sz - 1);
		case 27:
			return strncpy(buf, "esc", sz - 1);
		case 127:
			return strncpy(buf, "delete", sz - 1);
		case '#':
		case '=':
		case '\\':
			buf[0] = '\\';
			buf[1] = ch;
			buf[2] = '\0';
			return buf;
	}
	// Okay, if its anything else, just go ahead and map it
	buf[0] = ch;
	buf[1] = '\0';
	return buf;
}

static const char *key_flag2str(int f)
{
	static const char *a[] = {"off", "on"};
	return a[!!f];
}

static const char *key_mouse2str(double x)
{
	static const char *a[] = {"off", "on"};
	return a[x < 0];
}

char *strip_nl(char *expr)
{
	size_t p = strlen(expr);

	if (p >= 2 && expr[p - 2] == '\r' && expr[p - 1] == '\n')
		expr[strlen(expr) - 2] = '\0';
	else if (p >= 1 && (expr[p - 1] == '\n' || expr[p - 1] == '\r'))
		expr[p - 1] = '\0';

	return expr;
}

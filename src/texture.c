/* Asteroids3D - a first person game of blowing up asteroids
 * Copyright (C) 2000 Stuart Mark Pomerantz <smp [at] psc edu>
 * Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2004 - 2006
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
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asteroids3D.h"

typedef struct ppm_file {
	FILE *fptr;
	long flen, i_off;
	void *image, *base, *uptr;
	int dim[2];
} PPM_file;

static char *add_alpha_channel_to_RGB_image(const unsigned char *, int, int);
static void average_RGBA_alpha_and_floor(unsigned char *, int, int, int);
static int ppm_open(const char *, PPM_file *);
static void ppm_close(PPM_file *);
static void rd_ast(const char *, int);
static void rd_neb(const char *, int);
static void rd_torp(const char *);

// Nebula billboard texture object list, and nebula display list
int nebula_dl[NUM_NEBULA_TEX];
unsigned int nebula_tex[NUM_NEBULA_TEX];

//-----------------------------------------------------------------------------
void init_textures(void)
{
	printf("Reading asteroid textures\n");
	rd_ast("ast_tex0.ppm", 0);
	rd_ast("ast_tex1.ppm", 1);
	rd_ast("ast_tex2.ppm", 2);
	rd_ast("ast_tex3.ppm", 3);
	rd_ast("ast_tex4.ppm", 4);
	rd_ast("ast_tex5.ppm", 5);
	printf("Reading nebula textures\n");
	rd_neb("nebula_tex0.ppm", 0);
	rd_neb("nebula_tex1.ppm", 1);
	rd_neb("nebula_tex2.ppm", 2);
	rd_neb("nebula_tex3.ppm", 3);
	printf("Reading torpedo texture\n");
	rd_torp("torp_tex0.ppm");

	/* now, bind to the special texture 0 to turn off texturing intially.
	 */
	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}

static char *add_alpha_channel_to_RGB_image(const unsigned char *image,
    int x, int y)
{
	char *alpha_image = malloc(x * y * 4);
	int i, idx = 0;

	for (i = 0; i < x * y * 4; i += 4) {
		alpha_image[i]   = image[idx++];
		alpha_image[i+1] = image[idx++];
		alpha_image[i+2] = image[idx++];
		alpha_image[i+3] = 255;
	}

	return alpha_image;
}

/* this guy will average the rgb intensities of a pixel
 * make the alpha value for that pixel that average.
 * this has the effect of making brighter pixels more opaque than
 * darker ones.
 * If the average is below the floor value, then the pixel alpha
 * is set to 0
 */
static void average_RGBA_alpha_and_floor(unsigned char *image,
    int x, int y, int flr)
{
	int ir, ig, ib, ia, i, new_alpha;

	for (i = 0; i < x * y * 4; i += 4) {
		ir = image[i];
		ig = image[i+1];
		ib = image[i+2];
		ia = image[i+3];

		new_alpha = (ir + ig + ib) / 3;
		if (new_alpha < flr)
			new_alpha = 0;

		image[i+3] = new_alpha;
	}

	return;
}

static int ppm_open(const char *name, PPM_file *data)
{
	char buf[64];
	if ((data->fptr = fopen(name, "rb")) == NULL) {
		fprintf(stderr, "Could not open %s: %s\n",
		        name, strerror(errno));
		return -errno;
	}

	if (fseek(data->fptr, 0, SEEK_END) < 0) {
		fprintf(stderr, "Could not fseek() in %s: %s\n",
		        name, strerror(errno));
		return -errno;
	}

	data->flen = ftell(data->fptr);
	fseek(data->fptr, 0, SEEK_SET);
	fgets(buf, sizeof(buf), data->fptr);
	if (strncmp(buf, "P6", 2) != 0) {
		fprintf(stderr, "Not a binary portable-pixelmap (PPM-P6) file");
		fclose(data->fptr);
		return -errno;
	}

	while (fgets(buf, sizeof(buf), data->fptr) != NULL)
		if (*buf != '#')
			break;

	sscanf(buf, "%d %d", &data->dim[0], &data->dim[1]);
	if (data->dim[0] < 1 || data->dim[1] < 1)
		return 0;
	fgets(buf, sizeof(buf), data->fptr); // color thingie
	data->i_off = ftell(data->fptr);

	if ((data->base = mmap(NULL, data->flen, PROT_READ, MAP_SHARED,
	    fileno(data->fptr), 0)) == NULL) {
		fprintf(stderr, "Could not mmap() \"%s\": %s\n",
		        name, strerror(errno));
		fclose(data->fptr);
		return -errno;
	}

	data->image = data->base + data->i_off;
	return 1;
}

static void ppm_close(PPM_file *data)
{
	munmap(data->base, data->flen);
	fclose(data->fptr);
	return;
}

static void rd_ast(const char *file, int spot)
{
	PPM_file data;

	if (ppm_open(file, &data) < 0)
		exit(EXIT_FAILURE);
	glGenTextures(1, &ast_tex[spot]);
	glBindTexture(GL_TEXTURE_2D, ast_tex[spot]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.dim[0], data.dim[1], 0,
	             GL_RGB, GL_UNSIGNED_BYTE, data.image);
	ppm_close(&data);

	printf("  [%02d] %s (%ld bytes, %dx%d)\n",
	       spot, file, data.flen, data.dim[0], data.dim[1]);
	return;
}

static void rd_neb(const char *file, int spot)
{
	PPM_file data;

	if (ppm_open(file, &data) < 0)
		exit(EXIT_FAILURE);
	glGenTextures(1, &nebula_tex[spot]);
	glBindTexture(GL_TEXTURE_2D, nebula_tex[spot]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.dim[0], data.dim[1], 0,
	             GL_RGB, GL_UNSIGNED_BYTE, data.image);
	ppm_close(&data);

	printf("  [%02d] %s (%ld bytes, %dx%d)\n",
	       spot, file, data.flen, data.dim[0], data.dim[1]);
	return;
}

static void rd_torp(const char *file)
{
	PPM_file data;

	if (ppm_open(file, &data) < 0)
		exit(EXIT_FAILURE);
	glGenTextures(1, &torp_tex);
	glBindTexture(GL_TEXTURE_2D, torp_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	data.uptr = add_alpha_channel_to_RGB_image(data.image, data.dim[0], data.dim[1]);
	average_RGBA_alpha_and_floor(data.uptr, data.dim[0], data.dim[1], 90);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.dim[0], data.dim[1], 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, data.uptr);
	free(data.uptr);
	ppm_close(&data);
	return;
}

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

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "asteroids3D.h"

// Size of the sound_buffer in, of course, bytes.
#define MAX_SOUND_BUF_BYTES 32768

typedef struct sound {
	void *data;
	unsigned int size;
} Sound;

static void init_oss_sound_device(void);
static void rd_sound(const char *, Sound *);
static void read_asteroid_sounds(void);
static void queue_sound(const Sound *);

// OSS/Free audio device file descriptor
static int audio_device_fd = -1;

// My sound buffer (doing this so sounds can be interrupted)
static char sound_buffer[MAX_SOUND_BUF_BYTES];
static int sound_buffer_start = 0, sound_buffer_end = 0;

// Sounds
static Sound ast_snd[NUM_AST_TYPES]; // asteroid sounds
static Sound ta_snd;                 // target aquired sound
static Sound tf_snd;                 // torpedo fired sound
static Sound coll_snd;               // shield collision sound

//-----------------------------------------------------------------------------
void init_sound(void)
{
	init_oss_sound_device();
	read_asteroid_sounds();
	return;
}

static void rd_sound(const char *file, Sound *sp)
{
	struct stat sb;
	int fd;

	if ((fd = open(file, O_RDONLY)) < 0)
		goto out;
	if (fstat(fd, &sb) < 0)
		goto out;

	sp->size = sb.st_size;
	sp->data = mmap(NULL, sp->size, PROT_READ, MAP_SHARED, fd, 0);
	if (sp->data == reinterpret_cast(void *, -1))
		goto out;

	close(fd);
	printf("  %s (%u bytes)\n", file, sp->size);
	return;

 out:
	fprintf(stderr, "error in rd_sound(\"%s\"): %s\n", file, strerror(errno));
	close(fd);
	return;
}

static void init_oss_sound_device(void)
{
	unsigned int format   = AFMT_U8;
	unsigned int channels = 1;
	unsigned int speed    = 8000;
	unsigned int frag_size;

	audio_device_fd = open("/dev/dsp", O_WRONLY | O_NONBLOCK);

	/* 0x7fff0008 is a magic OSS/Free number
	 *   7fff => no fragment limit
	 *   0008 => fragment size of 2^8 bytes
	 */
	frag_size = 0x7fff0008;
	ioctl(audio_device_fd, SNDCTL_DSP_SETFRAGMENT, &frag_size);
	ioctl(audio_device_fd, SNDCTL_DSP_SETFMT, &format);
	ioctl(audio_device_fd, SNDCTL_DSP_CHANNELS, &channels);
	ioctl(audio_device_fd, SNDCTL_DSP_SPEED, &speed);
	return;
}

static void read_asteroid_sounds(void)
{
	/* Note that if there are any sounds which are greater than
	MAX_SOUND_BUF_BYTES then there will be a memory leak / core dump / crash in
	queue_sound() when it does the memcpy. */

	printf("Reading sounds\n");
	rd_sound("ast_snd0.ub", &ast_snd[0]);
	rd_sound("ast_snd1.ub", &ast_snd[1]);
	rd_sound("ast_snd2.ub", &ast_snd[2]);
	rd_sound("ast_snd3.ub", &ast_snd[3]);
	rd_sound("ast_snd4.ub", &ast_snd[4]);
	rd_sound("ast_snd5.ub", &ast_snd[5]);
	rd_sound("ta_snd.ub", &ta_snd);
	rd_sound("tf_snd.ub", &tf_snd);
	rd_sound("coll_snd.ub", &coll_snd);
	return;
}

void play_sound(int type, int subtype)
{
	if (!optmap.play_sound_flag || audio_device_fd < 0)
		return;

	switch (type) {
		case SOUND_ASTEROID_HIT:
			if (subtype <= 5)
				queue_sound(&ast_snd[subtype]);
			else
				fprintf(stderr, "number of sounds != number of asteroid types!\n");
			break;

		case SOUND_TARGET_AQUIRED:
			queue_sound(&ta_snd);
			break;

		case SOUND_COLLISION_WARN:
			break;

		case SOUND_COLLISION:
			queue_sound(&coll_snd);
			break;

		case SOUND_TORP_FIRED:
			queue_sound(&tf_snd);
			break;

		default:
			fprintf(stderr, "no sound of this type!!\n");
			break;
	}
	return;
}

static void queue_sound(const Sound *s)
{
	/* Note that this memcpy is trusting the program not to feed it a sound of
	size greater than MAX_SOUND_BUF_BYTES. */
	memcpy(sound_buffer, s->data, s->size);
	sound_buffer_start = 0;
	sound_buffer_end   = s->size;
	return;
}

void process_sound(void)
{
#define BPT 256
	char buf[BPT];
	int k;

	/* a fragment (set in init_oss_sound_device() ) is 256 bytes
	 * there are 8000 bytes/second of sound
	 * and process_sound is called tps times per second.
	 *
	 * at a minimum I need to write out 1 full fragement
	 * and and I want some wiggle room to avoid an under-run, so
	 *
	 * 8000 bytes/60 ticks = 133 bytes/tick. round that to 256 bytes/tick.
	 * so, write one full fragment per tick.
	 */

	if (sound_buffer_end == 0) /* nothing to play, so return */
		return;

	/* so, its time to play a sound, what's it to you ?
	 *
	 * if (I know I have a full number of byte_per_tick bytes in the buffer)
	 * then
	 * copy byte_per_tick bytes into buf
	 * increment the sound_buffer_start counter so I pick up where I left off next time through
	 * write the buf to the sound card
	 * return;
	 *
	 * on the other hand,
	 * there must be either exactly byte_per_tick bytes in the buffer
	 * or less than byte_per_tick bytes in the buffer.
	 * so, figure out how many and put it in k
	 * copy those k bytes into buf
	 * set sound_buffer_end = sound_buffer_start = 0 because we're done and sound_buffer_end = 0 tell us so.
	 * write the buf to the sound card
	 * return
	 */

	if (sound_buffer_end - sound_buffer_start > BPT) {
		memcpy(buf, &sound_buffer[sound_buffer_start], BPT);
		sound_buffer_start += BPT;
		write(audio_device_fd, &buf, BPT);
		return;
	}

	k = sound_buffer_end - sound_buffer_start;
	memcpy(buf, &sound_buffer[sound_buffer_start], k);
	sound_buffer_end = sound_buffer_start = 0;
	write(audio_device_fd, &buf, k);
	return;
#undef BPT
}

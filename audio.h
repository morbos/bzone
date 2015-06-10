/*
 * audio.c: Bzone audio support
 *
 * Copyright 2015 Hedley Rainnie
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Header$
 */
#ifndef __AUDIO_H__
#define __AUDIO_H__

#define SILENCE    (0)
#define EXPLODE_LO (1 << 1)
#define EXPLODE_HI (1 << 2)
#define SHELL_LO   (1 << 3)
#define SHELL_HI   (1 << 4)
#define MOTOR_LO   (1 << 5)
#define MOTOR_HI   (1 << 6)
#define SMART      (1 << 7)

#define AUDACITY_WAV_HDR_OFF (0x2c / 2) // divide by 2 so its in samples (16bit)
#define CHECK(x) \
    if(g_smask & bit(x)) { \
        worklist[idx++] = sounds[x].ptr[sounds[x].idx]; \
	sounds[x].idx++; \
	if(sounds[x].idx == sounds[x].len) { \
	    sounds[x].idx = AUDACITY_WAV_HDR_OFF; \
            if(sounds[x].oneshot) { \
                g_smask &= ~bit(x); \
	    } \
	} \
    }

void add_sounds(void);
void enable_sound(u32);
void disable_sound(u32);
s16 get_sample(void);
void start_sample(u32);
#endif // __AUDIO_H__

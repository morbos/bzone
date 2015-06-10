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
#include "stm32f4_discovery.h"
#include "memory.h"
#include "data.h"
#include "audio.h"

const s16 explode_lo[]={
#include "expLo.hex"
};
const s16 explode_hi[]={
#include "expHi.hex"
};
const s16 shell_lo[]={
#include "shellLo.hex"
};
const s16 shell_hi[]={
#include "shellHi.hex"
};
const s16 motor_lo[]={
#include "motorLo.hex"
};
const s16 motor_hi[]={
#include "motorHi.hex"
};
// Pokey sounds:
// {1 radar,  2 bump,  4 blocked,  8 extra life,  0x10 enemy appears,  0x20 saucer hit,  0x40 short saucer sound,  0x80 high score melody}
const s16 radar[]={
#include "radar.hex"
};
const s16 bump[]={
#include "bump.hex"
};
const s16 blocked[]={
#include "blocked.hex"
};
const s16 life[]={
#include "life.hex"
};
const s16 enemy[]={
#include "start.hex"
};
const s16 saucer_hit[]={
#include "saucerhit.hex"
};
const s16 saucer[]={
#include "saucer.hex"
};
const s16 high_score[]={
#include "hiscore.hex"
};
const s16 smart[]={
#include "smart.hex"
};

// Add this to the mix

typedef struct _sound_rec {
    const s16 *ptr;
    u32 len; // Len of the sample after processing
    u32 idx; // Curr index into the sample. 
    u32 oneshot; // When 1 don't repeat
} sound_rec;
sound_rec sounds[]={
    { 0, 0, 0, 0 }, 
    { explode_lo, 0, 0, 1}, 
    { explode_hi, 0, 0, 1 }, 
    { shell_lo, 0, 0, 1 }, 
    { shell_hi, 0, 0, 1 }, 
    { motor_lo, 0, 0, 0 }, 
    { motor_hi, 0, 0, 0 }, 
    { smart, 0, 0, 1}, 
    { radar, 0, 0, 1 }, 
    { bump, 0, 0, 1}, 
    { blocked, 0, 0, 1}, 
    { life, 0, 0, 1}, 
    { enemy, 0, 0, 1}, 
    { saucer_hit, 0, 0, 1}, 
    { saucer, 0, 0, 1}, 
    { high_score, 0, 0, 1}
};
void
add_sounds()
{
    int i;
    u32 len;
    for(i=0;i < sizeof(sounds)/sizeof(sound_rec);i++) {
	u32 p = (u32)sounds[i].ptr;
	if(p) {
	    len = REG32(p+0x28);
	    len /= 2; // Len in 16bit samples
	    len += AUDACITY_WAV_HDR_OFF;
	    sounds[i].len = len;
	    sounds[i].idx = AUDACITY_WAV_HDR_OFF;
	}
    }
}
void
enable_sound(u32 mask)
{
    g_aud_smask |= mask;
}
void
disable_sound(u32 mask)
{
    g_aud_smask &= ~mask;
}
void
start_sample(u32 mask)
{
    // {1 radar,  2 bump,  4 blocked,  8 extra life,  0x10 enemy appears,  0x20 saucer hit,  0x40 short saucer sound,  0x80 high score melody}
    if(mask) {
	g_aud_smask |= (mask << 8);
    }
}
s16
get_sample()
{
    u32 i;
    s16 worklist[16];
    u16 idx=0;
    s16 mixer(s16 *dat, u16 factor, u32 n);
    if(0 == g_aud_enable) {
	return 0;
    }
    for(i=1;i < sizeof(sounds)/sizeof(sound_rec);i++) {
	if(g_aud_smask & bit(i)) {
	    worklist[idx++] = sounds[i].ptr[sounds[i].idx];
	    sounds[i].idx++;
	    if(sounds[i].idx == sounds[i].len) {
		sounds[i].idx = AUDACITY_WAV_HDR_OFF;
		if(sounds[i].oneshot) {
		    g_aud_smask &= ~bit(i);
		}
	    }
	}
    }
    if(idx == 1) {
	return worklist[0];
    } else {
	return mixer(worklist, 32768 / idx, idx<<1);
    }
}

/*
 * no_interface.c: null display for Atari Vector game simulator
 *
 * Copyright 1993, 1996 Eric Smith
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
#include "stm32f4xx.h"
#include "memory.h"
#include "display.h"
#include "game.h"
#include "board.h"
#include "data.h"

s32
iabs(s32 x)
{
    if(x < 0) {
	return -x;
    } else {
	return x;
    }
}
void
plot(s32 x, s32 y, s32 z)
{
    if(x > g_vctr_maxx) {
	GPIOC->BSRRH = 0xf;
	return;
    }
    if(x < 0) {
	GPIOC->BSRRH = 0xf;
	return;
    }
    if(y > g_vctr_maxy) {
	GPIOC->BSRRH = 0xf;
	return;
    }
    if(y < 0) {
	GPIOC->BSRRH = 0xf;
	return;
    }
    if(z) {
	GPIOC->BSRRL = 0xf;
    }
    // scale by 3x
    // dac_out((x << 1) + x, (y << 1) + y, z);
    // Looks on the scope as if X is wrapping. Scale it less. we do Y also for symmetry
    dac_out((x << 1)+x, (y << 1)+y, z);
}
void 
draw_line(s32 x1, s32 y1, s32 x2, s32 y2, s32 c, s32 z)
{
    s32 dx=iabs(x2-x1);
    s32 dy=iabs(y2-y1);
    s32 sx;
    s32 sy;
    s32 err;
    s32 e2;
    x1 += 256;
    x2 += 256;
    y1 += 256;
    y2 += 256;

    write_gpio(z);
    if(x1 < x2) {
	sx = 1;
    } else {
	sx = -1;
    }
    if(y1 < y2) {
	sy = 1;
    } else {
	sy = -1;
    }
    err = dx-dy;
    for(;;) {
	plot(x1, y1, z);
	if(x1 == x2 && y1 == y2) {
	    goto done;
	}
	e2 = 2*err;
	if(e2 > -dy) {
	    err -= dy;
	    x1 += sx;
	}
	if(x1 == x2 && y1 == y2) {
	    plot(x1, y1, z);
	    goto done;
	}
	if(e2 < dx) {
	    err += dx;
	    y1 += sy;
	}
    }
 done:
    write_gpio(0);
}

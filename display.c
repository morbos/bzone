/*
 * display.c: Atari DVG and AVG simulators
 *
 * Copyright 1991, 1992, 1996 Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/display.c,v 1.10 1997/05/17 22:25:36 eric Exp $
 */

#include <stdio.h>

#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "memory.h"
#include "display.h"
#include "game.h"
#include "data.h"

#define MAXSTACK 4

#define VCTR 0
#define HALT 1
#define SVEC 2
#define STAT 3
#define CNTR 4
#define JSRL 5
#define RTSL 6
#define JMPL 7
#define SCAL 8

#define DVCTR 0x01
#define DLABS 0x0a
#define DHALT 0x0b
#define DJSRL 0x0c
#define DRTSL 0x0d
#define DJMPL 0x0e
#define DSVEC 0x0f

#define twos_comp_val(num, bits) ((num&(1<<(bits-1)))?(num|~((1<<bits)-1)):(num&((1<<bits)-1)))

#define map_addr(n) (((n)<<1)+g_vctr_vector_mem_offset)

#define max(x,y) (((x)>(y))?(x):(y))

void 
avg_vector_timer(s32 deltax, s32 deltay)
{
#if 0
    deltax = labs(deltax);
    deltay = labs(deltay);
    g_vctr_vg_done_cyc += max(deltax, deltay) >> 17;
#endif
}
static void 
dvg_vector_timer(s32 scale)
{
    g_vctr_vg_done_cyc += 4 << scale;
}
static void 
dvg_draw_vector_list(void) 
{
    static s32 pc;
    static s32 sp;
    static s32 stack[MAXSTACK];
    
    static s32 scale;
    static s32 statz;
    
    static s32 currentx;
    static s32 currenty;
    
    s32 done = 0;
    
    s32 firstwd, secondwd;
    s32 opcode;
    
    s32 x, y;
    s32 z, temp;
    s32 a;
    
    s32 oldx, oldy;
    s32 deltax, deltay;
    pc = 0;
    sp = 0;
    scale = 0;
    statz = 0;
    if(g_vctr_portrait) {
	currentx = (1023) * 8192;
	currenty = (512) * 8192;
    } else {
	currentx = (512) * 8192;
	currenty = (1023) * 8192;
    }
    while(!done) {
	g_vctr_vg_done_cyc += 8;
	firstwd = memrdwd(map_addr(pc), 0, 0);
	opcode = firstwd >> 12;
	pc++;
	if((opcode >= 0 /* DVCTR */) && (opcode <= DLABS)) {
	    secondwd = memrdwd(map_addr(pc), 0, 0);
	    pc++;
	}
	switch(opcode) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	    y = firstwd & 0x03ff;
	    if(firstwd & 0x0400) {
		y = -y;
	    }
	    x = secondwd & 0x03ff;
	    if(secondwd & 0x400) {
		x = -x;
	    }
	    z = secondwd >> 12;
	    oldx = currentx;
	    oldy = currenty;
	    temp = (scale + opcode) & 0x0f;
	    if(temp > 9) {
		temp = -1;
	    }
	    deltax = (x << 21) >> (30 - temp);
	    deltay = (y << 21) >> (30 - temp);
	    currentx += deltax;
	    currenty -= deltay;
	    dvg_vector_timer(temp);
	    draw_line(oldx, oldy, currentx, currenty, 7, z);
	    break;
	case DLABS:
	    x = twos_comp_val(secondwd, 12);
	    y = twos_comp_val(firstwd, 12);
	    scale = secondwd >> 12;
	    currentx = x;
	    currenty = (896 - y);
	    break;
	case DHALT:
	    done = 1;
	    break;
	case DJSRL:
	    a = firstwd & 0x0fff;
	    stack[sp] = pc;
	    if(sp == (MAXSTACK - 1)) {
		done = 1;
		sp = 0;
	    } else {
		sp++;
	    }
	    pc = a;
	    break;
	case DRTSL:
	    if(sp == 0) {
		done = 1;
		sp = MAXSTACK - 1;
	    } else {
		sp--;
	    }
	    pc = stack[sp];
	    break;
	case DJMPL:
	    a = firstwd & 0x0fff;
	    pc = a;
	    break;
	case DSVEC:
	    y = firstwd & 0x0300;
	    if(firstwd & 0x0400) {
		y = -y;
	    }
	    x = (firstwd & 0x03) << 8;
	    if(firstwd & 0x04) {
		x = -x;
	    }
	    z = (firstwd >> 4) & 0x0f;
	    temp = 2 + ((firstwd >> 2) & 0x02) + ((firstwd >> 11) & 0x01);
	    oldx = currentx; oldy = currenty;
	    temp = (scale + temp) & 0x0f;
	    if(temp > 9) {
		temp = -1;
	    }
	    deltax = (x << 21) >> (30 - temp);
	    deltay = (y << 21) >> (30 - temp);
	    currentx += deltax;
	    currenty -= deltay;
	    dvg_vector_timer(temp);
	    draw_line(oldx, oldy, currentx, currenty, 7, z);
	    break;
	default:
	    done = 1;
	}
    }
}
void 
avg_draw_vector_list(void)
{
    static int pc;
    static int sp;
    static int stack [MAXSTACK];
    
    static long scale;
    static int statz;
    static int color;
    
    static long currentx;
    static long currenty;
    
    int done = 0;
    
    int firstwd, secondwd;
    int opcode;
    
    int x, y, z, b, l, d, a;
    
    long oldx, oldy;
    long deltax, deltay;
    
    pc = 0;
    sp = 0;
    scale = 16384;
    statz = 0;
    color = 0;
    if(g_vctr_portrait) {
	currentx = 384 * 8192;
	currenty = 512 * 8192;
    } else {
	currentx = 512 * 8192;
	currenty = 384 * 8192;
    }
    
    firstwd = memrdwd(map_addr(pc), 0, 0);
    secondwd = memrdwd(map_addr(pc+1), 0, 0);
    if((firstwd == 0) && (secondwd == 0))	{
	for(;;);
    }
    while(!done) {
	g_vctr_vg_done_cyc += 8;
	firstwd = memrdwd(map_addr(pc), 0, 0);
	opcode = firstwd >> 13;
	pc++;
	if(opcode == VCTR) {
	    secondwd = memrdwd(map_addr(pc), 0, 0);
	    pc++;
	}
	if((opcode == STAT) && ((firstwd & 0x1000) != 0)) {
	    opcode = SCAL;
	}
	switch(opcode) {
	case VCTR:
	    x = twos_comp_val(secondwd,13);
	    y = twos_comp_val(firstwd,13);
	    z = 2 * (secondwd >> 13);
	    if(z == 2) {
		z = statz;
	    }
	    oldx = currentx; oldy = currenty;
	    deltax = x * scale; deltay = y * scale;
	    currentx += deltax;
	    currenty -= deltay;
	    avg_vector_timer(deltax, deltay);
	    draw_line(oldx>>13, oldy>>13, currentx>>13, currenty>>13, color, z);
	    break;
	case SVEC:
	    x = twos_comp_val(firstwd, 5) << 1;
	    y = twos_comp_val(firstwd >> 8, 5) << 1;
	    z = 2 * ((firstwd >> 5) & 7);
	    if(z == 2) {
		z = statz;
	    }
	    oldx = currentx; oldy = currenty;
	    deltax = x * scale; deltay = y * scale;
	    currentx += deltax;
	    currenty -= deltay;
//	    avg_vector_timer(labs(deltax), labs(deltay));
	    avg_vector_timer(deltax, deltay);
	    draw_line(oldx>>13, oldy>>13, currentx>>13, currenty>>13, color, z);
	    break;
	case STAT:
	    color = firstwd & 0x0f;
	    statz = (firstwd >> 4) & 0x0f;
	    /* should do e, h, i flags here! */
	    break;
	case SCAL:
	    b = (firstwd >> 8) & 0x07;
	    l = firstwd & 0xff;
	    scale = (16384 - (l << 6)) >> b;
	    /* scale = (1.0-(l/256.0)) * (2.0 / (1 << b)); */
	    break;
	case CNTR:
	    d = firstwd & 0xff;
	    if(g_vctr_portrait) {
		currentx = 384 * 8192;
		currenty = 512 * 8192;
	    } else {
		currentx = 512 * 8192;
		currenty = 384 * 8192;
	    }
	    break;
	case RTSL:
	    if(sp == 0) {
		for(;;);
	    } else {
		sp--;
	    }
	    pc = stack[sp];
	    break;
	case HALT:
	    done = 1;
	    break;
	case JMPL:
	    a = firstwd & 0x1fff;
	    pc = a;
	    break;
	case JSRL:
	    a = firstwd & 0x1fff;
	    stack[sp] = pc;
	    if(sp == (MAXSTACK - 1)) {
		for(;;);
	    } else {
		sp++;
	    }
	    pc = a;
	    break;
	default:
	    for(;;);
	}
    }
}

s32 
vg_done(u32 cyc)
{
    if(g_vctr_vg_busy && (cyc > g_vctr_vg_done_cyc)) {
	g_vctr_vg_busy = 0;
    }
    return !g_vctr_vg_busy;
}

void 
vg_go(u32 cyc)
{
    g_vctr_vg_busy = 1;
    g_vctr_vg_done_cyc = cyc + 8;
//    dvg_draw_vector_list();
    avg_draw_vector_list();
}

void 
vg_reset(u32 cyc)
{
    g_vctr_vg_busy = 0;
}


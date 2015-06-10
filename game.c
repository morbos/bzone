/*
 * game.c: Atari Vector game definitions & setup functions
 *
 * Copyright 1991, 1992, 1993, 1996 Hedley Rainnie and Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/game.c,v 1.19 1997/05/17 22:25:36 eric Exp $
 */

#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "display.h"
#include "memory.h"
#include "game.h"
#include "data.h"

void 
setup_game(void)
{
    g_vctr_vector_mem_offset = 0x2000;
    g_sys_optionreg[0] = (~0xE8) & 0xff; // Inverted! 0xE8 -> English, bonus 15k&100k, missile @10k, 5 tanks
}

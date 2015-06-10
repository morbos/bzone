/*
 * pokey.h: POKEY chip simulation functions
 *
 * Copyright 1991, 1992, 1993, Hedley Rainnie and Eric Smith
 * Copyright 1996 Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/pokey.h,v 1.2 1997/05/17 22:25:36 eric Exp $
 */

#define MAX_POKEY 1  /* POKEYs are numbered 0 .. MAX_POKEY - 1 */

u8 pokey_read (int pokeynum, int reg, int PC, unsigned long cyc);
void pokey_write (int pokeynum, int reg, u8 val, int PC, unsigned long cyc);

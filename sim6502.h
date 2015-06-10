/*
 * sim6502.h: 6502 simulator for Atari Vector game simulator
 *
 * Copyright 1991-1993 Hedley Rainnie, Doug Neubauer, and Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/sim6502.h,v 1.11 1997/05/17 22:25:36 eric Exp $
 */

#define WRAP_CYC_COUNT 1000000000

void sim_6502(void);

#define N_BIT 0x80
#define V_BIT 0x40
#define B_BIT 0x10
#define D_BIT 0x08
#define I_BIT 0x04
#define Z_BIT 0x02
#define C_BIT 0x01

/*
 * board.c: Bzone board support
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
#ifndef __BOARD_H__
#define __BOARD_H__
#define START1_PIN	bit(5)
#define FIRE_PIN	bit(4)
#define LT_FWD_PIN	bit(3)
#define LT_REV_PIN	bit(2)
#define RT_FWD_PIN	bit(1)
#define RT_REV_PIN	bit(0)

#define START1_LED_PIN	bit(4)

void init_board(void);
void dac_out(int ch1, int ch2, int z);

#endif // __BOARD_H__

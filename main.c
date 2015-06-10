#define MAIN
/*
 * main.c: Atari Vector game simulator
 *
 * Copyright 1991, 1992, 1993, 1996 Hedley Rainnie, Doug Neubauer, and Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/main.c,v 1.12 1997/05/17 22:25:36 eric Exp $
 */

#ifndef __GNUC__
#define inline
#endif

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "memory.h"
#include "game.h"
#include "display.h"
#include "sim6502.h"
#include "data.h"

RCC_ClocksTypeDef RCC_Clocks;

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void 
assert_failed(uint8_t* file, uint32_t line)
{ 
    for(;;);
}
int 
main()
{
    add_sounds();

    init_board();

    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);

    setup_game ();
    g_cpu_save_PC = (memrd(0xfffd,0,0) << 8) | memrd(0xfffc,0,0);
    g_cpu_save_A = 0;
    g_cpu_save_X = 0;
    g_cpu_save_Y = 0;
    g_cpu_save_flags = 0;
    g_cpu_save_totcycles = 0;
    g_cpu_irq_cycle = 6144;

    sim_6502 ();

}

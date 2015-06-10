/*
 * memory.c: memory and I/O functions for Atari Vector game simulator
 *
 * Copyright 1991, 1993, 1996, 2014, 2015 Hedley Rainnie and Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/memory.c,v 1.23 1997/05/17 22:25:36 eric Exp $
 */
#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "memory.h"
#include "display.h"
#include "game.h"
#include "misc.h"
#include "sim6502.h"
#include "audio.h"
#include "data.h"
#include "board.h"
#include "mathbox.h"

/*
 * This used to decrement the switch variable if it was non-zero, so that
 * they would automatically release.  This has been changed to increment
 * it if less than zero, so switches set by the debugger will release, but
 * to leave it alone if it is greater than zero, for keyboard handling.
 */
s32 
check_switch_decr(s32 *sw)
{
    if((*sw) < 0) {
	(*sw)++;
	if((*sw) == 0)
	    ;
    }
    return ((*sw) != 0);
}
u8 
MEMRD(u32 addr, s32 PC, u32 cyc)
{
    u8 tag;
    u8 result=0;
    
    if(!(tag=g_sys_mem[addr].tagr)) {
	return(g_sys_mem[addr].cell);
    }
    switch(tag & 0x3f) {
    case MEMORY:
	result = g_sys_mem[addr].cell;
	break;
    case MEMORY1:
	result = g_sys_sram[addr&0x3ff];
	break;
    case MEMORY_BB:
	for(;;);
    case VECRAM:
	result = g_sys_vram[addr&0xfff];
	break;
    case COININ:
	result =
	    ((! check_switch_decr(&g_sys_cslot_right))) |
	    ((! check_switch_decr(&g_sys_cslot_left)) << 1) |
	    ((! check_switch_decr(&g_sys_cslot_util)) << 2) |
	    ((! check_switch_decr(&g_sys_slam)) << 3) |
	    ((! g_sys_self_test) << 4) |
	    (1 << 5) | /* signature analysis */
	    (vg_done(cyc) << 6) |
	    /* clock toggles at 3 KHz */
	    ((cyc >> 1) & 0x80);
	
	break;
    case EAROMRD:
	result = 0;
	break;
    case OPTSW1:
	result = g_sys_optionreg[0];
	break;
    case OPTSW2:
	result = g_sys_optionreg[1];
	break;
    case OPT1_2BIT:
	result = 0xfc | ((g_sys_optionreg[0] >> (2 * (3 - (addr & 0x3)))) & 0x3);
	break;
    case ASTEROIDS_SW1:
	break;
    case ASTEROIDS_SW2:
	break;
    case POKEY1:
	result = pokey_read(0, addr & 0x0f, PC, cyc);
	break;
    case POKEY2:
	result = pokey_read (1, addr & 0x0f, PC, cyc);
	break;
    case POKEY3:
	result = pokey_read (2, addr & 0x0f, PC, cyc);
	break;
    case POKEY4:
	result = pokey_read (3, addr & 0x0f, PC, cyc);
	break;
    case BZ_SOUND:
	for(;;);
	break;
    case BZ_INPUTS:
	result = g_soc_curr_switch = read_gpio();
	break;
    case MBLO:
	result = mb_result & 0xff;
	break;
    case MBHI:
	result = (mb_result >> 8) & 0xff;
	break;
    case MBSTAT:
	result = 0x00;  /* always done! */
	break;
    case UNKNOWN:
	result = 0xff;
	for(;;);
	break;
    default:
	result = 0xff;
	for(;;);
	break;
    }
    if(tag & BREAKTAG) {
	;
    }
    return(result);
}
void 
MEMWR(u32 addr, s32 val, s32 PC, u32 cyc)
{
    u8                 tag;
    u32    aud_saucer_fire;
    s32            newbank;
    s32                  i;
    u8                temp;

   
    if(!(tag=g_sys_mem[addr].tagw)) {
	g_sys_mem[addr].cell = val;
    } else {
	switch(tag & 0x3f) {
	case MEMORY:
	    g_sys_mem[addr].cell = val;
	    break;
	case MEMORY1:
	    g_sys_sram[addr&0x3ff] = val;
	    break;
	case MEMORY_BB:
	    break;
	case VECRAM:
	    g_sys_vram[addr&0xfff] = val;
	    break;
	case COINOUT:
	    newbank = (val>>2) & 1;
	    g_sys_bank = newbank;
	    break;
	case INTACK:
	    g_cpu_irq_cycle = cyc + 6144;
	    break;
	case WDCLR:
	case EAROMCON:
	case EAROMWR:
	    /* none of these are implemented yet, but they're OK. */
	    break;
	case VGRST:
	    vg_reset(cyc);
	    break;
	case VGO:
	    g_vctr_vg_count++;
	    while(0 == g_soc_sixty_hz);
	    g_soc_sixty_hz = 0;
	    vg_go(cyc);
	    break;
	case DMACNT:
	    break;
	case COLORRAM:
	    break;
	case TEMP_OUTPUTS:
	    break;
	case ASTEROIDS_OUT:
	    break;
	case ASTEROIDS_EXP:
	    break;
	case ASTEROIDS_THUMP:
	    break;
	case ASTEROIDS_SND:
	    break;
	case ASTEROIDS_SND_RST:
	    break;
	case POKEY1:
	    pokey_write(0, addr & 0x0f, val, PC, cyc);
	    break;
	case POKEY2:
	    pokey_write(1, addr & 0x0f, val, PC, cyc);
	    break;
	case POKEY3:
	    pokey_write(2, addr & 0x0f, val, PC, cyc);
	    break;
	case POKEY4:
	    pokey_write(3, addr & 0x0f, val, PC, cyc);
	    break;
	case BZ_SOUND:
/*
	    BZ_SOUNDS[7]  motoren
	    BZ_SOUNDS[6]  start led
	    BZ_SOUNDS[5]  sound en
	    BZ_SOUNDS[4]  engine H/L
	    BZ_SOUNDS[3]  shell L/S
	    BZ_SOUNDS[2]  shell enabl
	    BZ_SOUNDS[1]  explo L/S
	    BZ_SOUNDS[0]  explo en
*/
	    if(val & bit(5)) {
		g_aud_enable = 1;
	    } else {
		g_aud_enable = 0;
	    }
	    if(val & bit(0)) {  // expl
		enable_sound((val & bit(1)) ? EXPLODE_HI : EXPLODE_LO);
	    }
	    if(val & bit(2)) {  // shell
		enable_sound((val & bit(3)) ? SHELL_HI : SHELL_LO);
	    }
	    if(val & bit(7)) {  // motor
		disable_sound(MOTOR_HI);
		disable_sound(MOTOR_LO);
		enable_sound((val & bit(4)) ? MOTOR_HI : MOTOR_LO);
	    }
	    write_gpio_leds((val & bit(6)) ? 1 : 0);
	    break;
	case MBSTART:
	    /* printf("@%04x MBSTART wr addr %04x val %02x\n", PC, addr & 0x1f, val); */
	    mb_go(addr & 0x1f, val);
	    break;
	case IGNWRT:
	    break;
	case ROMWRT:
	    break;
	case UNKNOWN:
	    for(;;);
	    break;
	default:
	    for(;;);
	    break;
	}
    }
}

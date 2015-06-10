/*
 * data.h: Global data defs
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
#ifndef _DATA_H_
#define _DATA_H_

#ifdef MAIN
#  define DECLARE(x,y,z) x y=(z)
#  define DECLAREBSS(x,y) x y
#else
#  define DECLARE(x,y,z) extern x y
#  define DECLAREBSS(x,y) extern x y
#endif

// Soc
DECLAREBSS(RCC_ClocksTypeDef, RCC_Clocks);
DECLAREBSS(volatile u32, g_soc_sixty_hz);
DECLAREBSS(volatile u32, g_soc_curr_switch);
// Codec/Audio
DECLAREBSS(DMA_Stream_TypeDef, *AUDIO_MAL_DMA_STREAM);
DECLARE(u32, g_codec_volume, 0xd0);
// Audio
DECLARE(s32, g_aud_explosion, -1);
DECLAREBSS(u32, g_aud_idx);
DECLAREBSS(u32, g_aud_ping_pong);
DECLAREBSS(u32, g_aud_smask);
DECLAREBSS(s16, g_aud_abuf[2][16]);
DECLAREBSS(u32, g_aud_curr_saucer);
DECLAREBSS(u32, g_aud_curr_exp);
DECLAREBSS(u32, g_aud_curr_thump);
DECLAREBSS(u32, g_aud_last_saucer);
DECLAREBSS(u32, g_aud_saucer_fire);
DECLAREBSS(u32, g_aud_enable);
// emulated cpu
//DECLARE(u32, g_cpu_irq_cycle_off, 10000); // Was 6144.. caused wierd NMI in an NMI problem
DECLARE(u32, g_cpu_irq_cycle_off, 6144);
DECLAREBSS(u8, g_cpu_save_A);
DECLAREBSS(u8, g_cpu_save_X);
DECLAREBSS(u8, g_cpu_save_Y);
DECLAREBSS(u8, g_cpu_save_flags);
DECLAREBSS(u16, g_cpu_save_PC);
DECLAREBSS(u8, g_cpu_SP);
DECLAREBSS(u8, g_cpu_save_totcycles);
DECLAREBSS(u32, g_cpu_cyc_wraps);
DECLAREBSS(u32, g_cpu_irq_cycle);
// Vector graphics
DECLARE(u32, g_vctr_delay_factor, 1);
DECLARE(u32, g_vctr_post_delay, 100);
DECLAREBSS(u32, g_vctr_portrait);
DECLAREBSS(u32, g_vctr_vg_busy);
DECLAREBSS(u32, g_vctr_vg_done_cyc); /* cycle after which VG will be done */
DECLAREBSS(u32, g_vctr_vector_mem_offset);
DECLAREBSS(u32, g_vctr_vg_count);
DECLARE(u32, g_vctr_maxx, 1280);
DECLARE(u32, g_vctr_maxy, 1024);
// System vars
DECLAREBSS(s32, g_sys_breakflag);
DECLAREBSS(s32, g_sys_points);
DECLAREBSS(u8, g_sys_sram[0x400]); // ZP
DECLAREBSS(u8, g_sys_vram[0x1000]); // vector ram
DECLAREBSS(s32, g_sys_bank); /* RAM bank select */
DECLAREBSS(s32, g_sys_self_test);
/* input switch counters */
DECLAREBSS(s32, g_sys_cslot_left);
DECLAREBSS(s32, g_sys_cslot_right);
DECLAREBSS(s32, g_sys_cslot_util);
DECLAREBSS(s32, g_sys_slam);
DECLAREBSS(s32, g_sys_start1);
DECLAREBSS(s32, g_sys_start2);
DECLAREBSS(switch_rec, g_sys_switches[2]);
DECLAREBSS(u8, g_sys_optionreg[MAX_OPT_REG]);

extern elem g_sys_mem[];

#define	bit(x)   ((u32)(1UL << x))
#define REG32(x) ((*(volatile u32  *)(x)))

#endif // _DATA_H_

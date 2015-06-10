/*
 * sim6502.c: 6502 simulator for Atari Vector game simulator
 *
 * Copyright 1991, 1993, 1996 Hedley Rainnie, Doug Neubauer, and Eric Smith
 * Copyright 2015 Hedley Rainnie
 *
 * 6502 simulator by Hedley Rainnie, Doug Neubauer, and Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/sim6502.c,v 1.19 1997/05/17 22:25:36 eric Exp $
 */

#ifndef __GNUC__
#define inline
#endif

#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "memory.h"
#include "audio.h"
#include "game.h"
#include "misc.h"
#include "sim6502.h"
#include "macro6502.h"
#include "display.h"  /* needed so we can wrap vg_done_cyc and last_vgo_cyc */
#include "data.h"

void
xyzzy()
{
}
#if 0
u16 pc_ring[64];
u16 ring_idx = 0;
#else

#ifdef TRACEFIFO
struct _fifo {
    u16 PC;
    u16 SP;
    u8 A;
    u8 X;
    u8 Y;
    u8 flags;
};
struct _fifo fifo[8192];
unsigned short pcpos=0;
#endif

#endif

void 
dopush(u8 val, u16 PC)
{
  u16 addr;
  addr = g_cpu_SP + 0x100;
  g_cpu_SP--;
  g_cpu_SP &= 0xff;
  memwr (addr, val, PC, 0);
}
u8 
dopop(u16 PC)
{
  u16 addr;
  g_cpu_SP++;
  g_cpu_SP &= 0xff;
  addr = g_cpu_SP + 0x100;
  return (memrd(addr, PC, 0));
}
void 
sim_6502(void)
{
    s32 PC;
    s32 opcode;
    s32 addr;
    s32 A;
    s32 X;
    s32 Y;
    DECLARE_CC;
    u32 totcycles;

    A = g_cpu_save_A;
    X = g_cpu_save_X;
    Y = g_cpu_save_Y;
    byte_to_flags (g_cpu_save_flags);
    PC = g_cpu_save_PC;
    totcycles = g_cpu_save_totcycles;

test_interrupt:

#ifdef WRAP_CYC_COUNT
    if(totcycles > WRAP_CYC_COUNT) {
	if(g_cpu_irq_cycle >= (totcycles - WRAP_CYC_COUNT)) {
	    g_cpu_irq_cycle -= WRAP_CYC_COUNT;
	}
	if(g_vctr_vg_done_cyc >= (totcycles - WRAP_CYC_COUNT)) {
	    g_vctr_vg_done_cyc -= WRAP_CYC_COUNT;
	}
	totcycles -= WRAP_CYC_COUNT;
	g_cpu_cyc_wraps++;
    }
#endif

    if(totcycles > g_cpu_irq_cycle) {
	if(!g_sys_self_test) {
	    /* do NMI */
	    dopush(PC >> 8, PC);
	    dopush(PC & 0xff, PC);
	    dopush(flags_to_byte, PC);
	    SET_I;
	    PC = memrdwd(0xfffa, PC, totcycles);
	    totcycles += 7;
//          g_cpu_irq_cycle += 6144; // <<<<<< hkjr 03/30/14. NMI in an MMI occasionally with a # like this...
	    g_cpu_irq_cycle += g_cpu_irq_cycle_off;
	}
    }

    while(1) {
#if 0
	pc_ring[ring_idx++] = PC;
	ring_idx &= 0x3f;
#endif	
#ifdef TRACEFIFO
      fifo[pcpos].PC = PC;
      fifo[pcpos].A = A;
      fifo[pcpos].X = X;
      fifo[pcpos].Y = Y;
      fifo[pcpos].flags = flags_to_byte;
      fifo[pcpos].SP = g_cpu_SP;
      pcpos = (pcpos + 1) & 0x1fff;
#endif
	if(PC == 0x7985) { // pokey audio
	    start_sample(A);
	} else if(PC == 0x6a22) { // smart missile (pokey ch3&4)
	    enable_sound(SMART);
	}
	opcode = g_sys_mem[PC++].cell;
	switch(opcode) { 			/* execute opcode */
	case 0x69:  /* ADC */  EA_IMM    DO_ADC   C( 2)  break;
	case 0x6d:  /* ADC */  EA_ABS    DO_ADC   C( 3)  break;
	case 0x65:  /* ADC */  EA_ZP     DO_ADC   C( 4)  break;
	case 0x61:  /* ADC */  EA_IND_X  DO_ADC   C( 6)  break;
	case 0x71:  /* ADC */  EA_IND_Y  DO_ADC   C( 5)  break;
	case 0x75:  /* ADC */  EA_ZP_X   DO_ADC   C( 4)  break;
	case 0x7d:  /* ADC */  EA_ABS_X  DO_ADC   C( 4)  break;
	case 0x79:  /* ADC */  EA_ABS_Y  DO_ADC   C( 4)  break;
	    
	case 0x29:  /* AND */  EA_IMM    DO_AND   C( 2)  break;
	case 0x2d:  /* AND */  EA_ABS    DO_AND   C( 4)  break;
	case 0x25:  /* AND */  EA_ZP     DO_AND   C( 3)  break;
	case 0x21:  /* AND */  EA_IND_X  DO_AND   C( 6)  break;
	case 0x31:  /* AND */  EA_IND_Y  DO_AND   C( 5)  break;
	case 0x35:  /* AND */  EA_ZP_X   DO_AND   C( 4)  break;
	case 0x39:  /* AND */  EA_ABS_Y  DO_AND   C( 4)  break;
	case 0x3d:  /* AND */  EA_ABS_X  DO_AND   C( 4)  break;
	    
	case 0x0e:  /* ASL */  EA_ABS    DO_ASL   C( 6)  break;
	case 0x06:  /* ASL */  EA_ZP     DO_ASL   C( 5)  break;
	case 0x0a:  /* ASL */            DO_ASLA  C( 2)  break;
	case 0x16:  /* ASL */  EA_ZP_X   DO_ASL   C( 6)  break;
	case 0x1e:  /* ASL */  EA_ABS_X  DO_ASL   C( 7)  break;
	    
	case 0x90:  /* BCC */		 DO_BCC   C( 2)  goto test_interrupt;
	case 0xb0:  /* BCS */		 DO_BCS   C( 2)  goto test_interrupt;
	case 0xf0:  /* BEQ */		 DO_BEQ   C( 2)  goto test_interrupt;
	case 0x30:  /* BMI */		 DO_BMI   C( 2)  goto test_interrupt;
	case 0xd0:  /* BNE */		 DO_BNE   C( 2)  goto test_interrupt;
	case 0x10:  /* BPL */		 DO_BPL   C( 2)  goto test_interrupt;
	case 0x50:  /* BVC */		 DO_BVC   C( 2)  goto test_interrupt;
	case 0x70:  /* BVS */		 DO_BVS   C( 2)  goto test_interrupt;
	    
	case 0x2c:  /* BIT */  EA_ABS    DO_BIT   C( 4)  break;
	case 0x24:  /* BIT */  EA_ZP     DO_BIT   C( 3)  break;
	    
#if 1
	case 0x00:  /* BRK */            DO_BRK   C( 7)  break;
#endif
	    
	case 0x18:  /* CLC */            DO_CLC   C( 2)  break;
	case 0xd8:  /* CLD */            DO_CLD   C( 2)  break;
	case 0x58:  /* CLI */            DO_CLI   C( 2)  goto test_interrupt;
	case 0xb8:  /* CLV */            DO_CLV   C( 2)  break;
	    
	case 0xc9:  /* CMP */  EA_IMM    DO_CMP   C( 2)  break;
	case 0xcd:  /* CMP */  EA_ABS    DO_CMP   C( 4)  break;
	case 0xc5:  /* CMP */  EA_ZP     DO_CMP   C( 3)  break;
	case 0xc1:  /* CMP */  EA_IND_X  DO_CMP   C( 6)  break;
	case 0xd1:  /* CMP */  EA_IND_Y  DO_CMP   C( 5)  break;
	case 0xd5:  /* CMP */  EA_ZP_X   DO_CMP   C( 4)  break;
	case 0xd9:  /* CMP */  EA_ABS_Y  DO_CMP   C( 4)  break;
	case 0xdd:  /* CMP */  EA_ABS_X  DO_CMP   C( 4)  break;
	    
	case 0xe0:  /* CPX */  EA_IMM    DO_CPX   C( 2)  break;
	case 0xec:  /* CPX */  EA_ABS    DO_CPX   C( 4)  break;
	case 0xe4:  /* CPX */  EA_ZP     DO_CPX   C( 3)  break;
	    
	case 0xc0:  /* CPY */  EA_IMM    DO_CPY   C( 2)  break;
	case 0xcc:  /* CPY */  EA_ABS    DO_CPY   C( 4)  break;
	case 0xc4:  /* CPY */  EA_ZP     DO_CPY   C( 3)  break;
	    
	case 0xce:  /* DEC */  EA_ABS    DO_DEC   C( 6)  break;
	case 0xc6:  /* DEC */  EA_ZP     DO_DEC   C( 5)  break;
	case 0xd6:  /* DEC */  EA_ZP_X   DO_DEC   C( 6)  break;
	case 0xde:  /* DEC */  EA_ABS_X  DO_DEC   C( 7)  break;
	    
	case 0xca:  /* DEX */            DO_DEX   C( 2)  break;
	case 0x88:  /* DEY */            DO_DEY   C( 2)  break;
	    
	case 0x49:  /* EOR */  EA_IMM    DO_EOR   C( 2)  break;
	case 0x4d:  /* EOR */  EA_ABS    DO_EOR   C( 4)  break;
	case 0x45:  /* EOR */  EA_ZP     DO_EOR   C( 3)  break;
	case 0x41:  /* EOR */  EA_IND_X  DO_EOR   C( 6)  break;
	case 0x51:  /* EOR */  EA_IND_Y  DO_EOR   C( 5)  break;
	case 0x55:  /* EOR */  EA_ZP_X   DO_EOR   C( 4)  break;
	case 0x59:  /* EOR */  EA_ABS_Y  DO_EOR   C( 4)  break;
	case 0x5d:  /* EOR */  EA_ABS_X  DO_EOR   C( 4)  break;
	    
	case 0xee:  /* INC */  EA_ABS    DO_INC   C( 6)  break;
	case 0xe6:  /* INC */  EA_ZP     DO_INC   C( 5)  break;
	case 0xf6:  /* INC */  EA_ZP_X   DO_INC   C( 6)  break;
	case 0xfe:  /* INC */  EA_ABS_X  DO_INC   C( 7)  break;
	    
	case 0xe8:  /* INX */            DO_INX   C( 2)  break;
	case 0xc8:  /* INY */            DO_INY   C( 2)  break;
	    
	case 0x4c:  /* JMP */  EA_ABS    DO_JMP   C( 3)  goto test_interrupt;
	case 0x6c:  /* JMP */  EA_IND    DO_JMP   C( 5)  goto test_interrupt;
	    
	case 0x20:  /* JSR */  EA_ABS    DO_JSR   C( 6)  goto test_interrupt;
	    
	case 0xa9:  /* LDA */  EA_IMM    DO_LDA   C( 2)  break;
	case 0xad:  /* LDA */  EA_ABS    DO_LDA   C( 4)  break;
	case 0xa5:  /* LDA */  EA_ZP     DO_LDA   C( 3)  break;
	case 0xa1:  /* LDA */  EA_IND_X  DO_LDA   C( 6)  break;
	case 0xb1:  /* LDA */  EA_IND_Y  DO_LDA   C( 5)  break;
	case 0xb5:  /* LDA */  EA_ZP_X   DO_LDA   C( 4)  break;
	case 0xb9:  /* LDA */  EA_ABS_Y  DO_LDA   C( 4)  break;
	case 0xbd:  /* LDA */  EA_ABS_X  DO_LDA   C( 4)  break;
	    
	case 0xa2:  /* LDX */  EA_IMM    DO_LDX   C( 2)  break;
	case 0xae:  /* LDX */  EA_ABS    DO_LDX   C( 4)  break;
	case 0xa6:  /* LDX */  EA_ZP     DO_LDX   C( 3)  break;
	case 0xbe:  /* LDX */  EA_ABS_Y  DO_LDX   C( 4)  break;
	case 0xb6:  /* LDX */  EA_ZP_Y   DO_LDX   C( 4)  break;
	    
	case 0xa0:  /* LDY */  EA_IMM    DO_LDY   C( 2)  break;
	case 0xac:  /* LDY */  EA_ABS    DO_LDY   C( 4)  break;
	case 0xa4:  /* LDY */  EA_ZP     DO_LDY   C( 3)  break;
	case 0xb4:  /* LDY */  EA_ZP_X   DO_LDY   C( 4)  break;
	case 0xbc:  /* LDY */  EA_ABS_X  DO_LDY   C( 4)  break;
	    
	case 0x4e:  /* LSR */  EA_ABS    DO_LSR   C( 6)  break;
	case 0x46:  /* LSR */  EA_ZP     DO_LSR   C( 5)  break;
	case 0x4a:  /* LSR */            DO_LSRA  C( 2)  break;
	case 0x56:  /* LSR */  EA_ZP_X   DO_LSR   C( 6)  break;
	case 0x5e:  /* LSR */  EA_ABS_X  DO_LSR   C( 7)  break;
	    
	case 0xea:  /* NOP */                     C( 2)  break;
	    
	case 0x09:  /* ORA */  EA_IMM    DO_ORA   C( 2)  break;
	case 0x0d:  /* ORA */  EA_ABS    DO_ORA   C( 4)  break;
	case 0x05:  /* ORA */  EA_ZP     DO_ORA   C( 3)  break;
	case 0x01:  /* ORA */  EA_IND_X  DO_ORA   C( 6)  break;
	case 0x11:  /* ORA */  EA_IND_Y  DO_ORA   C( 5)  break;
	case 0x15:  /* ORA */  EA_ZP_X   DO_ORA   C( 4)  break;
	case 0x19:  /* ORA */  EA_ABS_Y  DO_ORA   C( 4)  break;
	case 0x1d:  /* ORA */  EA_ABS_X  DO_ORA   C( 4)  break;
	    
	case 0x48:  /* PHA */            DO_PHA   C( 3)  break;
	case 0x08:  /* PHP */            DO_PHP   C( 3)  break;
	case 0x68:  /* PLA */            DO_PLA   C( 4)  break;
	case 0x28:  /* PLP */            DO_PLP   C( 4)  goto test_interrupt;
	    
	case 0x2e:  /* ROL */  EA_ABS    DO_ROL   C( 6)  break;
	case 0x26:  /* ROL */  EA_ZP     DO_ROL   C( 5)  break;
	case 0x2a:  /* ROL */            DO_ROLA  C( 2)  break;
	case 0x36:  /* ROL */  EA_ZP_X   DO_ROL   C( 6)  break;
	case 0x3e:  /* ROL */  EA_ABS_X  DO_ROL   C( 7)  break;
	    
	case 0x6e:  /* ROR */  EA_ABS    DO_ROR   C( 6)  break;
	case 0x66:  /* ROR */  EA_ZP     DO_ROR   C( 5)  break;
	case 0x6a:  /* ROR */            DO_RORA  C( 2)  break;
	case 0x76:  /* ROR */  EA_ZP_X   DO_ROR   C( 6)  break;
	case 0x7e:  /* ROR */  EA_ABS_X  DO_ROR   C( 7)  break;
	    
	case 0x40:  /* RTI */            DO_RTI   C( 6)  goto test_interrupt;
	case 0x60:  /* RTS */            DO_RTS   C( 6)  goto test_interrupt;
	    
	case 0xe9:  /* SBC */  EA_IMM    DO_SBC   C( 2)  break;
	case 0xed:  /* SBC */  EA_ABS    DO_SBC   C( 4)  break;
	case 0xe5:  /* SBC */  EA_ZP     DO_SBC   C( 3)  break;
	case 0xe1:  /* SBC */  EA_IND_X  DO_SBC   C( 6)  break;
	case 0xf1:  /* SBC */  EA_IND_Y  DO_SBC   C( 5)  break;
	case 0xf5:  /* SBC */  EA_ZP_X   DO_SBC   C( 4)  break;
	case 0xf9:  /* SBC */  EA_ABS_Y  DO_SBC   C( 4)  break;
	case 0xfd:  /* SBC */  EA_ABS_X  DO_SBC   C( 4)  break;
	    
	case 0x38:  /* SEC */            DO_SEC   C( 2)  break;
	case 0xf8:  /* SED */            DO_SED   C( 2)  break;
	case 0x78:  /* SEI */            DO_SEI   C( 2)  break;
	    
	case 0x8d:  /* STA */  EA_ABS    DO_STA   C( 4)  break;
	case 0x85:  /* STA */  EA_ZP     DO_STA   C( 3)  break;
	case 0x81:  /* STA */  EA_IND_X  DO_STA   C( 6)  break;
	case 0x91:  /* STA */  EA_IND_Y  DO_STA   C( 6)  break;
	case 0x95:  /* STA */  EA_ZP_X   DO_STA   C( 4)  break;
	case 0x99:  /* STA */  EA_ABS_Y  DO_STA   C( 5)  break;
	case 0x9d:  /* STA */  EA_ABS_X  DO_STA   C( 5)  break;
	    
	case 0x8e:  /* STX */  EA_ABS    DO_STX   C( 4)  break;
	case 0x86:  /* STX */  EA_ZP     DO_STX   C( 3)  break;
	case 0x96:  /* STX */  EA_ZP_Y   DO_STX   C( 4)  break;
	    
	case 0x8c:  /* STY */  EA_ABS    DO_STY   C( 4)  break;
	case 0x84:  /* STY */  EA_ZP     DO_STY   C( 3)  break;
	case 0x94:  /* STY */  EA_ZP_X   DO_STY   C( 4)  break;
	    
	case 0xaa:  /* TAX */            DO_TAX   C( 2)  break;
	case 0xa8:  /* TAY */            DO_TAY   C( 2)  break;
	case 0x98:  /* TYA */            DO_TYA   C( 2)  break;
	case 0xba:  /* TSX */            DO_TSX   C( 2)  break;
	case 0x8a:  /* TXA */            DO_TXA   C( 2)  break;
	case 0x9a:  /* TXS */            DO_TXS   C( 2)  break;
	    
	default:
	    break;
	}
    }
    for(;;);
}

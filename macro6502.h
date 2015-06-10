/*
 * macro6502.h: macros for 6502 simulator and binary translator
 *
 * Copyright 1991-1993 Eric Smith
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/macro6502.h,v 1.3 1997/05/17 22:25:36 eric Exp $
 */


/***********************************************************************
*
* cycle and byte count macros
*
***********************************************************************/


/* instruction byte count macro, for translated code only */
#ifdef KEEP_ACCURATE_PC
#define B(val) { PC += val; }
#else
#define B(val)
#endif

/* instruction cycle count macro */
#ifdef NO_CYCLE_COUNT
#define C(val)
#else
#define C(val) { totcycles += val; }
#endif


/***********************************************************************
*
* flag utilities
*
***********************************************************************/

#ifdef CC_INDIVIDUAL_VARS

  #define DECLARE_CC \
    register s32 C_flag; \
    register s32 Z_flag; \
    register s32 N_flag; \
    register s32 V_flag; \
    register s32 D_flag; \
    register s32 I_flag

  /* don't use expressions as the arguments to these macros (or at least not
     expressions with side effects */

  #define setflags(val) \
    { \
      Z_flag = ((val) == 0); \
      N_flag = (((val) & 0x80) != 0); \
    }

  #define flags_to_byte  (N_flag << 7 | V_flag << 6 | D_flag << 3 | \
			  I_flag << 2 | Z_flag << 1 | C_flag)

  #define byte_to_flags(b) \
    { \
      N_flag = (((b) & N_BIT) != 0); \
      V_flag = (((b) & V_BIT) != 0); \
   /* B_flag = (((b) & B_BIT) != 0); */ \
      D_flag = (((b) & D_BIT) != 0); \
      I_flag = (((b) & I_BIT) != 0); \
      Z_flag = (((b) & Z_BIT) != 0); \
      C_flag = (((b) & C_BIT) != 0); \
    }

  #define STO_N(val) { N_flag = ((val) != 0); }
  #define SET_N { N_flag = 1; }
  #define CLR_N { N_flag = 0; }
  #define TST_N (N_flag)

  #define STO_V(val) { V_flag = ((val) != 0); }
  #define SET_V { V_flag = 1; }
  #define CLR_V { V_flag = 0; }
  #define TST_V (V_flag)

  #define STO_D(val) { D_flag = ((val) != 0); }
  #define SET_D { D_flag = 1; }
  #define CLR_D { D_flag = 0; }
  #define TST_D (D_flag)

  #define STO_I(val) { I_flag = ((val) != 0); }
  #define SET_I { I_flag = 1; }
  #define CLR_I { I_flag = 0; }
  #define TST_I (I_flag)

  #define STO_Z(val) { Z_flag = ((val) != 0); }
  #define SET_Z { Z_flag = 1; }
  #define CLR_Z { Z_flag = 0; }
  #define TST_Z (Z_flag)

  #define STO_C(val) { C_flag = ((val) != 0); }
  #define SET_C { C_flag = 1; }
  #define CLR_C { C_flag = 0; }
  #define TST_C (C_flag)

#else /* ! CC_INDIVIDUAL_VARS */

  #define DECLARE_CC \
    register s32 CC

  #define setflags(val) \
    { \
      CC &= ~ (Z_BIT | N_BIT); \
      if ((val) == 0) \
        CC |= Z_BIT; \
      if ((val) & 0x80) \
        CC |= N_BIT; \
    }

  #define flags_to_byte (CC)  /* NOTE:  CC is not an argument to the macro! */

  #define byte_to_flags(b) { CC = (b); }

  #define STO_N(val) { if (val) CC |= N_BIT; else CC &= ~ N_BIT; }
  #define SET_N { CC |= N_BIT; }
  #define CLR_N { CC &= ~ N_BIT; }
  #define TST_N ((CC & N_BIT) != 0)

  #define STO_V(val) { if (val) CC |= V_BIT; else CC &= ~ V_BIT; }
  #define SET_V { CC |= V_BIT; }
  #define CLR_V { CC &= ~ V_BIT; }
  #define TST_V ((CC & V_BIT) != 0)

  #define STO_D(val) { if (val) CC |= D_BIT; else CC &= ~ D_BIT; }
  #define SET_D { CC |= D_BIT; }
  #define CLR_D { CC &= ~ D_BIT; }
  #define TST_D ((CC & D_BIT) != 0)

  #define STO_I(val) { if (val) CC |= I_BIT; else CC &= ~ I_BIT; }
  #define SET_I { CC |= I_BIT; }
  #define CLR_I { CC &= ~ I_BIT; }
  #define TST_I ((CC & I_BIT) != 0)

  #define STO_Z(val) { if (val) CC |= Z_BIT; else CC &= ~ Z_BIT; }
  #define SET_Z { CC |= Z_BIT; }
  #define CLR_Z { CC &= ~ Z_BIT; }
  #define TST_Z ((CC & Z_BIT) != 0)

  #define STO_C(val) { if (val) CC |= C_BIT; else CC &= ~ C_BIT; }
  #define SET_C { CC |= C_BIT; }
  #define CLR_C { CC &= ~ C_BIT; }
  #define TST_C ((CC & C_BIT) != 0)

#endif

/***********************************************************************
*
* effective address calculation for simulated code
*
***********************************************************************/

#define EA_IMM   { addr = PC++; }
#define EA_ABS   { addr = memrdwd (PC,PC,totcycles);     PC += 2; }
#define EA_ABS_X { addr = memrdwd (PC,PC,totcycles) + X; PC += 2; }
#define EA_ABS_Y { addr = memrdwd (PC,PC,totcycles) + Y; PC += 2; }
#define EA_ZP    { addr = memrd (PC,PC,totcycles);       PC++; }
#define EA_ZP_X  { addr = (memrd (PC,PC,totcycles) + X) & 0xff; PC++; }
#define EA_ZP_Y  { addr = (memrd (PC,PC,totcycles) + Y) & 0xff; PC++; }

#define EA_IND_X { addr = (memrd (PC,PC,totcycles) + X) & 0xff; addr = memrdwd (addr,PC,totcycles); PC++; }
/* Note that indirect indexed will do the wrong thing if the zero page address
   plus X is $FF, because the 6502 doesn't generate a carry */

#define EA_IND_Y { addr = memrd (PC,PC,totcycles); addr = memrdwd (addr,PC,totcycles) + Y; PC++; }
/* Note that indexed indirect will do the wrong thing if the zero page address
   is $FF, because the 6502 doesn't generate a carry */

#define EA_IND   { addr = memrdwd (PC,PC,totcycles); addr = memrdwd (addr,PC,totcycles); PC += 2; }
/* Note that this doesn't handle the NMOS 6502 indirect bug, where the low
   byte of the indirect address is $FF */


/***********************************************************************
*
* effective address calculation for translated code
*
***********************************************************************/

/*
 * The translator doesn't use an EA macro for immediate mode, as there are
 * specific instruction macros for immediate mode (DO_LDAI, DO_ADCI, etc.).
 *
 * A useful optimiztion would be to avoid using memrd if the translator
 * knows that the operand is in RAM or ROM.
 */

#define TR_ABS(arg)   { addr = arg; }
#define TR_ABS_X(arg) { addr = arg + X; }
#define TR_ABS_Y(arg) { addr = arg + Y; }
#define TR_ZP(arg)    { addr = arg; }
#define TR_ZP_X(arg)  { addr = (arg + X) & 0xff; }
#define TR_ZP_Y(arg)  { addr = (arg + Y) & 0xff; }

#define TR_IND_X(arg) { addr = memrdwd ((arg + X) & 0xff, PC, totcycles); }
/* Note that indirect indexed will do the wrong thing if the zero page address
   plus X is $FF, because the 6502 doesn't generate a carry */
/* The translator can't trivially check for this, because it doesn't know
   what is in the X register */

#define TR_IND_Y(arg) { addr = memrdwd (arg, PC, totcycles) + Y; }
/* Note that indexed indirect will do the wrong thing if the zero page address
   is $FF, because the 6502 doesn't generate a carry */
/* The translator should check for this */

#define TR_IND(arg)   { addr = memrdwd (addr, PC, totcycles); }
/* Note that this doesn't handle the NMOS 6502 indirect bug, where the low
   byte of the indirect address is $FF */
/* The translator should check for this */


/***********************************************************************
*
* loads and stores
*
***********************************************************************/

#define DO_LDA  { A = memrd (addr, PC, totcycles); setflags (A); }
#define DO_LDX  { X = memrd (addr, PC, totcycles); setflags (X); }
#define DO_LDY  { Y = memrd (addr, PC, totcycles); setflags (Y); }

#define DO_LDAI(data) { A = data; }
#define DO_LDXI(data) { X = data; }
#define DO_LDYI(data) { Y = data; }

#define DO_STA  { memwr (addr, A, PC, totcycles); }
#define DO_STX  { memwr (addr, X, PC, totcycles); }
#define DO_STY  { memwr (addr, Y, PC, totcycles); }

/***********************************************************************
*
* register transfers
*
***********************************************************************/

#define DO_TAX { X = A; setflags (X); }
#define DO_TAY { Y = A; setflags (Y); }
#define DO_TSX { X = g_cpu_SP; setflags (X); }
#define DO_TXA { A = X; setflags (A); }
#define DO_TXS { g_cpu_SP = X; }
#define DO_TYA { A = Y; setflags (A); }

/***********************************************************************
*
* index arithmetic
*
***********************************************************************/

#define DO_DEX { X = (X - 1) & 0xff; setflags (X); }
#define DO_DEY { Y = (Y - 1) & 0xff; setflags (Y); }
#define DO_INX { X = (X + 1) & 0xff; setflags (X); }
#define DO_INY { Y = (Y + 1) & 0xff; setflags (Y); }

/***********************************************************************
*
* stack operations
*
***********************************************************************/

#define DO_PHA { dopush (A, PC); }
#define DO_PHP { dopush (flags_to_byte, PC); }
#define DO_PLA { A = dopop(PC); setflags (A); }
#define DO_PLP { u8 f; f = dopop(PC); byte_to_flags (f); }

/***********************************************************************
*
* logical instructions
*
***********************************************************************/

#define DO_AND  { A &= memrd (addr, PC, totcycles); setflags (A); }
#define DO_ORA  { A |= memrd (addr, PC, totcycles); setflags (A); }
#define DO_EOR  { A ^= memrd (addr, PC, totcycles); setflags (A); }

#define DO_ANDI(data) { A &= data; setflags (A); }
#define DO_ORAI(data) { A |= data; setflags (A); }
#define DO_EORI(data) { A ^= data; setflags (A); }

#define DO_BIT \
  { \
    u8 bval; \
    bval = memrd(addr, PC, totcycles); \
    STO_N (bval & 0x80); \
    STO_V (bval & 0x40); \
    STO_Z ((A & bval) == 0); \
  }

/***********************************************************************
*
* arithmetic instructions
*
***********************************************************************/

#define DO_ADCI(data) \
  { \
    u16 wtemp; \
    if(TST_D) \
      { \
	u16 nib1, nib2; \
	u16 result1, result2; \
	u16 result3, result4; \
	wtemp = A; \
	nib1 = data & 0xf; \
	nib2 = wtemp & 0xf; \
	result1 = nib1+nib2+TST_C; /* Add carry */ \
	if(result1 >= 10) \
	  { \
	    result1 = result1 - 10; \
	    result2 = 1; \
	  } \
	else \
	  result2 = 0; \
	nib1 = (data & 0xf0) >> 4; \
	nib2 = (wtemp & 0xf0) >> 4; \
	result3 = nib1+nib2+result2; \
	if(result3 >= 10) \
	  { \
	    result3 = result3 - 10; \
	    result4 = 1; \
	  } \
	else \
	  result4 = 0; \
	STO_C (result4); \
	CLR_V; \
	wtemp = (result3 << 4) | (result1); \
	A = wtemp & 0xff; \
	setflags (A); \
      } \
    else \
      { \
	wtemp = A; \
	wtemp += TST_C;	   /* add carry */ \
	wtemp += data; \
	STO_C (wtemp & 0x100); \
	STO_V ((((A ^ data) & 0x80) == 0) && (((A ^ wtemp) & 0x80) != 0)); \
	A = wtemp & 0xff; \
	setflags (A); \
      } \
  }

#define DO_SBCI(data) \
  { \
    u16 wtemp; \
    if (TST_D) \
      { \
	s32 nib1, nib2; \
	s32 result1, result2; \
	s32 result3, result4; \
	wtemp = A; \
	nib1 = data & 0xf; \
	nib2 = wtemp & 0xf; \
	result1 = nib2-nib1-!TST_C; /* Sub borrow */ \
	if(result1 < 0) \
	  { \
	    result1 += 10; \
	    result2 = 1; \
	  } \
	else \
	  result2 = 0; \
	nib1 = (data & 0xf0) >> 4; \
	nib2 = (wtemp & 0xf0) >> 4; \
	result3 = nib2-nib1-result2; \
	if(result3 < 0) \
	  { \
	    result3 += 10; \
	    result4 = 1; \
	  } \
	else \
	  result4 = 0; \
	STO_C (!result4); \
	CLR_V; \
	wtemp = (result3 << 4) | (result1); \
	A = wtemp & 0xff; \
	setflags (A); \
      } \
    else \
      { \
	wtemp = A; \
	wtemp += TST_C; \
	wtemp += (data ^ 0xff); \
	STO_C (wtemp & 0x100); \
	STO_V ((((A ^ data) & 0x80) == 0) && (((A ^ wtemp) & 0x80) != 0)); \
	A = wtemp & 0xff; \
	setflags (A); \
      } \
  }

#define DO_ADC { u8 bval; bval = memrd (addr, PC, totcycles); DO_ADCI (bval); }
#define DO_SBC { u8 bval; bval = memrd (addr, PC, totcycles); DO_SBCI (bval); }

#define docompare(bval,reg) \
  { \
    STO_C (reg >= bval); \
    STO_Z (reg == bval); \
    STO_N ((reg - bval) & 0x80); \
  }

#define DO_CMP { u8 bval; bval = memrd (addr, PC, totcycles); docompare (bval, A); }
#define DO_CPX { u8 bval; bval = memrd (addr, PC, totcycles); docompare (bval, X); }
#define DO_CPY { u8 bval; bval = memrd (addr, PC, totcycles); docompare (bval, Y); }

#define DO_CMPI(data) { docompare (data, A); }
#define DO_CPXI(data) { docompare (data, X); }
#define DO_CPYI(data) { docompare (data, Y); }

/***********************************************************************
*
* read/modify/write instructions (INC, DEC, shifts and rotates)
*
***********************************************************************/

#define DO_INC \
  { \
    u8 bval; \
    bval = memrd(addr, PC, totcycles) + 1; \
    setflags (bval); \
    memwr(addr,bval, PC, totcycles); \
  }

#define DO_DEC \
  { \
    u8 bval; \
    bval = memrd(addr, PC, totcycles) - 1; \
    setflags (bval); \
    memwr(addr,bval, PC, totcycles); \
  }

#define DO_ROR_int(val) \
  { \
    u8 oldC = TST_C; \
    STO_C (val & 0x01); \
    val >>= 1; \
    if (oldC) \
      val |= 0x80; \
    setflags (val); \
  }

#define DO_RORA DO_ROR_int (A)

#define DO_ROR \
  { \
    u8 bval; \
    bval = memrd (addr, PC, totcycles); \
    DO_ROR_int (bval); \
    memwr (addr, bval, PC, totcycles); \
  }

#define DO_ROL_int(val) \
  { \
    u8 oldC = TST_C; \
    STO_C (val & 0x80); \
    val = (val << 1) & 0xff; \
    val |= oldC; \
    setflags (val); \
  }

#define DO_ROLA DO_ROL_int (A)

#define DO_ROL \
  { \
    u8 bval; \
    bval = memrd (addr, PC, totcycles); \
    DO_ROL_int (bval); \
    memwr (addr, bval, PC, totcycles); \
  }

#define DO_ASL_int(val) \
  { \
    STO_C (val & 0x80); \
    val = (val << 1) & 0xff; \
    setflags (val); \
  }

#define DO_ASLA DO_ASL_int (A)

#define DO_ASL \
  { \
    u8 bval; \
    bval = memrd (addr, PC, totcycles); \
    DO_ASL_int (bval); \
    memwr (addr, bval, PC, totcycles); \
  }

#define DO_LSR_int(val) \
  { \
    STO_C (val & 0x01); \
    val >>= 1; \
    setflags (val); \
  }

#define DO_LSRA DO_LSR_int (A)

#define DO_LSR \
  { \
    u8 bval; \
    bval = memrd (addr, PC, totcycles); \
    DO_LSR_int (bval); \
    memwr (addr, bval, PC, totcycles); \
  }

/***********************************************************************
*
* flag manipulation
*
***********************************************************************/

#define DO_CLC { CLR_C; }
#define DO_CLD { CLR_D; }
#define DO_CLI { CLR_I; }
#define DO_CLV { CLR_V; }

#define DO_SEC { SET_C; }
#define DO_SED { SET_D; }
#define DO_SEI { SET_I; }

/***********************************************************************
*
* instruction flow:  branches, jumps, calls, returns, SWI
*
***********************************************************************/

#define DO_JMP { PC = addr; }

#define TR_JMP { PC = addr; continue; }

#define DO_JSR \
  { \
    PC--; \
    dopush(PC >> 8, PC); \
    dopush(PC & 0xff, PC); \
    PC = addr; \
  }

/*
 * Note that the argument to TR_JSR is the address of the JSR instruction,
 * _not_ the address of the target.  The address of the target has to be
 * set up beforehand by the TR_EA_ABS macro or the like.
 */
#define TR_JSR(arg) \
  { \
    dopush ((arg + 2) >> 8, PC); \
    dopush ((arg + 2) & 0xff, PC); \
    PC = addr; \
    continue; \
  }

#define DO_RTI \
  { \
    u8 f; \
    f = dopop(PC); \
    byte_to_flags (f); \
    PC = dopop(PC); /* & 0xff */ \
    PC |= dopop(PC) << 8; \
  }

#define TR_RTI \
  { \
    u8 f; \
    f = dopop(PC); \
    byte_to_flags (f); \
    PC = dopop(PC); /* & 0xff */ \
    PC |= dopop(PC) << 8; \
    continue; \
  }

#define DO_RTS \
  { \
    PC = dopop(PC); /* & 0xff */ \
    PC |= dopop(PC) << 8; \
    PC++; \
  }

#define TR_RTS \
  { \
    PC = dopop(PC); /* & 0xff */ \
    PC |= dopop(PC) << 8; \
    PC++; \
    continue; \
  }


#define DO_BRK \
  { \
      for(;;);  \
  }
#if 0
#define TR_BRK(arg) \
  { \
    ; \
  }
#endif

#define dobranch(bit,sign) \
  { \
    s32 offset; \
    if (bit == sign) \
      { \
	offset = memrd (PC, PC, totcycles); \
	if (offset & 0x80) \
          offset |= 0xff00; \
	PC = (PC + 1 + offset) & 0xffff; \
      } \
    else \
      PC++; \
  }

#define trbranch(bit,sign,dest) \
  { \
    if (bit == sign) \
      { \
        PC = dest; \
        continue; \
      } \
  }

#define DO_BCC { dobranch (TST_C, 0); }
#define DO_BCS { dobranch (TST_C, 1); }
#define DO_BEQ { dobranch (TST_Z, 1); }
#define DO_BMI { dobranch (TST_N, 1); }
#define DO_BNE { dobranch (TST_Z, 0); }
#define DO_BPL { dobranch (TST_N, 0); }
#define DO_BVC { dobranch (TST_V, 0); }
#define DO_BVS { dobranch (TST_V, 1); }

//#define CHECK_PC { if((PC == 0x7B71) || (PC == 0x7B81)) xyzzy(); }
#define TR_BCC(addr) { trbranch (TST_C, 0, addr); }
#define TR_BCS(addr) { trbranch (TST_C, 1, addr); }
#define TR_BEQ(addr) { trbranch (TST_Z, 1, addr); }
#define TR_BMI(addr) { trbranch (TST_N, 1, addr); }
#define TR_BNE(addr) { trbranch (TST_Z, 0, addr); }
#define TR_BPL(addr) { trbranch (TST_N, 0, addr); }
#define TR_BVC(addr) { trbranch (TST_V, 0, addr); }
#define TR_BVS(addr) { trbranch (TST_V, 1, addr); }

/***********************************************************************
*
* misc.
*
***********************************************************************/

#define DO_NOP

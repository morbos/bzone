/*
 * memory.h: memory and I/O functions for Atari Vector game simulator
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
 * $Header: /usr2/eric/vg/atari/vecsim/RCS/memory.h,v 1.13 1997/05/17 22:25:36 eric Exp $
 */

typedef struct _elem {
    u8 cell;
    u8 tagr;
    u8 tagw;
#ifdef MAGIC_PC
    u8 magic;  /* flag indicating interrupt OK here */
#else
    u8 pad;
#endif
} elem;

/* types of access */
#define RD 1
#define WR 2

/* tags */
#define MEMORY   0 /* memory, no special processing */
#define ROMWRT   1 /* ROM write, just print a message */
#define IGNWRT   2 /* spurious write that we don't care about */
#define UNKNOWN  3 /* don't know what it is */

#define COININ	 4 /* coin, slam, self test, diag, VG halt, 3KHz inputs */
#define COINOUT  5 /* coin counter and invert X & Y outputs */
#define WDCLR	 6
#define INTACK	 7

#define VGRST    8
#define VGO      9 /* VGO */
#define DMACNT   10 /* DVG only */

#define VECRAM	 11 /* MEMORY /* Vector RAM */
/*
 * VECRAM used to be 11 so we could do some special debugging stuff on
 * writes, but that is no longer necessary.
 */

#define COLORRAM 12

#define POKEY1   13 /* Pokey */
#define POKEY2   14 /* Pokey */
#define POKEY3   15 /* Pokey */
#define POKEY4   16 /* Pokey */

#define OPTSW1        17
#define OPTSW2        18
#define OPT1_2BIT     19

/* 20 and 21 no longer used */

#define EAROMCON      22
#define EAROMWR       23
#define EAROMRD       24

#define MBLO          25
#define MBHI          26
#define MBSTART       27
#define MBSTAT        28

#define GRAVITAR_IN1  29
#define GRAVITAR_IN2  30

#define SD_INPUTS     31

#define TEMP_OUTPUTS  32

#define BZ_SOUND      33
#define BZ_INPUTS     34

#define LUNAR_MEM     35
#define LUNAR_SW1     36
#define LUNAR_SW2     37
#define LUNAR_POT     38
#define LUNAR_OUT     39
#define LUNAR_SND     40
#define LUNAR_SND_RST 41

#define ASTEROIDS_SW1     42
#define ASTEROIDS_SW2     43
#define ASTEROIDS_OUT     44
#define ASTEROIDS_EXP     45
#define ASTEROIDS_THUMP   46
#define ASTEROIDS_SND     47
#define ASTEROIDS_SND_RST 48

#define AST_DEL_OUT	49

#define RB_SW           50
#define RB_SND          51
#define RB_SND_RST      52
#define RB_JOY          53

#define MEMORY1         54 // Special for ST build
#define MEMORY_BB       55 // Special for ST build (bit bucket var)

#define BREAKTAG  0x80

#define memrd(addr,PC,cyc) (MEMRD(addr,PC,cyc))

u8 MEMRD(u32 addr, s32 PC, u32 cyc);

#define memrdwd(addr,PC,cyc) ((MEMRD(addr,PC,cyc) | (MEMRD((addr)+1,PC,cyc) << 8)))

#define memwr(addr,val,PC,cyc) MEMWR(addr,val,PC,cyc);

void MEMWR(u32 addr, s32 val, s32 PC, u32 cyc);

#define MAX_OPT_REG 3

typedef struct
{
  s32 left;
  s32 right;
  s32 fire;
  s32 thrust;
  s32 hyper;
  s32 shield;
  s32 abort;
} switch_rec;

#define leftfwd left
#define leftrev thrust
#define rightfwd right
#define rightrev hyper

typedef struct {
    char *name;
    u32 addr;
    u32 len;
    u32 offset;
} rom_info;

typedef struct {
    u32 addr;
    u32 len;
    s32 dir;
    s32 tag;
} tag_info;

void read_rom_image (char *fn, u32 faddr, u32 len, u32 offset);
void tag_area (u32 addr, u32 len, s32 dir, s32 tag);
void setup_roms_and_tags (rom_info *rom_list, tag_info *tag_list);
void copy_rom (u32 source, u32 dest, u32 len);


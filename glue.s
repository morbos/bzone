	 @ glue.s: Linkage to dwt counters
	 @
	 @ Copyright 2014 Hedley Rainnie
	 @
	 @    This program is free software; you can redistribute it and/or modify
	 @    it under the terms of the GNU General Public License as published by
	 @    the Free Software Foundation; either version 2 of the License, or
	 @    (at your option) any later version.
	 @
	 @    This program is distributed in the hope that it will be useful,
	 @    but WITHOUT ANY WARRANTY; without even the implied warranty of
	 @    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 @    GNU General Public License for more details.
	 @
	 @    You should have received a copy of the GNU General Public License
	 @    along with this program; if not, write to the Free Software
	 @    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	 @
	 @
	.syntax unified
	.text
	.globl	init_dwt
init_dwt:	
	ldr	r0,=0xe0001000	@ CM[34] DWT offset
	ldr	r1,[r0]
	mov	r2,#0
	str	r2,[r0,#4]	@ clear cycle count
	orr	r1,r1,#1
	str	r1,[r0]		@ enable the counter
@ return counter val	
	.globl	get_dwt
get_dwt:	
	ldr	r1,=0xe0001004
	ldr	r0,[r1]
	bx	r14
@ return counter val (and clear it after)
	.globl	get_and_clear_dwt
get_and_clear_dwt:	
	ldr	r1,=0xe0001004
	mov	r2,#0
	ldr	r0,[r1]
	str	r2,[r1]
	bx	r14
	.globl	clr_dwt
clr_dwt:	
	ldr	r1,=0xe0001004
	mov	r0,#0
	str	r0,[r1]
	bx	r14
	.end

	 @ mixer.s: mono wav file blender
	 @
	 @ Copyright 2014, 2015 Hedley Rainnie
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
	.text
	@ r0 - ptr to s16 sample table
	@ r1 - q15 factor (1/N)
	@ r2 - # of samples [31:0] ( << 1 so it can be used for 1/2 word access)
	.global mixer
mixer:
	push	{r4,lr}
	@ Now to cycle over all the points
	mov	r4,#0	@ clear acc
loop:
	sub	r2,#2
	ldrsh	r3,[r0,r2]	@ get 1 sample
	@ acc = acc + sample * factor
	smlawb	r4,r1,r3,r4 @ (r4[31:0] = (r1[31:0]*r3[r15:0])+r4[15:0])
	bne	loop	   @ go around if more
	lsl	r0,r4,#1   @ adj the q15 return value
	pop	{r4,pc}
	.end

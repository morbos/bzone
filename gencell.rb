#! /usr/bin/ruby
#
# gencell.rb: Given a game emit a cells.s file that
#             represents the tagged image of that rom

if ARGV.length != 3
    printf "gencell: rompath gamename outfile\n"
    exit(1)
end

# types of access
RD=1
WR=2

# tags
MEMORY=0 # memory, no special processing
ROMWRT=1 # ROM write, just print a message
IGNWRT=2 # spurious write that we don't care about
UNKNOWN=3 # don't know what it is

COININ=4 # coin, slam, self test, diag, VG halt, 3KHz inputs
COINOUT=5 # coin counter and invert X & Y outputs
WDCLR=6
INTACK=7

VGRST=8
VGO=9 # VGO
DMACNT=10 # DVG only

VECRAM=11 # MEMORY # Vector RAM

COLORRAM=12

POKEY1=13 # Pokey
POKEY2=14 # Pokey
POKEY3=15 # Pokey
POKEY4=16 # Pokey

OPTSW1=17
OPTSW2=18
OPT1_2BIT=19
# 20 and 21 no longer used
EAROMCON=22
EAROMWR=23
EAROMRD=24
MBLO=25
MBHI=26
MBSTART=27
MBSTAT=28
GRAVITAR_IN1=29
GRAVITAR_IN2=30
SD_INPUTS=31
TEMP_OUTPUTS=32
BZ_SOUND=33
BZ_INPUTS=34
LUNAR_MEM=35
LUNAR_SW1=36
LUNAR_SW2=37
LUNAR_POT=38
LUNAR_OUT=39
LUNAR_SND=40
LUNAR_SND_RST=41
ASTEROIDS_SW1=42
ASTEROIDS_SW2=43
ASTEROIDS_OUT=44
ASTEROIDS_EXP=45
ASTEROIDS_THUMP=46
ASTEROIDS_SND=47
ASTEROIDS_SND_RST=48
AST_DEL_OUT=49
RB_SW=50
RB_SND=51
RB_SND_RST=52
RB_JOY=53
MEMORY1=54 # Special for ST build
MEMORY_BB=55 # Special for ST build (bit bucket var)
MEMORY_BANK0=56 # Special for ST build. Handle bank
MEMORY_BANK1=57 # Special for ST build. Handle bank
BREAKTAG=0x80

@gamename = ""

BZ = "Battlezone"
bz_layout = [
	[0x0000, 0x0400, RD | WR, MEMORY1, ""],
	[0x0800,      1, RD,      COININ, ""],
	[0x0a00,      1, RD,      OPTSW1, "" ],
	[0x0c00,      1, RD,      OPTSW2, "" ],
	[0x1000,      1,      WR, COINOUT, "" ],
	[0x1200,      1,      WR, VGO, "" ],
	[0x1400,      1,      WR, WDCLR, "" ],
	[0x1600,      1,      WR, VGRST, "" ],
	[0x1800,      1, RD,      MBSTAT, "" ],
	[0x1810,      1, RD,      MBLO, "" ],
	[0x1818,      1, RD,      MBHI, "" ],
	[0x1820,   0x10, RD | WR, POKEY1, "" ],
	[0x1828,      1, RD,      BZ_INPUTS, "" ],
	[0x1840,      1,      WR, BZ_SOUND, "" ],
	[0x1860,   0x20,      WR, MBSTART, "" ],
	[0x2000, 0x1000, RD | WR, VECRAM, "" ],
	[0xa000, 0x1000, RD | WR, VECRAM, "" ],
	[0x5000, 0x0800, RD, ROMWRT, "036414-02.e1" ],
	[0x5800, 0x0800, RD, ROMWRT, "036413-01.h1"],
	[0x6000, 0x0800, RD, ROMWRT, "036412-01.j1" ],
	[0x6800, 0x0800, RD, ROMWRT, "036411-01.k1" ],
	[0x7000, 0x0800, RD, ROMWRT, "036410-01.lm1" ],
	[0x7800, 0x0800, RD, ROMWRT, "036409-01.n1" ],
	[0x3000, 0x0800, RD, ROMWRT, "036422-01.bc3" ],
	[0x3800, 0x0800, RD, ROMWRT, "036421-01.a3" ]
]


AST = "Asteroids"
ast_layout = [
  [ 0x0000, 0x0400, RD | WR, MEMORY1, ""],  # RAM
  [ 0x2000,      8, RD,      ASTEROIDS_SW1, ""],
  [ 0x2400,      8, RD,      ASTEROIDS_SW2, ""],
  [ 0x2800,      4, RD,      OPT1_2BIT, ""],
  # Asteroids uses an LSR instruction to get bit 0 of some
  # of its inputs into the carry flag.  The write may be safely
  # ignored, so we mark it as memory.  ELS 920721
  [ 0x2000,      8,      WR, MEMORY_BB, ""],
  [ 0x2400,      8,      WR, MEMORY_BB, ""],
  [ 0x2802,      4,      WR, MEMORY_BB, ""],
  [ 0x3000,      1,      WR, VGO, ""],
  [ 0x3200,      1,      WR, ASTEROIDS_OUT, ""],
  [ 0x3400,      1,      WR, WDCLR, ""],
  [ 0x3600,      1,      WR, ASTEROIDS_EXP, ""],
  [ 0x3800,      1,      WR, DMACNT, ""],
  [ 0x3A00,      1,      WR, ASTEROIDS_THUMP, ""],
  [ 0x3C00,      6,      WR, ASTEROIDS_SND, ""],
  [ 0x3E00,      1,      WR, ASTEROIDS_SND_RST, ""],
  [ 0x4000, 0x0800, RD | WR, VECRAM, ""],
  [ 0x6800, 0x0800, RD, ROMWRT, "035145-04e.ef2"],
  [ 0x7000, 0x0800, RD, ROMWRT, "035144-04e.h2",],
  [ 0x7800, 0x0800, RD, ROMWRT, "035143-02.j2",],
  [ 0x5000, 0x0800, RD, ROMWRT, "035127-02.np3",],
]

@code = Array.new
@curr = 0
def start
   for i in 0..65535
      @code[i] = [0,UNKNOWN,UNKNOWN,0]
   end
end
def finish
   $of = File.open(ARGV[2], "w")
   printf $of,"\t.section .rodata\n"
   printf $of,"\t.global g_sys_mem\n"
   printf $of,"g_sys_mem:\n"

   for i in 0..65535
      $idx = i*4
      printf $of,"\t.byte 0x%02x @ 0x%04x\n", @code[i][0], $idx
      printf $of,"\t.byte 0x%02x @ 0x%04x\n", @code[i][1], $idx+1
      printf $of,"\t.byte 0x%02x @ 0x%04x\n", @code[i][2], $idx+2
      printf $of,"\t.byte 0x%02x @ 0x%04x\n", @code[i][3], $idx+3
   end
   printf $of,"\t.end\n"
   $of.close()
end
def emit(x)
   $sa = x[0]
   $len = x[1]
   @curr = $sa
   if(x[4] != "") 
	$fin = File.open(ARGV[0] + "/" + @gamename + "/" + x[4], "rb")
	for i in $sa..$sa+$len-1
	   $val = $fin.read(1)
  	   $x =  $val.unpack('C')
           @code[i] = [$x[0],0,ROMWRT,0]
        end
        $fin.close()
   else
      for i in @curr..($sa+$len-1)
	 $mask = (x[2] & (RD|WR))
         if (RD|WR) == $mask 
             @code[i] = [0,x[3],x[3],0]
         elsif RD == $mask 
	     if @code[i][2] == UNKNOWN
                @code[i] = [0,x[3],UNKNOWN,0]
             else
                @code[i][1] = x[3]
	     end
         elsif WR == $mask
	     if @code[i] == UNKNOWN
                @code[i] = [0,UNKNOWN,x[3],0]
             else
                @code[i][2] = x[3]
             end
         end
      end
      @curr = $sa+$len
   end
end
if 0 == ARGV[1].casecmp(BZ)
     @gamename = BZ
     start
     bz_layout.sort! {|a, b| a[0] <=> b[0]}
     bz_layout.each { |x| emit(x) }
     for i in 0..5   # processor vector alias
        @code[0xfffa+i] = @code[0x7ffa+i]
     end

     # Hand edit on the bzone rom to NOP out a CMP (0,X) that can
     # go out of range. On the real HW or full mem map, fine. On the
     # STM.. not so fine
     # 0x6b79 C1 00       CMP (0,X)
     # 
     if @code[0x6b79][0] == 0xc1 && @code[0x6b7a][0] == 0x00 then
         @code[0x6b79][0] = 0xea
         @code[0x6b7a][0] = 0xea
     else
         print "Rom mismatch on BZ for patch\n"
     end
     finish
elsif 0 == ARGV[1].casecmp(AST)
     @gamename = AST
     start
     ast_layout.sort! {|a, b| a[0] <=> b[0]}
     ast_layout.each { |x| emit(x) }
     for i in 0..5   # processor vector alias
        @code[0xfffa+i] = @code[0x7ffa+i]
     end
     finish
end

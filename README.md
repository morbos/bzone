
                          bzone
                          -----


0) Intro
This is vecsim specifically targeted to the STM32F407.

1) Setup
- A stock STM32F4discovery board with the 407 chip requires some
  re-work to handle the use case we have. Firstly, the LRCK to the
  codec is wired to PA4 on the STM32407VG device. We need PA4 for a
  DAC output. Soln: Wire the LRCK from the codec to PA15. The codec
  defn for CODEC_I2S_WS_PIN is altered to achieve that. You will need
  to Xacto cut the trace from LRCK to PA4. Once this change is done,
  the default of SWD for debug will still work but be advised that
  JTAG won't be availble.
- Wire up a 4bit ladder dac from GPIOC[3:0] to the ladder dac (curr
  its either max or 0 in the code).
- controls are on GPIOB[5:0]={start,fire,lf,lr,rf,rr}
- GPIOC[4] is for the start LED
- GPIOB[5]=start1
- GPIOC[7] = GPIOC[6]. Please ensure a jumper block is placed to short
  those two pins together. This ties MCLK to an internal timer for
  audio to function correctly.
- Make sure that HSE_VALUE in:
  Libraries/CMSIS/ST/STM32F4xx/Include/stm32f4xx.h
  is 8Mhz for the discovery board. The library as shipped has 25Mhz
  there. 
- Flash main.axf to the board
2) On performance
- Last project, Asteroids used a 48khz sample rate. This project is
  using 16khz. This is a win for filesize as the binary is still
  topping out near the max image size of 1MB.
  Vector workload on this build has not been quantified. Late in the
  game, clipping had to be added in the point subr. That is a critical
  path function and 4 compares had to go in. Still, it seems quite
  spry and the game is playable. 



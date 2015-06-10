/*
 * board.c: Bzone board support
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
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio_codec.h"
#include "stm32f4xx.h"
#include "game.h"
#include "memory.h"
#include "data.h"
#include "board.h"

void 
TIM3_IRQHandler(void)
{
    /* Clear the IT pending Bit */
    TIM3->SR = (uint16_t)~TIM_IT_Update;
    if(g_aud_smask) {
	s16 x;
	if(g_aud_smask == 1) {
	    x = 0;
	} else {
	    x = get_sample();
	}
	g_aud_abuf[g_aud_ping_pong][g_aud_idx] = x;
	g_aud_abuf[g_aud_ping_pong][g_aud_idx+1] = x;
	g_aud_idx += 2;
	if(g_aud_idx == 16) {
	    g_aud_ping_pong = !g_aud_ping_pong;
	    g_aud_idx = 0;
	}
    }
}
uint16_t 
EVAL_AUDIO_GetSampleCallBack(void)
{
    return 0;
}
void 
init_gpio()
{
    /*Enable or disable the AHB1 peripheral clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;

    /*
      DAC GPIO Configuration	
      PA4	 ------> DAC_OUT1
      PA5	 ------> DAC_OUT2
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin */
    // Legend:
    // start -5
    // Fire - 4
    // LF   - 3
    // LR   - 2
    // RF   - 1
    // RR   - 0
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin */
    // Ladder dac[3:0] - GPIO[3:0]
    // LED1 - GPIO[4]
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1|GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}
void
init_dac()
{
    DAC_InitTypeDef  DAC_InitStructure;
    /* DAC Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = 0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* Enable DAC Channel1 */
    DAC_Cmd(DAC_Channel_1, ENABLE);

    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = 0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    /* Enable DAC Channel2 */
    DAC_Cmd(DAC_Channel_2, ENABLE);

}
void
dac_out(int ch1, int ch2, int z)
{
    static int old_ch1;
    static int old_ch2;
    int dx;
    int dy;
    int iabs(int x);
    DAC->DHR12R1 = ch1;
    DAC->DHR12R2 = ch2;
    DAC->SWTRIGR |= bit(1)|bit(0);
    // In the off state, if the differential is large
    // wait here till the beam gets closer before leaving.
    if(!z) {
	dx = iabs(ch1 - old_ch1);
	dy = iabs(ch2 - old_ch2);
	if((dx > 10) || (dy > 10)) {
	    int i;
	    int tmp = (dx+dy) * g_vctr_delay_factor;
	    for(i=0;i < tmp;i++) {
		asm volatile("nop");
	    }
	}
    }
    old_ch1 = ch1;
    old_ch2 = ch2;
}
void 
TIM5_IRQHandler(void)
{
    /* Clear the IT pending Bit */
    TIM5->SR = (uint16_t)~TIM_IT_Update;
    g_soc_sixty_hz = 1;
}
void 
init_timer(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable the TIM5 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
 
    /* TIM5 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,  ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 16667 - 1; // 60hz
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 84 MHz Clock down to 1 MHz (adjust per your clock)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5,  &TIM_TimeBaseStructure);
    /* TIM IT enable */
    TIM_ITConfig(TIM5,  TIM_IT_Update,  ENABLE);
    /* TIM5 enable counter */
    TIM_Cmd(TIM5,  ENABLE);

    /* Enable the TIM3 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
  
    /* GPIOC clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,  ENABLE);
 
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);

    /* GPIOC Configuration: TIM3 CH1 (PC6) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Input/Output controlled by peripheral
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // Button to ground expectation
    GPIO_Init(GPIOC,  &GPIO_InitStructure);
 
    /* Connect TIM3 pins to AF */
    GPIO_PinAFConfig(GPIOC,  GPIO_PinSource6,  GPIO_AF_TIM3);
  
    TIM_TimeBaseStructure.TIM_Period = 2-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 128-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,  &TIM_TimeBaseStructure);
    /* TIM IT enable */
    TIM_ITConfig(TIM3,  TIM_IT_Update,  ENABLE);
    TIM_TIxExternalClockConfig(TIM3,  TIM_TIxExternalCLK1Source_TI1,  TIM_ICPolarity_Rising,  0);
    TIM_Cmd(TIM3,  ENABLE);

}
void
write_gpio(int x)
{
    if(x == 0) {
	int i;
        for(i=0; i < g_vctr_post_delay; i++) {
            asm volatile("nop");
        }
	GPIOC->BSRRH = 0xf;
    } else {
//	GPIOC->BSRRL = x << 6;
	GPIOC->BSRRL = 0xf;
    }
}
void
write_gpio_leds(int val)
{
    if(val & bit(0)) {
	GPIOC->BSRRL = START1_LED_PIN;
    } else {
	GPIOC->BSRRH = START1_LED_PIN;
    }
}
u16
read_gpio()
{
    return GPIOB->IDR;
}
void
init_board()
{
    /* Enable CRC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

    /* Initialize I2S interface */  
    EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
  
    /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */  
    EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, g_codec_volume, I2S_AudioFreq_16k);

    EVAL_AUDIO_Play((uint16_t*)&g_aud_abuf[0], 16*8);

//    init_dwt(); // Perf counter
    init_gpio();
    init_timer();
    init_dac();

    // Now sync to the DMA. This locks the 48khz IRQ service to the 
    // double buffer dma
    g_aud_ping_pong = DMA_GetCurrentMemoryTarget(AUDIO_MAL_DMA_STREAM);
    while(g_aud_ping_pong == DMA_GetCurrentMemoryTarget(AUDIO_MAL_DMA_STREAM)) {
	;
    }
    g_aud_ping_pong = !g_aud_ping_pong;
    g_aud_idx = 0;
    g_aud_smask = 1; // bit0 is the sound of... silence (and also enables 48khz audio)
}

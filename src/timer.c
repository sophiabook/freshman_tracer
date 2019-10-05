/*
 * timer.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void ms_wait(uint32_t ms) {
	SysTick->LOAD	=	8000 - 1; // reset value for count-down timer
	SysTick->VAL	=	0; // initial value
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // count start
	for(uint32_t i=0; i<ms; i++){
		while( !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) );
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // count stop
}

void sysclk_init(uint8_t multiple) { // max clock = 64MHz
	if( multiple <= 0 || multiple > 16 ) {
		return;
	}
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2; // PLL <== HSI/2 = 4MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = PLL / 2 = 32MHz
	RCC->CFGR |= ( (multiple - 1) << RCC_CFGR_PLLMUL_Pos);
	FLASH->ACR |= FLASH_ACR_LATENCY_1; // flash access latency for 48 < HCLK <= 72. This statement must be placed immediately after PLL multiplication.
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)); // wait until PLL is ready
	RCC->CFGR |= RCC_CFGR_SW_PLL; // PLL as system clock
	while( (RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL ); // wait until PLL clock supply starts
	SystemCoreClockUpdate();
}



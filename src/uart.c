/*
 * uart.c
 *
 *  Created on: 2018/03/03
 *      Author: iLand
 */

#include "stm32f3xx.h"

#include <stdio.h>

#include <stdarg.h>

#include <stdlib.h>

#include "uart.h"

void usart_io_init(void) {

	// GPIO as Tx and Rx

	// NOTE: The index for AFR is vailable only in 0 or 1.

	//       In detail, you should refer in the programming reference guide.

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER |= (GPIO_MODE_AF_PP << GPIO_MODER_MODER9_Pos); // Alternate function in PA9

	GPIOA->AFR[1] |= (GPIO_AF7_USART1 << GPIO_AFRH_AFRH1_Pos); // alternate function select in high resistor

	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR9_Pos);

	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_Msk;



	GPIOA->MODER |= (GPIO_MODE_AF_PP << GPIO_MODER_MODER10_Pos); // Alternate function in PA10

	GPIOA->AFR[1] |= (GPIO_AF7_USART1 << GPIO_AFRH_AFRH2_Pos); // anternate function select in high resistor

	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR10_Pos);

	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_Msk;

}



// Tx/Rx Baud rate = f_ck / USARTDIV (16x over sampling)

// SET_BIT() and CLEAR_BIT() macros are available in stm32f3xx.h .

void usart_init(uint32_t baudrate) {

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK; // system clock == usart1 clock



	// In this project, word length is set to 8bit.

	USART1->CR1 &= ~USART_CR1_OVER8; // not OVER8

	USART1->BRR |= (uint32_t)(64e6 / baudrate);

	usart_io_init();

	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmitter & receiver enable



	USART1->CR1 |= USART_CR1_UE; // enable USART1

}



void send_char(char c) {

	// Do not fail to set TE bit ==> Write data to USART_TDR ==> Wait until TDR_TC is set.

	while( !(USART1->ISR & USART_ISR_TXE) );

	USART1->TDR = c;

	while( !(USART1->ISR & USART_ISR_TC) );

}



void send_str(const char *str) {

	uint8_t p = 0;

	while( str[p] != '\0' ) {

		send_char(str[p]);

		p++;

	}

}



char get_char(void) {

	// Do not fail to set CR1_RE bit ==> Wait until RXNE bit is set. ==> Read from RDR

	while( !(USART1->ISR & USART_ISR_RXNE) );

	char buf = USART1->RDR;

	return buf;

}



void get_str(char *str) {

	char buf;

	uint8_t p = 0;

	do {

		buf = get_char();

		str[p++] = buf;

		if( buf == 127 ) p -= 2; // back space

	} while( buf != '\n' && buf != '\r' );

	str[p-1] = '\0';

}



void uart_printf(const char* format, ...) {

	va_list arg;

	va_start(arg, format);



	vsprintf(buf, format, arg);

	send_str(buf);

	va_end(arg);

}

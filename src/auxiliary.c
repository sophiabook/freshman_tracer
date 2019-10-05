/*
 * auxiliary.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void select_mode(int8_t *mode){
	*mode = 0x00;
	uart_printf("%s%d\r", "mode:", *mode);//���[�h�ԍ���UART�ő��M
	while(!SW3){
		if(SW1){
			*mode += 1;
			*mode = (*mode + 16) % 16;
			uart_printf("%s%d\r", "mode:", *mode);//���[�h�ԍ���UART�ő��M
			ms_wait(250);
		}else if(SW2){
			*mode -= 1;
			*mode = (*mode + 16) % 16;
			uart_printf("%s%d\r", "mode:", *mode);//���[�h�ԍ���UART�ő��M
			ms_wait(250);
		}//SW1,SW2��������ĂȂ��Ƃ���UART�Ń��[�h�ԍ��𑗐M���Ȃ�
		GPIOB->ODR &= ~0x000000C3U;//��������LED�S����
		GPIOB->ODR |= (uint32_t)(((*mode << 4) & 0xC0) | (*mode & 0x03));//LED�Ɍ��݂̒l��\��

		// *mode== 0== 0b0000 �̂Ƃ� GPIOB->ODR |= 0x00 == 0b00000000
		// *mode== 1== 0b0001 �̂Ƃ� GPIOB->ODR |= 0x01 == 0b00000001
		// *mode== 2== 0b0010 �̂Ƃ� GPIOB->ODR |= 0x02 == 0b00000010
		// *mode== 3== 0b0011 �̂Ƃ� GPIOB->ODR |= 0x03 == 0b00000011
		// *mode== 4== 0b0100 �̂Ƃ� GPIOB->ODR |= 0x40 == 0b01000000
		// *mode== 5== 0b0101 �̂Ƃ� GPIOB->ODR |= 0x41 == 0b01000001
		// *mode== 6== 0b0110 �̂Ƃ� GPIOB->ODR |= 0x42 == 0b01000010
		// *mode== 7== 0b0111 �̂Ƃ� GPIOB->ODR |= 0x43 == 0b01000011
		// *mode== 8== 0b1000 �̂Ƃ� GPIOB->ODR |= 0x80 == 0b10000000
		// *mode== 9== 0b1001 �̂Ƃ� GPIOB->ODR |= 0x81 == 0b10000001
		// *mode==10== 0b1010 �̂Ƃ� GPIOB->ODR |= 0x82 == 0b10000010
		// *mode==11== 0b1011 �̂Ƃ� GPIOB->ODR |= 0x83 == 0b10000011
		// *mode==12== 0b1100 �̂Ƃ� GPIOB->ODR |= 0xC0 == 0b11000000
		// *mode==13== 0b1101 �̂Ƃ� GPIOB->ODR |= 0xC1 == 0b11000001
		// *mode==14== 0b1110 �̂Ƃ� GPIOB->ODR |= 0xC2 == 0b11000010
		// *mode==15== 0b1111 �̂Ƃ� GPIOB->ODR |= 0xC3 == 0b11000011

		//0xC0 == 0b11000000, 0x03 == 0b00000011

	}
	ms_wait(1000);
}

void do_mode(int8_t mode){
	switch(mode){
		//----�����F��----
		case 1:
			find_line();
			break;

		//----�g���[�X����J�n----
		case 2:
			trace();
			ms_wait(100);
			break;

		//----���[�^�`�F�b�N----
		case 3:
			mt_check();
			break;

		//----�Z���T�`�F�b�N----
		case 4:
			sensor_check();
			break;

		//----����Ȃ���PID�p�����[�^�[����----
		case 5:
			tuning_run();
			break;
	}
	ms_wait(1000);
}

void led_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // IO portB clock enable

	//LED�o�͐ݒ�
	GPIOB->MODER &= ~GPIO_MODER_MODER0_Msk; // initialize PB0 function
	GPIOB->MODER &= ~GPIO_MODER_MODER1_Msk; // initialize PB1 function
	GPIOB->MODER &= ~GPIO_MODER_MODER6_Msk; // initialize PB6 function
	GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk; // initialize PB7 function

	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER0_Pos); // set PB0 as GPIO for output
	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER1_Pos); // set PB1 as GPIO for output
	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER6_Pos); // set PB6 as GPIO for output
	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER7_Pos); // set PB7 as GPIO for output
}

//====�|�[�g�̏����ݒ�====
void button_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // IO portA clock enable

	//�X�C�b�`���͐ݒ�
	GPIOA->MODER &= ~GPIO_MODER_MODER11_Msk; // initialize PA11 function
	GPIOA->MODER &= ~GPIO_MODER_MODER12_Msk; // initialize PA12 function
	GPIOA->MODER &= ~GPIO_MODER_MODER15_Msk; // initialize PA15 function
	GPIOA->MODER |= (GPIO_MODE_INPUT << GPIO_MODER_MODER11_Pos); // set PA11 as GPIO for input
	GPIOA->MODER |= (GPIO_MODE_INPUT << GPIO_MODER_MODER12_Pos); // set PA12 as GPIO for input
	GPIOA->MODER |= (GPIO_MODE_INPUT << GPIO_MODER_MODER15_Pos); // set PA15 as GPIO for input
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR11_Msk;//GPIOx_PUPDR���W�X�^��PA11�̕�����������
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12_Msk;//GPIOx_PUPDR���W�X�^��PA12�̕�����������
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR15_Msk;//GPIOx_PUPDR���W�X�^��PA15�̕�����������
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR11_Pos);//PA11�v���A�b�v��R�L����
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR12_Pos);//PA12�v���A�b�v��R�L����
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR15_Pos);//PA15�v���A�b�v��R�L����
}

void init(void){
	sysclk_init(16);//SYSCLK:64MHz
	led_init();
	button_init();
	pwm_init();
	adc_init();
	usart_init(19200);
	tim6_init();
}

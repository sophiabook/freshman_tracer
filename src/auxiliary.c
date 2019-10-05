/*
 * auxiliary.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void select_mode(int8_t *mode){
	*mode = 0x00;
	uart_printf("%s%d\r", "mode:", *mode);//モード番号をUARTで送信
	while(!SW3){
		if(SW1){
			*mode += 1;
			*mode = (*mode + 16) % 16;
			uart_printf("%s%d\r", "mode:", *mode);//モード番号をUARTで送信
			ms_wait(250);
		}else if(SW2){
			*mode -= 1;
			*mode = (*mode + 16) % 16;
			uart_printf("%s%d\r", "mode:", *mode);//モード番号をUARTで送信
			ms_wait(250);
		}//SW1,SW2が押されてないときはUARTでモード番号を送信しない
		GPIOB->ODR &= ~0x000000C3U;//いったんLED全消灯
		GPIOB->ODR |= (uint32_t)(((*mode << 4) & 0xC0) | (*mode & 0x03));//LEDに現在の値を表示

		// *mode== 0== 0b0000 のとき GPIOB->ODR |= 0x00 == 0b00000000
		// *mode== 1== 0b0001 のとき GPIOB->ODR |= 0x01 == 0b00000001
		// *mode== 2== 0b0010 のとき GPIOB->ODR |= 0x02 == 0b00000010
		// *mode== 3== 0b0011 のとき GPIOB->ODR |= 0x03 == 0b00000011
		// *mode== 4== 0b0100 のとき GPIOB->ODR |= 0x40 == 0b01000000
		// *mode== 5== 0b0101 のとき GPIOB->ODR |= 0x41 == 0b01000001
		// *mode== 6== 0b0110 のとき GPIOB->ODR |= 0x42 == 0b01000010
		// *mode== 7== 0b0111 のとき GPIOB->ODR |= 0x43 == 0b01000011
		// *mode== 8== 0b1000 のとき GPIOB->ODR |= 0x80 == 0b10000000
		// *mode== 9== 0b1001 のとき GPIOB->ODR |= 0x81 == 0b10000001
		// *mode==10== 0b1010 のとき GPIOB->ODR |= 0x82 == 0b10000010
		// *mode==11== 0b1011 のとき GPIOB->ODR |= 0x83 == 0b10000011
		// *mode==12== 0b1100 のとき GPIOB->ODR |= 0xC0 == 0b11000000
		// *mode==13== 0b1101 のとき GPIOB->ODR |= 0xC1 == 0b11000001
		// *mode==14== 0b1110 のとき GPIOB->ODR |= 0xC2 == 0b11000010
		// *mode==15== 0b1111 のとき GPIOB->ODR |= 0xC3 == 0b11000011

		//0xC0 == 0b11000000, 0x03 == 0b00000011

	}
	ms_wait(1000);
}

void do_mode(int8_t mode){
	switch(mode){
		//----白線認識----
		case 1:
			find_line();
			break;

		//----トレース動作開始----
		case 2:
			trace();
			ms_wait(100);
			break;

		//----モータチェック----
		case 3:
			mt_check();
			break;

		//----センサチェック----
		case 4:
			sensor_check();
			break;

		//----走りながらPIDパラメーター調整----
		case 5:
			tuning_run();
			break;
	}
	ms_wait(1000);
}

void led_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // IO portB clock enable

	//LED出力設定
	GPIOB->MODER &= ~GPIO_MODER_MODER0_Msk; // initialize PB0 function
	GPIOB->MODER &= ~GPIO_MODER_MODER1_Msk; // initialize PB1 function
	GPIOB->MODER &= ~GPIO_MODER_MODER6_Msk; // initialize PB6 function
	GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk; // initialize PB7 function

	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER0_Pos); // set PB0 as GPIO for output
	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER1_Pos); // set PB1 as GPIO for output
	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER6_Pos); // set PB6 as GPIO for output
	GPIOB->MODER |= (GPIO_MODE_OUTPUT_PP << GPIO_MODER_MODER7_Pos); // set PB7 as GPIO for output
}

//====ポートの初期設定====
void button_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // IO portA clock enable

	//スイッチ入力設定
	GPIOA->MODER &= ~GPIO_MODER_MODER11_Msk; // initialize PA11 function
	GPIOA->MODER &= ~GPIO_MODER_MODER12_Msk; // initialize PA12 function
	GPIOA->MODER &= ~GPIO_MODER_MODER15_Msk; // initialize PA15 function
	GPIOA->MODER |= (GPIO_MODE_INPUT << GPIO_MODER_MODER11_Pos); // set PA11 as GPIO for input
	GPIOA->MODER |= (GPIO_MODE_INPUT << GPIO_MODER_MODER12_Pos); // set PA12 as GPIO for input
	GPIOA->MODER |= (GPIO_MODE_INPUT << GPIO_MODER_MODER15_Pos); // set PA15 as GPIO for input
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR11_Msk;//GPIOx_PUPDRレジスタのPA11の部分を初期化
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12_Msk;//GPIOx_PUPDRレジスタのPA12の部分を初期化
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR15_Msk;//GPIOx_PUPDRレジスタのPA15の部分を初期化
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR11_Pos);//PA11プルアップ抵抗有効化
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR12_Pos);//PA12プルアップ抵抗有効化
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR15_Pos);//PA15プルアップ抵抗有効化
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

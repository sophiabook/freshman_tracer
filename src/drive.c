/*
 * drive.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void pwm_init(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // IO portA clock enable

	// PA0 を Alternate Function に設定する
	GPIOA->MODER &= ~(1 << 0);
	GPIOA->MODER |= 1 << 1;
	// PA0 を AF1 に設定する
	GPIOA->AFR[0] |= (GPIO_AF1_TIM2 << GPIO_AFRL_AFRL0_Pos);

	// PA1 を Alternate Function に設定する
	GPIOA->MODER &= ~(1 << 2);
	GPIOA->MODER |= 1 << 3;
	// PA1 を AF1 に設定する
	GPIOA->AFR[0] |= (GPIO_AF1_TIM2 << GPIO_AFRL_AFRL1_Pos);


	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Clock supply enable

	TIM2->CR1 |= TIM_CR1_CEN;						//タイマ有効
	TIM2->CR2 = 0;
	TIM2->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE |
			TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;	//PWMモード1
	TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;		//TIM2_CH1,CH2出力有効
	TIM2->BDTR = TIM_BDTR_MOE;

	TIM2->CNT = 0;					//タイマカウンタ値を0にリセット
	TIM2->PSC = 64-1;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM2->ARR = INTERVAL;	//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定
	TIM2->CCR1 = 0;				//タイマカウンタの比較一致値
	TIM2->CCR2 = 0;				//タイマカウンタの比較一致値

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // IO portB clock enable

	// PB4 を Alternative Function に設定する
	GPIOB->MODER &= ~(1 << 8);
	GPIOB->MODER |= 1 << 9;
	// PB4 を AF2 に設定する
	GPIOB->AFR[0] |= (GPIO_AF2_TIM3 << GPIO_AFRL_AFRL4_Pos);

	// PB5 を Alternative Function に設定する
	GPIOB->MODER &= ~(1 << 10);
	GPIOB->MODER |= 1 << 11;
	// PB5 を AF2 に設定する
	GPIOB->AFR[0] |= (GPIO_AF2_TIM3 << GPIO_AFRL_AFRL5_Pos);

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Clock supply enable

	TIM3->CR1 = TIM_CR1_CEN;						//タイマ有効
	TIM3->CR2 = 0;
	TIM3->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE |
			TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;	//PWMモード1
	TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;		//TIM2_CH1,CH2出力有効
	TIM3->BDTR = TIM_BDTR_MOE;

	TIM3->CNT = 0;					//タイマカウンタ値を0にリセット
	TIM3->PSC = 64-1;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM3->ARR = INTERVAL;	//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定
	TIM3->CCR1 = 0;				//タイマカウンタの比較一致値
	TIM3->CCR2 = 0;				//タイマカウンタの比較一致値

}

void motor_out(uint16_t state){
	if(state == ON){
		NVIC_EnableIRQ(TIM2_IRQn); // assign to NVIC.
		NVIC_EnableIRQ(TIM3_IRQn); // assign to NVIC.
		NVIC_SetPriority(TIM2_IRQn, 2);//割り込み優先度を設定
		NVIC_SetPriority(TIM3_IRQn, 3);//割り込み優先度を設定
		TIM2->CR1 |= TIM_CR1_CEN;//タイマ有効
		TIM3->CR1 |= TIM_CR1_CEN;//タイマ有効
	}else if(state == OFF){
		motor_R(0.0F);//右モーター停止
		motor_L(0.0F);//左モーター停止
		NVIC_DisableIRQ(TIM2_IRQn);//割り込みハンドラを無効化(するかもしれない)
		NVIC_DisableIRQ(TIM3_IRQn);//割り込みハンドラを無効化(するかもしれない)
		TIM2->CR1 &= ~TIM_CR1_CEN;//タイマ無効
		TIM3->CR1 &= ~TIM_CR1_CEN;//タイマ無効
		TIM2->CNT = 0;				// Reset Counter
		TIM3->CNT = 0;				// Reset Counter
	}
}



//====右モータ回転====
void motor_R(float duty){//-1.0<=duty<0.0なら逆回転、duty==0.0なら停止、0.0<duty<=1.0なら正回転
	if(duty >= 0.0F){//正回転または停止
		if(MOTOR_R_FOWARD == 1){//MOTOR_R_FOWARDが1か2かはどう配線するかで決まる
			TIM2->CCR1 = INTERVAL * duty;
			TIM2->CCR2 = 0;
		}else{
			TIM2->CCR1 = 0;
			TIM2->CCR2 = INTERVAL * duty;
		}
	}else{//逆回転
		if(MOTOR_R_FOWARD == 2){//MOTOR_R_FOWARDが1か2かはどう配線するかで決まる
			TIM2->CCR1 = INTERVAL * (-duty);
			TIM2->CCR2 = 0;
		}else{
			TIM2->CCR1 = 0;
			TIM2->CCR2 = INTERVAL * (-duty);
		}
	}
}

//====左モータ回転====
void motor_L(float duty){//duty>0なら正回転、duty==0なら停止、duty<0なら逆回転
	if(duty >= 0.0F){//正回転または停止
		if(MOTOR_L_FOWARD == 1){//MOTOR_R_FOWARDが1か2かはどう配線するかで決まる
			TIM3->CCR1 = INTERVAL * duty;
			TIM3->CCR2 = 0;
		}else{
			TIM3->CCR1 = 0;
			TIM3->CCR2 = INTERVAL * duty;
		}
	}else{//逆回転
		if(MOTOR_L_FOWARD == 2){//MOTOR_R_FOWARDが1か2かはどう配線するかで決まる
			TIM3->CCR1 = INTERVAL * (-duty);
			TIM3->CCR2 = 0;
		}else{
			TIM3->CCR1 = 0;
			TIM3->CCR2 = INTERVAL * (-duty);
		}
	}
}


//====モーターチェック====
void mt_check(void){
	uart_printf("%s\r", "mt_check");//文字列mt_checkをUARTで送信
	ms_wait(100);
	while(!SW3){
		if(SW1 && SW2){			//スイッチ1と2が同時に押されていれば
			motor_R(-DUTY_SLOW);//右モータ逆回転
			motor_L(-DUTY_SLOW);//左モータ逆回転
		}else if(SW1){			//スイッチ1だけが押されていれば
			motor_R(DUTY_SLOW);	//右モータ正回転
		}else if(SW2){			//スイッチ2だけが押されていれば
			motor_L(DUTY_SLOW);	//左モータ正回転
		}else{					//ボタンが押されてなければ
			motor_R(0.0F);		//右モータ停止
			motor_L(0.0F);		//左モータ停止
		}
	}
	motor_R(0.0F);				//右モータ停止
	motor_L(0.0F);				//左モータ停止
}





//====トレース開始====
void trace(void){
	uart_printf("%s\r", "trace start");
	TF.FLAG.STOP = 0;
	TF.FLAG.CTRL = 1;
	TF.FLAG.DEF = 1;
	pid_init();
	//motor_out(ON);
	sensor_out(ON);
	while(!SW3);
	sensor_out(OFF);
	//motor_out(OFF);
	motor_R(0.0F);
	motor_L(0.0F);
	TF.FLAGS = 0x00;
	TF.FLAG.STOP = 1;
}

void tuning_run(){
	uart_printf("%s\r", "tuning_run");
	TF.FLAG.STOP = 0;
	TF.FLAG.CTRL = 1;
	TF.FLAG.DEF = 1;
	pid_init();
	//motor_out(ON);
	sensor_out(ON);
	pid_tuning();
	sensor_out(OFF);
	//motor_out(OFF);
	motor_R(0.0F);
	motor_L(0.0F);
	TF.FLAGS = 0x00;
	TF.FLAG.STOP = 1;
}

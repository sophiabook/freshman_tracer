/*
 * sensor.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void tim6_init(void) {

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; // Clock supply enable
	TIM6->CR1 = TIM_CR1_CEN;		//タイマ有効
	TIM6->CR2 = 0;
	TIM6->DIER = TIM_DIER_UIE;		//タイマ更新割り込みを有効に
	TIM6->CNT = 0;					//タイマカウンタ値を0にリセット
	TIM6->PSC = 64-1;				//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM6->ARR = 1000-1;				//タイマカウンタの上限値。1000に設定。ゆえに1msごとに割り込み発生

	TIM6->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす
}


void sensor_out(uint16_t state){
	if(state == ON){
		NVIC_EnableIRQ(TIM6_DAC1_IRQn);			//タイマ更新割り込みハンドラを有効に
		NVIC_SetPriority(TIM6_DAC1_IRQn, 1);	//タイマ更新割り込みの割り込み優先度を設定
		ms_wait(10);//センサー4つ分のAD変換第1回目が終わるまで待機
	}else if(state == OFF){
		NVIC_DisableIRQ(TIM6_DAC1_IRQn);		//割り込みハンドラを無効化(するかもしれない)
	}
}

//====AD変換の初期化====
void adc_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // IO portA clock enable
	//
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER4_Pos);//PA4をアナログモードに
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER5_Pos);//PA5をアナログモードに
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER6_Pos);//PA6をアナログモードに
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER7_Pos);//PA7をアナログモードに

	ADC2->CFGR &= ~ADC_CFGR_RES_Msk;//分解能を12ビット長に
	ADC2->CFGR &= ~ADC_CFGR_ALIGN_Msk;//データを右詰めに

	RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1; // PLL 1x prescale supply
    RCC->AHBENR |= RCC_AHBENR_ADC12EN;
    ADC2->CR &= ~ADC_CR_ADVREGEN_Msk;
    //while(ADC2->CR & ADC_CR_ADVREGEN_Msk);
    ADC2->CR |= ADC_CR_ADVREGEN_0; // enable voltage regulator
    //while(!(ADC2->CR & ADC_CR_ADVREGEN_0));
    //ADC2->CR &= ~ADC_CR_ADCALDIF;
    //ADC2->CR |= ADC_CR_ADCAL;
    //while(ADC2->CR & ADC_CR_ADCAL);
    //ADC2->SMPR1 &= ~(ADC_SMPR1_SMP1 | ADC_SMPR1_SMP2 | ADC_SMPR1_SMP3 | ADC_SMPR1_SMP4);
    ADC2->SQR1 &= ~ADC_SQR1_L_Msk; // 1 channel
    ADC2->CR |= ADC_CR_ADEN; // ADC1 peripheral enabled
    //while(!(ADC2->ISR & ADC_ISR_ADRDY));
    //while(~(ADC2->ISR & ADC_ISR_ADRDY));だとループから抜けられないかも
}

//====AD変換の結果を取得する====
uint32_t adc_read(uint32_t ch){
	ADC2->SQR1 &= ~ADC_SQR1_SQ1_Msk; // delete the channel information
	ADC2->SQR1 |= (ch << ADC_SQR1_SQ1_Pos); // channel number of ADC1
    ADC2->CR |= ADC_CR_ADSTART; // start a/d convert
	while(!(ADC2->ISR & ADC_ISR_EOC_Pos)); // wait until ADC is done
	ADC2->ISR |= ADC_ISR_EOC;
    return ADC2->DR;
}


//====センサチェック====
void sensor_check(void){
	uart_printf("%s\r", "sensor_check");//文字列ad_checkをUARTで送信
	ms_wait(200);
	sensor_out(ON);
    while(!SW3){
		GPIOB->ODR &= ~0x000000C3U;//いったんLED全消灯
		if(ad_l < 4096 * THRESHOLD){		//左外側のセンサ反応で
			LED4(ON);
		}
		if(ad_fl < 4096 * THRESHOLD){		//左内側のセンサ反応で
			LED3(ON);
		}
		if(ad_fr < 4096 * THRESHOLD){		//右内側のセンサ反応で
			LED2(ON);
		}
		if(ad_r < 4096 * THRESHOLD){		//右外側のセンサ反応で
			LED1(ON);
		}
		//センサのAD値をPCに出力
		uart_printf("%s%d\t%s%d\t%s%d\t%s%d\r", "ad_l:", ad_l, "ad_fl:", ad_fl, "ad_fr:", ad_fr, "ad_r:", ad_r);
		ms_wait(1000);
	}
    sensor_out(OFF);
    GPIOB->ODR &= ~0x000000C3U;//いったんLED全消灯
}

void TIM6_DAC1_IRQHandler(void){

	static uint8_t tp = 0;
	if( !(TIM6->SR & TIM_SR_UIF) ){
		return;
	}

	int32_t val_diff;
	uint32_t val_sum;
	float pid;

	switch(tp){

		//----センサ処理----
		case 0:
			//右センサ値の取得
			ad_r = adc_read(IR_SENSOR_R);//受光部のアナログ値を記録
			//左センサ値の取得
			ad_l = adc_read(IR_SENSOR_L);//受光部のアナログ値を記録
			break;

		case 1:
			//正面右センサ値の取得
			ad_fr = adc_read(IR_SENSOR_FR);//受光部のアナログ値を記録
			//正面左センサ値の取得
			ad_fl = adc_read(IR_SENSOR_FL);//受光部のアナログ値を記録
			break;

		case 2:
			if(TF.FLAG.CTRL){
				val_diff = (int32_t)(ad_fl - adfl_min) - (int32_t)(ad_fr - adfr_min);
				val_sum = ad_fl + ad_fr;
				pid = pid_sample(val_sum, set_point);
				if((val_diff > CENTER_RANGE) && (val_sum < val_min + 500)){
					position = LEFT;
				}else if((val_diff < -CENTER_RANGE) && (val_sum < val_min + 500)){
					position = RIGHT;
				}
				if((ad_l < threshold_l) && (ad_r > threshold_r)){//左センサー白線検知、右非検知
					position = RIGHT;
				}else if((ad_r < threshold_r) && (ad_l > threshold_l)){//右センサー白線検知、左非検知
					position = LEFT;
				}else if((ad_l < threshold_l) && (ad_r < threshold_r)){//左右センサー白線検知
					//白線の交差点
				}
				if(position == LEFT){
					motor_L(math_limit(power + pid, -1.0F, 1.0F));
					motor_R(math_limit(power - pid, -1.0F, 1.0F));
				}else if(position == RIGHT){
					motor_L(math_limit(power - pid, -1.0F, 1.0F));
					motor_R(math_limit(power + pid, -1.0F, 1.0F));
				}
			}
			break;
	}

	//====タスクポインタを進める====
	tp = (tp+1) % 3;

	ms_count++;

	TIM6->SR &= ~TIM_SR_UIF;
}

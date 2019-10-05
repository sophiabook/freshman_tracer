/*
 * sensor.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void tim6_init(void) {

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; // Clock supply enable
	TIM6->CR1 = TIM_CR1_CEN;		//�^�C�}�L��
	TIM6->CR2 = 0;
	TIM6->DIER = TIM_DIER_UIE;		//�^�C�}�X�V���荞�݂�L����
	TIM6->CNT = 0;					//�^�C�}�J�E���^�l��0�Ƀ��Z�b�g
	TIM6->PSC = 64-1;				//�^�C�}�̃N���b�N���g�����V�X�e���N���b�N/64=1MHz�ɐݒ�
	TIM6->ARR = 1000-1;				//�^�C�}�J�E���^�̏���l�B1000�ɐݒ�B�䂦��1ms���ƂɊ��荞�ݔ���

	TIM6->EGR = TIM_EGR_UG;			//�^�C�}�ݒ�𔽉f�����邽�߂Ƀ^�C�}�X�V�C�x���g���N����
}


void sensor_out(uint16_t state){
	if(state == ON){
		NVIC_EnableIRQ(TIM6_DAC1_IRQn);			//�^�C�}�X�V���荞�݃n���h����L����
		NVIC_SetPriority(TIM6_DAC1_IRQn, 1);	//�^�C�}�X�V���荞�݂̊��荞�ݗD��x��ݒ�
		ms_wait(10);//�Z���T�[4����AD�ϊ���1��ڂ��I���܂őҋ@
	}else if(state == OFF){
		NVIC_DisableIRQ(TIM6_DAC1_IRQn);		//���荞�݃n���h���𖳌���(���邩������Ȃ�)
	}
}

//====AD�ϊ��̏�����====
void adc_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // IO portA clock enable
	//
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER4_Pos);//PA4���A�i���O���[�h��
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER5_Pos);//PA5���A�i���O���[�h��
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER6_Pos);//PA6���A�i���O���[�h��
	GPIOA->MODER |= (GPIO_MODE_ANALOG << GPIO_MODER_MODER7_Pos);//PA7���A�i���O���[�h��

	ADC2->CFGR &= ~ADC_CFGR_RES_Msk;//����\��12�r�b�g����
	ADC2->CFGR &= ~ADC_CFGR_ALIGN_Msk;//�f�[�^���E�l�߂�

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
    //while(~(ADC2->ISR & ADC_ISR_ADRDY));���ƃ��[�v���甲�����Ȃ�����
}

//====AD�ϊ��̌��ʂ��擾����====
uint32_t adc_read(uint32_t ch){
	ADC2->SQR1 &= ~ADC_SQR1_SQ1_Msk; // delete the channel information
	ADC2->SQR1 |= (ch << ADC_SQR1_SQ1_Pos); // channel number of ADC1
    ADC2->CR |= ADC_CR_ADSTART; // start a/d convert
	while(!(ADC2->ISR & ADC_ISR_EOC_Pos)); // wait until ADC is done
	ADC2->ISR |= ADC_ISR_EOC;
    return ADC2->DR;
}


//====�Z���T�`�F�b�N====
void sensor_check(void){
	uart_printf("%s\r", "sensor_check");//������ad_check��UART�ő��M
	ms_wait(200);
	sensor_out(ON);
    while(!SW3){
		GPIOB->ODR &= ~0x000000C3U;//��������LED�S����
		if(ad_l < 4096 * THRESHOLD){		//���O���̃Z���T������
			LED4(ON);
		}
		if(ad_fl < 4096 * THRESHOLD){		//�������̃Z���T������
			LED3(ON);
		}
		if(ad_fr < 4096 * THRESHOLD){		//�E�����̃Z���T������
			LED2(ON);
		}
		if(ad_r < 4096 * THRESHOLD){		//�E�O���̃Z���T������
			LED1(ON);
		}
		//�Z���T��AD�l��PC�ɏo��
		uart_printf("%s%d\t%s%d\t%s%d\t%s%d\r", "ad_l:", ad_l, "ad_fl:", ad_fl, "ad_fr:", ad_fr, "ad_r:", ad_r);
		ms_wait(1000);
	}
    sensor_out(OFF);
    GPIOB->ODR &= ~0x000000C3U;//��������LED�S����
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

		//----�Z���T����----
		case 0:
			//�E�Z���T�l�̎擾
			ad_r = adc_read(IR_SENSOR_R);//������̃A�i���O�l���L�^
			//���Z���T�l�̎擾
			ad_l = adc_read(IR_SENSOR_L);//������̃A�i���O�l���L�^
			break;

		case 1:
			//���ʉE�Z���T�l�̎擾
			ad_fr = adc_read(IR_SENSOR_FR);//������̃A�i���O�l���L�^
			//���ʍ��Z���T�l�̎擾
			ad_fl = adc_read(IR_SENSOR_FL);//������̃A�i���O�l���L�^
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
				if((ad_l < threshold_l) && (ad_r > threshold_r)){//���Z���T�[�������m�A�E�񌟒m
					position = RIGHT;
				}else if((ad_r < threshold_r) && (ad_l > threshold_l)){//�E�Z���T�[�������m�A���񌟒m
					position = LEFT;
				}else if((ad_l < threshold_l) && (ad_r < threshold_r)){//���E�Z���T�[�������m
					//�����̌����_
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

	//====�^�X�N�|�C���^��i�߂�====
	tp = (tp+1) % 3;

	ms_count++;

	TIM6->SR &= ~TIM_SR_UIF;
}

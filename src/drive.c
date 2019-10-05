/*
 * drive.c
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#include "global.h"

void pwm_init(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // IO portA clock enable

	// PA0 �� Alternate Function �ɐݒ肷��
	GPIOA->MODER &= ~(1 << 0);
	GPIOA->MODER |= 1 << 1;
	// PA0 �� AF1 �ɐݒ肷��
	GPIOA->AFR[0] |= (GPIO_AF1_TIM2 << GPIO_AFRL_AFRL0_Pos);

	// PA1 �� Alternate Function �ɐݒ肷��
	GPIOA->MODER &= ~(1 << 2);
	GPIOA->MODER |= 1 << 3;
	// PA1 �� AF1 �ɐݒ肷��
	GPIOA->AFR[0] |= (GPIO_AF1_TIM2 << GPIO_AFRL_AFRL1_Pos);


	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Clock supply enable

	TIM2->CR1 |= TIM_CR1_CEN;						//�^�C�}�L��
	TIM2->CR2 = 0;
	TIM2->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE |
			TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;	//PWM���[�h1
	TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;		//TIM2_CH1,CH2�o�͗L��
	TIM2->BDTR = TIM_BDTR_MOE;

	TIM2->CNT = 0;					//�^�C�}�J�E���^�l��0�Ƀ��Z�b�g
	TIM2->PSC = 64-1;					//�^�C�}�̃N���b�N���g�����V�X�e���N���b�N/64=1MHz�ɐݒ�
	TIM2->ARR = INTERVAL;	//�^�C�}�J�E���^�̏���l�B��芸����DEFAULT_INTERVAL(params.h)�ɐݒ�
	TIM2->CCR1 = 0;				//�^�C�}�J�E���^�̔�r��v�l
	TIM2->CCR2 = 0;				//�^�C�}�J�E���^�̔�r��v�l

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // IO portB clock enable

	// PB4 �� Alternative Function �ɐݒ肷��
	GPIOB->MODER &= ~(1 << 8);
	GPIOB->MODER |= 1 << 9;
	// PB4 �� AF2 �ɐݒ肷��
	GPIOB->AFR[0] |= (GPIO_AF2_TIM3 << GPIO_AFRL_AFRL4_Pos);

	// PB5 �� Alternative Function �ɐݒ肷��
	GPIOB->MODER &= ~(1 << 10);
	GPIOB->MODER |= 1 << 11;
	// PB5 �� AF2 �ɐݒ肷��
	GPIOB->AFR[0] |= (GPIO_AF2_TIM3 << GPIO_AFRL_AFRL5_Pos);

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Clock supply enable

	TIM3->CR1 = TIM_CR1_CEN;						//�^�C�}�L��
	TIM3->CR2 = 0;
	TIM3->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE |
			TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;	//PWM���[�h1
	TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;		//TIM2_CH1,CH2�o�͗L��
	TIM3->BDTR = TIM_BDTR_MOE;

	TIM3->CNT = 0;					//�^�C�}�J�E���^�l��0�Ƀ��Z�b�g
	TIM3->PSC = 64-1;					//�^�C�}�̃N���b�N���g�����V�X�e���N���b�N/64=1MHz�ɐݒ�
	TIM3->ARR = INTERVAL;	//�^�C�}�J�E���^�̏���l�B��芸����DEFAULT_INTERVAL(params.h)�ɐݒ�
	TIM3->CCR1 = 0;				//�^�C�}�J�E���^�̔�r��v�l
	TIM3->CCR2 = 0;				//�^�C�}�J�E���^�̔�r��v�l

}

void motor_out(uint16_t state){
	if(state == ON){
		NVIC_EnableIRQ(TIM2_IRQn); // assign to NVIC.
		NVIC_EnableIRQ(TIM3_IRQn); // assign to NVIC.
		NVIC_SetPriority(TIM2_IRQn, 2);//���荞�ݗD��x��ݒ�
		NVIC_SetPriority(TIM3_IRQn, 3);//���荞�ݗD��x��ݒ�
		TIM2->CR1 |= TIM_CR1_CEN;//�^�C�}�L��
		TIM3->CR1 |= TIM_CR1_CEN;//�^�C�}�L��
	}else if(state == OFF){
		motor_R(0.0F);//�E���[�^�[��~
		motor_L(0.0F);//�����[�^�[��~
		NVIC_DisableIRQ(TIM2_IRQn);//���荞�݃n���h���𖳌���(���邩������Ȃ�)
		NVIC_DisableIRQ(TIM3_IRQn);//���荞�݃n���h���𖳌���(���邩������Ȃ�)
		TIM2->CR1 &= ~TIM_CR1_CEN;//�^�C�}����
		TIM3->CR1 &= ~TIM_CR1_CEN;//�^�C�}����
		TIM2->CNT = 0;				// Reset Counter
		TIM3->CNT = 0;				// Reset Counter
	}
}



//====�E���[�^��]====
void motor_R(float duty){//-1.0<=duty<0.0�Ȃ�t��]�Aduty==0.0�Ȃ��~�A0.0<duty<=1.0�Ȃ琳��]
	if(duty >= 0.0F){//����]�܂��͒�~
		if(MOTOR_R_FOWARD == 1){//MOTOR_R_FOWARD��1��2���͂ǂ��z�����邩�Ō��܂�
			TIM2->CCR1 = INTERVAL * duty;
			TIM2->CCR2 = 0;
		}else{
			TIM2->CCR1 = 0;
			TIM2->CCR2 = INTERVAL * duty;
		}
	}else{//�t��]
		if(MOTOR_R_FOWARD == 2){//MOTOR_R_FOWARD��1��2���͂ǂ��z�����邩�Ō��܂�
			TIM2->CCR1 = INTERVAL * (-duty);
			TIM2->CCR2 = 0;
		}else{
			TIM2->CCR1 = 0;
			TIM2->CCR2 = INTERVAL * (-duty);
		}
	}
}

//====�����[�^��]====
void motor_L(float duty){//duty>0�Ȃ琳��]�Aduty==0�Ȃ��~�Aduty<0�Ȃ�t��]
	if(duty >= 0.0F){//����]�܂��͒�~
		if(MOTOR_L_FOWARD == 1){//MOTOR_R_FOWARD��1��2���͂ǂ��z�����邩�Ō��܂�
			TIM3->CCR1 = INTERVAL * duty;
			TIM3->CCR2 = 0;
		}else{
			TIM3->CCR1 = 0;
			TIM3->CCR2 = INTERVAL * duty;
		}
	}else{//�t��]
		if(MOTOR_L_FOWARD == 2){//MOTOR_R_FOWARD��1��2���͂ǂ��z�����邩�Ō��܂�
			TIM3->CCR1 = INTERVAL * (-duty);
			TIM3->CCR2 = 0;
		}else{
			TIM3->CCR1 = 0;
			TIM3->CCR2 = INTERVAL * (-duty);
		}
	}
}


//====���[�^�[�`�F�b�N====
void mt_check(void){
	uart_printf("%s\r", "mt_check");//������mt_check��UART�ő��M
	ms_wait(100);
	while(!SW3){
		if(SW1 && SW2){			//�X�C�b�`1��2�������ɉ�����Ă����
			motor_R(-DUTY_SLOW);//�E���[�^�t��]
			motor_L(-DUTY_SLOW);//�����[�^�t��]
		}else if(SW1){			//�X�C�b�`1������������Ă����
			motor_R(DUTY_SLOW);	//�E���[�^����]
		}else if(SW2){			//�X�C�b�`2������������Ă����
			motor_L(DUTY_SLOW);	//�����[�^����]
		}else{					//�{�^����������ĂȂ����
			motor_R(0.0F);		//�E���[�^��~
			motor_L(0.0F);		//�����[�^��~
		}
	}
	motor_R(0.0F);				//�E���[�^��~
	motor_L(0.0F);				//�����[�^��~
}





//====�g���[�X�J�n====
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

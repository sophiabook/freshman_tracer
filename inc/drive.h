/*
 * drive.h
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#ifndef DRIVE_H_
#define DRIVE_H_

//====PWM====
#define INTERVAL 10000
#define DUTY_FAST 0.75F
#define DUTY_SLOW 0.5F

//====�@�̂��O�i����ۂ̃��[�^�̉�]����====
#define MOTOR_R_FOWARD 2//���ۂɃ��[�^�[���񂵂Ă݂�1��2�����߂�
#define MOTOR_L_FOWARD 1//���ۂɃ��[�^�[���񂵂Ă݂�1��2�����߂�

//====�@�̂̑O�i�E��ށE��~��\���ϐ�====
#define FWD 2
#define BACK 1

void pwm_init(void);
void motor_R(float duty);
void motor_L(float duty);
void mt_check(void);
void trace(void);
void tuning_run();
void motor_out(uint16_t state);

#endif /* DRIVE_H_ */

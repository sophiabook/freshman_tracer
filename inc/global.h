/*
 * global.h
 *
 *  Created on: 2018/03/03
 *      Author: iLand
 */

#include "stm32f3xx.h"
#include "uart.h"
#include "sensor.h"
#include "auxiliary.h"
#include "timer.h"
#include "drive.h"
#include "control.h"

#ifndef GLOBAL_H_
#define GLOBAL_H_

/*------------------------------------------------------------
		���p�E�\���̂̒�`
------------------------------------------------------------*/
//���p�E�\���̂Ƃ́A���p�̂ƍ\���̂�g�ݍ��킹�����̂ŁA
//�����̈ꊇ������A�����o�P�ʂł̑�����\�Ȃ��̂ł���B
//�Ⴆ�΁A�ȉ���tracer_flags���p�E�\���̂�MF�ł́A
//TF.FLAGS = 0x10;�ƑS�̂��ꊇ�ύX�ł���ق��A
//TF.FLAG.DECL = 1;�ƃ����o���w�肵�ĒP�̂ŕύX���邱�Ƃ��o����B
//�Q�l�F
//https://www.grapecity.com/tools/support/powernews/column/clang/033/page02.htm

//----�t���O���p�E�\����----
//���Ԃ͊Ԉ���Ă܂���
#ifndef __TRACER_FLAGS__					//�Ή��t�@�C���ň�x���ǂݍ��܂�Ă��Ȃ��ꍇ�ȉ����`
	#define __TRACER_FLAGS__				//�ǂݍ��񂾂��Ƃ�\��
	typedef union {						//���p�̂̐錾
		unsigned char FLAGS;
		struct tc_flags{				//�\���̂̐錾
			unsigned char RSV:1;		//�\���r�b�g(B0)		(:1��1�r�b�g���̈Ӗ��A�r�b�g�t�B�[���h)
			unsigned char SCND:1;		//�񎟃t���O(B1)
			unsigned char SLAL:1;		//����t���O(B2)
			unsigned char CTRL:1;		//����t���O(B3)
			unsigned char ACCL:1;		//�����t���O(B4)
			unsigned char DECL:1;		//�����t���O(B5)
			unsigned char DEF:1;		//�葬�t���O(B6)
			unsigned char STOP:1;		//��~�t���O(B7)
		}FLAG;
	} tracer_flags;
#endif

#ifdef EXTERN							//�Ή��t�@�C����EXTERN����`����Ă���ꍇ
	/*�O���[�o���ϐ��̒�`*/
	volatile tracer_flags TF;			//���[�^�X�e�[�^�X���܂Ƃ߂����p�E�\����
#else									//�Ή��t�@�C����EXTERN����`����Ă��Ȃ��ꍇ
	/*�O���[�o���ϐ��̐錾*/
	extern volatile tracer_flags TF;
#endif

#endif /* GLOBAL_H_ */

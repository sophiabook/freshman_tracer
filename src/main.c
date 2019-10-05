/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#define EXTERN
#include "global.h"


int main(void)
{
	TF.FLAGS = 0x00;
	TF.FLAG.STOP = 1;
	//----�ϐ��錾----
	int8_t mode;//���[�h�ϐ�

	//----�e�평����----
	init();
	//----���C�����[�v----
	while(1){
		select_mode(&mode); //���[�h�Z���N�g
		do_mode(mode);
	}
	return 0;
}

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
	//----変数宣言----
	int8_t mode;//モード変数

	//----各種初期化----
	init();
	//----メインループ----
	while(1){
		select_mode(&mode); //モードセレクト
		do_mode(mode);
	}
	return 0;
}

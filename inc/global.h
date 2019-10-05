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
		共用・構造体の定義
------------------------------------------------------------*/
//共用・構造体とは、共用体と構造体を組み合わせたもので、
//内部の一括操作も、メンバ単位での操作も可能なものである。
//例えば、以下のtracer_flags共用・構造体のMFでは、
//TF.FLAGS = 0x10;と全体を一括変更できるほか、
//TF.FLAG.DECL = 1;とメンバを指定して単体で変更することも出来る。
//参考：
//https://www.grapecity.com/tools/support/powernews/column/clang/033/page02.htm

//----フラグ共用・構造体----
//順番は間違ってません
#ifndef __TRACER_FLAGS__					//対応ファイルで一度も読み込まれていない場合以下を定義
	#define __TRACER_FLAGS__				//読み込んだことを表す
	typedef union {						//共用体の宣言
		unsigned char FLAGS;
		struct tc_flags{				//構造体の宣言
			unsigned char RSV:1;		//予備ビット(B0)		(:1は1ビット分の意味、ビットフィールド)
			unsigned char SCND:1;		//二次フラグ(B1)
			unsigned char SLAL:1;		//旋回フラグ(B2)
			unsigned char CTRL:1;		//制御フラグ(B3)
			unsigned char ACCL:1;		//加速フラグ(B4)
			unsigned char DECL:1;		//減速フラグ(B5)
			unsigned char DEF:1;		//定速フラグ(B6)
			unsigned char STOP:1;		//停止フラグ(B7)
		}FLAG;
	} tracer_flags;
#endif

#ifdef EXTERN							//対応ファイルでEXTERNが定義されている場合
	/*グローバル変数の定義*/
	volatile tracer_flags TF;			//モータステータスをまとめた共用・構造体
#else									//対応ファイルでEXTERNが定義されていない場合
	/*グローバル変数の宣言*/
	extern volatile tracer_flags TF;
#endif

#endif /* GLOBAL_H_ */

/*
 * control.c
 *
 *  Created on: 2018/03/11
 *      Author: iLand
 */

//PID制御の参考文献
//http://monoist.atmarkit.co.jp/mn/articles/1007/26/news083.html
//http://cranberrytree.blogspot.jp/2014/06/pid.html

#include "global.h"
static float kp = 0.0004F;//比例ゲイン
static float ki = 0.0F;//積分ゲイン
static float kd = 0.00002F;//微分ゲイン
static int32_t diff[2];
static float integral;
static uint32_t adl_min = 4095, adl_max = 0, adr_min = 4095, adr_max = 0;

void pid_init(void){
	diff[0] = diff[1] = 0;
	integral = 0.0F;
}

void pid_set(float k_p, float k_i, float k_d){
	if(k_p > 0.0F){
		kp = k_p;
	}else{
		kp = 0.0F;
	}
	if(k_i > 0.0F){
		ki = k_p;
	}else{
		ki = 0.0F;
	}
	if(k_d > 0.0F){
		kd = k_d;
	}else{
		kd = 0.0F;
	}
}

void pid_tuning(){
	uart_printf("%s\r", "pid_tuning");
	uart_printf("%s%f\t%s%f\t%s%f\t%s%f\r", "kp=", kp, "ki=", ki, "kd=", kd, "power=", power);
	float kp_step = 0.00001F, ki_step = 0.0000001F, kd_step = 0.000001F, p_step = 0.01F;
	uart_printf("%s%f\t%s%f\t%s%f\t%s%f\r", "kp_step=", kp_step, "ki_step=", ki_step, "kd_step=", kd_step, "p_step=", p_step);
	char c = get_char();
	while(c != 'q'){
		if(c == 'a'){              //kp +
			kp += kp_step;
		}else if(c == 'z'){         //kp -
			kp -= kp_step;
			if(kp < 0.0F){
				kp = 0.0F;
			}
		}else if(c == 's'){         //ki +
			ki += ki_step;
		}else if(c == 'x'){         //ki -
			ki -= ki_step;
			if(ki < 0.0F){
				ki = 0.0F;
			}
		}else if(c == 'd'){         //kd +
			kd += kd_step;
		}else if(c == 'c'){         //kd -
			kd -= kd_step;
			if(kd < 0.0F){
				kd = 0.0F;
			}
		}else if(c == 'f'){         //motor power +
			power += p_step;
			if(power > 1.0F){
				power = 1.0F;
			}
		}else if(c == 'v'){         //motor power -
			power -= p_step;
			if(power < 0.0F){
				power = 0.0F;
			}
		}else if(c == 'g'){         //motor on
			motor_out(ON);
		}else if(c == 'b'){         //motor off
			motor_out(OFF);
		}
		uart_printf("%s%f\t%s%f\t%s%f\t%s%f\r", "kp=", kp, "ki=", ki, "kd=", kd, "power=", power);
		c = get_char();
	}
}

void find_line(void){
	uart_printf("%s\r", "find_line");
	TF.FLAG.CTRL = 0;
	ms_count = 0;
	GPIOB->ODR |= 0x000000C3U;//LED全点灯
	sensor_out(ON);
	while(ms_count < 4000){
		if(ad_fl + ad_fr < val_min){
			val_min = ad_fl + ad_fr;
		}
		if(ad_l < adl_min){
			adl_min = ad_l;
		}
		if(adl_max < ad_l){
			adl_max = ad_l;
		}
		if(ad_r < adr_min){
			adr_min = ad_r;
		}
		if(adr_max < ad_r){
			adr_max = ad_r;
		}
		if(ad_fl < adfl_min){
			adfl_min = ad_fl;
		}
		if(ad_fr < adfr_min){
			adfr_min = ad_fr;
		}
	}
	sensor_out(OFF);
	set_point = val_min;
	threshold_l = (adl_min + adl_max) / 2;
	threshold_r = (adr_min + adr_max) / 2;
	GPIOB->ODR &= ~0x000000C3U;//LED全消灯
	ms_count = 0;
	uart_printf("%s%d\r", "set_point=", set_point);
	uart_printf("%s%d\t%s%d\r", "threshold_l=", threshold_l, "threshold_r=", threshold_r);
	uart_printf("%s%d\t%s%d\r", "adfl_min=", adfl_min, "adfr_min=", adfr_min);
	uart_printf("%s%d\t%s%d\r", "adl_min=", adl_min, "adr_min=", adr_min);
	uart_printf("%s%d\t%s%d\r", "adl_max=", adl_max, "adr_max=", adr_max);
}


float pid_sample(uint32_t input_val, uint32_t target_val){
	float p, i, d;

	diff[0] = diff[1];
	diff[1] = (int32_t)input_val - (int32_t)target_val;
	integral += (diff[1] + diff[0]) / 2.0 * DELTA_T;

	p = kp * diff[1];
	i = ki * integral;
	d = kd * (diff[1] - diff[0]) / DELTA_T;

	return math_limit(p + i + d, -1.0F, 1.0F);
}

float math_limit(float val, float lower, float upper){
	if(val < lower){
		return lower;
	}else if(upper < val){
		return upper;
	}else{
		return val;
	}
}

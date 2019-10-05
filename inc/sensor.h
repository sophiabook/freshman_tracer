/*
 * sensor.h
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#ifndef SENSOR_H_
#define SENSOR_H_

//====センサ設定====
#define IR_SENSOR_R  4
#define IR_SENSOR_FR 3
#define IR_SENSOR_FL 2
#define IR_SENSOR_L  1

//====センサ閾値====
#define THRESHOLD 0.5F

#ifdef EXTERN
volatile uint32_t ad_l, ad_fl, ad_fr, ad_r;				//センサAD値格納変数．1が内側
#else
extern volatile uint32_t ad_l, ad_fl, ad_fr, ad_r;
#endif

void tim6_init(void);
void sensor_out(uint16_t state);
void adc_init();
uint32_t adc_read(uint32_t ch);
void sensor_check(void);

#endif /* SENSOR_H_ */

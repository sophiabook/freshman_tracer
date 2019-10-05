/*
 * control.h
 *
 *  Created on: 2018/03/11
 *      Author: iLand
 */

#ifndef CONTROL_H_
#define CONTROL_H_

//====PID====
#define DELTA_T 0.003//処理周期（4msecの場合）

//====位置====
#define RIGHT 0
#define LEFT 1
#define CENTER_RANGE 100

#ifdef EXTERN
volatile float power = 1.0F;//モーター出力目標値
volatile uint32_t set_point;
volatile uint32_t val_min = 8190;//センサー値はそれぞれ12bit分解能で和の最大値が2 ^ 12 + 2 ^ 12 == 4095 + 4095
volatile uint32_t position;
volatile uint32_t threshold_l, threshold_r;
volatile uint32_t adfl_min = 4095, adfr_min = 4095;
#else
extern volatile float power;
extern volatile uint32_t set_point;
extern volatile uint32_t val_min;
extern volatile uint32_t position;
volatile uint32_t threshold_l, threshold_r;
extern volatile uint32_t adfl_min, adfr_min;
#endif

void pid_init(void);
void pid_tuning(void);
void pid_set(float, float, float);
void find_line(void);
float pid_sample(uint32_t, uint32_t);
float math_limit(float, float, float);

#endif /* CONTROL_H_ */

/*
 * timer.h
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifdef EXTERN
volatile uint16_t ms_count;//モーター出力目標値(%)
#else
extern volatile uint16_t ms_count;
#endif

void ms_wait(uint32_t ms);
void sysclk_init(uint8_t multiple);

#endif /* TIMER_H_ */

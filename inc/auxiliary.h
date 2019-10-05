/*
 * auxiliary.h
 *
 *  Created on: 2018/03/08
 *      Author: iLand
 */

#ifndef AUXILIARY_H_
#define AUXILIARY_H_

//====スイッチ設定====
//押されたら1以上、押されてないと0
#define SW1 !(GPIOA->IDR & GPIO_IDR_11)
#define SW2 !(GPIOA->IDR & GPIO_IDR_12)
#define SW3 !(GPIOA->IDR & GPIO_IDR_15)

//====LED設定====
#define LED1(x) (x ? (GPIOB->ODR |= GPIO_ODR_0) : (GPIOB->ODR &= ~GPIO_ODR_0))
#define LED2(x) (x ? (GPIOB->ODR |= GPIO_ODR_1) : (GPIOB->ODR &= ~GPIO_ODR_1))
#define LED3(x) (x ? (GPIOB->ODR |= GPIO_ODR_6) : (GPIOB->ODR &= ~GPIO_ODR_6))
#define LED4(x) (x ? (GPIOB->ODR |= GPIO_ODR_7) : (GPIOB->ODR &= ~GPIO_ODR_7))

//====汎用====
#define ON 1
#define OFF 0

void select_mode(int8_t *mode);
void do_mode(int8_t);
void led_init(void);
void button_init(void);
void init(void);

#endif /* AUXILIARY_H_ */

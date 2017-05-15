#ifndef _DELAY_H_
#define _DELAY_H_

#include <stdint.h>

static volatile uint32_t TimingDelay;

void Systick_Init();
void delay_ms(volatile uint32_t nTime);
void delay_us(volatile uint32_t nTime);

#endif

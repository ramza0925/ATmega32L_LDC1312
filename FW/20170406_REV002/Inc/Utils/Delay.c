#include "Delay.h"
#include "stm32f10x_conf.h"

void Systick_Init()
{
    RCC_ClocksTypeDef rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);
    
    //1msec Tick »ý¼º.
    if(SysTick_Config(rcc_clocks.SYSCLK_Frequency/1000))
    {
        while(1);
    }
}
void delay_ms(volatile uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

void delay_us(volatile uint32_t nTime)
{
    volatile uint32_t cnt = nTime;
    
    while(cnt > 0)
    {
        cnt--;
    }
}

void SysTick_Handler()
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


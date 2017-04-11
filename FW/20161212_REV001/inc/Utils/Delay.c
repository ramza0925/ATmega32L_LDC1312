#include <ADuCM360.h>

#include <DioLib.h>
#include <IntLib.h>
#include <GptLib.h>

#include "Delay.h"

unsigned int dly_cnt;     // 1[msec] tmr interrupt count


void Timer_Init(){
    //--------- Timer 0 Setup 1 msec -----------------------
    GptLd(pADI_TM0, 32);
    GptCfg(pADI_TM0,TCON_CLK_LFOSC,TCON_PRE_DIV1,TCON_MOD_PERIODIC|TCON_RLD_DIS|TCON_ENABLE_EN|TCON_UP_DIS );
    Timer_Start();
}

void Timer_Start(){
    NVIC_EnableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Enable !!!
}

void Timer_Stop(){
    NVIC_DisableIRQ(TIMER0_IRQn);
}

void delay_us(long int length){ 
   while (--length > 0){
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
   }
}

void delay_ms (long int length){
   dly_cnt = 0;
   while (length > dly_cnt) {};
}

void GP_Tmr0_Int_Handler(void){
   // Timer0 Interrupt : 1msec
   GptClrInt(pADI_TM0, TSTA_TMOUT);
   dly_cnt++; 
}


#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <ADuCM360.h>
#include <ClkLib.h>
#include <IntLib.h>
#include <DioLib.h>
#include <WdtLib.h>
#include <UrtLib.h>
#include <FeeLib.h>

#include "inc\GLCD\SPLC501C.h"
#include "inc\GLCD\graphic.h"

#include "inc\Variable.h"
#include "inc\Utils\Flash.h"
#include "inc\Utils\Delay.h"
#include "inc\LDC\LDC1612.h"
#include "inc\Menu\Menu.h"

#define SW_ENT			0xE0
#define SW_UP			0xB0
#define SW_NEXT			0xD0
#define SW_DOWN			0x70
#define SW_UPDN			0x30
#define SW_UP_ENT		0xA0
#define SW_DN_ENT		0x60
#define SW_DN_NEXT		0x50
#define SW_UP_NEXT		0x90
#define SW_UP_DN_NEXT	0x10

#define SWITCH_DELAY    300

//System Mode
enum{
	NORMAL_MODE = 0x60,
	GLCD_MODE = 0x40,
	UART_MODE = 0x20,
	BOTH_MODE = 0x00,
	SYS_MODE_CNT = 0x04,
}Sys_Mode;

//Operation Mode
enum{
	DP_MODE = 0,
	SET_MODE,
	DEBUG_MODE,
	OFFSET_MODE,
	CONTRAST_MODE,
	OP_MODE_CNT,
}Op_Mode;

uint8_t dpMode;
uint8_t sysMode;

#define FlashBufferSize 30
long FlashData[FlashBufferSize];

uint8_t index;
uint8_t lcd_contract;

#ifdef DATA_DEBUG
	uint16_t cnt;
#endif

void WatchDog_Initialize();
void Clock_Initialize();
void DIO_Initialize();
void UART_Initialize();
uint8_t Get_System_Mode();
void Read_Eeprom();
void Write_Eeprom();
void System_Mode_Check();
uint32_t Read_LDC();
float Cal_Distance(uint32_t ldc_d);
float CUR_Output(uint32_t ldc_d);
float Cal_Percent(float cur);
void CHK_Button();


void test(){
    
    offset = 0x00001258;
    
    while(TRUE){
        Set_Offset(CH0,offset);
        
        sprintf(LCD,"CH0: %08lX", Get_Offset(CH0));
		GLCD_GoTo(12, 7);
		GLCD_WriteString5x7(LCD);
        delay_us(50);
    }
}

int main(){
	uint16_t i;

    dpMode = DP_MODE;					    // 디스플레이 모드설정
    
    WatchDog_Initialize();                  // 32초후에 리셋이 실행되도록 와치독 초기화
    Clock_Initialize();                     // 클럭 초기화(16MHz)             
	DIO_Initialize();					    // 디지털 IO 초기화 
	Timer_Init();
	Read_Eeprom();						    // EEPROM에 저장된 값 읽어오기
	if(lcd_contract <= 0){
        lcd_contract = 3;                   //만약 LCD 밝기가 저장이 안되어있다 면 초기값 입력  
	}
    
	System_Mode_Check();                    // 시스템 모드 체크(0x00: 일반 모드, 0x20: 통신모드, 0x40: GLCD 모드)
	LDC_Init(offset);				        // LDC1612 초기화
	
	if(sysMode == GLCD_MODE){
	    reloadFlag = TRUE;                  //디버그 모드에서 화면 갱신용 플래그

        //오프셋 변수 배열에 저장
	    for(i=0; i<4; i++)
		    offset_arr[i] = (offset>>(12-(4*i))) & 0x000F;
	    debugPage = 0;
	}
#ifdef TEST
    test();
#endif
    while(TRUE){
        //WdtClrInt();                        // 와치독 타이머 리셋 
		ldc_data= Read_LDC();               // LDC 값 읽어오기
		distance = Cal_Distance(ldc_data);  // 거리 값으로 환산.
		current = CUR_Output(ldc_data);     // 전류 출력
		percent = Cal_Percent(current);     // 퍼센트 값 산출
		
		if(sysMode == GLCD_MODE){
			if(dpMode == DP_MODE){
				Dsp_Distance(distance);
			}
			else if(dpMode == SET_MODE){
				Dsp_Setting();
			}
			else if(dpMode == DEBUG_MODE){
				Dsp_Debugging();
			}
			else if(dpMode == OFFSET_MODE){
				Dsp_Offset();
			}
            else if(dpMode == CONTRAST_MODE){
				Dsp_Contrast();
			}
			CHK_Button();
		}
    }
}

void WatchDog_Initialize(){
    //WdtCfg(T3CON_PRE_DIV256,T3CON_IRQ_DIS,T3CON_PD_EN);   // Enable Watchdog timer to reset CPU on time-out
    //WdtLd(0x1000);                                        // Set timeout period to ~32 seconds
    //WdtGo(T3CON_ENABLE_EN);                               // Start the watchdog timer

    WdtCfg(T3CON_PRE_DIV1, T3CON_IRQ_EN,T3CON_PD_DIS);
}

void Clock_Initialize(){
    //---------- Disable clock to unused peripherals ----------
    ClkDis(CLKDIS_DISSPI0CLK|CLKDIS_DISSPI1CLK|CLKDIS_DISPWMCLK|CLKDIS_DIST1CLK|CLKDIS_DISDMACLK);
    // Select CD0 for CPU clock - 16Mhz clock
    ClkCfg(CLK_CD0,CLK_HF,CLKSYSDIV_DIV2EN_DIS,CLK_UCLKCG);
    // Select CD0 for UART System clock
    ClkSel(CLK_OFF,CLK_CD0,CLK_CD0,CLK_OFF);  

    //ClkDis(CLKDIS_DISSPI0CLK|CLKDIS_DISSPI1CLK|CLKDIS_DISI2CCLK|CLKDIS_DISPWMCLK|CLKDIS_DISDACCLK|CLKDIS_DIST1CLK);
    // Select CD0 for CPU clock - 16Mhz clock
    //ClkCfg(CLK_CD2,CLK_HF,CLKSYSDIV_DIV2EN_EN,CLK_UCLKCG);
    // Select CD0 for UART System clock
    //ClkSel(CLK_CD7,CLK_CD7,CLK_CD0,CLK_CD7); 
}

//DIO Initialize
void DIO_Initialize(){
    // GPIO0 Port Function Setting
    // P0.0 (NONE)      - GPIO  - IN
    // P0.1 (RXD)       - UART  - IN
    // P0.2 (TXD)       - UART_ - OUT
    // P0.3 (LDC_INTB)  - GPIO  - IN
    // P0.4 (BTN_ENTER) - GPIO  - IN
    // P0.5 (BTN_NEXT)  - GPIO  - IN
    // P0.6 (BTN_UP)    - GPIO  - IN
    // P0.7 (BTN_DOWN)  - GPIO  - IN
    DioCfg(pADI_GP0, 0x003C);
    DioOen(pADI_GP0, 0x04);
    DioPul(pADI_GP0, 0x00);

    // GPIO1 Port Function Setting
    // P1.0 (GLCD_/CS)  - GPIO  - OUT
    // P1.1 (GLCD_/RES) - GPIO  - OUT
    // P1.2 (GLCD_A0)   - GPIO  - OUT
    // P1.3 (GLCD_SCL)  - GPIO  - OUT
    // P1.4 (GLCD_SI)   - GPIO  - OUT
    // P1.5 (MODE0)     - GPIO  - IN
    // P1.6 (MODE1)     - GPIO  - IN
    // P1.7 (LDC_SD)    - GPIO  - OUT
    DioCfg(pADI_GP1, 0x0000);
    DioOen(pADI_GP1, 0x9F);
    DioPul(pADI_GP1, 0x00);

    // GPIO1 Port Function Setting
    // P2.0 (LDC_SCL)   - SCL  - OUT
    // P2.1 (LDC_SDA)   - SDA  - OUT
    // P2.2 (BM)        - BOOT - NONE
    //DioCfg(pADI_GP2, 0x0005);   // GPIO1 Port Function Setting
    //DioOen(pADI_GP2, 0xFF);
    //DioPul(pADI_GP2, 0x00);

    DioCfg(pADI_GP2, 0x0000);
    DioOce(pADI_GP2, BIT0|BIT1);
    DioOen(pADI_GP2, 0x03);
    DioPul(pADI_GP2, 0x00);
    
}

//EEPROM 읽기(초기에 한번 세팅화면에서 빠져나올때 한번)
void Read_Eeprom(){
    uint8_t i = 0, flashCnt = 0;
    
    NVIC_DisableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Enable !!!  
    ReadFromFlash(FlashData, 0x10000, sizeof(FlashData));
    NVIC_EnableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Enable !!!      

	for(i=0 ; i< LDC_LENGTH; i++){
		ldc[i] = (uint32_t)FlashData[flashCnt++];
	}
	lcd_contract = (uint8_t)FlashData[flashCnt++];
	offset = (uint16_t)FlashData[flashCnt++];
	//gain = (uint16_t)FlashData[flashCnt++];
}

void Write_Eeprom(){
    uint8_t i = 0, flashCnt = 0;
    uint32_t uiFEESTA;
    
    for(i=0 ; i< LDC_LENGTH; i++){
		FlashData[flashCnt++] = ldc[i];
	}
    FlashData[flashCnt++] = lcd_contract;
    FlashData[flashCnt++] = offset;

   NVIC_DisableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Enable !!!      
  
   FeePErs(0x10000);
   WriteToFlash(FlashData, 0x10000, sizeof(FlashData));
   
   do
      uiFEESTA = FeeSta();   
   //wait for the flash controller to finish  
   while((uiFEESTA & FEESTA_CMDBUSY) == FEESTA_CMDBUSY);  
   
   NVIC_EnableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Enable !!!      
}

//모드를 확인하여 해당 항목에 대한 초기화 진행 
void System_Mode_Check(){
	sysMode = Get_System_Mode();

#ifdef MODE_CHECK
    sysMode = GLCD_MODE;
#endif

	switch(sysMode){
		case GLCD_MODE:
			GLCD_Initialize();
			GLCD_ClearScreen();
			GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
			break;
		case UART_MODE:
			UART_Initialize();
			break;
        case BOTH_MODE:
            GLCD_Initialize();
			GLCD_ClearScreen();
			GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
            UART_Initialize();
            break;
	}

#ifdef MODE_CHECK
    sprintf(LCD,"MODE: %d",Get_System_Mode());
    GLCD_GoTo(16, 5);
    GLCD_WriteString5x7(LCD);
#endif
}

//System Mode Switch Check
uint8_t Get_System_Mode(){
    uint8_t sw_check = 0;

    sw_check = DioRd(pADI_GP1) & 0x60;

    return sw_check;
}


//UART Initialize
void UART_Initialize(){
}

//주파수 값 읽기
uint32_t Read_LDC(){
	uint32_t tmp = 0x00000000;

	if((Get_Status()&CH0_UNREAD_CONV) == CH0_UNREAD_CONV) {
	
		//LDC 값을 읽어온다.
		tmp |= Read_Data(DATA_CH0_MSB);
		tmp = (tmp << 16);
		tmp |= Read_Data(DATA_CH0_LSB);

		//오류가 없을 경우에만 데이터 저장.
		if((tmp&0xF0000000) == 0) ldc_data = tmp >> 8;
	}
	
#ifdef DATA_DEBUG
	if(dpMode == DP_MODE || dpMode == SET_MODE){
		sprintf(LCD,"DATA: %08lX", ldc_data);
		GLCD_GoTo(16, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif
	return ldc_data;
}

//주파수를 가지고 거리값 계산 
float Cal_Distance(uint32_t ldc_d){
	float dst = 0.0f, m = 0.0f;
	float y1, y2, x1, x2;
	uint8_t i;

	for(i = 0;i < LDC_LENGTH; i++){
		if(ldc[i] <= ldc_d) break;
	}

	index = i;
	y1 = (float)index-1;
	y2 = (float)index;
	x1 = (float)ldc[index-1];
	x2 = (float)ldc[index];
	m = (y2-y1)/(x2 - x1);
	dst = m*(ldc_d-x1) + y1;

#ifdef DST_DEBUG
	if(dpMode != DEBUG_MODE){
		sprintf(LCD,"DST: %2.2f(mm)",dst);
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif	
	return dst;
}

//주파수를 받아서 전류값계산 
float CUR_Output(uint32_t ldc_d){
	float cur = 0.0f, m = 0.0f;
	float x1, x2, y1, y2;

	y2 = MIN_CURR + UNIT_CURR * (float)index;
	y1 = MIN_CURR + UNIT_CURR * ((float)index-1);
	x1 = (float)ldc[index-1];
	x2 = (float)ldc[index];
	
	m = (y2 - y1)/(x2 - x1);
	cur = m * (ldc_d - x1) + y1;

	if(cur < MIN_CURR) cur = MIN_CURR;
	else if(cur > MAX_CURR) cur = MAX_CURR;
	
#ifdef CUR_DEBUG
	if(dpMode != DEBUG_MODE){
		sprintf(LCD,"CURRENT: %2.2f(mA)",cur);
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif

	//AD5421_Output(cur);

	return cur;
}

float Cal_Percent(float cur){
	float per;
	
	per = ((cur - MIN_CURR)/(MAX_CURR - MIN_CURR))*100.0f;
	if(per < 0.0f) per = 0.0f;
	else if(per >100.0f) per = 100.0f;

	return per;
}



//버튼 값을 확인하여 그에 맞는 기능을 수행
void CHK_Button(){
	uint8_t btn, i;
	uint16_t tmp = 0;

	btn = DioRd(pADI_GP0) & 0xF0;
#ifdef KEY_TEST
	sprintf(LCD, "Button Number: %02d", btn);
	GLCD_GoTo(12, 5);
	GLCD_WriteString5x7(LCD);
#endif
	switch(btn){
		case SW_ENT:
			if(dpMode == DP_MODE) {
				dpMode = CONTRAST_MODE;
				GLCD_ClearScreen();
			}
			else if(dpMode == SET_MODE){
                ldc[position] = ldc_data;
				sprintf(LCD, "%02dmm(%08lX)saved!",position, ldc_data);
				GLCD_GoTo(12, 4);
				GLCD_WriteString5x7(LCD);
				if(position < LDC_LENGTH -1 ) position++;
			}
			else if(dpMode == DEBUG_MODE){
				reloadFlag = TRUE;
				GLCD_ClearScreen();
			}
			else if (dpMode == OFFSET_MODE){
				for(i=0; i<4 ; i++){
					tmp |= offset_arr[i]<<(12-(4*i));
				}
				Offset_Setting(CH0,tmp);
				offset = Get_Offset(CH0);
				sprintf(LCD, "OFFSET %04X SAVE!",offset);
				GLCD_GoTo(12, 5);
				GLCD_WriteString5x7(LCD);
			}
			delay_ms(SWITCH_DELAY);
			break;
			
		case SW_NEXT:
			if(dpMode == DP_MODE) {
				if (++Display_Unit > 3) Display_Unit = 0;
                delay_ms(SWITCH_DELAY);
			}
			else if(dpMode == SET_MODE){
                Write_Eeprom();
				dpMode = DP_MODE;
				GLCD_ClearScreen();
			}
			else if(dpMode == DEBUG_MODE){
				dpMode = DP_MODE;
				debugPage = 0;
				GLCD_ClearScreen();
			}
			else if(dpMode == OFFSET_MODE){
				dpMode = SET_MODE;
				position = 0;
				GLCD_ClearScreen();
				GLCD_GoTo(40, 4);
				GLCD_WriteString5x7("None Act!!");
			}
            else if(dpMode == CONTRAST_MODE){
				dpMode = OFFSET_MODE;
				GLCD_ClearScreen();
			}
			delay_ms(SWITCH_DELAY);
			break;
			
		case SW_UP:
			if(dpMode == SET_MODE){
				if(position > 0) position--;
			}
			else if(dpMode == DEBUG_MODE){
				if(debugPage < 3) {
					debugPage++;
					GLCD_ClearScreen();
				}
			}
			else if(dpMode == OFFSET_MODE){
				if(offset_arr[position] == 0x000F) 
					offset_arr[position] = 0x0000;
				else
					offset_arr[position]++;
			}
            else if(dpMode == CONTRAST_MODE){
                if (++lcd_contract >= 7) lcd_contract = 7;
				GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
            }
			delay_ms(SWITCH_DELAY);
			break;
			
		case SW_DOWN:
			if(dpMode == SET_MODE){
				if(position < 10) position ++;
			}
			else if(dpMode == DEBUG_MODE){
				if(debugPage > 0) {
					debugPage--;
					GLCD_ClearScreen();
				}
			}
			else if(dpMode == OFFSET_MODE){
				if(position == 3) position = 0;
				else position++;
			}
            else if(dpMode == CONTRAST_MODE){
                if (--lcd_contract <= 0) lcd_contract = 0;
				GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
            }
			delay_ms(SWITCH_DELAY);
			break;

		case SW_UP_NEXT:
            dpMode = DEBUG_MODE;
			GLCD_ClearScreen();
            delay_ms(SWITCH_DELAY);
			break;
		case SW_UPDN:
			DioClr(pADI_GP1, 0x02);
			delay_ms(10);
			DioSet(pADI_GP1, 0x02);
			delay_ms(10);
			GLCD_Initialize();
			GLCD_ClearScreen();
			GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
            delay_ms(SWITCH_DELAY);
			break;
	}
}

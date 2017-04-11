#include <stdint.h>
#include <stdio.h>
//#include <math.h>

#include <ADuCM360.h>
#include <ClkLib.h>
#include <IntLib.h>
#include <DioLib.h>
#include <WdtLib.h>
//#include <UrtLib.h>
#include <FeeLib.h>
#include <AdcLib.h>
#include <DacLib.h>

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

//키를 누를때 가지는 딜레이
#define SWITCH_DELAY    200

 // DAC Default Output Values
#define DEFAULT4mA      0xD800000          // DAC value that nominally gives 4mA
#define DEFAULT20mA     0x5280000         // DAC value that nominally gives 20mA

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
	_4MA_SET_MODE,
	_20MA_SET_MODE,
	OP_MODE_CNT,
}Op_Mode;

uint8_t dpMode;
uint8_t sysMode;

#define FlashBufferSize 30
long FlashData[FlashBufferSize];

uint8_t index;                          //배열에서의 위치를 표시하기 위한 변수
uint8_t lcd_contract;                   //LDC 밝기를 저장하는 변수

uint32_t ulDacCode = 0;                 // Latest Code written to VDAC to set loop current
uint32_t ul4mAVal = 0;                  // Variable used to store DAC output code that generates 4mA
uint32_t ul20mAVal = 0;                 // Variable used to store DAC output code that generates 20mA
uint8_t ucADC0Rdy = 0;                  // Flag used in DAC calibration routine
int32_t lDAC4mAAIN9 = 0;                // AIN9 4mA calibration value
int32_t lDAC20mAAIN9 = 0;               // AIN9 20mA calibration value
int32_t ulADC0DAT = 0;                  // Variable used to store ADC0 result. used to calculate compensation value for DAC output

float fVolts = 0.0;                     // ADC to voltage constant
uint8_t ucFirstLoop = 0;                // Flag used to indicate the first pass through the main while loop. On 1st pass, the VDAC is setup coursely, after this it is fine-tuned
float fAIN9Voltage = 0.0;               // Used for determining AIN9 feedback voltage
float fCalVoltage = 0.0;                // Stores Expected AIN9 voltage for present Temperature reading
int32_t uiNumCodesAdjust = 0;


//volatile unsigned char ucADCERR = 0;    // Used to indicate an ADC error

uint32_t first_start = 0;               //메모리에 저장을 이전에 했는지 검사하는 변수

void WatchDog_Initialize();
void Clock_Initialize();
void DIO_Initialize();
void UART_Initialize();
uint8_t Get_System_Mode();
void Read_Eeprom();
void Write_Eeprom();
void System_Mode_Check();
void ADC0INIT();  
void DACINIT();
uint32_t Read_LDC();
float Cal_Distance(uint32_t ldc_d);
float Cal_Percent(float cur);
float Cal_Current(float percent);
void CUR_Output(float percent);
void CHK_Button();
void DisableIRQ();
void EnableIRQ();

int main(){
	uint16_t i;

    dpMode = DP_MODE;					    // 디스플레이 모드설정
    fVolts  = (1.2 / 268435456);            // Internal reference - calcualte LSB voltage value (28Bits)      
    ucFirstLoop = 1;
    
    WatchDog_Initialize();                  // 와치독 초기화
    Clock_Initialize();                     // 클럭 초기화(16MHz)             
	DIO_Initialize();					    // 디지털 IO 초기화 
	Timer_Init();
	Read_Eeprom();						    // EEPROM에 저장된 값 읽어오기

    //만약 처음 시작이라면값들을 임의로 넣고 초기화
	if(first_start != 100){
        for(i=0 ; i< LDC_LENGTH; i++){
		    ldc[i] = 0;
	    }
	    lcd_contract = 4;
	    offset = 0;
	    ul4mAVal = DEFAULT4mA;
        ul20mAVal = DEFAULT20mA;
        lDAC4mAAIN9 = 0;
        lDAC20mAAIN9= 0;
        first_start = 100;

        Write_Eeprom();
	}
	if(lcd_contract <= 0){
        lcd_contract = 3;                   //만약 LCD 밝기가 저장이 안되어있다 면 초기값 입력  
	}
    
	System_Mode_Check();                    // 시스템 모드 체크(0x00: 일반 모드, 0x20: 통신모드, 0x40: GLCD 모드)
	
	ADC0INIT();
    AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT);     // Enable ADC0 in continuous mode
    NVIC_EnableIRQ(ADC0_IRQn); 
    delay_ms(100);
    
    DACINIT();
    
	LDC_Init(offset);				        // LDC1612 초기화
	
	if(sysMode == GLCD_MODE){
	    reloadFlag = TRUE;                  //디버그 모드에서 화면 갱신용 플래그

        //오프셋 변수 배열에 저장
	    for(i=0; i<4; i++)
		    offset_arr[i] = (offset>>(12-(4*i))) & 0x000F;
	    debugPage = 0;
	}
    
    while(TRUE){
        //WdtClrInt();                          // 와치독 타이머 리셋 
		ldc_data= Read_LDC();                   // LDC 값 읽어오기
		distance = Cal_Distance(ldc_data);      // 거리 값으로 환산.
		percent = Cal_Percent(distance);        // 퍼센트 값 산출
		current = Cal_Current(percent);         // 전류 계산
        CUR_Output(percent);
		
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
            else if(dpMode == _4MA_SET_MODE){
				Dsp_4mA_Setting();
			}
            else if(dpMode == _20MA_SET_MODE){
				Dsp_20mA_Setting();
			}
			CHK_Button();
		}
    }
}

//와치독 초기화 (지금은 안씀)
void WatchDog_Initialize(){
    //32초뒤에 리셋
    //WdtCfg(T3CON_PRE_DIV256,T3CON_IRQ_DIS,T3CON_PD_EN);   // Enable Watchdog timer to reset CPU on time-out
    //WdtLd(0x1000);                                        // Set timeout period to ~32 seconds
    //WdtGo(T3CON_ENABLE_EN);                               // Start the watchdog timer

    //비활성화
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
    DioCfg(pADI_GP0, 0x403C);
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

void DisableIRQ(){
    NVIC_DisableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Disable !!! 
    NVIC_DisableIRQ(ADC0_IRQn);  // ADC0 Interrupt Disable !!! 
}
void EnableIRQ(){
    NVIC_EnableIRQ(TIMER0_IRQn);  // TMR0 Interrupt Enable !!!  
    NVIC_EnableIRQ(ADC0_IRQn);  // ADC0 Interrupt Enable !!!   
}


//EEPROM 읽기(초기에 한번 세팅화면에서 빠져나올때 한번)
void Read_Eeprom(){
    uint8_t i = 0, flashCnt = 0;

    DisableIRQ();
    ReadFromFlash(FlashData, 0x10000, sizeof(FlashData));
    EnableIRQ();

	for(i=0 ; i< LDC_LENGTH; i++){
		ldc[i] = (uint32_t)FlashData[flashCnt++];
	}
	lcd_contract = (uint8_t)FlashData[flashCnt++];
	offset = (uint16_t)FlashData[flashCnt++];
	ul4mAVal = (uint32_t)FlashData[flashCnt++];
    ul20mAVal = (uint32_t)FlashData[flashCnt++];
    lDAC4mAAIN9 = (int32_t)FlashData[flashCnt++];
    lDAC20mAAIN9= (int32_t)FlashData[flashCnt++];
    first_start = (uint32_t)FlashData[flashCnt++];
}

void Write_Eeprom(){
    uint8_t i = 0, flashCnt = 0;
    uint32_t uiFEESTA;
    
    for(i=0 ; i< LDC_LENGTH; i++){
		FlashData[flashCnt++] = ldc[i];
	}
    FlashData[flashCnt++] = lcd_contract;
    FlashData[flashCnt++] = offset;
    FlashData[flashCnt++] = ul4mAVal;
    FlashData[flashCnt++] = ul20mAVal;
    FlashData[flashCnt++] = lDAC4mAAIN9;
    FlashData[flashCnt++] = lDAC20mAAIN9;

    FlashData[flashCnt++] = first_start;

    DisableIRQ();     
  
    FeePErs(0x10000);
    WriteToFlash(FlashData, 0x10000, sizeof(FlashData));
   
    do
        uiFEESTA = FeeSta();   
    //wait for the flash controller to finish  
    while((uiFEESTA & FEESTA_CMDBUSY) == FEESTA_CMDBUSY);  

    EnableIRQ();
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

//ADC0 will measure AIN9 input - used to improve DAC output linearity performance
void ADC0INIT(){
    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                                         // Enable ADC0 in Idle mode
    AdcRng(pADI_ADC0,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G2,ADCCON_ADCCODE_INT);    // Internal Reference selected, PGA gain of 2
    AdcBuf(pADI_ADC0,ADCCFG_EXTBUF_OFF,ADCCON_BUFPOWN|ADCCON_BUFPOWP|ADCCON_BUFBYPP|ADCCON_BUFBYPN);                             // External Buffer OFF, ADC
    AdcPin(pADI_ADC0,ADCCON_ADCCN_AGND,ADCCON_ADCCP_AIN9);                      // Select AIN9 as the positive input; AGND as negative input
    AdcFlt(pADI_ADC0,124,0xD00,FLT_NORMAL);                                     // Select 3.75Hz sampling rate
    AdcMski(pADI_ADC0,ADCMSKI_RDY,1);                                           // Enable ADC0 interrupt
}

// Setup DAC for NPN output mode. Calibrate if necessary
void DACINIT(){
    DacCfg(DACCON_CLR_Off,DACCON_RNG_IntVref,DACCON_CLK_HCLK|DACCON_NPN,DACCON_MDE_12bit);
    DacWr(0,DEFAULT20mA);
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

//거리값에서 퍼센트 값을 계산
float Cal_Percent(float dst){
	float per;
	
	per = ((dst - MIN_DST)/(MAX_DST- MIN_DST))*100.0f;

	return per;
}

//퍼센트 값을 받아서 전류값을 계산 (디스플레이용)
float Cal_Current(float percent){
    float cur_dat;

    if(dpMode == _4MA_SET_MODE) cur_dat = 4.0f;
    else if(dpMode == _20MA_SET_MODE) cur_dat = 20.0f;
    else cur_dat = ((percent/100.0f)*(MAX_CURR-MIN_CURR))+MIN_CURR;  
    
    return cur_dat;
}

//퍼센트 값을  받아서 전류값출력 
void CUR_Output(float percent){
    //float cur_Adc_Percent = 0.0f;
    //float gap_Adc_Percent = 0.0f;

    if(dpMode == _4MA_SET_MODE) ulDacCode = ul4mAVal;
    else if(dpMode == _20MA_SET_MODE) ulDacCode = ul20mAVal;
    else ulDacCode = (uint32_t)((percent/100.0f)*((float)ul20mAVal-(float)ul4mAVal)+(float)ul4mAVal);
/*    else{
        ulDacCode = (uint32_t)((percent/100.0f)*((float)ul20mAVal-(float)ul4mAVal)+(float)ul4mAVal);
        DacWr(0,ulDacCode);
        AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                                     // Enable ADC0 in Idle mode
        delay_us(0xFF);                                                           // delay for AIN9 measurement to update
        ucADC0Rdy = 0;
        AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT);                                     // Enable ADC0 in continuous mode
        while (ucADC0Rdy == 0){}
        cur_Adc_Percent = ((ulADC0DAT - lDAC4mAAIN9)/(lDAC20mAAIN9 - lDAC4mAAIN9))*100.0f;
        gap_Adc_Percent = percent - cur_Adc_Percent;
        ulDacCode += (uint32_t)((gap_Adc_Percent/100.0f)*((float)ul20mAVal-(float)ul4mAVal)+(float)ul4mAVal); 
        DacWr(0,ulDacCode);
    }   
*/
    DacWr(0,ulDacCode);
#ifdef CUR_DEBUG
	if(dpMode != DEBUG_MODE){
		sprintf(LCD,"DAC: %08X",ulDacCode);
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif
}

//버튼 값을 확인하여 그에 맞는 기능을 수행
void CHK_Button(){
	uint8_t btn, i;
	uint16_t tmp = 0;

	btn = DioRd(pADI_GP0) & 0xF0;
#ifdef KEY_TEST
	sprintf(LCD, "Button Number: %02x", btn);
	GLCD_GoTo(12, 5);
	GLCD_WriteString5x7(LCD);
#endif
	switch(btn){
		case SW_ENT:
            switch(dpMode){
                case DP_MODE:
                    dpMode = CONTRAST_MODE;
				    GLCD_ClearScreen();
                    break;
                    
                case SET_MODE:
                    ldc[position] = ldc_data;
				    sprintf(LCD, "%02dmm(%08lX)saved!",position, ldc_data);
				    GLCD_GoTo(12, 4);
				    GLCD_WriteString5x7(LCD);
				    if(position < LDC_LENGTH -1 ) position++;
                    break;
                    
                case DEBUG_MODE:
                    reloadFlag = TRUE;
				    GLCD_ClearScreen();
                    break;
                    
                case OFFSET_MODE:
                    for(i=0; i<4 ; i++){
					    tmp |= offset_arr[i]<<(12-(4*i));
				    }
				    Offset_Setting(CH0,tmp);
				    offset = Get_Offset(CH0);
				    sprintf(LCD, "OFFSET %04X SAVE!",offset);
				    GLCD_GoTo(12, 5);
				    GLCD_WriteString5x7(LCD);
                    break;
            }
			delay_ms(SWITCH_DELAY);
			break;
			
		case SW_NEXT:
            switch(dpMode){
                case DP_MODE:
                    if (++Display_Unit > 3) Display_Unit = 0;
                    delay_ms(SWITCH_DELAY);
                    break;
                    
                case SET_MODE:
                    dpMode = _4MA_SET_MODE;
				    GLCD_ClearScreen();
                    break;
                    
                case DEBUG_MODE:
                    dpMode = DP_MODE;
				    debugPage = 0;
				    GLCD_ClearScreen();
                    break;
                    
                case OFFSET_MODE:
                    dpMode = SET_MODE;
				    position = 0;
				    GLCD_ClearScreen();
				    GLCD_GoTo(40, 4);
				    GLCD_WriteString5x7("None Act!!");
                    break;
                    
                case CONTRAST_MODE:
                    dpMode = OFFSET_MODE;
				    GLCD_ClearScreen();
                    break;
                    
                case _4MA_SET_MODE:
                    dpMode = _20MA_SET_MODE;
				    GLCD_ClearScreen();
                    break;
                    
                case _20MA_SET_MODE:
                    Write_Eeprom();
				    dpMode = DP_MODE;
				    GLCD_ClearScreen();
                    break;
            }
			delay_ms(SWITCH_DELAY);
			break;
			
		case SW_UP:
            switch(dpMode){
                case SET_MODE:
                    if(position > 0) position--;
                    break;
                case DEBUG_MODE:
                    if(debugPage < 3) {
					    debugPage++;
					    GLCD_ClearScreen();
				    }
                    break;
                    
                case OFFSET_MODE:
                    if(offset_arr[position] == 0x000F) offset_arr[position] = 0x0000;
				    else offset_arr[position]++;
                    break;
                    
                case CONTRAST_MODE:
                    if (++lcd_contract >= 7) lcd_contract = 7;
				    GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
                    break;
                    
                case _4MA_SET_MODE:
                    ul4mAVal = (ul4mAVal-0x10000);                                          // Current output increases when DAC voltage decreases 
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                                     // Enable ADC0 in Idle mode
                    delay_us(0xFF);                                                        // delay for AIN9 measurement to update
                    ucADC0Rdy = 0;
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT);                                     // Enable ADC0 in continuous mode
                    while (ucADC0Rdy == 0){}
                    lDAC4mAAIN9 = ulADC0DAT;
                    break;
                    
                case _20MA_SET_MODE:
                    ul20mAVal = (ul20mAVal-0x10000);                                        // Current output increases when DAC voltage decreases 
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                                     // Enable ADC0 in Idle mode
                    delay_us(0xFF);                                                           // delay for AIN9 measurement to update
                    ucADC0Rdy = 0;
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT);                                     // Enable ADC0 in continuous mode
                    while (ucADC0Rdy == 0){}
                    lDAC20mAAIN9 = ulADC0DAT;
                    break;
            }
			delay_ms(SWITCH_DELAY);
			break;
			
		case SW_DOWN:
            switch(dpMode){
                case SET_MODE:
                    if(position < 10) position ++;
                    break;
                    
                case DEBUG_MODE:
                    if(debugPage > 0) {
					    debugPage--;
					    GLCD_ClearScreen();
				    }
                    break;
                    
                case OFFSET_MODE:
                    if(position == 3) position = 0;
				    else position++;
                    break;
                    
                case CONTRAST_MODE:
                    if (--lcd_contract <= 0) lcd_contract = 0;
				    GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
                    break;
                    
                case _4MA_SET_MODE:
                    ul4mAVal = (ul4mAVal+0x10000);                                  // Current output increases when DAC voltage decreases 
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                                     // Enable ADC0 in Idle mode
                    delay_us(0xFF);                                                        // delay for AIN9 measurement to update
                    ucADC0Rdy = 0;
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT);                                     // Enable ADC0 in continuous mode
                    while (ucADC0Rdy == 0){}
                    lDAC4mAAIN9 = ulADC0DAT;
                    break;
                    
                case _20MA_SET_MODE:
                    ul20mAVal = (ul20mAVal+0x10000);                                  // Current output increases when DAC voltage decreases 
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                                     // Enable ADC0 in Idle mode
                    delay_us(0xFF);                                                           // delay for AIN9 measurement to update
                    ucADC0Rdy = 0;
                    AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT);                                     // Enable ADC0 in continuous mode
                    while (ucADC0Rdy == 0){}
                    lDAC20mAAIN9 = ulADC0DAT;
                    break;
            }
			delay_ms(SWITCH_DELAY);
			break;
            
        //디버그 모드 진입
		case SW_UP_NEXT:
            if(dpMode != DEBUG_MODE){
                dpMode = DEBUG_MODE;
			    GLCD_ClearScreen();
            }
            delay_ms(SWITCH_DELAY);
			break;
            
        //디스플레이 재 초기화 
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

        case SW_DN_ENT:
            NVIC_SystemReset();
            break;
	}
}


void ADC0_Int_Handler(){
    volatile uint32_t uiADCSTA = 0;
    static int32_t arr_ADC[12];
    static int64_t total_sum = 0;
    static uint8_t cnt = 0;
    static int32_t min = 0xffffffff;
    static int32_t max = 0x00000000;
   
    uiADCSTA = AdcSta(pADI_ADC0);
    if ((uiADCSTA & 0x10) != 0x10){                       // Check for an error condition
        if(cnt < 12){
            arr_ADC[cnt] = AdcRd(pADI_ADC0);
            if(arr_ADC[cnt] < min) min = arr_ADC[cnt];
            if(arr_ADC[cnt] > max) max = arr_ADC[cnt];
            total_sum += (int64_t)arr_ADC[cnt];
            cnt++;
        }
        else{
            ulADC0DAT =(int32_t)((total_sum - min - max)/10);
            ucADC0Rdy = 1;
            cnt = 0;
            total_sum = 0;
            min = 0xffffffff;
            max = 0x00000000;
        }
    }
}

/*
void ADC0_Int_Handler(){
    ulADC0DAT =AdcRd(pADI_ADC0);
    ucADC0Rdy = 1;
}
*/

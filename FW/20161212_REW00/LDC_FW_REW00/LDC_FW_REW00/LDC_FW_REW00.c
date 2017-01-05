//////////////////////////////////////////////////////////////
//               *******Clock*******                        //
//////////////////////////////////////////////////////////////
#define F_CPU 1000000UL

//////////////////////////////////////////////////////////////
//               *******Include*******                      //
//////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "INC/AD5421/AD5421.h"
#include "INC/GLCD/graphic.h"
#include "INC/GLCD/SPLC501C.h"
#include "INC/LDC1312/LDC1312.h"


//////////////////////////////////////////////////////////////
//               *******Variables*******                    //
//////////////////////////////////////////////////////////////
//#define DEBUG
//#define KEY_TEST
#define DATA_DEBUG
//#define DST_DEBUG
//#define CUR_DEBUG


#define TRUE			1
#define FALSE			0

#define SW_ENT			7
#define SW_UP			11
#define SW_NEXT			14
#define SW_DOWN			13
#define SW_UPDN			9
#define SW_UP_ENT		3
#define SW_DN_ENT		5
#define SW_DN_NEXT		12
#define SW_UP_NEXT		10
#define SW_UP_DN_NEXT	8

#define SW_IN					((PINC & 0xf0) >> 4)

#define SETBIT(ADDRESS,BIT)     (ADDRESS|=(1<<BIT))
#define CLEARBIT(ADDRESS,BIT)   (ADDRESS&=~(1<<BIT))
#define TESTBIT(ADDRESS,BIT)    (ADDRESS&(1<<BIT))

#define HW_REV		0x00
#define SW_REV		0x00

#define MIN_DST		0.0f
#define MAX_DST		10.0f
#define MIN_CURR	4.0f
#define MAX_CURR	20.0f

//System Mode
enum{
	NORMAL_MODE = 0x00,
	UART_MODE = 0x40,
	GLCD_MODE = 0xC0,
}Sys_Mode;

//Operation Mode
enum{
	DP_MODE = 0,
	OFFSET_GAIN_MODE = 1,
	SET_MODE = 2,
	DEBUG_MODE = 3,
	OFFSET_MODE=4,
	GAIN_MODE=5,
}Op_Mode;

uint8_t opMode = DP_MODE;
uint8_t sysMode = GLCD_MODE;

uint8_t Display_Unit = 0;				// Display Unit

char LCD[20];
uint8_t arr[4] = {1,4,8,16};

uint16_t ldc[11];
uint8_t index;
#define LDC_LENGTH	11
#define UNIT_CURR	(MAX_CURR-MIN_CURR)/(LDC_LENGTH-1)

uint16_t ldc_data;						//Sensor data
float distance;						//Object Distance
float current;							//Output Current
float percent;							//output percent

uint8_t position;
uint8_t lcd_contract;
uint8_t reloadFlag;
uint16_t config, divider, settlecount, rcount, error_conf, mux_conf, i_drive, status, offset, offset_temp, gain, gain_temp, debugPage;

//////////////////////////////////////////////////////////////
//              *******Functions*******                     //
//////////////////////////////////////////////////////////////

//DIO Initialize
void DIO_Initialize(){
	DDRA = 0b00100111;			// PA7 : MODE2 				(INPUT)
								// PA6 : MODE1 				(INPUT)
								// PA5 : AD5421 - LDAC 		(OUTPUT)
								// PA4 : AD5421 - FAULT 	(INPUT)
								// PA3 : AD5421 - SDO 		(INPUT)
								// PA2 : AD5421 - SDI 		(OUTPUT)
								// PA1 : AD5421 - SCLK 		(OUTPUT)
								// PA0 : AD5421 - /SYNC 	(OUTPUT)
								
	DDRB = 0b11111111;			// PB7 : ISP - SCK			(OUTPUT)
								// PB6 : ISP - MISO			(OUTPUT)
								// PB5 : ISP - MOSI			(OUTPUT)
								// PB4 : N.C.
								// PB3 : N.C.
								// PB2 : N.C.
								// PB1 : N.C.
								// PB0 : N.C.

	DDRC = 0b00000111;			// PC7 : BUTTON - ENTER		(INPUT)
								// PC6 : BUTTON - UP		(INPUT)
								// PC5 : BUTTON - DOWN		(INPUT)
								// PC4 : BUTTON - NEXT		(INPUT)
								// PC3 : N.C.
								// PC2 : LDC - SD			(OUTPUT)
								// PC1 : LDC - SDA			(I2C-OUTPUT)
								// PC0 : LDC - SCL			(I2C-OUTPUT)	

	DDRD = 0b11111010;			// PB7 : LCD - SI			(OUTPUT)
								// PB6 : LCD - SCL			(OUTPUT)
								// PB5 : LCD - A0			(OUTPUT)
								// PB4 : LCD - /RES			(OUTPUT)
								// PB3 : LCD - /CS			(OUTPUT)
								// PB2 : LDC - INTB			(INPUT)
								// PB1 : UART - TXD			(OUTPUT)
								// PB0 : UART - RXD			(INPUT)	

	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x03;
	PORTD = 0x10;

	SETBIT(PORTC,PC2);
	_delay_ms(10);
	CLEARBIT(PORTC,PC2);
	_delay_ms(10);
}

//UART Initialize
void UART_Initialize(){

}

//System Mode Switch Check
uint8_t Get_System_Mode(){
	uint8_t sw = GLCD_MODE;
	sw = PINA & 0xC0;
	return sw;
}

//LCD 기본 화면 을 띄움
void LCD_DefaultScreen(char* title, char* menu1, char* menu2, char* menu3, char* menu4){
	GLCD_GoTo(16, 0);
	GLCD_WriteString5x7(title);
	GLCD_GoTo(8,7);
	GLCD_WriteString5x7(menu1);
	GLCD_GoTo(50,7);
	GLCD_WriteString5x7(menu2);
	GLCD_GoTo(70,7);
	GLCD_WriteString5x7(menu3);
	GLCD_GoTo(105,7);
	GLCD_WriteString5x7(menu4);
	
	GLCD_Line(0,8,132,8);
	GLCD_Line(0,54,132,54);	
}

//////////////////////////////////////////////////////////////
//             *******MAIN Functions*******                 //
//////////////////////////////////////////////////////////////
//모드를 확인하여 해당 항목에 대한 초기화 진행 
void System_Mode_Check(){
	sysMode = Get_System_Mode();
	switch(sysMode){
		case GLCD_MODE:
			GLCD_Initialize();
			GLCD_ClearScreen();
			GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
			break;
		case UART_MODE:
			UART_Initialize();
			break;
	
	}
}

//EEPROM 저장 (현재는 사용하지 않음)
//void Write_Eeprom(){
//	uint8_t i;
//	for(i=0 ; i< 17; i++){
//		eeprom_update_word((uint16_t*)((i+1)*5), freq[i]);
//	}
//}

//EEPROM 읽기(초기에 한번 세팅화면에서 빠져나올때 한번)
void Read_Eeprom(){
	uint8_t i;
	for(i=0 ; i< LDC_LENGTH; i++){
		ldc[i] = eeprom_read_word((uint16_t*)((i+1)*5));
	}
	lcd_contract = eeprom_read_byte((uint8_t*)((LDC_LENGTH+1)*5));
	offset = eeprom_read_word((uint16_t*)((LDC_LENGTH+2)*5));
	gain = eeprom_read_byte((uint8_t*)((LDC_LENGTH+3)*5));
}

//전류 출력(3.8~21mA)
void AD5421_Output(float CurrValue){
	double Cur_Per1 = 0.0f;
	double OutCurrent = 0.0f;
	float Min_mA = 3.8f;
   	float Max_mA = 21.0f;
	
	//if (LRV_Data == 0) CurrValue = 3.9;
   	if (CurrValue < Min_mA) { CurrValue = Min_mA; }
   	if (CurrValue >= Max_mA) { CurrValue = Max_mA; }

   	// Min_mA ~ Max_mA Range Output
   	Cur_Per1 = ((CurrValue - Min_mA) / (Max_mA - Min_mA));
   	OutCurrent = 65535 * Cur_Per1;
	
	AD5421_WriteDac((unsigned short)OutCurrent);
}

//주파수 값 읽기
uint16_t Read_LDC(){
	uint16_t tmp;

	//LDC 값을 읽어온다.
	tmp = Read_Data(DATA_CH0);

	//오류가 없을 경우에만 데이터 저장.
	if((tmp&0xF000) == 0)  ldc_data = tmp;
	
#ifdef DATA_DEBUG
	if(opMode != DEBUG_MODE && opMode != OFFSET_GAIN_MODE){
		sprintf(LCD,"DATA: %04x(%04x)",tmp, Get_Status());
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif
	return ldc_data;
}

//주파수를 가지고 거리값 계산 
float Cal_Distance(uint16_t ldc_d){
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
	if(opMode != DEBUG_MODE){
		sprintf(LCD,"DST: %2.2f(mm)",dst);
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif	
	return dst;
}

//주파수를 받아서 전류값계산 
float CUR_Output(uint16_t ldc_d){
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
	if(opMode != DEBUG_MODE){
		sprintf(LCD,"CURRENT: %2.2f(mA)",cur);
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif

	AD5421_Output(cur);

	return cur;
}

float Cal_Percent(float cur){
	float per;
	
	per = ((cur - MIN_CURR)/(MAX_CURR - MIN_CURR))*100.0f;
	if(per < 0.0f) per = 0.0f;
	else if(per >100.0f) per = 100.0f;

	return per;
}

//거리 표시 화면을 보여줌.
void Dsp_Distance(float dis){
	
	LCD_DefaultScreen("INDUCTIVE SENSING", "SETTING","", "",  "UNIT");

	if (Display_Unit == 0) {	// mm
		sprintf(LCD, "%2.2f", dis);	
		GLCD_GoTo(106, 4);
		GLCD_WriteString5x7(" mm ");
	} else if (Display_Unit == 1) {	// mil
		sprintf(LCD, "%2.1f", dis/0.0254f);
		GLCD_GoTo(106, 4);
		GLCD_WriteString5x7("mil");
	} else if (Display_Unit == 2) {	// Percent
		sprintf(LCD, "%2.1f", percent);	
		GLCD_GoTo(106, 4);
		GLCD_WriteString5x7(" %  ");
	} else if (Display_Unit == 3) {	// mA
		sprintf(LCD, "%2.2f", current);
		GLCD_GoTo(106, 4);
		GLCD_WriteString5x7(" mA ");
	}
	GLCD_GoTo(12, 2);
	GLCD_WriteString19x24(LCD);
}

//세팅 화면을 보여줌.
void Dsp_Offset_Gain(){
	
	LCD_DefaultScreen("OFFSET&GAIN SET", "", "1", "2", "NEXT");

	sprintf(LCD, "1. OFFSET : %04x", Get_Offset(CH0));
	GLCD_GoTo(12, 2);
	GLCD_WriteString5x7(LCD);

	sprintf(LCD, "2. GAIN : %04x", Get_Offset(CH0));
	GLCD_GoTo(12, 3);
	GLCD_WriteString5x7(LCD);

	sprintf(LCD, "READ DATA : %04x", ldc_data);
	GLCD_GoTo(12, 4);
	GLCD_WriteString5x7(LCD);
}

void Dsp_Offset(){
	uint8_t i;
	
	LCD_DefaultScreen("OFFSET SETTING", "SAVE", "UP", "->", "EXIT");

	sprintf(LCD, "CHANGE VAL : %04x", offset_temp);
	GLCD_GoTo(12, 2);
	GLCD_WriteString5x7(LCD);

	sprintf(LCD, "CUR OFFSET : %04x", Get_Offset(CH0));
	GLCD_GoTo(12, 2);
	GLCD_WriteString5x7(LCD);

	sprintf(LCD, "READ DATA : %04x", ldc_data);
	GLCD_GoTo(12, 4);
	GLCD_WriteString5x7(LCD);
}

void Dsp_Gain(){
	uint8_t i;
	
	LCD_DefaultScreen("GAIN SETTING", "SAVE", "<-", "->", "EXIT");
	
	for(i=0; i<4; i++){
		sprintf(LCD, "%02d", arr[i]);
		if(i==position){
			GLCD_GoTo(16 + (18*(i)), 3);
			GLCD_InvWriteString5x7(LCD);
		}
		else{
			GLCD_GoTo(16 + (20*(i)), 3);
			GLCD_InvWriteString5x7(LCD);
		}
	}
}

//세팅 화면을 보여줌.
void Dsp_Setting(){
	int i;

	LCD_DefaultScreen("SETTING MODE", "SAVE", "<-", "->", "EXIT");

	for(i = 0; i<LDC_LENGTH ; i++){
		sprintf(LCD, "%02d", i);
		if(i==position){
			GLCD_GoTo(16 + (18*(i%6)), 2 + (i/6));
			GLCD_InvWriteString5x7(LCD);
		}
		else{
			GLCD_GoTo(16 + (18*(i%6)), 2 + (i/6));
			GLCD_WriteString5x7(LCD);
		}
		
	}
}

//디버깅화면을 보여줌.
void Dsp_Debugging(){
	uint8_t i;
	
	if(reloadFlag){
		config = Get_Config();
		divider = Get_Divider(CH0);
		settlecount = Get_SettleCount(CH0);
		rcount = Get_RCount(CH0);
		error_conf = Get_Error_Config();
		mux_conf = Get_MUX();
		i_drive	= Get_IDreive(CH0);
		ldc_data= Read_Data(DATA_CH0);
		status = Get_Status();
		gain = Get_Gain();
		offset = Get_Offset(CH0);
		reloadFlag = FALSE;
	}

	LCD_DefaultScreen("Debug Mode", "REROAD", "", "", "EXIT");
	
	switch(debugPage){
		case 0:
			sprintf(LCD, "CONFIG: %04x", config);
			GLCD_GoTo(12, 2);
			GLCD_WriteString5x7(LCD);
	
			sprintf(LCD, "DIVIDER: %04x", divider);
			GLCD_GoTo(12, 3);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "SETTLECnt: %04x", settlecount);
			GLCD_GoTo(12, 4);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "RCnt: %04x", rcount);
			GLCD_GoTo(12, 5);
			GLCD_WriteString5x7(LCD);
			break;
		case 1:
			sprintf(LCD, "ERRORConf: %04x", error_conf);
			GLCD_GoTo(12, 2);
			GLCD_WriteString5x7(LCD);
	
			sprintf(LCD, "MUX: %04x", mux_conf);
			GLCD_GoTo(12, 3);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "IDRIVE: %04x", i_drive);
			GLCD_GoTo(12, 4);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "fREQ: %04x", ldc_data);
			GLCD_GoTo(12, 5);
			GLCD_WriteString5x7(LCD);
			break;

		case 2:
			sprintf(LCD, "STATUS: %04x", status);
			GLCD_GoTo(12, 2);
			GLCD_WriteString5x7(LCD);
	
			sprintf(LCD, "OFFSET: %04x", offset);
			GLCD_GoTo(12, 3);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "GAIN: %04x", gain);
			GLCD_GoTo(12, 4);
			GLCD_WriteString5x7(LCD);
			break;

		case 3:
		for(i = 0; i<LDC_LENGTH ; i++){
			sprintf(LCD, "%04x",ldc[i]);
			GLCD_GoTo(16 + (40*(i%3)), 2 + (i/3));
			GLCD_WriteString5x7(LCD);
		}
			break;
	}
	

	
}

//버튼 값을 확인하여 그에 맞는 기능을 수행
void CHK_Button(){
	uint8_t btn;

	btn = SW_IN;
#ifdef KEY_TEST
	sprintf(LCD, "Button Number: %02d", btn);
	GLCD_GoTo(12, 5);
	GLCD_WriteString5x7(LCD);
#endif
	switch(btn){
		case SW_ENT:
			if(opMode == DP_MODE) {
				opMode = OFFSET_GAIN_MODE;
				GLCD_ClearScreen();
			}
			else if(opMode == SET_MODE){
				eeprom_update_word((uint16_t*)((position+1)*5), ldc_data);
				sprintf(LCD, "%02d mm (%04x) saved!",position, ldc_data);
				GLCD_GoTo(12, 4);
				GLCD_WriteString5x7(LCD);
				if(position < LDC_LENGTH -1 ) position++;
			}
			else if(opMode == DEBUG_MODE){
				reloadFlag = TRUE;
				GLCD_ClearScreen();
			}
			else if(opMode == GAIN_MODE){

			}
			else if (opMode == OFFSET_MODE){
				Offset_Setting(CH0,offset_temp);
				offset = Get_Offset(CH0);
				sprintf(LCD, "Offset(%04x)!", offset);
				GLCD_GoTo(12, 5);
				GLCD_WriteString5x7(LCD);
			}
			_delay_ms(50);
			break;
			
		case SW_NEXT:
			if(opMode == DP_MODE) {
				if (++Display_Unit > 3) Display_Unit = 0;
				GLCD_ClearScreen();
			}
			else if(opMode == SET_MODE){
				opMode = DP_MODE;
				Read_Eeprom();
				GLCD_ClearScreen();
			}
			else if(opMode == OFFSET_GAIN_MODE){
				opMode = SET_MODE;
				position = 0;
				GLCD_ClearScreen();
				GLCD_GoTo(40, 5);
				GLCD_WriteString5x7("None Act!!");
			}
			else if(opMode == DEBUG_MODE){
				opMode = DP_MODE;
				debugPage = 0;
				GLCD_ClearScreen();
			}
			else if(opMode == GAIN_MODE){
				opMode = OFFSET_GAIN_MODE;
				GLCD_ClearScreen();
			}
			else if(opMode == OFFSET_MODE){
				opMode = OFFSET_GAIN_MODE;
				GLCD_ClearScreen();
			}
			_delay_ms(50);
			break;
			
		case SW_UP:
			if(opMode == SET_MODE){
				if(position > 0) position--;
			}
			else if(opMode == DP_MODE){
				if (++lcd_contract >= 7) lcd_contract = 7;
				GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
				eeprom_update_byte((uint8_t*)((LDC_LENGTH+1)*5), lcd_contract);
			}
			else if(opMode == DEBUG_MODE){
				if(debugPage < 3) {
					debugPage++;
					GLCD_ClearScreen();
				}
			}
			else if(opMode == OFFSET_GAIN_MODE){
				opMode = OFFSET_MODE;
				position = 0;
				GLCD_ClearScreen();
			}
			else if(opMode == GAIN_MODE){

			}
			else if(opMode == OFFSET_MODE){

			}
			_delay_ms(50);
			break;
			
		case SW_DOWN:
			if(opMode == SET_MODE){
				if(position < 10) position ++;
			}
			else if(opMode == DP_MODE){
				if (--lcd_contract <= 0) lcd_contract = 0;
				GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
				eeprom_update_byte((uint8_t*)((LDC_LENGTH+1)*5), lcd_contract);
			}
			else if(opMode == DEBUG_MODE){
				if(debugPage > 0) {
					debugPage--;
					GLCD_ClearScreen();
				}
			}
			else if(opMode == OFFSET_GAIN_MODE){
				opMode = GAIN_MODE;
				position = 0;
				GLCD_ClearScreen();
			}
			else if(opMode == GAIN_MODE){

			}
			else if(opMode == OFFSET_MODE){

			}
			_delay_ms(50);
			break;

		case SW_UP_NEXT:
			if(opMode == OFFSET_GAIN_MODE){
				offset = 0x0000;
				offset_temp = 0x0000;
				Offset_Setting(CH0,offset);
				eeprom_update_word((uint16_t*)((LDC_LENGTH+2)*5), offset);
				sprintf(LCD, "Offset(%04x) INIT!", offset);
				GLCD_GoTo(12, 5);
				GLCD_WriteString5x7(LCD);
			}
			break;
		case SW_UPDN:
			CLEARBIT(PORTD,PD4);
			_delay_ms(10);
			SETBIT(PORTD,PD4);
			_delay_ms(10);
			GLCD_Initialize();
			GLCD_ClearScreen();
			GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
			break;

		case SW_UP_DN_NEXT:
			opMode = DEBUG_MODE;
			GLCD_ClearScreen();
			break;
	}
}

//////////////////////////////////////////////////////////////
//                   *******MAIN*******                     //
//////////////////////////////////////////////////////////////
int main(){
	DIO_Initialize();		// DIO Initialize
	Read_Eeprom();			// Read EEPROM
	System_Mode_Check();
	AD5421_Init();			// AD5421 Initialize
	LDC_Init(offset);				// LDC1312 Initialize
	AD5421_Output(3.8f);

	opMode = DP_MODE;		//Operation Mode
	sysMode = GLCD_MODE;	//System Mode
	reloadFlag = TRUE;
	offset_temp = offset;
	gain_temp = gain;
	debugPage = 0;

    while(TRUE){
		ldc_data= Read_LDC();
		distance = Cal_Distance(ldc_data);
		current = CUR_Output(ldc_data);
		percent = Cal_Percent(current);
		
		if(sysMode == GLCD_MODE){
			if(opMode == DP_MODE){
				Dsp_Distance(distance);
			}
			else if(opMode == OFFSET_GAIN_MODE){
				Dsp_Offset_Gain();
			}
			else if(opMode == SET_MODE){
				Dsp_Setting();
			}
			else if(opMode == DEBUG_MODE){
				Dsp_Debugging();
			}
			else if(opMode == OFFSET_MODE){
				Dsp_Offset();
			}
			else if(opMode == GAIN_MODE){
				Dsp_Gain();
			}
			CHK_Button();
		}
    }
}
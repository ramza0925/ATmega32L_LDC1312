#include "Menu.h"

char LCD[20];
uint8_t Display_Unit;				    // Display Unit
uint32_t ldc[11];

uint16_t position;
uint16_t config, divider, settlecount, rcount, error_conf, mux_conf, i_drive, status, offset, /*gain,*/ debugPage;
uint8_t reloadFlag;

uint32_t ldc_data;						//Sensor data
float distance;							//Object Distance
float current;							//Output Current
float percent;							//output percent

uint16_t offset_arr[4];
uint8_t arr[4] = {1,4,8,16};

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

//거리 표시 화면을 보여줌.
void Dsp_Distance(){
	LCD_DefaultScreen("INDUCTIVE SENSING", "SETTING","", "",  "UNIT");

    switch(Display_Unit){
        case MM:
            sprintf(LCD, "%5.2F", distance);	
		    GLCD_GoTo(106, 4);
		    GLCD_WriteString5x7(" mm ");
            break;
        case MIL:
            sprintf(LCD, "%5.1F", distance/0.0254f);
		    GLCD_GoTo(106, 4);
		    GLCD_WriteString5x7("mil");
            break;
        case PERCENT:
            sprintf(LCD, "%5.1F", percent);	
		    GLCD_GoTo(106, 4);
		    GLCD_WriteString5x7(" %  ");
            break;
        case MA:
            sprintf(LCD, "%5.2F", current);
		    GLCD_GoTo(106, 4);
		    GLCD_WriteString5x7(" mA ");
            break;
    }
	GLCD_GoTo(12, 2);
	GLCD_WriteString19x24(LCD);
}

void Dsp_Contrast(){
	
	LCD_DefaultScreen("CONTRAST SETTING", "", "UP", "DOWN", "NEXT");

	sprintf(LCD, "%d", lcd_contract);
	GLCD_GoTo(70, 2);
	GLCD_WriteString19x24(LCD);
}

//Offset 값을 조정하는 화면
void Dsp_Offset(){
	uint8_t i;
	
	LCD_DefaultScreen("OFFSET SETTING", "SAVE", "UP", "->", "NEXT");

	for(i=0 ; i<4 ; i++){
		sprintf(LCD, "%x", offset_arr[i]);
		if(i==position){
			GLCD_GoTo(30 + (20*(i)), 2);
			GLCD_InvWriteString5x7(LCD);
		}
		else{
			GLCD_GoTo(30 + (20*(i)), 2);
			GLCD_WriteString5x7(LCD);
		}
	}

	sprintf(LCD, "CUR OFFSET : %04X", Get_Offset(CH0));
	GLCD_GoTo(12, 3);
	GLCD_WriteString5x7(LCD);

	sprintf(LCD, "DATA : %08lX", ldc_data);
	GLCD_GoTo(12, 4);
	GLCD_WriteString5x7(LCD);
}



//세팅 화면을 보여줌.
void Dsp_Setting(){
	int i;

	LCD_DefaultScreen("SETTING MODE", "SAVE", "<-", "->", "NEXT");

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
		//ldc_data= Read_Data(DATA_CH0);
		status = Get_Status();
		//gain = Get_Gain();
		offset = Get_Offset(CH0);
		reloadFlag = FALSE;
	}

	LCD_DefaultScreen("Debug Mode", "REROAD", "", "", "EXIT");
	
	switch(debugPage){
		case 0:
			sprintf(LCD, "CONFIG: %04X", config);
			GLCD_GoTo(12, 2);
			GLCD_WriteString5x7(LCD);
	
			sprintf(LCD, "DIVIDER: %04X", divider);
			GLCD_GoTo(12, 3);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "SETTLECnt: %04X", settlecount);
			GLCD_GoTo(12, 4);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "RCnt: %04X", rcount);
			GLCD_GoTo(12, 5);
			GLCD_WriteString5x7(LCD);
			break;
		case 1:
			sprintf(LCD, "ERRORConf: %04X", error_conf);
			GLCD_GoTo(12, 2);
			GLCD_WriteString5x7(LCD);
	
			sprintf(LCD, "MUX: %04X", mux_conf);
			GLCD_GoTo(12, 3);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "IDRIVE: %04X", i_drive);
			GLCD_GoTo(12, 4);
			GLCD_WriteString5x7(LCD);

			sprintf(LCD, "fREQ: %08lX", ldc_data);
			GLCD_GoTo(12, 5);
			GLCD_WriteString5x7(LCD);
			break;

		case 2:
			sprintf(LCD, "STATUS: %04X", status);
			GLCD_GoTo(12, 2);
			GLCD_WriteString5x7(LCD);
	
			sprintf(LCD, "OFFSET: %04X", offset);
			GLCD_GoTo(12, 3);
			GLCD_WriteString5x7(LCD);

            sprintf(LCD, "Contra: %04X", lcd_contract);
			GLCD_GoTo(12, 4);
			GLCD_WriteString5x7(LCD);
			break;

		case 3:
		    for(i = 0; i<LDC_LENGTH ; i++){
			    sprintf(LCD, "%6lX",ldc[i]&0xFFFFFF);
			    GLCD_GoTo(10 + (40*(i%3)), 2 + (i/3));
			    GLCD_WriteString5x7(LCD);
		    }
			break;
	}
}


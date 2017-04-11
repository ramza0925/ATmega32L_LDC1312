#include <stdint.h>
#include <stdio.h>

#include <GLCD/SPLC501C.h>
#include <GLCD/graphic.h>
#include <Variable.h>
#include <LDC/LDC1612.h>
#include <Menu/Menu.h>

//키를 누를때 가지는 딜레이
#define SWITCH_DELAY    200


//시스템 모드.
enum{
	NORMAL_MODE = 0x60,
	GLCD_MODE = 0x40,
	UART_MODE = 0x20,
	BOTH_MODE = 0x00,
	SYS_MODE_CNT = 0x04,
}Sys_Mode;

//동작 모드.
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
uint8_t index;                          //배열에서의 위치를 표시하기 위한 변수
uint8_t lcd_contract;                   //LDC 밝기를 저장하는 변수



//와치독 설정.
void Watchdog_Init(){

}

//클럭 설정.
void Clock_Init(){

}

//타이머 설정 (Delay를 위한 타이머)
void Timer_Init(){

}

void IO_Init(){

}

void Flash_Read(){

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
#ifdef CUR_DEBUG
	if(dpMode != DEBUG_MODE){
		sprintf(LCD,"DAC: %08X",0xffff);
		GLCD_GoTo(8, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif
}

int main(){
    //변수 초기화.
    dpMode = DP_MODE;

    
    Watchdog_Init();                //와치독 설정.
    Clock_Init();                   //클럭 설정.
    Timer_Init();                   //타이머 설정.
    IO_Init();                      //IO 설정.
    Flash_Read();                   //메모리 읽기.

    System_Mode_Check();            //시스템 모드 체크.

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

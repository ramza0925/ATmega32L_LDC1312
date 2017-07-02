#include <stdint.h>
#include <stdio.h>

#include "HW_Config.h"

#include "Utils/Delay.h"
#include "GLCD/SPLC501C.h"
#include "GLCD/graphic.h"
#include "LDC/LDC1612.h"
#include "DAC/DACX750.h"

#include "Variable.h"
#include "Menu/Menu.h"

#define SW_ENT			0xD
#define SW_UP			0xB
#define SW_NEXT			0x7
#define SW_DOWN			0xE
#define SW_UPDN			0xA
#define SW_UP_ENT		0x9
#define SW_DN_ENT		0xC
#define SW_DN_NEXT		0x6
#define SW_UP_NEXT		0x3
#define SW_UP_DN_NEXT	0x2

//Flash Addr
#define FLASH_START_ADDR        ((uint32_t)0x0801FC00)
//#define FLASH_END_ADDR          ((uint32_t)0x0801FFFF)
#define FLASH_OFFSET            4

//Ű�� ������ ������ ������
#define SWITCH_DELAY    200

//�ý��� ���.
enum{
	NORMAL_MODE = 0x0030,
	GLCD_MODE = 0x0020,
	UART_MODE = 0x0010,
	FW_UPDATE_MODE = 0x0000,
	SYS_MODE_CNT = 0x0004,
}Sys_Mode;

//���� ���.
enum{
	DP_MODE = 0,
	SET_MODE,
	DEBUG_MODE,
	OFFSET_MODE,
	CONTRAST_MODE,
	OP_MODE_CNT,
}Op_Mode;

uint16_t dpMode;
uint16_t sysMode;
uint8_t index;                          //�迭������ ��ġ�� ǥ���ϱ� ���� ����
uint8_t lcd_contract;                   //LDC ��⸦ �����ϴ� ����

//System Mode Switch Check
uint16_t Get_System_Mode()
{

    return GPIO_ReadInputData(GPIO_MODE) & 0x0030;
}

void Read_Flash_Data()
{
    uint8_t i;
    
    for(i=0 ; i< LDC_LENGTH; i++)
    {
		ldc[i] = *(uint32_t*)(FLASH_START_ADDR+(FLASH_OFFSET*i));
	}
    lcd_contract = (uint8_t)(*(uint32_t*)(FLASH_START_ADDR+(FLASH_OFFSET*(i++))));
    offset = (uint16_t)(*(uint32_t*)(FLASH_START_ADDR+(FLASH_OFFSET*(i++))));
}

void Write_Flash_Data()
{
    uint8_t i;
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(FLASH_START_ADDR);
    for(i=0 ; i< LDC_LENGTH; i++)
    {
        FLASH_ProgramWord(FLASH_START_ADDR+(FLASH_OFFSET*i), ldc[i]);
    }
    FLASH_ProgramWord(FLASH_START_ADDR+(FLASH_OFFSET*(i++)), (uint32_t)lcd_contract);
    FLASH_ProgramWord(FLASH_START_ADDR+(FLASH_OFFSET*(i++)), (uint32_t)offset);
    FLASH_Lock();
}

//��带 Ȯ���Ͽ� �ش� �׸� ���� �ʱ�ȭ ���� 
void System_Mode_Check()
{
	sysMode = Get_System_Mode();

	switch(sysMode)
    {
		case GLCD_MODE:
            GLCD_IO_Init();
            GLCD_Initialize();
			GLCD_ClearScreen();
			GLCD_WriteCommand(SPLC501C_VOLTAGE_RATIO | lcd_contract);
			break;
		case UART_MODE:
			USART1_Init();
			break;
        default:
            break;
	}
}

//���ļ� �� �б�
uint32_t Read_LDC()
{
	uint32_t tmp = 0x00000000;

	if((Get_Status()&CH0_UNREAD_CONV) == CH0_UNREAD_CONV) 
    {
	
		//LDC ���� �о�´�.
		tmp |= Read_Data(DATA_CH0_MSB);
		tmp = (tmp << 16);
		tmp |= Read_Data(DATA_CH0_LSB);

		//������ ���� ��쿡�� ������ ����.
		if((tmp&0xF0000000) == 0) ldc_data = tmp >> 8;
	}

#ifdef DATA_DEBUG
	if(dpMode == DP_MODE || dpMode == SET_MODE)
    {
		sprintf(LCD,"DATA: %08lX", ldc_data);
		GLCD_GoTo(16, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif
	return ldc_data;
}


//���ļ��� ������ �Ÿ��� ��� 
float Cal_Distance(uint32_t ldc_d)
{
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

	return dst;
}

//�Ÿ������� �ۼ�Ʈ ���� ���
float Cal_Percent(float dst)
{
	float per;
	
	per = ((dst - MIN_DST)/(MAX_DST- MIN_DST))*100.0f;

	return per;
}

//�ۼ�Ʈ ���� �޾Ƽ� �������� ��� (���÷��̿�)
float Cal_Current(float percent)
{
    float cur_dat;

    cur_dat = ((percent/100.0f)*(MAX_CURR-MIN_CURR))+MIN_CURR;  
    
    return cur_dat;
}

void Read_Temperature()
{
    uint16_t tmp;
    float temp;

    tmp = ADC_GetConversionValue(ADC1);
    temp = (((float)tmp/4096)*3.3)*100;

#ifdef TEMP_DEBUG
	if(dpMode == DP_MODE)
    {
		sprintf(LCD,"TEMP: %4.2FC", temp);
		GLCD_GoTo(16, 5);
		GLCD_WriteString5x7(LCD);
	}
#endif
}

//��ư ���� Ȯ���Ͽ� �׿� �´� ����� ����
void CHK_Button()
{
	uint16_t btn, i;
	uint16_t tmp = 0;

	btn = GPIO_ReadInputData(GPIO_KEY) & 0x000F;
    
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
                    Write_Flash_Data();
				    dpMode = DP_MODE;
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
                    
            }
			delay_ms(SWITCH_DELAY);
			break;
            
        //����� ��� ����
		case SW_UP_NEXT:
            if(dpMode != DEBUG_MODE){
                dpMode = DEBUG_MODE;
			    GLCD_ClearScreen();
            }
            delay_ms(SWITCH_DELAY);
			break;
            
        //���÷��� �� �ʱ�ȭ 
		case SW_UPDN:
			GPIO_ResetBits(GPIO_GLCD, GPIO_GLCD_NRES_Pin);
			delay_ms(10);
			GPIO_SetBits(GPIO_GLCD, GPIO_GLCD_NRES_Pin);
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


int main()
{
    //���� �ʱ�ȭ.
    dpMode = DP_MODE;

    RCC_Configuration();            //���� Ŭ�� ��� ����. 64MHz

    Read_Flash_Data();

    NVIC_Configuration();           //Systick ���ͷ�Ʈ ����.

    Systick_Init(); 

    if(lcd_contract <= 0 || lcd_contract > 7)
    {
        lcd_contract = 3;           //���� LCD ��Ⱑ ������ �ȵǾ��ִ� �� �ʱⰪ �Է�  
	}

    System_Mode_Check();            //�ý��� ��� üũ.

    if(sysMode == GLCD_MODE)
    {
	    GLCD_GoTo(16, 1);
	    GLCD_WriteString5x7("RCC, Timer, Flash Init.");
    }
    
    GPIO_Configuration();
    if(sysMode == GLCD_MODE)
    {
	    GLCD_GoTo(16, 2);
	    GLCD_WriteString5x7("GPIO_Configuration.");
    }

    I2C1_Init();
    if(sysMode == GLCD_MODE)
    {
	    GLCD_GoTo(16,3);
	    GLCD_WriteString5x7("I2C1_Init.");
    }
    
    DACX750_Configuration();
    if(sysMode == GLCD_MODE)
    {
	    GLCD_GoTo(16,4);
	    GLCD_WriteString5x7("DACX750_Configuration.");
    }

    ADC1_Configuration();
    if(sysMode == GLCD_MODE)
    {
	    GLCD_GoTo(16,5);
	    GLCD_WriteString5x7("ADC1_Configuration.");
    }

    LDC_Init(offset);

    if(sysMode == GLCD_MODE)
    {
	    reloadFlag = TRUE;                  //����� ��忡�� ȭ�� ���ſ� �÷���
	    debugPage = 0;

        //������ ���� �迭�� ����
	    for(int i=0; i<4; i++)
		    offset_arr[i] = (offset>>(12-(4*i))) & 0x000F;

	    GLCD_GoTo(16,6);
	    GLCD_WriteString5x7("LDC_Init.");
        GLCD_ClearScreen();
	}
    while(TRUE){
		ldc_data= Read_LDC();                   // LDC �� �о����
		distance = Cal_Distance(ldc_data);      // �Ÿ� ������ ȯ��.
		percent = Cal_Percent(distance);        // �ۼ�Ʈ �� ����
		current = Cal_Current(percent);         // ���� ���
        DAC_Current_Out(current);
        Read_Temperature();
		
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
        delay_ms(300);
    }
}

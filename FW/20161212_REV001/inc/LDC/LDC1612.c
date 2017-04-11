#include "LDC1612.h"
#include "../Utils/Delay.h"
#include "../Variable.h"


#ifdef LDC_TEST
    #include <stdio.h>
    #include "../GLCD/SPLC501C.h"

	char TEST_LCD[20];
#endif

void DIO_INIT();

void SleepMode_Control(uint16_t mode){
	uint16_t tmp;

	tmp = Read_Data(CONFIG);

	if(mode == SLEEP_MODE_EN)
		Set_Config(tmp|SLEEP_MODE_EN);
	else
		Set_Config(tmp&~SLEEP_MODE_EN);
}

void LDC_Init(uint16_t offset){

    DIO_INIT();
    
	SleepMode_Control(SLEEP_MODE_EN);							//설정 전 슬립모드 시작
	//채널 및 클럭 설정
	Set_Config(ACTIVE_CHAN_CH0|RP_OVERRIDE_EN|SENSOR_ACTIVATE_LOW|AUTO_AMP_DIS|REF_CLK_SRC_IN|INTB_EN|HIGH_CURRENT_DRV_DIS);
	Set_Divider(CH0, FIN_DIVIDER, FREF_DIVIDER);				//클럭 분배 설정 FIN_DIVIDER = 0x0002, FREF_DIVIDER = 0x000A
	Set_RCount(CH0, RCOUNT);									//변환 시간 설정 RCount  = 0x04d6
	Set_SettleCount(CH0,SETTLECOUNT);							//안정화 시간 설정Settle Count = 0x000A
	//오류 보고 환경 설정
	Set_Error_Config(UR_ERR2OUT_DIS|OR_ERR2OUT_DIS|WD_ERR2OUT_EN|AH_ERR2OUT_DIS|AL_ERR2OUT_DIS
						|UR_ERR2INT_DIS|OR_ERR2INT_DIS|WD_ERR2INT_EN|AH_ERR2INT_DIS|AL_ERR2INT_DIS|ZC_ERR2INT_DIS|DRDY_2INT_DIS);
	Set_MUX(DEGHITCH_3R3M);										//채널 동작 설정
	Set_IDrive(CH0,18,0);										//구동 전류 설정
	//Set_Gain(gain);											//출력 이득 설정
	if(offset != 0xFFFF) Set_Offset(CH0,offset);
	SleepMode_Control(SLEEP_MODE_DIS);
}

void Set_Config(uint16_t conf){
	uint16_t conf_data;
	
	conf_data = conf | 0x0001;
	Write_Data(conf_data, CONFIG);
}

uint16_t Get_Config(){
	return Read_Data(CONFIG);
}

void Set_Error_Config(uint16_t err_conf){
	Write_Data(err_conf, ERROR_CONFIG);
}

uint16_t Get_Error_Config(){
	return Read_Data(ERROR_CONFIG);
}

uint16_t Get_Status(){
	return Read_Data(STATUS);
}

void Set_Divider(uint8_t ch, uint16_t fin_div, uint16_t fref_div){
	uint16_t div;

	div = (fin_div<<12) | fref_div;
	Write_Data(div, CLOCK_DEVIDERS_CH0 + ch);
}
uint16_t Get_Divider(uint8_t ch){
	return Read_Data(CLOCK_DEVIDERS_CH0 + ch);
}

void Set_SettleCount(uint8_t ch, uint16_t settleCnt){
	Write_Data(settleCnt, SETTLECOUNT_CH0 + ch);
}

uint16_t Get_SettleCount(uint8_t ch){
	return Read_Data(SETTLECOUNT_CH0 + ch);
}
void Set_RCount(uint8_t ch, uint16_t rCnt){
	Write_Data(rCnt, RCOUNT_CH0 + ch);
}

uint16_t Get_RCount(uint8_t ch){
	return Read_Data(RCOUNT_CH0 + ch);
}

void Set_IDrive(uint8_t ch, uint16_t iDrv, uint16_t init_iDrv){
	uint16_t tmp;
	tmp = (iDrv << 11) | (init_iDrv << 6);
	Write_Data(tmp,DRIVE_CURRENT_CH0 + ch);
}

uint16_t Get_IDreive(uint8_t ch){
	return Read_Data(DRIVE_CURRENT_CH0 + ch);
}

void Set_MUX(uint16_t mux){
	uint16_t mux_data;

	mux_data = mux | 0x0208;
	Write_Data(mux_data,MUX_CONFIG);
}

uint16_t Get_MUX(){
	return Read_Data(MUX_CONFIG);
}

void Set_Gain(uint16_t gain){
	Write_Data(gain, RESET_DEV);
}

uint16_t Get_Gain(){
	return Read_Data(RESET_DEV);
}

uint16_t Get_Data(uint8_t ch){
	return Read_Data(DATA_CH0_MSB + ch);
}

void Set_Offset(uint8_t ch, uint16_t offset){

#ifdef TEST
        sprintf(TEST_LCD,"offset: %08lX", offset);
		GLCD_GoTo(12, 1);
		GLCD_WriteString5x7(TEST_LCD);
#endif
	Write_Data(offset, OFFSET_CH0 + ch);
}

uint16_t Get_Offset(uint8_t ch){
	return Read_Data(OFFSET_CH0 + ch);
}

void Reset_Device(){
	Write_Data(0x8000, RESET_DEV);
}

void Offset_Setting(uint8_t ch, uint16_t offset){
	SleepMode_Control(SLEEP_MODE_EN);
	delay_ms(10);
	Set_Offset(CH0,offset);
	SleepMode_Control(SLEEP_MODE_DIS);
	delay_ms(100);
}

void Gain_Setting(uint16_t gain){
	SleepMode_Control(SLEEP_MODE_EN);
	delay_ms(10);
	Set_Gain(gain);
	SleepMode_Control(SLEEP_MODE_DIS);
	delay_ms(100);
}

void DIO_INIT(){
    LDC_SCL_H;
    LDC_SDA_H;     
}

void Start_Condition(){
    LDC_SDA_L;
    delay_us(1);
    LDC_SCL_L;
    delay_us(1);
}

void End_Condition(){
    LDC_SCL_H;
    delay_us(1);
    LDC_SDA_H;
    delay_us(1);
}

void Bit_Write(uint8_t bit){
    if(bit) LDC_SDA_H;
    else LDC_SDA_L;
    delay_us(100);
    LDC_SCL_H;
    delay_us(1);
    LDC_SCL_L;
    delay_us(1);
}

uint8_t ACK_Check(){
    uint8_t ack_Flag = 0;

    DioOen(pADI_GP2, 0x01);
    
    delay_us(100);
    ack_Flag = DioRd(pADI_GP2)&0x01;
    LDC_SCL_H;
    delay_us(1);
    LDC_SCL_L;
    delay_us(1);
    
    DioOen(pADI_GP2, 0x03);
    
    return ack_Flag;
}

void ACK_Out(){
    DioOen(pADI_GP2, 0x03);
    Bit_Write(LOW);
    DioOen(pADI_GP2, 0x01);
}

void Write_Data(uint16_t data, uint8_t addr){
    uint8_t i, j;
    uint8_t data_arr[4];

    data_arr[0] = LDC_ADDR;
    data_arr[1] = addr;
    
    data_arr[2] = (uint8_t)((data >> 8) & 0x00ff);
    data_arr[3] = (uint8_t)(data & 0x00ff);
    

#ifdef LDC_TEST
    sprintf(TEST_LCD, "Data: %08lX", data);
	GLCD_GoTo(12, 2);
	GLCD_WriteString5x7(TEST_LCD);

    sprintf(TEST_LCD, "data_arr[2]: %04x", data_arr[2]);
	GLCD_GoTo(12, 3);
	GLCD_WriteString5x7(TEST_LCD);

    sprintf(TEST_LCD, "data_arr[3]: %04x", data_arr[3]);
	GLCD_GoTo(12, 4);
	GLCD_WriteString5x7(TEST_LCD);
#endif

    Start_Condition();

    for(i=0; i < 4; i++){
        for(j=0 ; j<8 ; j++){
            if(((data_arr[i]>>(7-j)) & 0x01) == 0x01) Bit_Write(HIGH);
            else Bit_Write(LOW);
        }
        ACK_Check();
    }
    End_Condition();
    delay_us(100);
}

uint16_t Read_Data(uint8_t addr){
    uint8_t i, j;
    uint8_t byte_buf = 0;
    uint16_t buf = 0;
    uint8_t data_arr[2];
    uint8_t read_addr;

    data_arr[0] = LDC_ADDR;
    data_arr[1] = addr;

    Start_Condition();

    for(i=0; i < sizeof(data_arr); i++){
        for(j=0 ; j<8 ; j++){
            if(((data_arr[i]>>(7-j)) & 0x01) == 0x01)Bit_Write(HIGH);
            else Bit_Write(LOW);
        }
        ACK_Check();
    }

    End_Condition();

    delay_us(1);

    Start_Condition();
    
    read_addr = LDC_ADDR + 1;
    
     for(j=0 ; j<8 ; j++){
        if(((read_addr>>(7-j)) & 0x01) == 0x01) Bit_Write(HIGH);
        else Bit_Write(LOW);
     }
     ACK_Check();

     DioOen(pADI_GP2, 0x01);
     delay_us(100);
     for(i=0; i < 2; i++){
        byte_buf = 0;
        for(j=0 ; j<8 ; j++){
            byte_buf |= (uint8_t)(((DioRd(pADI_GP2)>>1)&0x0001)<<(7-j));
            LDC_SCL_H;
            delay_us(1);
            LDC_SCL_L;
            delay_us(1);
        }
        ACK_Out();

#ifdef LDC_TEST
    sprintf(TEST_LCD, "%d buf is %04x", i+1, byte_buf);
	GLCD_GoTo(12, 5+i);
	GLCD_WriteString5x7(TEST_LCD);
#endif
        buf |= (byte_buf<<8*(1-i));
    }

    DioOen(pADI_GP2, 0x03);
    End_Condition();
    delay_us(100);
    
    return buf;
}
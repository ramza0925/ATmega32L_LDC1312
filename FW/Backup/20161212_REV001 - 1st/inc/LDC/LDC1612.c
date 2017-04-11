#include "LDC1612.h"
#include "../Utils/Delay.h"


//#define DEBUG

#ifdef DEBUG
    #include <stdio.h>
    #include "../GLCD/SPLC501C.h"

	char TEST_LCD[20];
#endif

void I2C_Init();

void SleepMode_Control(uint16_t mode){
	uint16_t tmp;

	tmp = Read_Data(CONFIG);

	if(mode == SLEEP_MODE_EN)
		Set_Config(tmp|SLEEP_MODE_EN);
	else
		Set_Config(tmp&~SLEEP_MODE_EN);
}

void LDC_Init(uint16_t offset){

    I2C_Init();
    
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

void I2C_Init(){
    // Enable I2C Master mode, baud rate and interrupt sources
    I2cMCfg(I2CMCON_TXDMA_DIS|I2CMCON_RXDMA_DIS, I2CMCON_IENCMP_EN|I2CMCON_IENRX_EN|I2CMCON_IENTX_EN, I2CMCON_MAS_EN); 
    I2cBaud(0x4E,0x4F); // 100kHz clock	

    NVIC_EnableIRQ(I2CM_IRQn);
}

uint16_t Read_Data(uint8_t addr){
	uint8_t i;
	uint16_t buf = 0x0000;

    I2cTx(MASTER, addr); 
    I2cMWrCfg((LDC_ADDR<< 1));
    delay_ms(1);

    I2cMRdCfg(LDC_ADDR<<1, 2, DISABLE);
    delay_ms(1);
	for(i=2 ; i>0 ; i--){
		 buf |= (uint16_t)(I2cRx(MASTER)<<(8*(i-1)));
         delay_ms(1);
	}
	return buf;
}


void Write_Data(uint16_t data, uint8_t addr){
	uint8_t byte;
	uint8_t i;

    I2cTx(MASTER, addr);
    I2cMWrCfg(LDC_ADDR<<1);
    delay_ms(1);
    I2cTx(MASTER, addr);
    I2cMWrCfg((LDC_ADDR<<1)+1);
     
	for(i=2 ; i>0 ; i--){
		byte = (uint8_t)((data >> (8*(i-1)))&0x00ff);
		I2cTx(MASTER, byte);							//I2C 데이터 전송
		delay_ms(1);
	}
}

// I2C0 master handler 
void I2C0_Master_Int_Handler() {
    uint32_t uiStatus;
    uiStatus = I2cSta(MASTER);

    if((uiStatus & I2CMSTA_RXREQ) == I2CMSTA_RXREQ){       // Master Recieve IRQ
    }
  
    if((uiStatus & I2CMSTA_TXREQ) == I2CMSTA_TXREQ){ // Master Transmit IRQ        
    }     
    if((uiStatus & I2CMSTA_TCOMP_SET) == I2CMSTA_TCOMP_SET){ // communication complete     
    }
#ifdef DEBUG
    sprintf(TEST_LCD, "I2C Status: %x", uiStatus);
	GLCD_GoTo(12, 5);
	GLCD_WriteString5x7(TEST_LCD);
#endif
}
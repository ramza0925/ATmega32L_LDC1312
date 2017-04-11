#include "I2C.h"
#include "../GLCD/SPLC501C.h"

//#define DEBUG

void I2C_Init(){
	
	TWBR = 0x02;							//Freq = CPU_Clock/(16+2*TWBR[0]-4^TWSR[0]) = 50KHz
	TWSR = 0x00;

	TWCR = (1 << TWEN) | (1 << TWEA);		//I2C 활성화  ACK 허용.

#ifdef DEBUG
	GLCD_GoTo(8, 5);
	GLCD_WriteString5x7("LDC INIT---!");
#endif
	
}

void I2C_Start(){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));				//시작완료 대기
#ifdef DEBUG
	GLCD_GoTo(8, 5);
	GLCD_WriteString5x7("LDC START---!");
#endif
}

void I2C_Transmit(uint8_t data){
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1<<TWEA);
	while(!(TWCR & (1 << TWINT)));			//전송완료 대기
#ifdef DEBUG
	GLCD_GoTo(8, 5);
	GLCD_WriteString5x7("LDC TRANS---!");
#endif
}

uint8_t I2C_Receive(){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1<<TWEA);
	while(!(TWCR & (1 << TWINT)));			//수신완료 대기
#ifdef DEBUG
	GLCD_GoTo(8, 5);
	GLCD_WriteString5x7("LDC RECEIVE---!");
#endif
	return TWDR;
}

void I2C_Stop(){
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWEA);
#ifdef DEBUG
	GLCD_GoTo(8, 5);
	GLCD_WriteString5x7("LDC STOP---!");
#endif
}
#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <avr/io.h>

#define I2C_SCL			PC0
#define I2C_SDA			PC1

void I2C_Init(void);
void I2C_Start(void);
void I2C_Transmit(uint8_t data);
uint8_t I2C_Receive(void);
void I2C_Stop(void);

#endif

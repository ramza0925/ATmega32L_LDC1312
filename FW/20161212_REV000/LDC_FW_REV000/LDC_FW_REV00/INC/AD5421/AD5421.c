/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "AD5421.h"		// AD5421 definitions.

unsigned char data[5] = {0x03, 0x00, 0x00, 0x00, 0x00};

void SPI_Write()
{	
	ADI_CS_LOW;
	ADI_SDI_LOW;
	ADI_CLK_LOW;
	
	for (int j = 1; j <= 3; j++)
	{	
		for (int i = 0; i <= 7; i++)
		{
			ADI_SDI_LOW;
			ADI_CLK_HIGH;
		
			if (data[j] & 0x80) ADI_SDI_HIGH;
			else ADI_SDI_LOW;
		
			ADI_CLK_LOW;
			data[j] = data[j] << 1;
		}
		ADI_SDI_LOW;	
	}
	
	ADI_CS_HIGH;
	_delay_us(30);
	ADI_LDAC_LOW; _delay_us(5);
	ADI_LDAC_HIGH;
}

unsigned long SPI_Read()
{
	uint16_t ADI_ReadData = 0x0000;
	
	for (int i = 0; i < 16; i++)
	{
		ADI_CLK_HIGH; _delay_us(1);
		ADI_CLK_LOW;
		
		if (PINA & 0x08) ADI_ReadData |= 0x01;
		ADI_ReadData <<= 1;
	}
	
	return ADI_ReadData;
}

unsigned char AD5421_Init(void)
{
	unsigned char status = 0x1;
	ADI_CS_HIGH;
	ADI_LDAC_HIGH;
	// setup 

	AD5421_SetRegisterValue(AD5421_REG_CTRL,
		AD5421_CTRL_WATCHDOG_DISABLE |
		AD5421_CTRL_ADC_SOURCE_TEMP |
		AD5421_CTRL_ADC_ENABLE
	);	
/*
	AD5421_SetRegisterValue(AD5421_REG_CTRL,
		AD5421_CTRL_WATCHDOG_DISABLE
	);	
*/	
	if(AD5421_GetRegisterValue(AD5421_REG_CTRL) != 0x1180)
    {
		status = 0x0;
	} 

	return (status);
}
/***************************************************************************//**
 * @brief Writes the value to the register
 *
 * @param -  regAddress - The address of the register to write to.
 * @param -  regValue - The value to write to the register.
 *
 * @return  None.    
*******************************************************************************/
void AD5421_SetRegisterValue(unsigned char regAddress, unsigned short regValue)
{
	data[1] = regAddress;
	data[2] = (unsigned char)((regValue & 0xFF00) >> 8);
	data[3] = (unsigned char)((regValue & 0x00FF) >> 0);
	
	SPI_Write();
}
/***************************************************************************//**
 * @brief Reads the value of the selected register
 *
 * @param regAddress - The address of the register to read.
 *
 * @return data - The value of the selected register register.
*******************************************************************************/
unsigned long AD5421_GetRegisterValue(unsigned char regAddress)
{
	unsigned long receivedData = 0x00;	
	
	data[1] = regAddress | AD5421_READ;
	data[2] = 0x00;
	data[3] = 0x00;
	
	ADI_CS_LOW; 
	SPI_Write();
	ADI_CS_HIGH;
	_delay_us(5);
		
	ADI_CS_LOW;
	// SPI_Read(data,3);
	receivedData = SPI_Read();
	ADI_CS_HIGH;
	//receivedData += (data[1] << 8);
	//receivedData += (data[2] << 0);

    return receivedData;
}
/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @param value - The value to be vritten to the DAC register.
 *
 * @return None.
*******************************************************************************/
void AD5421_WriteDac(unsigned short value)
{
	AD5421_SetRegisterValue(AD5421_REG_DAC_DATA, value);
}
/***************************************************************************//**
 * @brief Read the temperature of the die.
 *
 * @return temp - die temp in degrees C.
*******************************************************************************/
unsigned char AD5421_GetTemperature(void)
{
	unsigned char temp = 0;
	temp = (unsigned char)AD5421_GetRegisterValue(AD5421_REG_FAULT) & 0xFF;
	temp = temp - 128;
	temp = temp * 1.771;
	temp = 125 - temp;
	
	return temp;
}
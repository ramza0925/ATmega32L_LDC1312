/***************************************************************************//**
 *   @file   AD5421.h
 *   @brief  Header file of AD5421 Driver.
 *   @author Bancisor Mihai
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: 593
*******************************************************************************/
#ifndef _AD5421_H_
#define _AD5421_H_

#include <avr/io.h>
#include <util/delay.h>

//******************************************************************************/
/* GPIO Definitions                                                           */
/******************************************************************************/
#define ADI_CS_LOW      (PORTA &= ~0x01)
#define ADI_CS_HIGH		(PORTA |= 0x01)
#define ADI_CLK_LOW		(PORTA &= ~0x02)
#define ADI_CLK_HIGH	(PORTA |= 0x02)
#define ADI_LDAC_LOW	(PORTA &= ~0x20)
#define ADI_LDAC_HIGH	(PORTA |= 0x20)
#define ADI_SDI_LOW		(PORTA &= ~0x04)
#define ADI_SDI_HIGH	(PORTA |= 0x04)
/******************************************************************************/
/* AD5421                                                                    */
/******************************************************************************/
/* Registers */
#define AD5421_REG_DAC_DATA				0x1
#define AD5421_REG_CTRL					0x2
#define AD5421_REG_OFFSET				0x3
#define AD5421_REG_GAIN					0x4
/* load dac and fault shared the same register number. Writing to it will cause
 * a dac load command, reading from it will return the fault status register */
#define AD5421_REG_LOAD_DAC				0x5
#define AD5421_REG_FAULT				0x5
#define AD5421_REG_FORCE_ALARM_CURRENT	0x6
#define AD5421_REG_RESET				0x7
#define AD5421_REG_START_CONVERSION		0x8
#define AD5421_REG_NOOP					0x9

#define AD5421_CTRL_WATCHDOG_TIME(x)	(((x) & 0x7) << 13)
#define AD5421_CTRL_WATCHDOG_DISABLE	(1 << 12)
#define AD5421_CTRL_AUTO_FAULT_READBACK	(1 << 11)
#define AD5421_CTRL_MIN_CURRENT			(1 << 9)
#define AD5421_CTRL_ADC_SOURCE_TEMP		(1 << 8)
#define AD5421_CTRL_ADC_ENABLE			(1 << 7)
#define AD5421_CTRL_PWR_DOWN_INT_VREF	(1 << 6)

#define AD5421_FAULT_SPI				(1 << 15)
#define AD5421_FAULT_PEC				(1 << 14)
#define AD5421_FAULT_OVER_CURRENT		(1 << 13)
#define AD5421_FAULT_UNDER_CURRENT		(1 << 12)
#define AD5421_FAULT_TEMP_OVER_140		(1 << 11)
#define AD5421_FAULT_TEMP_OVER_100		(1 << 10)
#define AD5421_FAULT_UNDER_VOLTAGE_6V	(1 << 9)
#define AD5421_FAULT_UNDER_VOLTAGE_12V	(1 << 8)

#define AD5421_READ						(1 << 7)
/******************************************************************************/
/* Functions Prototypes                                                       */
/******************************************************************************/
/* Initializes the SPI communication peripheral and resets the part. */
unsigned char AD5421_Init(void);
/* Writes the value to the register. */
void AD5421_SetRegisterValue(unsigned char regAddress, unsigned short regValue);
/* Reads the value of the selected register. */
unsigned long AD5421_GetRegisterValue(unsigned char regAddress);
/* Writes to the DAC register. */
void AD5421_WriteDac(unsigned short value);
/* Read the temperature of the die. */
unsigned char AD5421_GetTemperature(void);

#endif // _AD5421_H

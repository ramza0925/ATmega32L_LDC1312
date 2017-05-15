#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define GPIO_USART              GPIOA
#define GPIO_USART_Rx_Pin       GPIO_Pin_10
#define GPIO_USART_Tx_Pin       GPIO_Pin_9

#define GPIO_KEY                GPIOC
#define GPIO_KEY_DOWN_Pin       GPIO_Pin_0
#define GPIO_KEY_ENTER_Pin      GPIO_Pin_1
#define GPIO_KEY_UP_Pin         GPIO_Pin_2
#define GPIO_KEY_NEXT_Pin       GPIO_Pin_3

#define GPIO_MODE               GPIOC
#define GPIO_MODE0_Pin          GPIO_Pin_4
#define GPIO_MODE1_Pin          GPIO_Pin_5

#define GPIO_GLCD               GPIOB
#define GPIO_GLCD_SCL_Pin       GPIO_Pin_10
#define GPIO_GLCD_SI_Pin        GPIO_Pin_11
#define GPIO_GLCD_NRES_Pin      GPIO_Pin_12
#define GPIO_GLCD_NCS_Pin       GPIO_Pin_13
#define GPIO_GLCD_AO_Pin        GPIO_Pin_14

#define GPIO_LDC                GPIOB
#define GPIO_LDC_SD_Pin         GPIO_Pin_1
#define GPIO_LDC_INTB_Pin       GPIO_Pin_5
#define GPIO_LDC_SCL_Pin        GPIO_Pin_6
#define GPIO_LDC_SDA_Pin        GPIO_Pin_7

#define SPIy            SPI1
#define SPIy_CLK        RCC_APB2Periph_SPI1
#define SPIy_GPIO       GPIOA
#define SPIy_PIN_ALARM  GPIO_Pin_2
#define SPIy_PIN_CLR    GPIO_Pin_3
#define SPIy_PIN_LATCH  GPIO_Pin_4
#define SPIy_PIN_SCK    GPIO_Pin_5
#define SPIy_PIN_MISO   GPIO_Pin_6
#define SPIy_PIN_MOSI   GPIO_Pin_7

#define GPIO_ADC                GPIOA
#define GPIO_ADC_PIN            GPIO_Pin_0

void RCC_Configuration();
void NVIC_Configuration();
void GPIO_Configuration();
void USART1_Init();
void I2C1_Init();
void GLCD_IO_Init();
void ADC1_Configuration();

#endif //_HW_CONFIG_H_
#include "HW_Config.h"

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif


PUTCHAR_PROTOTYPE
{
    //Wirte a character to the USART
    if(ch == '\n')
    {
        USART_SendData(USART1, '\r');
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, '\n');
    }
    else
    {
        USART_SendData(USART1, (uint8_t)ch);
    }

    //loop until the end of transmission
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    
    return ch;
}


void RCC_Configuration()
{
    //Reset
    RCC_DeInit();

    //activate HSI
    RCC_HSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    //configure prescalers
    RCC_HCLKConfig(RCC_SYSCLK_Div1);    //HCLK = 64MHz, AHB
    RCC_PCLK1Config(RCC_HCLK_Div2);     //APB1 = 32MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);     //APB1 = 64MHz

    //set up FLASH
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    //set up PLL
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16); //(8M / 2) * 16 = 64MHz
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    //Set up SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);      //SYSCLK = PLLCLK => 64MHz
    while(RCC_GetSYSCLKSource() != 0x08);

    //set up Peripheral clock(GPIOA, GPIOB, GPIOC, USART1, I2C)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
}


void NVIC_Configuration()

{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
}

void GPIO_Configuration()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //Configure Mode Pin as in Floating
    GPIO_InitStructure.GPIO_Pin = GPIO_MODE0_Pin|GPIO_MODE1_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_MODE, &GPIO_InitStructure);

    //Configure LDC Module
    //I2C Pin
    GPIO_InitStructure.GPIO_Pin = GPIO_LDC_SCL_Pin|GPIO_LDC_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIO_LDC, &GPIO_InitStructure);
    //LDC_SD Pin
    GPIO_InitStructure.GPIO_Pin = GPIO_LDC_SD_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_LDC, &GPIO_InitStructure);
    //LDC_INTB Pin
    GPIO_InitStructure.GPIO_Pin = GPIO_LDC_INTB_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_LDC, &GPIO_InitStructure); 

    //CUSTOM_SPI Configure
    GPIO_InitStructure.GPIO_Pin = SPIy_PIN_SCK|SPIy_PIN_MOSI|SPIy_PIN_CLR|SPIy_PIN_LATCH;
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SPIy_PIN_MISO|SPIy_PIN_ALARM;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SPIy_GPIO, &GPIO_InitStructure);
}

void USART1_Init()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //UARTConfiguration
    //Configure USART_Tx as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_USART_Tx_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIO_USART, &GPIO_InitStructure);

    //Configure USART_Rx as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_USART_Rx_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_USART, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    //Enable the USART1
    USART_Cmd(USART1, ENABLE);
}

void I2C1_Init()
{
    I2C_InitTypeDef I2C_InitStructure;

    I2C_Cmd(I2C1, ENABLE);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 200000;
    I2C_InitStructure.I2C_OwnAddress1 = 0x2B;
    
    I2C_Init(I2C1, &I2C_InitStructure);    
}

void GLCD_IO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //Configure GLCD Pin as out pp
    GPIO_InitStructure.GPIO_Pin = GPIO_GLCD_AO_Pin|GPIO_GLCD_NCS_Pin|GPIO_GLCD_NRES_Pin|
        GPIO_GLCD_SCL_Pin|GPIO_GLCD_SI_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_GLCD, &GPIO_InitStructure);

    //Configure KEY Pin as in_Floating
    GPIO_InitStructure.GPIO_Pin = GPIO_KEY_DOWN_Pin|GPIO_KEY_UP_Pin|GPIO_KEY_ENTER_Pin|GPIO_KEY_NEXT_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
}

void ADC1_Configuration()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //ADC  = 64MHz/6 = 10Mhz
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIO_ADC, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    ADC_Cmd(ADC1, ENABLE);
}


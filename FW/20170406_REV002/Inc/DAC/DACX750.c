#include "DACX750.h"

#define MAX_CURRENT     24.0                //최대 전류 
#define MIN_CURRENT     0.0                 //최소 전류 
#define RESOLATION      65536               //해상도 16bit

//CUSTOM SPI Data Write
void Write_SPI(uint8_t data)
{
    uint8_t mask;
    
    for(mask = 0x80; mask != 0; mask >>= 1)
    {
        DACX750_SCK_LOW();
        if(mask & data)
        {
            DACX750_MOSI_HIGH();
        }
        else
        {
            DACX750_MOSI_LOW();
        }
        DACX750_SCK_HIGH();
    }
    DACX750_SCK_LOW();
}

//CUSTOM SPI Data Read
uint8_t Read_SPI()
{
    uint8_t buf = 0x00;
    uint8_t i= 0;

    for(i = 0; i < 8 ; i++)
    {
        DACX750_SCK_LOW();
        buf |= (GPIO_ReadInputDataBit(SPIy_GPIO, SPIy_PIN_MISO) << (8 -i));
        DACX750_SCK_HIGH();
    }
    return buf;
}

void Write_DAC_Data(uint8_t addr, uint16_t data)
{
    //1. LATCH LOW
    DACX750_LATCH_LOW();
    //2. Write Operation(24bit)
    Write_SPI(addr);
    Write_SPI((uint8_t)(data >> 8));
    Write_SPI((uint8_t)data);
    //3. LATCH HIGH and LOW
    DACX750_LATCH_HIGH();
    DACX750_LATCH_LOW();
}


uint16_t Read_DAC_Data(uint8_t addr)
{
    uint16_t tmp = 0x0000;

    //1. Write Read Operation
    Write_DAC_Data(REG_RD, addr);

    //2. LATCH LOW
    DACX750_LATCH_LOW();
    //3. Read Data(24bit)
    tmp = Read_SPI();
    tmp = 0x0000;
    tmp |= (uint16_t)Read_SPI() << 8;
    tmp |= (uint16_t)Read_SPI();   
    //4. LATCH HIGH and LOW
    DACX750_LATCH_HIGH();
    DACX750_LATCH_LOW();
    
    return tmp;
}

void DAC_REGBit_Write(uint16_t addr, uint16_t bit, uint16_t enable)
{
    uint16_t tmp;
    tmp = Read_DAC_Data(addr);

    if(enable) tmp |= (uint16_t)bit;
    else tmp &= ~((uint16_t)bit);

    Write_DAC_Data(addr, tmp);
}

void DAC_Reset()
{
    Write_DAC_Data(WR_RST_REG, RST);
}

void DAC_Clear()
{
    DACX750_CLR_HIGH();
    delay_us(10);
    DACX750_CLR_LOW();
}

void DAC_Set_Range(uint16_t range)
{
    uint16_t tmp;
    tmp = Read_DAC_Data(RD_CONT_REG);
    
    if(range > 4 && range < 8)
    {
        tmp &= ~((uint16_t)RANGE);
        tmp |= range;
        
        Write_DAC_Data(WR_CONT_REG, tmp);
    }
}

//DACX750 Configuer
void DACX750_Configuration()
{
    DAC_Reset();
    DAC_Clear();
    
    //Control Register Configure
    Write_DAC_Data(WR_CONT_REG, RANGE_0_24mA|OUTEN_EN);    
}

//Current Out
void DAC_Current_Out(float cur)
{
    uint16_t tmp;

    //tmp = (uint16_t)(((cur/16) - (1/4)) * 65536);                  //4~20mA일 경우
    tmp = (uint16_t)((cur/(MAX_CURRENT-MIN_CURRENT))*RESOLATION);   //0~24mA일 경우 
    Write_DAC_Data(WR_DAC_DATA_REG,tmp);
}


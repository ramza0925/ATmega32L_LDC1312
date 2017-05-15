#ifndef _DACX750_H_
#define _DACX750_H_

#include <stdint.h>

#include "HW_Config.h"
#include "Utils/Delay.h"

//Write Address
#define NO_OPERATION        0x00
#define WR_DAC_DATA_REG     0x01
#define REG_RD              0x02
#define WR_CONT_REG         0x55
#define WR_RST_REG          0x56
#define WR_CONF_REG         0x57
#define WR_DAC_GAIN_CAL_REG 0x58
#define WR_DAC_ZERO_CAL_REG 0x59
#define WDT_RST             0x95

//Read Register Address
#define RD_STA_REG          0x00
#define RD_DAC_DATA_REG     0x01
#define RD_CONT_REG         0x02
#define RD_CONF_REG         0x0B
#define RD_DAC_GAIN_CAL_REG 0x13
#define RD_DAC_ZERO_CAL_REG 0x17

//Control Register 
#define REXT                (0x01 << 13)
#define REXT_EN             (0x01 << 13)
#define REXT_DIS            (0x00 << 13)

#define OUTEN               (0x01 << 12)
#define OUTEN_EN            (0x01 << 12)
#define OUTEN_DIS           (0x00 << 12)

#define SRCLK               (0x0F << 8)
#define SRCLK_DEFAULT       (0x00 << 8)
#define SRCLK_258065        (0x00 << 8)
#define SRCLK_200000        (0x01 << 8)
#define SRCLK_153845        (0x02 << 8)
#define SRCLK_131145        (0x03 << 8)
#define SRCLK_115940        (0x04 << 8)
#define SRCLK_69565         (0x05 << 8)
#define SRCLK_37560         (0x06 << 8)
#define SRCLK_25805         (0x07 << 8)
#define SRCLK_20150         (0x08 << 8)
#define SRCLK_16030         (0x09 << 8)
#define SRCLK_10295         (0x0A << 8)
#define SRCLK_8280          (0x0B << 8)
#define SRCLK_6900          (0x0C << 8)
#define SRCLK_5530          (0x0D << 8)
#define SRCLK_4240          (0x0E << 8)
#define SRCLK_3300          (0x0F << 8)

#define SRSTEP              (0x07 << 5)
#define SRSTEP_DEFAULT      (0x00 << 5)
#define SRCSTEP_0R0625_1    (0x00 << 5)
#define SRCSTEP_0R125_2     (0x01 << 5)
#define SRCSTEP_0R125_4     (0x02 << 5)
#define SRCSTEP_0R5_8       (0x03 << 5)
#define SRCSTEP_1_16        (0x04 << 5)
#define SRCSTEP_2_32        (0x05 << 5)
#define SRCSTEP_4_64        (0x06 << 5)
#define SRCSTEP_8_125       (0x07 << 5)

#define SREN                (0x01 << 4)
#define SREN_EN             (0x01 << 4)
#define SREN_DIS            (0x00 << 4)

#define DCEN                (0x01 << 3)
#define DCEN_EN             (0x01 << 3)
#define DCEN_DIS            (0x00 << 3)

#define RANGE               (0x07 << 0)
#define RANGE_DEFAULT       (0x00 << 0)
#define RANGE_4_20mA        (0x05 << 0)
#define RANGE_0_20mA        (0x06 << 0)
#define RANGE_0_24mA        (0x07 << 0)

//Configuration Register
#define CALEN               (0x01 << 5)
#define CALEN_EN            (0x01 << 5)
#define CALEN_DIS           (0x00 << 5)

#define HARTEN              (0x01 << 4)
#define HARTEN_EN           (0x01 << 4)
#define HARTEN_DIS          (0x00 << 4)

#define CRCEN               (0x01 << 3)
#define CRCEN_EN            (0x01 << 3)
#define CRCEN_DIS           (0x00 << 3)

#define WDEN                (0x01 << 2)
#define WDEN_EN             (0x01 << 2)
#define WDEN_DIS            (0x00 << 2)

#define WDPD                (0x03 << 0)
#define WDPD_DEFAULT        (0x00 << 0)
#define WDPD_10ms           (0x00 << 0)
#define WDPD_51ms           (0x01 << 0)
#define WDPD_102ms          (0x02 << 0)
#define WDPD_204ms          (0x03 << 0)

//Reset Register
#define RST                 (0x01 << 0)

//Status Register
#define CRC_FLT             (0x01 << 4)
#define WD_FLT              (0x01 << 3)
#define I_FLT               (0x01 << 2)
#define SR_ON               (0x01 << 1)
#define T_FLT               (0x01 << 0)

#define DACX750_LATCH_HIGH()        GPIO_SetBits(SPIy_GPIO, SPIy_PIN_LATCH)
#define DACX750_LATCH_LOW()         GPIO_ResetBits(SPIy_GPIO, SPIy_PIN_LATCH)
#define DACX750_MOSI_HIGH()         GPIO_SetBits(SPIy_GPIO, SPIy_PIN_MOSI)
#define DACX750_MOSI_LOW()          GPIO_ResetBits(SPIy_GPIO, SPIy_PIN_MOSI)
#define DACX750_SCK_HIGH()          GPIO_SetBits(SPIy_GPIO, SPIy_PIN_SCK)
#define DACX750_SCK_LOW()           GPIO_ResetBits(SPIy_GPIO, SPIy_PIN_SCK)
#define DACX750_CLR_HIGH()          GPIO_SetBits(SPIy_GPIO, SPIy_PIN_CLR)
#define DACX750_CLR_LOW()           GPIO_ResetBits(SPIy_GPIO, SPIy_PIN_CLR)

void DACX750_Configuration();
void DAC_Current_Out(float cur);
uint16_t Read_DAC_Data(uint8_t addr);
void Write_DAC_Data(uint8_t addr, uint16_t data);

#endif
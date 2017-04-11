#ifndef __LDC1612_H__
#define __LDC1612_H__

#include <stdint.h>

#include <ADuCM360.h>
#include <DioLib.h>
#include <I2cLib.h>

//Register Map
#define DATA_CH0_MSB			0x00				//CH0_MSB ��ȯ����� ��������
#define DATA_CH0_LSB			0x01				//CH0_LSB ��ȯ���
#define DATA_CH1_MSB			0x02				//CH1 ��ȯ����� ������?
#define DATA_CH1_LSB			0x03				//CH1_LSB ��ȯ���
#define DATA_CH2_MSB			0x04				//CH2 ��ȯ����� ��������	(LDC1614 only)
#define DATA_CH2_LSB			0x05				//CH2_LSB ��ȯ���
#define DATA_CH3_MSB			0x06				//CH3 ��ȯ����� ��������	(LDC1614 only)
#define DATA_CH3_LSB			0x07				//CH3_LSB ��ȯ���
#define RCOUNT_CH0				0x08				//CH0 ���� ī���� ����  
#define RCOUNT_CH1				0x09				//CH1 ���� ī���� ����  
#define RCOUNT_CH2				0x0A				//CH2 ���� ī���� ����  		(LDC1614 only)
#define RCOUNT_CH3				0x0B				//CH3 ���� ī���� ����  		(LDC1614 only)
#define OFFSET_CH0				0x0C				//CH0 ������ �� ���� 
#define OFFSET_CH1				0x0D				//CH1 ������ �� ���� 
#define OFFSET_CH2				0x0E				//CH2 ������ �� ���� 			(LDC1614 only)
#define OFFSET_CH3				0x0F				//CH3 ������ �� ���� 			(LDC1614 only)
#define SETTLECOUNT_CH0			0x10				//CH0 ����ȭ ���� ī���� 
#define SETTLECOUNT_CH1			0x11				//CH1 ����ȭ ���� ī���� 
#define SETTLECOUNT_CH2			0x12				//CH2 ����ȭ ���� ī���� 	(LDC1614 only)
#define SETTLECOUNT_CH3			0x13				//CH3 ����ȭ ���� ī���� 	(LDC1614 only)
#define CLOCK_DEVIDERS_CH0		0x14				//CH0 ���۷����� ������Ŭ�� �й� ���� 
#define CLOCK_DEVIDERS_CH1		0x15				//CH1 ���۷����� ������Ŭ�� �й� ���� 
#define CLOCK_DEVIDERS_CH2		0x16				//CH2 ���۷����� ������Ŭ�� �й� ���� (LDC1614 only)
#define CLOCK_DEVIDERS_CH3		0x17				//CH3 ���۷����� ������Ŭ�� �й� ���� (LDC1614 only)
#define	STATUS					0x18				//����̽� ���� ���� 
#define ERROR_CONFIG			0x19				//����̽� ���� ���� ���� 
#define CONFIG					0x1A				//��ȯ ����
#define MUX_CONFIG				0x1B				//ä�� ��Ƽ �÷��� ���� 
#define RESET_DEV				0x1C				//����̽� ����
#define DRIVE_CURRENT_CH0		0x1E				//CH0 ���� ���� ���� ���� 
#define DRIVE_CURRENT_CH1		0x1F				//CH1 ���� ���� ���� ���� 
#define DRIVE_CURRENT_CH2		0x20				//CH2 ���� ���� ���� ���� 
#define DRIVE_CURRENT_CH3		0x21				//CH3 ���� ���� ���� ���� 
#define MANUFACTURER_ID			0x7E				//����̽� ������ ID
#define DEVICE_ID				0x7F				//����̽� ID

//DATA_CHx Register(CH0: 0x00, CH1: 0x02, Ch2: 0x04(LDC1314 Only), CH3:0x06(LDC1314 Only))
#define CHx_ERR_UR				(0x0001 << 15)		//CHx ��ȯ ���� �̸� �÷��� 
#define CHx_ERR_OR				(0x0001 << 14)		//CHx ��ȯ ���� �ʰ� �÷��� 
#define CHx_ERR_WD				(0x0001 << 13)		//CHx ��ȯ ���� ��ġ�� Ÿ�Ӿƿ� �÷��� 
#define CHx_ERR_AE				(0x0001 << 12)		//CHx ��ȯ ���� ���� ���� 
#define CHx_DATA				(0x0FFF << 0)		//CHx ��ȯ ��� �� 

//RCOUNT_CHx Register(CH0: 0x08, CH1: 0x09, Ch2: 0x0A(LDC1314 Only), CH3:0x0B(LDC1314 Only))
//#define CHx_RCOUNT				(0xFFFF << 0)

//OFFSET_CHx Register(CH0: 0x0C, CH1: 0x0D, Ch2: 0x0E(LDC1314 Only), CH3:0x0F(LDC1314 Only))
//#define CHx_OFFSET				(0xFFFF << 0)

//SETTLECOUNT_CHx Register(CH0: 0x10, CH1: 0x11, Ch2: 0x12(LDC1314 Only), CH3:0x13(LDC1314 Only))
//#define CHx_SETTLECOUNT			(0xFFFF << 0)

//CLOCK_DIVIDERS_CHx Register(CH0: 0x14, CH1: 0x15, Ch2: 0x16(LDC1314 Only), CH3:0x17(LDC1314 Only))
#define CHx_FIN_DIVIDER			(0x000F << 12)
#define CHx_FREF_DIVIDER		(0x03FF << 0)

//STATUS Register(0x18)
#define ERR_CHAN				(0x0003 << 14)		//���� �߻� ä��						
#define ERR_UR					(0x0001 << 13)		//���� �̸� ����						(1: O, 0: X)
#define ERR_OR					(0x0001 << 12)		//���� �ʰ� ���� 						(1: O, 0: X)
#define ERR_WD					(0x0001 << 11)		//��ġ�� Ÿ�Ӿƿ� 					(1: O, 0: X)
#define ERR_AHE					(0x0001 << 10)		//���� ���� ����						(1: O, 0: X)
#define ERR_ALE					(0x0001 << 9)		//���� ���� ���� 						(1: O, 0: X)
#define ERR_ZC					(0x0001 << 8)		//ī���� ���� ����					(1: O, 0: X)
#define DRDY					(0x0001 << 6)		//������ �غ� �Ϸ�					(1: O, 0: X)
#define CH0_UNREAD_CONV			(0x0001 << 3)		//CH0�� ���� ���� ��ȯ�� ����	(1: O, 0: X)
#define CH1_UNREAD_CONV			(0x0001 << 2)		//CH1�� ���� ���� ��ȯ�� ����	(1: O, 0: X)
#define CH2_UNREAD_CONV			(0x0001 << 1)		//CH2�� ���� ���� ��ȯ�� ����	(1: O, 0: X)
#define CH3_UNREAD_CONV			(0x0001 << 0)		//CH3�� ���� ���� ��ȯ�� ����	(1: O, 0: X)

//ERROR_CONFIG Register(0x19)
#define UR_ERR2OUT				(0x0001 << 15)		//���� �̸� ���� ��� ���� 		(1: O, 0: X)
#define UR_ERR2OUT_EN			(0x0001 << 15)
#define UR_ERR2OUT_DIS			(0x0000 << 15)

#define OR_ERR2OUT				(0x0001 << 14)		//���� �ʰ� ���� ��� ����		(1: O, 0: X)
#define OR_ERR2OUT_EN			(0x0001 << 14)
#define OR_ERR2OUT_DIS			(0x0000 << 14)

#define WD_ERR2OUT				(0x0001 << 13)		//��ġ�� Ÿ�Ӿƿ� ��� ���� 	(1: O, 0: X)
#define WD_ERR2OUT_EN			(0x0001 << 13)
#define WD_ERR2OUT_DIS			(0x0000 << 13)

#define AH_ERR2OUT				(0x0001 << 12)		//���� ���� ���� ��� ���� 		(1: O, 0: X)
#define AH_ERR2OUT_EN			(0x0001 << 12)
#define AH_ERR2OUT_DIS			(0x0000 << 12)

#define AL_ERR2OUT				(0x0001 << 11)		//���� ���� ���� ��� ���� 		(1: O, 0: X)
#define AL_ERR2OUT_EN			(0x0001 << 11)
#define AL_ERR2OUT_DIS			(0x0000 << 11)

#define UR_ERR2INT				(0x0001 << 7)		//���� �̸� ���� ���ͷ�Ʈ 		(1: O, 0: X)
#define UR_ERR2INT_EN			(0x0001 << 7)
#define UR_ERR2INT_DIS			(0x0000 << 7)

#define OR_ERR2INT				(0x0001 << 6)		//���� �ʰ� ���� ���ͷ�Ʈ 		(1: O, 0: X)
#define OR_ERR2INT_EN			(0x0001 << 6)
#define OR_ERR2INT_DIS			(0x0000 << 6)

#define WD_ERR2INT				(0x0001 << 5)		//��ġ��Ÿ�̸��� ���ͷ�Ʈ 	(1: O, 0: X)
#define WD_ERR2INT_EN			(0x0001 << 5)
#define WD_ERR2INT_DIS			(0x0000 << 5)

#define AH_ERR2INT				(0x0001 << 4)		//���� ���� ���� ���ͷ�Ʈ 		(1: O, 0: X)
#define AH_ERR2INT_EN			(0x0001 << 4)
#define AH_ERR2INT_DIS			(0x0000 << 4)

#define AL_ERR2INT				(0x0001 << 3)		//���� ���� ���� ���ͷ�Ʈ 		(1: O, 0: X)
#define AL_ERR2INT_EN			(0x0001 << 3)
#define AL_ERR2INT_DIS			(0x0000 << 3)

#define ZC_ERR2INT				(0x0001 << 2)		//ī���� ���� ���� ���ͷ�Ʈ 	(1: O, 0: X)
#define ZC_ERR2INT_EN			(0x0001 << 2)
#define ZC_ERR2INT_DIS			(0x0000 << 2)

#define DRDY_2INT				(0x0001 << 0)		//������ �غ� �Ϸ� ���ͷ�Ʈ 	(1: O, 0: X)
#define DRDY_2INT_EN			(0x0001 << 0)
#define DRDY_2INT_DIS			(0x0000 << 0)


//CONFIG Register (0x1A)
#define ACTIVE_CHAN				(0x0003 << 14)		//Ȱ�� ä�� ����(CH0(0), CH1(1), CH2(2), CH3(3))
#define ACTIVE_CHAN_CH0			(0x0000 << 14)
#define ACTIVE_CHAN_CH1			(0x0001 << 14)
#define ACTIVE_CHAN_CH2			(0x0002 << 14)
#define ACTIVE_CHAN_CH3			(0x0003	<< 14)

#define SLEEP_MODE_EN			(0x0001 << 13)		//���� ��� Ȱ��ȭ(Ȱ��ȭ(0), �������(1))
#define SLEEP_MODE_DIS			(0x0000 << 13)

#define RP_OVERRIDE_EN			(0x0001 << 12)		//���� RP ġȯ Ȱ��ȭ(����(0), ����(1))
#define RP_OVERRIDE_DIS			(0x0000 << 12)

#define SENSOR_ACTIVATE_SEL		(0x0001 << 11)		//���� Ȱ��ȭ ��� ����(������(1))
#define SENSOR_ACTIVATE_LOW		(0x0001 << 11)
#define SENSOR_ACTIVATE_FULL	(0x0000 << 11)

#define AUTO_AMP				(0x0001 << 10)		//�ڵ� ���� ���� Ȱ��ȭ(Ȱ��ȭ(0), ��Ȱ��ȭ(1))
#define AUTO_AMP_DIS			(0x0001 << 10)		
#define AUTO_AMP_EN				(0x0000 << 10)

#define REF_CLK_SRC				(0x0001 << 9)		//���� ���ļ� Ŭ�� ����(����(0), �ܺ�(1))
#define REF_CLK_SRC_EXT			(0x0001 << 9)
#define REF_CLK_SRC_IN			(0x0000 << 9)

#define INTB					(0x0001 << 7)		//���ͷ�Ʈ Ȱ��ȭ(Ȱ��ȭ(0), ��Ȱ��ȭ(1))
#define INTB_DIS				(0x0001 << 7)		
#define INTB_EN					(0x0000 << 7)

#define HIGH_CURRENT_DRV		(0x0001 << 6)		//�� ���� ���� ����(�Ϲ�(0), ������(1))
#define HIGH_CURRENT_DRV_EN		(0x0001 << 6)
#define HIGH_CURRENT_DRV_DIS	(0x0000 << 6)

//MUX_CONFIG Register (0x1B)
#define AUTOSCAN_EN				(0x0001 << 15)		//�ڵ� ��ĵ ��� Ȱ��ȭ(�̱�ä�� ���� ��ȯ(0), ���� ä���ڵ���ĵ(1))
#define AUTOSCAN_DIS			(0x0000 << 15)

#define RR_SEQUENCE				(0x0002 << 13)		//�ڵ� ��ĵ ä�� ����(CH0 & CH1(0), CH0 & CH1 & CH2(1), CH0 & CH1 & CH2& CH3(2), CH0 & CH1(3))
#define RR_SEQUENCE_CH0_1		(0x0000 << 13)
#define RR_SEQUENCE_CH0_2		(0x0001 << 13)
#define RR_SEQUENCE_CH0_3		(0x0002 << 13)

#define DEGLITCH				(0x0007 << 0)		//�Է� Deglitch ���� ��� ��(1M(1), 3.3M(4), 10M(5), 33M(7))
#define DEGHITCH_1M				(0x0001 << 0)
#define DEGHITCH_3R3M			(0x0004 << 0)
#define DEGHITCH_10M			(0x0005 << 0)
#define DEGHITCH_33M			(0x0007 << 0)


//RESET_DEV Register (0x1C)
#define RESET_DEVICE			(0x0001 << 15)		//����̽� ����(����(1))
#define OUTPUT_GAIN				(0x0002 << 9)		//��� ���� ����(1(0), 4(1), 8(2), 16(3))
#define OUTPUT_GAIN_1			(0x0000 << 9)
#define OUTPUT_GAIN_4			(0x0001 << 9)
#define OUTPUT_GAIN_8			(0x0002 << 9)
#define OUTPUT_GAIN_16			(0x0003 << 9)

//DEVICE_CURRENT_CHx Register(CH0: 0x1E, CH1: 0x1F, Ch2: 0x20(LDC1314 Only), CH3:0x21(LDC1314 Only))
#define CHx_IDRIVE				(0x001F << 11)		//CHx L-C ���� ���� ���� 
#define CHx_INIT_IDRIVE			(0x001F << 6)		//CHx ���� ���� ����

//MANUFACTURER_ID Register (0x7E)
#define MANUFACTURERID			(0x5449 << 0)

//DEVICE_ID Register (0x7F)
#define DEVICEID				(0x3055 << 0)


#define LDC_SD_H				DioSet(pADI_GP1, 0x80)
#define LDC_SD_L			    DioClr(pADI_GP1, 0x80)
#define LDC_SCL_H				DioSet(pADI_GP2, 0x01)
#define LDC_SCL_L				DioClr(pADI_GP2, 0x01)
#define LDC_SCK_H				DioSet(pADI_GP2, 0x02)
#define LDC_SCK_L				DioClr(pADI_GP2, 0x02)

#define LDC_ADDR					0x2B

#define CH0							0
#define CH1							1
#define CH2							2
#define CH3							3

#define FIN_DIVIDER					0x0002
#define FREF_DIVIDER				0x0004
#define SETTLECOUNT					0x000A
#define RCOUNT						0x04D6
#define OFFSET_POINT				0xD8EF


void LDC_Init(uint16_t offset);
void Set_Config(uint16_t conf);
uint16_t Get_Config();
uint16_t Get_Status();
void Set_Error_Config(uint16_t err_conf);
uint16_t Get_Error_Config();
void Set_Divider(uint8_t ch, uint16_t fin_div, uint16_t fref_div);
uint16_t Get_Divider(uint8_t ch);
void Set_SettleCount(uint8_t ch, uint16_t settleCnt);
uint16_t Get_SettleCount(uint8_t ch);
void Set_RCount(uint8_t ch, uint16_t rCnt);
uint16_t Get_RCount(uint8_t ch);
void Set_IDrive(uint8_t ch, uint16_t iDrv, uint16_t init_iDrv);
uint16_t Get_IDreive(uint8_t ch);
void Set_MUX(uint16_t mux);
uint16_t Get_MUX();
void Set_Gain(uint16_t gain);
uint16_t Get_Data(uint8_t ch);
void Set_Offset(uint8_t ch, uint16_t offset);
uint16_t Get_Offset(uint8_t ch);
void Reset_Device();
void Offset_Setting(uint8_t ch, uint16_t offset);
void Gain_Setting(uint16_t gain);
uint16_t Read_Data(uint8_t addr);
void Write_Data(uint16_t data, uint8_t addr);

#endif